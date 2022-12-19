#include "ESP.h"
#include "CDrawManager.h"
#include <string>
#include "Context.h"
#include "Util.h"

CESP gESP;

void HealthBar(C_Entity* pEntity, int x, int y, int height)
{
}

void CESP::Run(C_Entity* pLocal)
{
	if (!g_Options.esp_active)
		return;

	for (int i = 1; i <= Iface.Engine->GetMaxClients(); i++)
	{
		if (g_Options.esp_local && pLocal && pLocal->GetLifeState() == LIFE_ALIVE && pLocal->IsDormant() != true)
			Player_ESP(pLocal, pLocal);

		if (i == me)
			continue;

		C_Entity* pEntity = GetBaseEntity(i);

		if (!pEntity)
			continue;

		if (pEntity->IsDormant())
			continue;

		if (pEntity->GetLifeState() != LIFE_ALIVE)
			continue;

		if (g_Options.esp_enemyonly && pEntity->GetTeamNum() == pLocal->GetTeamNum())
			continue;

		Player_ESP(pLocal, pEntity);
	}

	if (g_Options.esp_debug)
	{
		for (int i = 0; i <= Iface.EntList->GetHighestEntityIndex(); i++)
		{
			C_Entity* entity = GetBaseEntity(i);

			if (!entity)
				continue;

			if (entity->IsDormant())
				continue;

			if (entity->GetLifeState() != LIFE_ALIVE)
				continue;

			int a, b;
			Vector v, c;

			entity->GetWorldSpaceCenter(v);

			if (pDrawing.WorldToScreen(v, c))
			{
				auto cid = entity->GetClientClass()->iClassID;
				int cgui = 0;

				const matrix3x4& vMatrix = entity->GetRgflCoordinateFrame();

				Vector vMin = (Vector&)entity->GetCollideableMins();
				Vector vMax = (Vector&)entity->GetCollideableMaxs();

				Vector vPointList[] = {
					Vector(vMin.x, vMin.y, vMin.z),
					Vector(vMin.x, vMax.y, vMin.z),
					Vector(vMax.x, vMax.y, vMin.z),
					Vector(vMax.x, vMin.y, vMin.z),
					Vector(vMax.x, vMax.y, vMax.z),
					Vector(vMin.x, vMax.y, vMax.z),
					Vector(vMin.x, vMin.y, vMax.z),
					Vector(vMax.x, vMin.y, vMax.z)
				};

				Vector vTransformed[8];

				for (int i = 0; i < 8; i++)
					for (int j = 0; j < 3; j++)
						vTransformed[i][j] = vPointList[i].Dot((Vector&)vMatrix[j]) + vMatrix[j][3];

				Vector flb, brt, blb, frt, frb, brb, blt, flt;

				if (!pDrawing.WorldToScreen(vTransformed[3], flb) ||
					!pDrawing.WorldToScreen(vTransformed[0], blb) ||
					!pDrawing.WorldToScreen(vTransformed[2], frb) ||
					!pDrawing.WorldToScreen(vTransformed[6], blt) ||
					!pDrawing.WorldToScreen(vTransformed[5], brt) ||
					!pDrawing.WorldToScreen(vTransformed[4], frt) ||
					!pDrawing.WorldToScreen(vTransformed[1], brb) ||
					!pDrawing.WorldToScreen(vTransformed[7], flt))
					return;

				Vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };

				float left = flb.x;
				float top = flb.y;
				float right = flb.x;
				float bottom = flb.y;

				for (int i = 0; i < 8; i++)
				{
					if (left > arr[i].x)
						left = arr[i].x;
					if (top < arr[i].y)
						top = arr[i].y;
					if (right < arr[i].x)
						right = arr[i].x;
					if (bottom > arr[i].y)
						bottom = arr[i].y;
				}

				float x = left;
				float y = bottom;
				float w = right - left;
				float h = top - bottom;

				x += ((right - left) / 3.8); //pseudo fix for those THICC boxes
				w -= ((right - left) / 3.8) * 1.9;

				if (cid == 88)
				{
					//Sentry;
					pDrawing.DrawString(x, cgui + y, Color::Red(), "-> Sentry <-");

					if (Util->IsVisible(pLocal, entity, pLocal->GetEyePosition(), (QAngle&)v))
					{
						cgui += 15;
						pDrawing.DrawString(x, cgui + y, Color::Grey(), "Potential Attacker [*]");
					}
				}

				if (cid == 89)
				{
					//Teleporter;
					pDrawing.DrawString(x, cgui + y, Color::Cyan(), "-> Teleporter <-");
				}
			}
		}
	}
}

void CESP::Player_ESP(C_Entity* pLocal, C_Entity* pEntity)
{
	player_info_t pInfo;
	if (!Iface.Engine->GetPlayerInfo(pEntity->GetIndex(), &pInfo))
		return;

	const matrix3x4& vMatrix = pEntity->GetRgflCoordinateFrame();

	Vector vMin = (Vector&)pEntity->GetCollideableMins();
	Vector vMax = (Vector&)pEntity->GetCollideableMaxs();

	Vector vPointList[] = {
		Vector(vMin.x, vMin.y, vMin.z),
		Vector(vMin.x, vMax.y, vMin.z),
		Vector(vMax.x, vMax.y, vMin.z),
		Vector(vMax.x, vMin.y, vMin.z),
		Vector(vMax.x, vMax.y, vMax.z),
		Vector(vMin.x, vMax.y, vMax.z),
		Vector(vMin.x, vMin.y, vMax.z),
		Vector(vMax.x, vMin.y, vMax.z)
	};

	Vector vTransformed[8];

	for (int i = 0; i < 8; i++)
		for (int j = 0; j < 3; j++)
			vTransformed[i][j] = vPointList[i].Dot((Vector&)vMatrix[j]) + vMatrix[j][3];

	Vector flb, brt, blb, frt, frb, brb, blt, flt;

	if (!pDrawing.WorldToScreen(vTransformed[3], flb) ||
		!pDrawing.WorldToScreen(vTransformed[0], blb) ||
		!pDrawing.WorldToScreen(vTransformed[2], frb) ||
		!pDrawing.WorldToScreen(vTransformed[6], blt) ||
		!pDrawing.WorldToScreen(vTransformed[5], brt) ||
		!pDrawing.WorldToScreen(vTransformed[4], frt) ||
		!pDrawing.WorldToScreen(vTransformed[1], brb) ||
		!pDrawing.WorldToScreen(vTransformed[7], flt))
		return; 

	Vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };

	float left = flb.x;
	float top = flb.y;
	float right = flb.x;
	float bottom = flb.y;

	for (int i = 0; i < 8; i++)
	{
		if (left > arr[i].x)
			left = arr[i].x;
		if (top < arr[i].y)
			top = arr[i].y;
		if (right < arr[i].x)
			right = arr[i].x;
		if (bottom > arr[i].y)
			bottom = arr[i].y;
	}

	float x = left;
	float y = bottom;
	float w = right - left;
	float h = top - bottom;

	x += ((right - left) / 4.8); //pseudo fix for those THICC boxes
	w -= ((right - left) / 4.8) * 1.9;

	Color clrPlayerCol = pDrawing.GetPlayerColor(pEntity);
	Color clrBoneCol = g_Options.esp_bones == 1 ? Color::White() : g_Options.esp_bones == 2 ? Color::Green() : clrPlayerCol;
	int iY = 0;
	//iHp is only for health bar
	int iHp = pEntity->GetHealth(), iMaxHp = pEntity->GetMaxHealth();
	if (iHp > iMaxHp)
		iHp = iMaxHp;

	if (g_Options.esp_sight)
	{
		Vector angs;

		pEntity->GetWorldSpaceCenter(angs);

		Vector forward;

		AngleVectors(Vector(pEntity->GetPitch(), pEntity->GetYawh(), 0), &forward);

		forward *= 100;

		Vector vFinal = angs + forward;

		Vector a;
		Vector b;

		if (pDrawing.WorldToScreen(angs, a) && pDrawing.WorldToScreen(vFinal, b))
		{
			pDrawing.DrawLine(a.x, a.y, b.x, b.y, Color::White());
		}

		if (pEntity == pLocal)
		{
			Vector angs;

			pEntity->GetWorldSpaceCenter(angs);

			Vector forward;

			AngleVectors(Vector(pContext.hfak1s.pitch, pContext.hfak1s.yaw, 0), &forward);

			forward *= 100;

			Vector vFinal = angs + forward;

			Vector a;
			Vector b;

			if (pDrawing.WorldToScreen(angs, a) && pDrawing.WorldToScreen(vFinal, b))
			{
				pDrawing.DrawLine(a.x, a.y, b.x, b.y, Color::Cyan());
			}
		}
	}

	if (pEntity->GetCond() & TFCond_Zoomed)
	{
		pDrawing.DrawString(x + w + 2, y + iY, Color::Cyan(), "*ZOOMING*");
		iY += pDrawing.GetESPHeight();
	}

	if (pEntity->GetCond() & TFCond_Cloaked)
	{
		pDrawing.DrawString(x + w + 2, y + iY, Color::Cyan(), "*CLOAKED*");
		iY += pDrawing.GetESPHeight();

		pEntity->SetCond(pEntity->GetCond() & ~TFCond_Cloaked);
	}

	if (pEntity->GetCond() & TFCond_Cloaked)
	{
		pDrawing.DrawString(x + w + 2, y + iY, Color::Cyan(), "*DISGUISED*");
		iY += pDrawing.GetESPHeight();

		pEntity->SetCond(pEntity->GetCond() & TFCond_Disguised);
	}

	if (g_Options.esp_box)
	{
		if (g_Options.esp_enemyonly)
		{
			pDrawing.OutlineRect(x - 1, y - 1, w + 2, h + 2, Color::Black());
			pDrawing.OutlineRect(x, y, w, h, Color::Grey());
			pDrawing.OutlineRect(x + 1, y + 1, w - 2, h - 2, Color::Black());
		}
		else
		{
			pDrawing.OutlineRect(x - 1, y - 1, w + 2, h + 2, Color::Black());
			pDrawing.OutlineRect(x, y, w, h, clrPlayerCol);
			pDrawing.OutlineRect(x + 1, y + 1, w - 2, h - 2, Color::Black());
		}
	}

	if (g_Options.esp_health == 2 || g_Options.esp_health == 3)
	{
		pDrawing.OutlineRect(x - 6, y - 1.5, 5, h + 1.9, Color::Black());
		pDrawing.DrawRect(x - 5, y + (h - (h / iMaxHp * iHp)) - 0.5, 3, (h / iMaxHp * iHp), Color::Green());
	}

	if (g_Options.esp_name)
	{
		pDrawing.DrawString(x - 2.43, y - 15, clrPlayerCol, pInfo.name);
		iY += pDrawing.GetESPHeight();
	}

	if (g_Options.esp_class)
	{
		pDrawing.DrawString(x + w + 2, y + iY, Color::Grey(), "%s", pEntity->szGetClass());
		iY += pDrawing.GetESPHeight();
	}

	if (g_Options.esp_health == 1 || g_Options.esp_health == 3)
	{
		pDrawing.DrawString(x + w + 2, y + iY, Color::Green(), "Health:	%i", pEntity->GetHealth());
		iY += pDrawing.GetESPHeight();
	}

	if (g_Options.esp_bones) //bones
	{
		static int iLeftArmBones[] = { 8, 7, 6, 4 };
		static int iRightArmBones[] = { 11, 10, 9, 4 };
		static int iHeadBones[] = { 0, 4, 1 };
		static int iLeftLegBones[] = { 14, 13, 1 };
		static int iRightLegBones[] = { 17, 16, 1 };

		DrawBone(pEntity, iLeftArmBones, 4, clrBoneCol);
		DrawBone(pEntity, iRightArmBones, 4, clrBoneCol);

		DrawBone(pEntity, iHeadBones, 3, clrBoneCol);

		DrawBone(pEntity, iLeftLegBones, 3, clrBoneCol);
		DrawBone(pEntity, iRightLegBones, 3, clrBoneCol);
	}
}
	 //My code, but creds to f1ssion for giving me the idea
void CESP::DrawBone(C_Entity* pEntity, int* iBones, int count, Color clrCol)
{
	for (int i = 0; i < count; i++)
	{
		if (i == count - 1)
			continue;

		Vector vBone1 = (Vector&)pEntity->GetHitboxPosition(iBones[i]);
		Vector vBone2 = (Vector&)pEntity->GetHitboxPosition(iBones[i + 1]);

		Vector vScr1, vScr2;

		if (!pDrawing.WorldToScreen(vBone1, vScr1) || !pDrawing.WorldToScreen(vBone2, vScr2))
			continue;

		pDrawing.DrawLine(vScr1.x, vScr1.y, vScr2.x, vScr2.y, clrCol);
	}
}