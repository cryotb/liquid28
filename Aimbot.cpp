#include "Aimbot.h"
#include "Util.h"
#include "C_BasePredictionSystem.h"
#include "C_CheatEvents.h"
#include "RS.h"
#include "G.hpp"

CAimbot gAim;
QUtils g_pQAngle;

Vector calc_angle(Vector src, Vector dst)
{
	Vector AimAngles, delta;
	float hyp;
	delta = src - dst;
	hyp = sqrtf((delta.x * delta.x) + (delta.y * delta.y)); //SUPER SECRET IMPROVEMENT CODE NAME DONUT STEEL
	AimAngles.x = atanf(delta.z / hyp) * RADPI;
	AimAngles.y = atanf(delta.y / delta.x) * RADPI;
	AimAngles.z = 0.0f;
	if (delta.x >= 0.0)
		AimAngles.y += 180.0f;
	return AimAngles;
}

void MakeVector(Vector angle, Vector& vector)
{
	float pitch, yaw, tmp;
	pitch = float(angle[0] * PI / 180);
	yaw = float(angle[1] * PI / 180);
	tmp = float(cos(pitch));
	vector[0] = float(-tmp * -cos(yaw));
	vector[1] = float(sin(yaw)*tmp);
	vector[2] = float(-sin(pitch));
}

float GetFOV(Vector angle, Vector src, Vector dst)
{
	Vector ang, aim;
	float mag, u_dot_v;
	ang = calc_angle(src, dst);


	MakeVector(angle, aim);
	MakeVector(ang, ang);

	mag = sqrtf(pow(aim.x, 2) + pow(aim.y, 2) + pow(aim.z, 2));
	u_dot_v = aim.Dot(ang);

	return RAD2DEG(acos(u_dot_v / (pow(mag, 2))));
}

bool CanShoot(C_Entity* entity)
{
	float srv = entity->GetTickBase() * pContext.g_Globals->interval_per_tick;
	float cht = entity->GetActiveWeapon()->GetNext();

	if (cht < srv)
	{
		pRs.miss[entity->GetIndex()] = true;

		return true;
	}

	return false;
}

void CAimbot::Run(C_Entity* g_Local, C_UserCmd* pCmd)
{
	QAngle old1 = pCmd->viewangles;
	float old2 = pCmd->forwardmove;
	float old3 = pCmd->sidemove;
	g_Options.iAimbotIndex = -1;

	if (!g_Options.aimbot_active)
		return;

	if (!Util->IsKeyPressed(6))
		return;

	if (!g_Local->GetActiveWeapon())
		return;

	C_Entity* pEntity = GetBaseEntity(GetBestTarget(g_Local));

	if (!pEntity)
		return;

	int iBestHitbox = GetBestHitbox(g_Local, pEntity);

	if (iBestHitbox == -1)
		return;

	Vector vEntity = (Vector&)pEntity->GetHitboxPosition(iBestHitbox);

	Vector vLocal = (g_pPredData008.new_origin + g_pPredData008.new_vecview);

	QAngle vAngs;

	VectorAngles((vEntity - vLocal), (Vector&)vAngs);

	ClampAngle((Vector&)vAngs);

	g_Options.iAimbotIndex = pEntity->GetIndex();

	if (g_Local->GetClassNum() == TF2_Sniper)
	{
		if (CanShoot(g_Local))
		{
			if (g_Options.aimbot_silent)
			{
				pCmd->viewangles = vAngs;
				normalize_sf(pCmd, old1, old2, old3);
			}

			if (g_Options.aimbot_autoshoot)
				pCmd->buttons |= IN_ATTACK;
		}
	}
	else
	{
		if (g_Options.aimbot_silent)
		{
			pCmd->viewangles = vAngs;
			normalize_sf(pCmd, old1, old2, old3);
		}

		if (g_Options.aimbot_autoshoot)
			pCmd->buttons |= IN_ATTACK;

		pCheatEvents.raise("* BULLET_TIME_IMPACTED *", 0x1C);
	}
}

float GetDistance(Vector vOrigin, Vector vLocalOrigin)
{
	Vector vDelta = vOrigin - vLocalOrigin;

	float m_fDistance = sqrt(vDelta.Length());

	if (m_fDistance < 1.0f)
		return 1.0f;

	return m_fDistance;
}


int CAimbot::GetBestTarget(C_Entity* pLocal)
{
	int iBestTarget = -1;
						 //this num could be smaller 
	float flDistToBest = 99999.f;

	QAngle vLocal = pLocal->GetEyePosition();

	for (int i = 1; i <= Iface.Engine->GetMaxClients(); i++)
	{
		if (i == me)
			continue;

		C_Entity* pEntity = GetBaseEntity(i);

		if (!pEntity)
			continue;

		if (pEntity->IsDormant())
			continue;

		CBaseCombatWeapon* pWpn = pLocal->GetActiveWeapon();

		if (pWpn->GetItemDefinitionIndex() == WPN_Vaccinator)
		{
			if (pEntity->GetLifeState() != LIFE_ALIVE)
				continue;
		}
		else
		{
			if (pEntity->GetLifeState() != LIFE_ALIVE ||
				pEntity->GetTeamNum() == pLocal->GetTeamNum())
				continue;
		}

		int iBestHitbox = GetBestHitbox(pLocal, pEntity);

		if (iBestHitbox == -1)
			continue;

		QAngle vEntity = pEntity->GetHitboxPosition(iBestHitbox); //pEntity->GetWorldSpaceCenter(vEntity);

		if (!g_Options.PlayerMode[i])
			continue;

		if (pEntity->GetCond() & TFCond_Ubercharged ||
			pEntity->GetCond() & TFCond_UberchargeFading ||
			pEntity->GetCond() & TFCond_Bonked)
			continue;

		auto pCommand = g::cmd;

		if (!pCommand)
			continue;

		float flFOV = GetFOV((Vector&)pCommand->viewangles, (Vector&)vLocal, (Vector&)vEntity);
		float distance = GetDistance((Vector&)vEntity, (Vector&)pLocal->GetEyePosition());

		if (distance < 180)//gCvars.aimbot.fov)
		{
			if (flFOV < flDistToBest && flFOV < 180)
			{
				//flDistToBest = flDistToTarget;
				flDistToBest = flFOV;
				iBestTarget = i;
				g_Options.iAimbotIndex = i;
			}
		}

		if (g_Options.PlayerMode[i] == 2) //always aim at rage targets first
			return i;
	}

	return iBestTarget;
}

int CAimbot::GetBestHitbox(C_Entity* pLocal, C_Entity* pEntity)
{
	int iBestHitbox = -1;

	if (!g_Options.aimbot_hitbox)
	{
		if (Util->IsHeadshotWeapon(pLocal, pLocal->GetActiveWeapon()))
			iBestHitbox = 0;
		else
			iBestHitbox = 4;
	}
	else
	{
		iBestHitbox = g_Options.aimbot_hitbox - 1;
	}

	if (g_Options.aimbot_hitscan)
	{
		for (int i = 0; i < 17; i++)
		{
			if (Util->IsVisible(pLocal, pEntity, (QAngle&)(g_pPredData008.new_origin + g_pPredData008.new_vecview), (QAngle&)pEntity->GetHitboxPosition(i)))
				return i;
		}
	}

	if (pEntity->GetHitboxPosition(iBestHitbox).IsZero())
		return -1;

	if (!Util->IsVisible(pLocal, pEntity, (QAngle&)(g_pPredData008.new_origin + g_pPredData008.new_vecview), (QAngle&)pEntity->GetHitboxPosition(iBestHitbox)))
		return -1;
	
	return iBestHitbox;
}