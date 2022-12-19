#include "CAntiAims.h"
#include "G.hpp"
#include "Util.h"

BOOL Visible()
{
	for (int i = 0; i <= Iface.Engine->GetMaxClients(); i++)
	{
		C_Entity* pEntity = GetBaseEntity(i);

		if (!pEntity || pEntity->GetLifeState() != LIFE_ALIVE || i == me)
			continue;

		if (Util->IsVisible(GetBaseEntity(me), pEntity, GetBaseEntity(me)->GetEyePosition(), pEntity->GetHitboxPosition(0)))
			return true;
	}

	return false;
}

C_AntiAims pAntiAims;

void C_AntiAims::run(bool &packet)
{
	auto ctx = pContext;
	auto engine = ctx.g_Engine;
	auto entlist = ctx.g_Ent;
	auto cvar = ctx.g_Cvar;
	auto cmd = ctx.cmd;
	auto old1 = cmd->viewangles;
	auto old2 = cmd->forwardmove;
	auto old3 = cmd->sidemove;

	if (g_Options.misc_caa > 0 && (!(cmd->buttons & IN_ATTACK)))
	{
		float p = g_Options.misc_paa;
		float y = g_Options.misc_yaa;
		auto &x = cmd->viewangles.pitch;
		auto &y1 = cmd->viewangles.yaw;

		if (p == 1)
		{
			//Up;
			x = -89;
		}

		if (p == 2)
		{
			//Down;
			x = 89;
		}

		if (p == 3)
		{
			x = -271;
		}

		if (p == 4)
		{
			x = 271;
		}

		if (p == 5)
		{
			static bool choke = false;

			if (choke)
			{
				packet = true;
				x = -89;
			}
			else
			{
				packet = false;
				x = 89;
			}

			choke = !choke;
		}

		if (p == 6)
		{
			static bool choke = false;

			if (choke)
			{
				packet = true;
				x = +89;
			}
			else
			{
				packet = false;
				x = -89;
			}

			choke = !choke;
		}

		if (p == 7)
		{
			static bool choke = false;
			static bool choks = false;

			if (choke)
			{
				packet = false;
				x = -89;
			}
			else
			{
				packet = true;

				choks = !choks;

				if (choks)
				{
					x = -89;
				}
				else
				{
					x = +89;
				}
			}

			choke = !choke;
		}

		if (p == 8)
		{
			static float dur = 0.f;

			x = -89;

			if (dur < 4)
			{
				packet = false;
				x = -50;
				dur += 0.4;
			}
			else if (dur < 8)
			{
				packet = true;
				x = 89;
				dur += 0.4;
			}
			else
			{
				dur = 0;
			}
		}
		
		if (y == 1)
		{
			y1 += 90;
		}

		if (y == 2)
		{
			y1 -= 90;
		}

		if (y == 3)
		{
			static bool choke = false;

			if (choke)
			{
				packet = true;
				y1 += 90;
			}
			else
			{
				packet = false;
				y1 += -90;
			}

			choke = !choke;
		}

		if (y == 4)
		{
			static bool choke = false;

			if (choke)
			{
				packet = true;
				y1 += -90;
			}
			else
			{
				packet = false;
				y1 += 90;
			}

			choke = !choke;
		}

		if (y == 5)
		{
			y1 = cmd->tick_count * 10 % 360;
		}

		if (y == 7)
		{
			y1 = cmd->tick_count * 3 % 360;
		}

		if (y == 5)
		{
			y1 = cmd->tick_count * 180 % 360;
		}

		if (y == 8)
		{
			static float f = 0.0f;
			static bool b = false;

			if (f < 4)
			{
				b = false;
				y1 += -90;
				f += 0.4;
			}
			else if (f < 8)
			{
				b = true;
				f += +0.7f;
			}
			else
			{
				f = 0.0f;
			}

			if (b)
			{
				static bool choke = false;

				if (choke)
				{
					packet = true;
					y1 += (y1 * 9);
				}
				else
				{
					packet = false;
					y1 += 90;
				}

				choke = !choke;
			}
		}

		if (y == 9)
		{
			float rotation = 90;
			static bool time = false;

			if (time)
				y1 = rotation + GetBaseEntity(me)->GetYawh();
			else
				y1 = rotation - GetBaseEntity(me)->GetYawh();

			time = !time;
		}

		if (y == 10)
		{
			static bool bSwitch = true;

			if (bSwitch)
			{
				y1 = 90;
			}
			else
			{
				y1 = -90;
			}

			bSwitch = !bSwitch;
		}

		if (y == 11)
		{
			static bool bSwitch = true;

			if (bSwitch)
			{
				y1 = -90;
			}
			else
			{
				y1 = 90;
			}

			bSwitch = !bSwitch;
		}

		if (y == 12)
		{
			static bool bSwitch = true;

			if (bSwitch)
			{
				y1 += g::local->GetYawh() * y1;
			}
			else
			{
				y1 -= g::local->GetYawh() * y1;
			}

			bSwitch = !bSwitch;
		}

		if (y == 13)
		{
			bool vis = Visible();

			static float time = 0.f;

			y1 = 0;

			if (time < 5.0f)
			{
				if (vis)
				{
					y1 -= 90;
				}
				else
				{
					y1 += 90;
				}

				time += 1.0f;
			}
			else if (time < 10.0f)
			{
				float ang = cos(g::real_angles.yaw + y1 + time);

				if (ang > 90) ang = 90;
				if (ang < -90) ang = -90;

				y1 += ang;

				g::sendpackets = time > 7.0f ? true : false;
			}
			else
			{
				g::sendpackets = true;
				time = 0.0f;
			}
		}

		normalize_sf(cmd, old1, old2, old3);
	}
}