#include "SDK.h"

class g
{
public:
	static C_Entity* local;

	static C_UserCmd* cmd;

	static bool sendpackets;

	static QAngle fake_angles;

	static QAngle real_angles;

	static float* third_pitch;
	
	static float* third_yaw;

	static void init_createmove(C_Entity* loc, C_UserCmd* cmd_t, bool& bsp, QAngle rl, QAngle fk);

	static void init_framestagenotify(float* tp, float* ty);
};