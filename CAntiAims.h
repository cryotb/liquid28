#include "SDK.h"
#include "Context.h"
#include "Aimbot.h"

class C_BaseContext
{
public:
	struct CBaseAAContext
	{
	public:
		float_t p;
		float_t y;

		C_UserCmd* cmd;
		C_Entity* local;
	};
};

class C_AntiAims
{
public:
	void run(bool &packet);
	void resolver();

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

extern C_AntiAims pAntiAims;