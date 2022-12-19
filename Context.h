#pragma once
#include "SDK.h"
class Context
{
public:
	Context();
	~Context();

	C_UserCmd* cmd;
	C_Entity* loc;
	EngineClient* g_Engine;
	ICvar* g_Cvar;
	CEntList* g_Ent;
	CGlobals* g_Globals;
	float *m_xPtr;
	float *m_yPtr;
	int *m_fov;
	QAngle hfak1s;

	float first;
	float second;
};

extern Context pContext;