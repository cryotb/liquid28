#include "Hk.h"

CBaseHooking008 g_Hooking;

void CBaseHooking008::un()
{
	g_Hooking.ClientMod->Unhook();
	g_Hooking.CHLClient->Unhook();
	FreeLibraryAndExitThread(g_Hooking.h, 0x08C);
}