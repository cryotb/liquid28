#include "SDK.h"
#include "Util.h"

int C_Entity::GetHealth() 
{
	DYNVAR_RETURN(int, this, "DT_BasePlayer", "m_iHealth");
}

int C_Entity::GetTeamNum()
{
	DYNVAR_RETURN(int, this, "DT_BaseEntity", "m_iTeamNum");
}

int C_Entity::GetFlags()
{
	DYNVAR_RETURN(int, this, "DT_BasePlayer", "m_fFlags");
}

BYTE C_Entity::GetLifeState()
{
	DYNVAR_RETURN(BYTE, this, "DT_BasePlayer", "m_lifeState");
}

int C_Entity::GetClassNum()
{
	DYNVAR_RETURN(int, this, "DT_TFPlayer", "m_PlayerClass", "m_iClass");
}

char* C_Entity::szGetClass()
{
	DYNVAR(iClass, int, "DT_TFPlayer", "m_PlayerClass", "m_iClass");

	switch (iClass.GetValue(this))
	{
	case TF2_Scout:
		return "Scout";
	case TF2_Soldier:
		return "Soldier";
	case TF2_Pyro:
		return "Pyro";
	case TF2_Demoman:
		return "Demoman";
	case TF2_Heavy:
		return "Heavy";
	case TF2_Engineer:
		return "Engineer";
	case TF2_Medic:
		return "Medic";
	case TF2_Sniper:
		return "Sniper";
	case TF2_Spy:
		return "Spy";
	default:
		return "Unknown class";
	}

	return "Unknown class"; //Just in case
}

int C_Entity::GetCond()
{
	DYNVAR_RETURN(int, this, "DT_TFPlayer", "m_Shared", "m_nPlayerCond");
}

CBaseCombatWeapon* C_Entity::GetActiveWeapon()
{
	DYNVAR(pHandle, DWORD, "DT_BaseCombatCharacter", "m_hActiveWeapon");
	return (CBaseCombatWeapon *)Iface.EntList->GetClientEntityFromHandle(pHandle.GetValue(this));
}

QAngle C_Entity::GetCollideableMins()
{
	DYNVAR_RETURN(QAngle, this, "DT_BaseEntity", "m_Collision", "m_vecMins");
}

QAngle C_Entity::GetCollideableMaxs()
{
	DYNVAR_RETURN(QAngle, this, "DT_BaseEntity", "m_Collision", "m_vecMaxs");
}

QAngle C_Entity::GetEyePosition()
{
	DYNVAR_RETURN(QAngle, this, "DT_BasePlayer", "localdata", "m_vecViewOffset[0]") + (QAngle&)this->GetAbsOrigin();
}

Vector C_Entity::GetEyePosition_t()
{
	DYNVAR_RETURN(Vector, this, "DT_BasePlayer", "localdata", "m_vecViewOffset[0]") + this->GetAbsOrigin();
}

QAngle C_Entity::GetAbsEyePosition()
{
	DYNVAR_RETURN(QAngle, this, "DT_BasePlayer", "localdata", "m_vecViewOffset[0]");
}

QAngle C_Entity::GetHitboxPosition(int iHitbox)
{
	DWORD *model = this->GetModel();
	if (!model)
		return QAngle();

	studiohdr_t *hdr = Iface.ModelInfo->GetStudiomodel(model);
	if (!hdr)
		return QAngle();

	matrix3x4 matrix[128];
	if (!this->SetupBones(matrix, 128, 0x100, 0))
		return QAngle();

	int HitboxSetIndex = *(int *)((DWORD)hdr + 0xB0);
	if (!HitboxSetIndex)
		return QAngle();

	mstudiohitboxset_t *pSet = (mstudiohitboxset_t *)(((PBYTE)hdr) + HitboxSetIndex);

	mstudiobbox_t* box = pSet->pHitbox(iHitbox);
	if (!box)
		return QAngle();

	Vector vCenter = (box->bbmin + box->bbmax) * 0.5f;

	Vector vHitbox;

	Util->VectorTransform(vCenter, matrix[box->bone], vHitbox);

	return (QAngle&)vHitbox;
}

int C_Entity::GetTickBase()
{
	DYNVAR_RETURN(int, this, "DT_BasePlayer", "localdata", "m_nTickBase");
}

float C_Entity::GetNext()
{
	DYNVAR_RETURN(float, this, "DT_BaseCombatWeapon", "LocalActiveWeaponData", "m_flNextPrimaryAttack");
}

float C_Entity::GetPitch()
{
	DYNVAR_RETURN(float, this, "DT_TFPlayer", "tfnonlocaldata", "m_angEyeAngles[0]");
}