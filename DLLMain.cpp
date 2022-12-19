#include "SDK.h"
#include "Client.h"
#include "Panels.h"
#include "Pred.h"
#include "C_BasePredictionSystem.h"
#include "Context.h"
#include "ConfigSystem.h"
#include "Util.h"
#include <d3d9.h>
#include "RS.h"
#include "Glow.h"
#include "imgui.h"
#include "DX.h"
#include "D3D.hpp"
#include "imgui_internal.h"
#include "DXSDK.h"
#include "CDrawManager.h"

C_DXSDK g_D3dSdk;
COffsets gOffsets;
CGlobalVariables g_Options;
CInterfaces Iface;

CreateInterface_t EngineFactory = NULL;
CreateInterface_t ClientFactory = NULL;
CreateInterface_t VGUIFactory = NULL;
CreateInterface_t VGUI2Factory = NULL;
CreateInterface_t VstdFactory = NULL;

std::unique_ptr<VMTManager> g_pD3DDevice;

uint32_t dxDevice;

using EndSceneFn = long(__stdcall*)(IDirect3DDevice9* device);

EndSceneFn oEndSceneFn;

HRESULT __stdcall hkEndScene_t(IDirect3DDevice9* pDevice)
{
	g_D3DHooks.EndScene(pDevice);
	return g_pD3DDevice->GetMethod<EndSceneFn>(42)(pDevice);
}

DWORD WINAPI dwMainThread( LPVOID lpArguments )
{
	//Config;
	//HTCCNKBRKYLC;
	pConfigSystemHb.rel_cfgh();

	if (Iface.Client == NULL)
	{
		DWORD ShaderBaseClass = **(DWORD **)(Util->FindSignatures(("shaderapidx9.dll"), ("A1 ?? ?? ?? ?? 50 8B 08 FF 51 0C")) + 1);

		VMTBaseManager* clientHook = new VMTBaseManager();
		VMTBaseManager* clientModeHook = new VMTBaseManager();
		VMTBaseManager* panelHook = new VMTBaseManager();
		VMTBaseManager* pVmt = new VMTBaseManager();

		ClientFactory = ( CreateInterfaceFn ) GetProcAddress( gSignatures.GetModuleHandleSafe( "client.dll" ), "CreateInterface" );
		EngineFactory = (CreateInterfaceFn)GetProcAddress(gSignatures.GetModuleHandleSafe("engine.dll"), "CreateInterface");
		VGUIFactory = (CreateInterfaceFn)GetProcAddress(gSignatures.GetModuleHandleSafe("vguimatsurface.dll"), "CreateInterface");
		VstdFactory = (CreateInterfaceFn)GetProcAddress(gSignatures.GetModuleHandleSafe("vstdlib.dll"), "CreateInterface");

		Iface.Client = ( CHLClient* )ClientFactory( "VClient017", NULL);
		Iface.EntList = ( CEntList* ) ClientFactory( "VClientEntityList003", NULL );
		Iface.Engine = ( EngineClient* ) EngineFactory( "VEngineClient013", NULL );
		Iface.Surface = ( ISurface* ) VGUIFactory( "VGUI_Surface030", NULL );
		Iface.EngineTrace = ( IEngineTrace* ) EngineFactory( "EngineTraceClient003", NULL );
		Iface.ModelInfo = ( IVModelInfo* ) EngineFactory( "VModelInfoClient006", NULL );
		Iface.Prediction = ( CPrediction* ) ClientFactory( "VClientPrediction001", NULL);
		Iface.Globals = *reinterpret_cast<CGlobals **>(gSignatures.GetEngineSignature("A1 ? ? ? ? 8B 11 68") + 8);
		Iface.cvar = (ICvar*)VstdFactory("VEngineCvar004", NULL);
		Iface.GameEvents = (IGameEventManager2*)EngineFactory("GAMEEVENTSMANAGER002", NULL);
		Iface.Helper = reinterpret_cast<IMoveHelper *>(gSignatures.GetEngineSignature("56 8B F1 8B 06 8B 80 ? ? ? ? FF D0 84 C0 75 07") + 6);

		pContext.g_Cvar = (ICvar*)VstdFactory("VEngineCvar004", NULL);
		pContext.g_Engine = (EngineClient*)EngineFactory("VEngineClient013", NULL);
		pContext.g_Ent = (CEntList*)ClientFactory("VClientEntityList003", NULL);
		pContext.g_Globals = *reinterpret_cast<CGlobals **>(gSignatures.GetEngineSignature("A1 ? ? ? ? 8B 11 68") + 8);

		pGlow_t.init();

		dxDevice = **(uint32_t**)(Util->FindSignatures("shaderapidx9.dll", "A1 ? ? ? ? 50 8B 08 FF 51 0C") + 1);

		g_pD3DDevice = std::make_unique<VMTManager>((PDWORD*)dxDevice);

		//toolkit::VMTBaseHook* pD3dHook = new toolkit::VMTHook((DWORD*)pD3ddevice);

		XASSERT(Iface.Client);
		XASSERT(Iface.EntList);
		XASSERT(Iface.Engine);
		XASSERT(Iface.Surface);
		XASSERT(Iface.EngineTrace);
		XASSERT(Iface.ModelInfo);

		AllocConsole();
		freopen("CONOUT$", "w", stdout);

		printf("\n%lu 0x", dxDevice);
		printf("\n	[Hook]:		%lu", pVmt);

		//Setup the Panel hook so we can draw.
		if( !Iface.Panels )
		{
			VGUI2Factory = (CreateInterfaceFn)GetProcAddress(gSignatures.GetModuleHandleSafe("vgui2.dll"), "CreateInterface");
			Iface.Panels = ( IPanel* ) VGUI2Factory( "VGUI_Panel009", NULL );
			XASSERT( Iface.Panels );

			if( Iface.Panels )
			{
				panelHook->Init(Iface.Panels);
				panelHook->HookMethod(&Hooked_PaintTraverse, gOffsets.iPaintTraverseOffset);
				panelHook->Rehook();
			}
		}

		DWORD dwClientModeAddress = gSignatures.GetClientSignature("8B 0D ? ? ? ? 8B 02 D9 05");
		XASSERT(dwClientModeAddress);
		Iface.ClientMode = **(ClientModeShared***)(dwClientModeAddress + 2);
		LOGDEBUG("g_pClientModeShared_ptr client.dll+0x%X", (DWORD)Iface.ClientMode - dwClientBase);
				
		clientHook->Init(Iface.Client);
		clientHook->HookMethod(&Hooked_KeyEvent, gOffsets.iKeyEventOffset);
		clientHook->HookMethod(&Hooked_FSN, 35);
		clientHook->Rehook();

		int index = gSignatures.GetClientSignature("66 C7 86 ? ? ? ? ? ? C6 86 ? ? ? ? ? 5E 5B");

		printf("\n%i", index);

		clientModeHook->Init(Iface.ClientMode);
		clientModeHook->HookMethod(&Hooked_CreateMove, gOffsets.iCreateMoveOffset); //ClientMode create move is called inside of CHLClient::CreateMove, and thus no need for hooking WriteUserCmdDelta.
		clientModeHook->HookMethod(&Hooked_OverrideView, 16);
		clientModeHook->Rehook();

		g_pD3DDevice->HookMethod(reinterpret_cast<void*>(hkEndScene_t), 42);
		
		pRs.run();
		/*		
		pD3dHook->HookMethod(Hooked_Sc19, 42);
		pD3dHook->Rehook();*/
	}
	return 0; //The thread has been completed, and we do not need to call anything once we're done. The call to Hooked_PaintTraverse is now our main thread.
}

BOOL APIENTRY DllMain(HMODULE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if(dwReason == DLL_PROCESS_ATTACH)
	{
		Log::Init(hInstance);
		CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)dwMainThread, NULL, 0, NULL ); 
	}
	return true;
}