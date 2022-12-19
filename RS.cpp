#include "Rs.h"
#include "Glow.h"

g_pEvents pRs;

int method[33];

void g_pEvents::run()
{
	Iface.GameEvents->AddListener(this, "player_hurt", true);
	Iface.GameEvents->AddListener(this, "player_death", false);
	Iface.GameEvents->AddListener(this, "player_spawn", false);
}

void g_pEvents::fsn()
{
	for (int i = 0; i <= Iface.Engine->GetMaxClients(); i++)
	{
		C_Entity* pBasFn = Iface.EntList->GetClientEntity(i);

		if (!pBasFn)
			continue;

		auto pReadOnlyAngles = pBasFn->GetPitch();
		auto pr = method[i];

		if (pBasFn->IsDormant() == false && pBasFn->GetLifeState() == LIFE_ALIVE)
		{
			auto pWriteOnlyPitch = reinterpret_cast<float*>(reinterpret_cast<DWORD>(pBasFn) + gNetVars.get_offset("DT_TFPlayer", "tfnonlocaldata", "m_angEyeAngles[0]"));
			auto pWriteOnlyYaw = reinterpret_cast<float*>(reinterpret_cast<DWORD>(pBasFn) + gNetVars.get_offset("DT_TFPlayer", "tfnonlocaldata", "m_angEyeAngles[1]"));

			if (g_Options.misc_resolver == 1)
			{
				//THIS
				if(pr == 0)
					*pWriteOnlyYaw = -90;
				else if(pr == 1)
					*pWriteOnlyYaw = 90;
				else
					*pWriteOnlyYaw = 0;
			}
		}
	}
}

void g_pEvents::FireGameEvent(IGameEvent* baseFn)
{
	Iface.cvar->ConsolePrintf("k");
	if (!strcmp(baseFn->GetName(), "player_death"))
	{
		int m_iKiller = Iface.Engine->GetPlayerForUserID(baseFn->GetInt("killer"));
		int m_iVictim = Iface.Engine->GetPlayerForUserID(baseFn->GetInt("userid"));

		if (m_iKiller = me)
			miss[m_iVictim] = false;
	}

	if (!strcmp(baseFn->GetName(), "player_spawn"))
	{
		Iface.cvar->ConsolePrintf("\nCONNECTED");

		pGlow_t.update = true;
	}
}

void g_pEvents::cmove()
{
	for (int i = 1; i <= Iface.Engine->GetMaxClients(); i++)
	{
		C_Entity* pBasFn = Iface.EntList->GetClientEntity(i);

		if (!pBasFn)
			continue;

		if (miss[i])
		{
			++method[i];

			miss[i] = false;
		}

		if (method[i] > 2)
			method[i] = 0;
	}
}