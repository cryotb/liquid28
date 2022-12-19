#include "SDK.h"

class CBaseHooking008
{
public:
	VMTBaseManager* CHLClient;
	VMTBaseManager* ClientMod;

	HMODULE h;

	void un();
};

extern CBaseHooking008 g_Hooking;