#pragma once

#ifndef DXSDK_H
#define DXSDK_H
#include <d3dx9.h>
#include <d3d9.h>
#include <d3d.h>
#include <d3d9types.h>
#include <Windows.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

class C_DXSDK
{
public:
	typedef D3DCOLOR _d3dcolor;

	inline void Background(LPDIRECT3DDEVICE9 pDevice, float X, float Y, float Width, float Height, D3DCOLOR Color)
	{
		struct Vertex2D
		{
			float m_X, m_Y, m_Z, m_T;
			DWORD m_Color;
		};
		Vertex2D Vertex[4];
		Vertex[0].m_Color = Vertex[1].m_Color = Vertex[2].m_Color = Vertex[3].m_Color = Color;
		Vertex[0].m_Z = Vertex[1].m_Z = Vertex[2].m_Z = Vertex[3].m_Z = 0;
		Vertex[0].m_T = Vertex[1].m_T = Vertex[2].m_T = Vertex[3].m_T = 0;
		Vertex[0].m_X = Vertex[2].m_X = X;
		Vertex[0].m_Y = Vertex[1].m_Y = Y;
		Vertex[1].m_X = Vertex[3].m_X = X + Width;
		Vertex[2].m_Y = Vertex[3].m_Y = Y + Height;
		pDevice->SetTexture(0, NULL);
		pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
		pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, Vertex, sizeof(Vertex2D));
	}

	inline void DrawLine(LPDIRECT3DDEVICE9 pDevice, float x1, float y1, float x2, float y2, float width, DWORD color)
	{
		ID3DXLine *m_Line;

		D3DXCreateLine(pDevice, &m_Line);
		D3DXVECTOR2 line[] = { D3DXVECTOR2(x1, y1), D3DXVECTOR2(x2, y2) };
		m_Line->SetWidth(width);
		m_Line->SetAntialias(1);
		m_Line->Begin();
		m_Line->Draw(line, 2, color);
		m_Line->End();
		m_Line->Release();
	}

	inline void DrawString(LPDIRECT3DDEVICE9 pDevice, LPCSTR text, INT x, INT y, _d3dcolor COL)
	{
		static LPD3DXFONT m_Font = NULL;
		D3DXCreateFontA(pDevice, 17, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"), &m_Font);

		RECT pRect;

		pRect.left = x;
		pRect.right = 984;
		pRect.top = y;
		pRect.bottom = y + 194;
		
		m_Font->DrawTextA(NULL, text, -1, &pRect, 0, COL);
	}

	_d3dcolor WHITE = D3DCOLOR_ARGB(255, 255, 255, 255);
	_d3dcolor BLUE = D3DCOLOR_ARGB(255, 0, 0, 255);
	_d3dcolor RED = D3DCOLOR_ARGB(255, 255, 0, 0);
};

extern C_DXSDK g_D3dSdk;

#endif