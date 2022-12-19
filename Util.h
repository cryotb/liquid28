#pragma once
#include "SDK.h"

class CUtil
{
public:
	void VectorTransform(const Vector& vSome, const matrix3x4& vMatrix, Vector& vOut);

	bool IsVisible(void* pLocal, void* pEntity, QAngle vStart, QAngle vEnd);

	bool IsKeyPressed(int i);

	bool IsHeadshotWeapon(C_Entity* pLocal, CBaseCombatWeapon* pWep);

	PVOID InitKeyValue();

	DWORD FindSignatures(const char* szModuleName, char* szPattern);
};

extern CUtil* Util;