#include "Misc.h"
#include "Util.h"
#include "C_BasePredictionSystem.h"

CMisc gMisc;

void CMisc::Run(C_Entity* pLocal, C_UserCmd* pCommand)
{
	if (!(g_pPredData008.old_flags & FL_ONGROUND) && pCommand->buttons & IN_JUMP)
	{
		//Autostrafe	
		if (g_Options.misc_autostrafe)
			if (pCommand->mousedx > 1 || pCommand->mousedx < -1)  //> 1 < -1 so we have some wiggle room
				pCommand->sidemove = pCommand->mousedx > 1 ? 450.f : -450.f;

		//Bunnyhop
		if (g_Options.misc_bunnyhop)
			pCommand->buttons &= ~IN_JUMP;
	}

	if (g_Options.misc_noisemaker_spam)
	{
		PVOID kv = Util->InitKeyValue();
		if (kv != NULL)
		{
			NoisemakerSpam(kv);
			Iface.Engine->ServerCmdKeyValues(kv);
		}
	}

	static auto DwOffys = gSignatures.GetEngineSignature("E8 ? ? ? ? D9 45 F8 8D");

	typedef int(__cdecl* HashFunc)(int a1, const char* a2, ...);

	static HashFunc a4 = (HashFunc)(DwOffys + 1);

	if (GetAsyncKeyState(VK_RSHIFT))
	{
	}
}
		  //Could be much simpler, but I don't want keyvals class
void CMisc::NoisemakerSpam(PVOID kv) //Credits gir https://www.unknowncheats.me/forum/team-fortress-2-a/141108-infinite-noisemakers.html
{
	char chCommand[30] = "use_action_slot_item_server";
	typedef int(__cdecl* HashFunc_t)(const char*, bool);
	static DWORD dwHashFunctionLocation = gSignatures.GetClientSignature("FF 15 ? ? ? ? 83 C4 08 89 06 8B C6");
	static HashFunc_t s_pfGetSymbolForString = (HashFunc_t)**(PDWORD*)(dwHashFunctionLocation + 0x2);
	*(PDWORD)((DWORD)kv + 0x4) = 0;
	*(PDWORD)((DWORD)kv + 0x8) = 0;
	*(PDWORD)((DWORD)kv + 0xC) = 0;
	*(PDWORD)((DWORD)kv + 0x10) = /*0x10000*/0xDEADBEEF;
	*(PDWORD)((DWORD)kv + 0x14) = 0;
	*(PDWORD)((DWORD)kv + 0x18) = 0; //Extra one the game isn't doing, but if you don't zero this out, the game crashes.
	*(PDWORD)((DWORD)kv + 0x1C) = 0;
	*(PDWORD)((DWORD)kv + 0) = s_pfGetSymbolForString(chCommand, 1);
}