#include "G.hpp"

C_UserCmd* g::cmd;

C_Entity* g::local;

QAngle g::fake_angles;

QAngle g::real_angles;

bool g::sendpackets;

float* g::third_pitch;

float* g::third_yaw;

void g::init_createmove(C_Entity* loc, C_UserCmd* cmd_t, bool& bsp, QAngle rl, QAngle fk)
{
	sendpackets = &bsp;

	real_angles = rl;
	fake_angles = fk;

	local = loc;

	cmd = cmd_t;
}

void g::init_framestagenotify(float* tp, float* ty)
{
	third_pitch = tp;
	third_yaw = ty;
}