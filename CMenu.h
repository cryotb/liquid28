#pragma once
#include <Windows.h>
#include "SDK.h"
#include <iostream>
#include <ios>

using namespace std;

class C_Menu
{
public:
	void CreateObjectFn4(string id, float &cvar, int &menu_index, int &vgui, int &scroller_index, int &vthis);
	void CreateObjectFn5(string id, float &cvar, int &menu_index, int &vgui, int &scroller_index, int &vthis, int max);
	void CreateObjectFn3(string id, float &cvar, int &menu_index, int &vgui, int &scroller_index, int &vthis);
	void CreateObjectFn1(string id, float &cvar, int &menu_index, int &vgui, int &scroller_index, int &vthis, string d[33], int max);
	void execute();

	float *cached[128];
	int this1;
	int this2;

	bool __key(INT vKey);
};

extern C_Menu pMenu;