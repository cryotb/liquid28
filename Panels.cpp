#include "SDK.h"
#include "Panels.h"
#include "CDrawManager.h"
#include "Util.h"
#include "ESP.h"
#include "C_CheatEvents.h"
#include <string>
#include <d3d9.h>

CScreenSize gScreenSize;
//===================================================================================
void __fastcall Hooked_PaintTraverse( PVOID pPanels, int edx, unsigned int vguiPanel, bool forceRepaint, bool allowForce )
{
	try
	{
		if (!strcmp("HudScope", Iface.Panels->GetName(vguiPanel)) && g_Options.acc_no_scope)
			return;

		VMTManager& hook = VMTManager::GetHook(pPanels);
		hook.GetMethod<void(__thiscall*)(PVOID, unsigned int, bool, bool)>(gOffsets.iPaintTraverseOffset)(pPanels, vguiPanel, forceRepaint, allowForce); //Call the original.

		static unsigned int vguiFocusOverlayPanel;

		if (vguiFocusOverlayPanel == NULL)
		{											//FocusOverlayPanel
			const char* szName = Iface.Panels->GetName(vguiPanel);
			if (szName[0] == 'F' && szName[5] == 'O' &&  szName[12] == 'P')
			{
				vguiFocusOverlayPanel = vguiPanel;
				Intro();
			}
		}

		if (vguiFocusOverlayPanel == vguiPanel)
		{
			int iWidth, iHeight; //Resolution fix, so this can work in Fullscreen
			Iface.Engine->GetScreenSize(iWidth, iHeight);
			if (gScreenSize.iScreenWidth != iWidth || gScreenSize.iScreenHeight != iHeight)
				Iface.Engine->GetScreenSize(gScreenSize.iScreenWidth, gScreenSize.iScreenHeight);

			if (Iface.Engine->IsDrawingLoadingImage() || !Iface.Engine->IsInGame() || !Iface.Engine->IsConnected() || Iface.Engine->Con_IsVisible())
			{
				int i = 43;

				static float init = 0.5;
				static int pI = 5;

				if (init < 383.0f)
				{
					i += 15;

					string s = "+++ Citadels.win +++";

					if (init < 183)
					{
						++pI;
					}
					else if (init > 0)
					{
						--pI;
					}

					pDrawing.DrawString(pI, 5, Color::Red(), "				%s", s.c_str());

					init += 0.09;
				}
				else
				{
					init = 0;
				}

				return; //No need to draw the rest.
			}

			C_Entity* pLocal = Iface.EntList->GetClientEntity(me);

			if (!pLocal)
				return;

			pMenu.execute();

			gESP.Run(pLocal);

			//Debug;
		}
	}
	catch (...)
	{
		Log::Fatal("Failed PaintTraverse");
	}
}
//===================================================================================
void Intro( void )
{
	try
	{
		pDrawing.Initialize(); //Initalize the drawing class.
		pDrawing_t.Initialize();

		gNetVars.Initialize();

		HWND tf2 = FindWindow(NULL, "Valve001");
	}
	catch(...)
	{
		Log::Fatal("Failed Intro");
	}
}