#pragma once
#include <d3dx9.h>
#include <d3d9.h>
#include <d3d.h>
#include <d3d9types.h>
#include <Windows.h>
#include "SDK.h"
#include "CDrawManager.h"
#include "DXSDK.h"
#include "G.hpp"

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

class Direct3D_Hooks
{
public:
	void EndScene(IDirect3DDevice9* pDevice);
};

extern Direct3D_Hooks g_D3DHooks;