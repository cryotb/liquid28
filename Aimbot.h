#pragma once
#include "SDK.h"
#include "Context.h"

class CAimbot
{
public:

	void Run(C_Entity* pLocal, C_UserCmd* pCommand);

private:

	int GetBestTarget(C_Entity* pLocal);

	int GetBestHitbox(C_Entity* pLocal, C_Entity* pEntity);

	inline static void normalize_sf(C_UserCmd* pCmd, QAngle m_vOldAngles, float m_fOldForward, float m_fOldSidemove)
	{
		float deltaView = pCmd->viewangles.pitch - m_vOldAngles.yaw;
		float f1;
		float f2;

		if (m_vOldAngles.yaw < 0.f)
			f1 = 360.0f + m_vOldAngles.yaw;
		else
			f1 = m_vOldAngles.yaw;

		if (pCmd->viewangles.yaw < 0.0f)
			f2 = 360.0f + pCmd->viewangles.yaw;
		else
			f2 = pCmd->viewangles.yaw;

		if (f2 < f1)
			deltaView = abs(f2 - f1);
		else
			deltaView = 360.0f - abs(f1 - f2);
		deltaView = 360.0f - deltaView;

		pCmd->forwardmove = cos(DEG2RAD(deltaView)) * m_fOldForward + cos(DEG2RAD(deltaView + 90.f)) * m_fOldSidemove;
		pCmd->sidemove = sin(DEG2RAD(deltaView)) * m_fOldForward + sin(DEG2RAD(deltaView + 90.f)) * m_fOldSidemove;
	}
};

extern CAimbot gAim; //dsajkhfdlksahf