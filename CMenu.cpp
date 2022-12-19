#include "CMenu.h"
#include <iostream>
#include "CDrawManager.h"
#include "Context.h"
#include "Util.h"
#include "ConfigSystem.h"

#define offset 183

C_Menu pMenu;

void C_Menu::execute()
{
	static int menu_index = 0;   //				Menu Index.
	static int scrol_index = 0; //				Scroller Index.
	static bool menu_activ = 0;

	int vthis = 0; //							This Index of single menu entry.
	int vgui = 5; //							Form Index;

	if (pMenu.__key(VK_UP))
	{
		menu_index -= 1;
		scrol_index -= 15;
	}
	else if (pMenu.__key(VK_DOWN))
	{
		menu_index += 1;
		scrol_index += 15;
	}

	if (pMenu.__key(VK_INSERT))
	{
		menu_activ = !menu_activ;
	}

	player_info_t local_info;

	string local_nam = local_info.name;

	if (!Iface.Engine->GetPlayerInfo(me, &local_info))
		return;

	pDrawing.DrawString(389, vgui, Color::Red(), "Liquid");
	pDrawing.DrawString(389 + 80, vgui, Color::Red(), "Hello, Bux ;)");

	vgui += 15;
	vthis += 1;

	float dif = abs(pContext.first - pContext.second);
	pDrawing.DrawString(389, vgui, Color::Red(), "Dif: %f", dif);
	vgui += 15;
	vthis += 1;

	/*
	vgui += 15;

	static float ang = pContext.loc->GetPitch();

	float ang3 = pContext.loc->GetPitch();

	float dif = (ang - ang3);

	pDrawing.DrawString(383, vgui, Color::Red(), "Ang:	%f", ang3);

	vgui += 15;

	pDrawing.DrawString(383 + 98, vgui, Color::Red(), "Dif:	%f", dif);

	vgui += 15;*/

	if (menu_activ != true)
		return;

	pDrawing.DrawRect(383, scrol_index + 4, 183, 15, Color(255, 255, 255, 83));

	pMenu.CreateObjectFn3("Aimbot", g_Options.aimbot_switch, menu_index, vgui, scrol_index, vthis);

	if (g_Options.aimbot_switch)
	{
		{
			pMenu.CreateObjectFn4("Active", g_Options.aimbot_active, menu_index, vgui, scrol_index, vthis);
			pMenu.CreateObjectFn4("Autofire", g_Options.aimbot_autoshoot, menu_index, vgui, scrol_index, vthis);
			pMenu.CreateObjectFn4("Hitscan", g_Options.aimbot_hitscan, menu_index, vgui, scrol_index, vthis);
			pMenu.CreateObjectFn4("Silent", g_Options.aimbot_silent, menu_index, vgui, scrol_index, vthis);
		}
	}

	pMenu.CreateObjectFn3("Visuals", g_Options.esp_switch, menu_index, vgui, scrol_index, vthis);

	if (g_Options.esp_switch)
	{
		{
			pMenu.CreateObjectFn4("Active", g_Options.esp_active, menu_index, vgui, scrol_index, vthis);
			pMenu.CreateObjectFn4("Activ3 Debug", g_Options.esp_debug, menu_index, vgui, scrol_index, vthis);
			pMenu.CreateObjectFn4("Name", g_Options.esp_name, menu_index, vgui, scrol_index, vthis);
			pMenu.CreateObjectFn4("Class", g_Options.esp_class, menu_index, vgui, scrol_index, vthis);
			pMenu.CreateObjectFn4("Outline", g_Options.esp_box, menu_index, vgui, scrol_index, vthis);
			pMenu.CreateObjectFn4("View", g_Options.esp_sight, menu_index, vgui, scrol_index, vthis);
			pMenu.CreateObjectFn4("Local", g_Options.esp_local, menu_index, vgui, scrol_index, vthis);
			pMenu.CreateObjectFn4("Opponents", g_Options.esp_enemyonly, menu_index, vgui, scrol_index, vthis);
			pMenu.CreateObjectFn4("Bones", g_Options.esp_bones, menu_index, vgui, scrol_index, vthis);
			pMenu.CreateObjectFn5("Health", g_Options.esp_health, menu_index, vgui, scrol_index, vthis, 3);
		}
	}

	pMenu.CreateObjectFn3("Accuracy", g_Options.acc_switch, menu_index, vgui, scrol_index, vthis);

	if (g_Options.acc_switch)
	{
		{
			pMenu.CreateObjectFn4("Disable Scope Visuals", g_Options.acc_no_scope, menu_index, vgui, scrol_index, vthis);
			pMenu.CreateObjectFn4("Disable Scope FOV", g_Options.acc_no_zoom, menu_index, vgui, scrol_index, vthis);
			pMenu.CreateObjectFn4("Disable Multicore", g_Options.acc_multicore, menu_index, vgui, scrol_index, vthis);
			pMenu.CreateObjectFn4("Directx Lines (Local)", g_Options.d3d_lines, menu_index, vgui, scrol_index, vthis);
			pMenu.CreateObjectFn4("Allow Custom FOV", g_Options.acc_custom_fov, menu_index, vgui, scrol_index, vthis);

			if (g_Options.acc_custom_fov)
			{
				pMenu.CreateObjectFn5("Default FOV", g_Options.acc_nzoo_fov, menu_index, vgui, scrol_index, vthis, 198);
				pMenu.CreateObjectFn5("Scoped FOV", g_Options.acc_zoom_fov, menu_index, vgui, scrol_index, vthis, 198);
			}
		}
	}

	pMenu.CreateObjectFn3("Misc", g_Options.misc_switch, menu_index, vgui, scrol_index, vthis);

	if (g_Options.misc_switch)
	{
		//Pa;
		string pa1[33]{
			"OFF",  //0
			"Up",
			"Down",
			"Fake Up",
			"Fake Down",
			"Real Fakes Up",
			"Real Fakes Down",
			"Real Fakes Random",
		};

		//Ca;
		string ca1[33]
		{
			"OFF",  //0
			"Left", //0
			"Right",
			"bSendPacket III",
			"bSendPacket I",
			"Slow Spin",
			"Fast Spin",
			"Treehouse I80",
			"Fake Flip",
			"Side Flick Jitter",
			"Dynamic Sideways",
			"Fake Side I",
			"Fake Side II",
			"Fake Side III",
		};

		SIZE_T pa1_t = size(pa1);
		SIZE_T ca1_t = size(ca1);

		pMenu.CreateObjectFn4("Bunnyhop", g_Options.misc_bunnyhop, menu_index, vgui, scrol_index, vthis);
		pMenu.CreateObjectFn4("Straf3", g_Options.misc_autostrafe, menu_index, vgui, scrol_index, vthis);
		pMenu.CreateObjectFn4("Noisemaker", g_Options.misc_noisemaker_spam, menu_index, vgui, scrol_index, vthis);
		pMenu.CreateObjectFn5("Fake lag", g_Options.misc_fakelag, menu_index, vgui, scrol_index, vthis, 14);
		pMenu.CreateObjectFn4("Thirdperson", g_Options.misc_thirdperson, menu_index, vgui, scrol_index, vthis);

		if (g_Options.misc_thirdperson)
		{
			pMenu.CreateObjectFn4("Fakes", g_Options.fakhs, menu_index, vgui, scrol_index, vthis);
			pMenu.CreateObjectFn4("Reals", g_Options.reals, menu_index, vgui, scrol_index, vthis);
		}
		pMenu.CreateObjectFn4("Cheater VS Cheater", g_Options.misc_caa, menu_index, vgui, scrol_index, vthis);

		if (g_Options.misc_caa)
		{
			pMenu.CreateObjectFn5("Anti- Anti (Aim)", g_Options.misc_resolver, menu_index, vgui, scrol_index, vthis, 2);
			pMenu.CreateObjectFn1("Pitch", g_Options.misc_paa, menu_index, vgui, scrol_index, vthis, pa1, pa1_t);
			pMenu.CreateObjectFn1("Yawhh", g_Options.misc_yaa, menu_index, vgui, scrol_index, vthis, ca1, ca1_t);
		}
	}
}

void C_Menu::CreateObjectFn4(string id, float &cvar, int &menu_index, int &vgui, int &scroller_index, int &vthis)
{
	if (vthis == menu_index)
	{
		if (pMenu.__key(VK_LEFT) && cvar > 0)
		{
			--cvar;
		}
		else if (pMenu.__key(VK_RIGHT) && cvar < 1)
		{
			++cvar;
		}
	}

	pDrawing_t.DrawString(1, 389, vgui, Color::White(), id.c_str());
	pDrawing_t.DrawString(0, 389 + offset, vgui, cvar ? Color::Blue() : Color::Red(), cvar ? "ON":"OFF");

	++vthis;
	vgui += 15;
}

void C_Menu::CreateObjectFn3(string id, float &cvar, int &menu_index, int &vgui, int &scroller_index, int &vthis)
{
	if (vthis == menu_index)
	{
		if (pMenu.__key(VK_LEFT) && cvar > 0)
		{
			--cvar;
		}
		else if (pMenu.__key(VK_RIGHT) && cvar < 1)
		{
			++cvar;
		}
	}

	pDrawing_t.DrawString(0, 389, vgui, cvar ? Color::White() : Color::White(), id.c_str());

	++vthis;
	vgui += 15;
}

void C_Menu::CreateObjectFn5(string id, float &cvar, int &menu_index, int &vgui, int &scroller_index, int &vthis, int max)
{
	if (vthis == menu_index)
	{
		if (pMenu.__key(VK_LEFT) && cvar > 0)
		{
			--cvar;
		}
		else if (pMenu.__key(VK_RIGHT) && cvar < max)
		{
			++cvar;
		}
	}

	pDrawing_t.DrawString(1, 389, vgui, Color::White(), id.c_str());
	pDrawing_t.DrawString(0, 389 + offset, vgui, cvar ? Color::Blue() : Color::Red (), "%f", cvar);

	++vthis;
	vgui += 15;
}

void C_Menu::CreateObjectFn1(string id, float &cvar, int &menu_index, int &vgui, int &scroller_index, int &vthis, string d[33], int max)
{
	if (vthis == menu_index)
	{
		if (pMenu.__key(VK_LEFT) && cvar > 0)
		{
			--cvar;
		}
		else if (pMenu.__key(VK_RIGHT) && cvar < max)
		{
			++cvar;
		}
	}

	pDrawing_t.DrawString(1, 389, vgui, Color::White(), id.c_str());

	int i = 0;
	string finalp = "OFF";

	for each (string s in d)
	{
		if (i == cvar && cvar > 0)
			finalp = d[i];

		++i;
	}

	pDrawing_t.DrawString(0, 389 + offset, vgui, cvar ? Color::Blue() : Color::Red(), finalp.c_str());

	++vthis;
	vgui += 15;
}

bool C_Menu::__key(INT vKey)
{
	if (GetAsyncKeyState(vKey) & 1)
		return true;

	return false;
}