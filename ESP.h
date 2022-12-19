#pragma once
#include "SDK.h"

class CESP
{
public:

	void Run(C_Entity* pLocal);

private:

	void Player_ESP(C_Entity* pLocal, C_Entity* pEntity);

	//Other funcs

	void DrawBone(C_Entity* pEntity, int* iBones, int count, Color clrCol);
};

extern CESP gESP; 