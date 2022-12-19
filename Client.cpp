#include "SDK.h"
#include "Client.h"
#include "Util.h"
#include "Aimbot.h"
#include "Triggerbot.h"
#include "Misc.h"
#include "C_BasePredictionSystem.h"
#include "CAntiAims.h"
#include "Context.h"
#include <intrin.h>
#include "CDrawManager.h"
#include <d3d9.h>
#include "RS.h"
#include "G.hpp"
#include "Glow.h"
#include "DXSDK.h"

#define _MAXPLAYC pContext.g_Engine->GetMaxClients()

static QAngle local_data = QAngle(0, 0, 0);
static QAngle fake_data = QAngle(0, 0, 0);
static BOOL scoped = FALSE;

//============================================================================================
bool __fastcall Hooked_CreateMove(PVOID ClientMode, int edx, float input_sample_frametime, C_UserCmd* pCommand)
{
	VMTManager& hook = VMTManager::GetHook(ClientMode); //Get a pointer to the instance of your VMTManager with the function GetHook.
	bool bReturn = hook.GetMethod<bool(__thiscall*)(PVOID, float, C_UserCmd*)>(gOffsets.iCreateMoveOffset)(ClientMode, input_sample_frametime, pCommand);

	try
	{
		if (!pCommand->command_number)
			return false;

		auto base = reinterpret_cast<uintptr_t>(_AddressOfReturnAddress()) - sizeof(uintptr_t);
		bool& bSendPacket = *(***reinterpret_cast<bool****>(base) - 1);

		C_Entity* pLocal = GetBaseEntity(me);
		pContext.cmd = pCommand;

		if (!pLocal)
			return bReturn;

		float oc = Iface.Globals->curtime;
		float of = Iface.Globals->frametime;

		static ConVar* m_pCheats = Iface.cvar->FindVar("sv_cheats");

		auto ctx = pContext;
		auto engine = ctx.g_Engine;
		auto entlist = ctx.g_Ent;
		auto cvar = ctx.g_Cvar;

		pContext.loc = pLocal;

		if (g_Options.misc_thirdperson)
		{
			if (m_pCheats->GetInt() != 1)
				m_pCheats->SetValue(1);

			engine->ClientCmd_Unrestricted("thirdperson");
		}
		else
		{
			engine->ClientCmd_Unrestricted("firstperson");
		}

		g_pBasePrediction.start(pLocal, pCommand);

		gMisc.Run(pLocal, pCommand);
		gAim.Run(pLocal, pCommand);
		gTrigger.Run(pLocal, pCommand);

		pRs.cmove();

		if(g_Options.misc_fakelag)
		{
			static int ticks = 0;

			if (ticks < 14)
			{
				bSendPacket = false;
				++ticks;
			}
			else
			{
				bSendPacket = true;
				ticks = 0;
			}
		}

		pAntiAims.run(bSendPacket);

			if(bSendPacket)
				fake_data = pCommand->viewangles;

			if (!(bSendPacket))
				local_data = pCommand->viewangles;

			g::init_createmove(pLocal, pCommand, bSendPacket, local_data, fake_data);

		g_pBasePrediction.enddh(pLocal, pCommand, oc, of);
	}



	catch(...)
	{
		Log::Fatal("Failed Hooked_CreateMove");
	}
	return false/*bReturn*/;
}
//============================================================================================
int __fastcall Hooked_KeyEvent(PVOID CHLClient, int edx, int eventcode, int keynum, const char *currentBinding)
{
	if (eventcode == 1)
	{
	}

	VMTManager &hook = VMTManager::GetHook(CHLClient); // Get a pointer to the instance of your VMTManager with the function GetHook.
	return hook.GetMethod<int(__thiscall *)(PVOID, int, int, const char *)>(gOffsets.iKeyEventOffset)(CHLClient, eventcode, keynum, currentBinding); // Call the original.
}

void __fastcall Hooked_FSN(PVOID CHLClient, void *_this, ClientFrameStage_t Stage)
{
	VMTManager &hook = VMTManager::GetHook(CHLClient);

	C_Entity* local = Iface.EntList->GetClientEntity(Iface.Engine->GetLocalPlayer());

	static QAngle old[33];

	if (Stage == FRAME_NET_UPDATE_START)
	{
		if(pContext.cmd)
			pContext.first = pContext.cmd->viewangles.pitch;
	}

	if (Stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
	{
		if (g_Options.misc_resolver == 1)
		{
			pRs.fsn();
		}

		if (g_Options.misc_resolver == 2)
		{
			for (int i = 0; i <= Iface.Engine->GetMaxClients(); i++)
			{
				C_Entity* cur = Iface.EntList->GetClientEntity(i);

				if (!cur || cur->IsDormant() || cur->GetLifeState() != LIFE_ALIVE)
					continue;

				auto pitch = cur->GetNetPtr_t<float*>("DT_TFPlayer", "tfnonlocaldata", "m_angEyeAngles[0]");
				auto yaw = cur->GetNetPtr_t<float*>("DT_TFPlayer", "tfnonlocaldata", "m_angEyeAngles[1]");

				static bool ang = false;

				if (GetAsyncKeyState(VK_MBUTTON) & 0x1)
				{
					ang = !ang;
				}

				if (ang)
				{
					*pitch = -89;
				}
				else
				{
					*pitch = 89;
				}
			}
		}
	}


	if (Stage == FRAME_RENDER_START)
	{
		if (!local)
			return hook.GetMethod<void(__fastcall *)(PVOID, void *, ClientFrameStage_t)>(35)(CHLClient, _this, Stage);

		pGlow_t.fsn();
		pContext.m_xPtr = reinterpret_cast<float*>(reinterpret_cast<DWORD>(local) + gNetVars.get_offset("DT_BasePlayer", "pl", "deadflag") + 4);
		pContext.m_yPtr = reinterpret_cast<float*>(reinterpret_cast<DWORD>(local) + gNetVars.get_offset("DT_BasePlayer", "pl", "deadflag") + 8);

		pContext.m_fov = reinterpret_cast<int*>(reinterpret_cast<DWORD>(local) + gNetVars.get_offset("DT_BasePlayer", "m_iFOV"));

		if (g_Options.misc_thirdperson)
		{
			if (g_Options.reals)
			{
				*pContext.m_xPtr = local_data.pitch;
				*pContext.m_yPtr = local_data.yaw;
			}
			
			if (g_Options.fakhs)
			{
				*pContext.m_xPtr = fake_data.pitch;
				*pContext.m_yPtr = fake_data.yaw;
			}
		}

		g::init_framestagenotify(pContext.m_xPtr, pContext.m_yPtr);

		pContext.hfak1s = fake_data;

		QAngle angles = fake_data;
		Vector forward;
		AngleVectors((Vector&)angles, &forward);
		Vector eyepos = (Vector&)local->GetEyePosition();
		forward = forward * 98 + eyepos;
		Vector screenForward, screenEyepos;
		if (pDrawing.WorldToScreen(eyepos, screenEyepos) && pDrawing.WorldToScreen(forward, screenForward))
		{
			pDrawing.DrawLine(screenForward.x, screenForward.y, screenEyepos.x, screenEyepos.y, Color::Cyan());
		}
	}

	if (Stage == FRAME_RENDER_END)
	{
		if (!local)
			return hook.GetMethod<void(__fastcall *)(PVOID, void *, ClientFrameStage_t)>(35)(CHLClient, _this, Stage);

		if (pContext.cmd)
			pContext.second = pContext.cmd->viewangles.pitch;

		if (g_Options.misc_thirdperson && g_Options.acc_multicore)
		{
			if (g_Options.misc_thirdperson)
			{
				if (g_Options.reals)
				{
					*pContext.m_xPtr = local_data.pitch;
					*pContext.m_yPtr = local_data.yaw;
				}
			}
		}
	}

	if (Stage == FRAME_NET_UPDATE_START)
	{
		if (!local)
			return hook.GetMethod<void(__fastcall *)(PVOID, void *, ClientFrameStage_t)>(35)(CHLClient, _this, Stage);

//		pContext.second = pContext.cmd->viewangles.pitch;

		if (g_Options.misc_thirdperson && g_Options.acc_multicore)
		{
			*pContext.m_xPtr = local_data.pitch;
			*pContext.m_yPtr = local_data.yaw;
		}
	}

	if (pContext.g_Engine->IsInGame() && pContext.g_Engine->IsConnected() && local && local->GetLifeState() == LIFE_ALIVE && Stage == FRAME_RENDER_START)
	{
		tf_cond pCond = (tf_cond)local->GetCond();

		if (pCond & TFCond_Zoomed)
		{
			scoped = TRUE;
			local->SetCond(pCond & ~TFCond_Zoomed);
		}
		else
		{
			scoped = FALSE;
		}
	}

	hook.GetMethod<void(__fastcall *)(PVOID, void *, ClientFrameStage_t)>(35)(CHLClient, _this, Stage);
}

bool __fastcall Hooked_NetMsg(PVOID CHLClient, INetChannel* pNetChan, void* edx, INetMessage& msg, bool bForceReliable, bool bVoice)
{
	VMTManager &hook = VMTManager::GetHook(CHLClient);
	return hook.GetMethod<bool(__fastcall *)(PVOID, INetChannel*, void*, INetMessage&, bool, bool)>(35)(CHLClient, pNetChan, edx, msg, bForceReliable, bVoice);
}

typedef void(__thiscall *pOverrideView) (void*, C_SetupV*);

void __fastcall Hooked_OverrideView(void* _this, void* _edx, C_SetupV* pSetup)
{
	//Setup;
	C_Entity* m_pLocalPlayer = GetBaseEntity(me);

	//Checks;
	if (!m_pLocalPlayer)
		return;

	static float fov_original = pSetup->m_fov;

	if (g_Options.acc_no_zoom && scoped == TRUE)
		pSetup->m_fov = fov_original;

	if (g_Options.acc_custom_fov)
	{
		if (scoped == TRUE)
		{
			pSetup->m_fov == g_Options.acc_zoom_fov;
		}
		else
		{
			pSetup->m_fov == g_Options.acc_nzoo_fov;
		}
	}

	VMTManager& hook = VMTManager::GetHook(_this);
	hook.GetMethod<pOverrideView>(16)(_this, pSetup);
}

void __stdcall hkReset(PVOID ptr, IDirect3DDevice9* thisptr, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	VMTManager& hook = VMTManager::GetHook(ptr);
	return hook.GetMethod<void(__stdcall *)(IDirect3DDevice9*, D3DPRESENT_PARAMETERS*)>(16)(thisptr, pPresentationParameters);
}