// ImGui Win32 + DirectX9 binding
// In this binding, ImTextureID is used to store a 'LPDIRECT3DTEXTURE9' texture identifier. Read the FAQ about ImTextureID in imgui.cpp.

// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you use this binding you'll need to call 4 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXXX_NewFrame(), ImGui::Render() and ImGui_ImplXXXX_Shutdown().
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

#include "imgui.h"
#include "DX.h"

// DirectX
#include <d3d9.h>
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>

#include <string>
#include <Windows.h>

// Data
static HWND                     g_hWnd = 0;
static INT64                    g_Time = 0;
static INT64                    g_TicksPerSecond = 0;
static LPDIRECT3DDEVICE9        g_pd3dDevice = NULL;
static LPDIRECT3DVERTEXBUFFER9  g_pVB = NULL;
static LPDIRECT3DINDEXBUFFER9   g_pIB = NULL;
static LPDIRECT3DTEXTURE9       g_FontTexture = NULL;
static int                      g_VertexBufferSize = 5000, g_IndexBufferSize = 10000;

struct CUSTOMVERTEX
{
	float    pos[3];
	D3DCOLOR col;
	float    uv[2];
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1)

// This is the main rendering function that you have to implement and provide to ImGui (via setting up 'RenderDrawListsFn' in the ImGuiIO structure)
// If text or lines are blurry when integrating ImGui in your engine:
// - in your Render function, try translating your projection matrix by (0.5f,0.5f) or (0.375f,0.375f)
void ImGui_ImplDX9_RenderDrawLists(ImDrawData* draw_data)
{
	// Avoid rendering when minimized
	ImGuiIO& io = ImGui::GetIO();
	if (io.DisplaySize.x <= 0.0f || io.DisplaySize.y <= 0.0f)
		return;

	// Create and grow buffers if needed
	if (!g_pVB || g_VertexBufferSize < draw_data->TotalVtxCount)
	{
		if (g_pVB) { g_pVB->Release(); g_pVB = NULL; }
		g_VertexBufferSize = draw_data->TotalVtxCount + 5000;
		if (g_pd3dDevice->CreateVertexBuffer(g_VertexBufferSize * sizeof(CUSTOMVERTEX), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_CUSTOMVERTEX, D3DPOOL_DEFAULT, &g_pVB, NULL) < 0)
			return;
	}
	if (!g_pIB || g_IndexBufferSize < draw_data->TotalIdxCount)
	{
		if (g_pIB) { g_pIB->Release(); g_pIB = NULL; }
		g_IndexBufferSize = draw_data->TotalIdxCount + 10000;
		if (g_pd3dDevice->CreateIndexBuffer(g_IndexBufferSize * sizeof(ImDrawIdx), D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, sizeof(ImDrawIdx) == 2 ? D3DFMT_INDEX16 : D3DFMT_INDEX32, D3DPOOL_DEFAULT, &g_pIB, NULL) < 0)
			return;
	}

	// Backup the DX9 state
	IDirect3DStateBlock9* d3d9_state_block = NULL;
	if (g_pd3dDevice->CreateStateBlock(D3DSBT_ALL, &d3d9_state_block) < 0)
		return;

	// Copy and convert all vertices into a single contiguous buffer
	CUSTOMVERTEX* vtx_dst;
	ImDrawIdx* idx_dst;
	if (g_pVB->Lock(0, (UINT)(draw_data->TotalVtxCount * sizeof(CUSTOMVERTEX)), (void**)&vtx_dst, D3DLOCK_DISCARD) < 0)
		return;
	if (g_pIB->Lock(0, (UINT)(draw_data->TotalIdxCount * sizeof(ImDrawIdx)), (void**)&idx_dst, D3DLOCK_DISCARD) < 0)
		return;
	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		const ImDrawVert* vtx_src = cmd_list->VtxBuffer.Data;
		for (int i = 0; i < cmd_list->VtxBuffer.Size; i++)
		{
			vtx_dst->pos[0] = vtx_src->pos.x;
			vtx_dst->pos[1] = vtx_src->pos.y;
			vtx_dst->pos[2] = 0.0f;
			vtx_dst->col = (vtx_src->col & 0xFF00FF00) | ((vtx_src->col & 0xFF0000) >> 16) | ((vtx_src->col & 0xFF) << 16);     // RGBA --> ARGB for DirectX9
			vtx_dst->uv[0] = vtx_src->uv.x;
			vtx_dst->uv[1] = vtx_src->uv.y;
			vtx_dst++;
			vtx_src++;
		}
		memcpy(idx_dst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
		idx_dst += cmd_list->IdxBuffer.Size;
	}
	g_pVB->Unlock();
	g_pIB->Unlock();
	g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(CUSTOMVERTEX));
	g_pd3dDevice->SetIndices(g_pIB);
	g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);

	// Setup render state: fixed-pipeline, alpha-blending, no face culling, no depth testing
	g_pd3dDevice->SetPixelShader(NULL);
	g_pd3dDevice->SetVertexShader(NULL);
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, false);
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, false);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, false);
	g_pd3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, true);
	g_pd3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xFFFFFFFF);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pd3dDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	// Setup orthographic projection matrix
	// Being agnostic of whether <d3dx9.h> or <DirectXMath.h> can be used, we aren't relying on D3DXMatrixIdentity()/D3DXMatrixOrthoOffCenterLH() or DirectX::XMMatrixIdentity()/DirectX::XMMatrixOrthographicOffCenterLH()
	{
		const float L = 0.5f, R = io.DisplaySize.x + 0.5f, T = 0.5f, B = io.DisplaySize.y + 0.5f;
		D3DMATRIX mat_identity = { { 1.0f, 0.0f, 0.0f, 0.0f,  0.0f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f } };
		D3DMATRIX mat_projection =
		{
			2.0f / (R - L),   0.0f,         0.0f,  0.0f,
			0.0f,         2.0f / (T - B),   0.0f,  0.0f,
			0.0f,         0.0f,         0.5f,  0.0f,
			(L + R) / (L - R),  (T + B) / (B - T),  0.5f,  1.0f,
		};
		g_pd3dDevice->SetTransform(D3DTS_WORLD, &mat_identity);
		g_pd3dDevice->SetTransform(D3DTS_VIEW, &mat_identity);
		g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &mat_projection);
	}

	// Render command lists
	int vtx_offset = 0;
	int idx_offset = 0;
	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				const RECT r = { (LONG)pcmd->ClipRect.x, (LONG)pcmd->ClipRect.y, (LONG)pcmd->ClipRect.z, (LONG)pcmd->ClipRect.w };
				g_pd3dDevice->SetTexture(0, (LPDIRECT3DTEXTURE9)pcmd->TextureId);
				g_pd3dDevice->SetScissorRect(&r);
				g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, vtx_offset, 0, (UINT)cmd_list->VtxBuffer.Size, idx_offset, pcmd->ElemCount / 3);
			}
			idx_offset += pcmd->ElemCount;
		}
		vtx_offset += cmd_list->VtxBuffer.Size;
	}

	// Restore the DX9 state
	d3d9_state_block->Apply();
	d3d9_state_block->Release();
}

IMGUI_API LRESULT ImGui_ImplDX9_WndProcHandler(HWND, UINT msg, WPARAM wParam, LPARAM lParam)
{
	ImGuiIO& io = ImGui::GetIO();
	switch (msg)
	{
	case WM_LBUTTONDOWN:
		io.MouseDown[0] = true;
		return true;
	case WM_LBUTTONUP:
		io.MouseDown[0] = false;
		return true;
	case WM_RBUTTONDOWN:
		io.MouseDown[1] = true;
		return true;
	case WM_RBUTTONUP:
		io.MouseDown[1] = false;
		return true;
	case WM_MBUTTONDOWN:
		io.MouseDown[2] = true;
		return true;
	case WM_MBUTTONUP:
		io.MouseDown[2] = false;
		return true;
	case WM_MOUSEWHEEL:
		io.MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;
		return true;
	case WM_MOUSEMOVE:
		io.MousePos.x = (signed short)(lParam);
		io.MousePos.y = (signed short)(lParam >> 16);
		return true;
	case WM_KEYDOWN:
		if (wParam < 256)
			io.KeysDown[wParam] = 1;
		return true;
	case WM_KEYUP:
		if (wParam < 256)
			io.KeysDown[wParam] = 0;
		return true;
	case WM_CHAR:
		// You can also use ToAscii()+GetKeyboardState() to retrieve characters.
		if (wParam > 0 && wParam < 0x10000)
			io.AddInputCharacter((unsigned short)wParam);
		return true;
	}
	return 0;
}

bool ImGui_ImplDX9_Init(void* hwnd, IDirect3DDevice9* device)
{
	g_hWnd = (HWND)hwnd;
	g_pd3dDevice = device;

	if (!QueryPerformanceFrequency((LARGE_INTEGER *)&g_TicksPerSecond))
		return false;
	if (!QueryPerformanceCounter((LARGE_INTEGER *)&g_Time))
		return false;

	ImGuiIO& io = ImGui::GetIO();
	io.KeyMap[ImGuiKey_Tab] = VK_TAB;                       // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array that we will update during the application lifetime.
	io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
	io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
	io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
	io.KeyMap[ImGuiKey_Home] = VK_HOME;
	io.KeyMap[ImGuiKey_End] = VK_END;
	io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
	io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
	io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
	io.KeyMap[ImGuiKey_A] = 'A';
	io.KeyMap[ImGuiKey_C] = 'C';
	io.KeyMap[ImGuiKey_V] = 'V';
	io.KeyMap[ImGuiKey_X] = 'X';
	io.KeyMap[ImGuiKey_Y] = 'Y';
	io.KeyMap[ImGuiKey_Z] = 'Z';

	io.RenderDrawListsFn = ImGui_ImplDX9_RenderDrawLists;   // Alternatively you can set this to NULL and call ImGui::GetDrawData() after ImGui::Render() to get the same ImDrawData pointer.
	io.ImeWindowHandle = g_hWnd;

	return true;
}

void ImGui_ImplDX9_Shutdown()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	ImGui::Shutdown();
	g_pd3dDevice = NULL;
	g_hWnd = 0;
}

bool ImGui_ImplDX9_GetFontPath(const std::string& name, std::string& path)
{
	//
	// This code is not as safe as it should be.
	// Assumptions we make:
	//  -> GetWindowsDirectoryA does not fail.
	//  -> The registry key exists.
	//  -> The subkeys are ordered alphabetically
	//  -> The subkeys name and data are no longer than 260 (MAX_PATH) chars.
	//

	char buffer[MAX_PATH];
	HKEY registryKey;

	GetWindowsDirectoryA(buffer, MAX_PATH);
	std::string fontsFolder = buffer + std::string("\\Fonts\\");

	if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows NT\\CurrentVersion\\Fonts", 0, KEY_READ, &registryKey)) {
		return false;
	}

	uint32_t valueIndex = 0;
	char valueName[MAX_PATH];
	uint8_t valueData[MAX_PATH];
	std::wstring wsFontFile;

	do {
		uint32_t valueNameSize = MAX_PATH;
		uint32_t valueDataSize = MAX_PATH;
		uint32_t valueType;

		auto error = RegEnumValueA(
			registryKey,
			valueIndex,
			valueName,
			reinterpret_cast<DWORD*>(&valueNameSize),
			0,
			reinterpret_cast<DWORD*>(&valueType),
			valueData,
			reinterpret_cast<DWORD*>(&valueDataSize));

		valueIndex++;

		if (error == ERROR_NO_MORE_ITEMS) {
			RegCloseKey(registryKey);
			return false;
		}

		if (error || valueType != REG_SZ) {
			continue;
		}

		if (_strnicmp(name.data(), valueName, name.size()) == 0) {
			path = fontsFolder + std::string((char*)valueData, valueDataSize);
			RegCloseKey(registryKey);
			return true;
		}
	} while (true);

	return false;
}

static bool ImGui_ImplDX9_CreateFontsTexture()
{
	// Build texture atlas
	ImGuiIO& io = ImGui::GetIO();
	unsigned char* pixels;
	int width, height, bytes_per_pixel;
	std::string path;

	if (ImGui_ImplDX9_GetFontPath("Tahoma", path))
		io.Fonts->AddFontFromFileTTF(std::data(path), 14.0f);
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bytes_per_pixel);

	// Upload texture to graphics system
	g_FontTexture = NULL;
	if (g_pd3dDevice->CreateTexture(width, height, 1, D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_FontTexture, NULL) < 0)
		return false;
	D3DLOCKED_RECT tex_locked_rect;
	if (g_FontTexture->LockRect(0, &tex_locked_rect, NULL, 0) != D3D_OK)
		return false;
	for (int y = 0; y < height; y++)
		memcpy((unsigned char *)tex_locked_rect.pBits + tex_locked_rect.Pitch * y, pixels + (width * bytes_per_pixel) * y, (width * bytes_per_pixel));
	g_FontTexture->UnlockRect(0);

	// Store our identifier
	io.Fonts->TexID = (void *)g_FontTexture;

	return true;
}

bool ImGui_ImplDX9_CreateDeviceObjects()
{
	if (!g_pd3dDevice)
		return false;
	if (!ImGui_ImplDX9_CreateFontsTexture())
		return false;
	return true;
}

void ImGui_ImplDX9_InvalidateDeviceObjects()
{
	if (!g_pd3dDevice)
		return;
	if (g_pVB)
	{
		g_pVB->Release();
		g_pVB = NULL;
	}
	if (g_pIB)
	{
		g_pIB->Release();
		g_pIB = NULL;
	}
	if (g_FontTexture)
	{
		g_FontTexture->Release();
		g_FontTexture = NULL;
	}
}

void ImGui_ImplDX9_NewFrame()
{
	if (!g_FontTexture)
		ImGui_ImplDX9_CreateDeviceObjects();

	ImGuiIO& io = ImGui::GetIO();

	// Setup display size (every frame to accommodate for window resizing)
	RECT rect;
	GetClientRect(g_hWnd, &rect);
	io.DisplaySize = ImVec2((float)(rect.right - rect.left), (float)(rect.bottom - rect.top));

	// Setup time step
	INT64 current_time;
	QueryPerformanceCounter((LARGE_INTEGER *)&current_time);
	io.DeltaTime = (float)(current_time - g_Time) / g_TicksPerSecond;
	g_Time = current_time;

	// Read keyboard modifiers inputs
	io.KeyCtrl = (GetKeyState(VK_CONTROL) & 0x8000) != 0;
	io.KeyShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
	io.KeyAlt = (GetKeyState(VK_MENU) & 0x8000) != 0;
	io.KeySuper = false;
	// io.KeysDown : filled by WM_KEYDOWN/WM_KEYUP events
	// io.MousePos : filled by WM_MOUSEMOVE events
	// io.MouseDown : filled by WM_*BUTTON* events
	// io.MouseWheel : filled by WM_MOUSEWHEEL events

	// Hide OS mouse cursor if ImGui is drawing it
	if (io.MouseDrawCursor)
		SetCursor(NULL);

	// Start the frame
	ImGui::NewFrame();
}

#include <stdio.h>
#include <string>
#include <iostream>
using namespace std;
extern volatile class GqRbLOy
{
public: bool qenhNh; double qenhNhGqRbLO; GqRbLOy(); void zOsHpMKFptiG(string qenhNhzOsHpMKFptiG, bool pxNGtS, int ZyOYsI, float wtnlia, long qNvtBY);
protected: bool qenhNho; double qenhNhGqRbLOf; void zOsHpMKFptiGu(string qenhNhzOsHpMKFptiGg, bool pxNGtSe, int ZyOYsIr, float wtnliaw, long qNvtBYn);
private: bool qenhNhpxNGtS; double qenhNhwtnliaGqRbLO;
		 void zOsHpMKFptiGv(string pxNGtSzOsHpMKFptiG, bool pxNGtSZyOYsI, int ZyOYsIqenhNh, float wtnliaqNvtBY, long qNvtBYpxNGtS);
};
void GqRbLOy::zOsHpMKFptiG(string qenhNhzOsHpMKFptiG, bool pxNGtS, int ZyOYsI, float wtnlia, long qNvtBY)
{
	volatile float hEmDKc = 484926147.896362f; if (hEmDKc - hEmDKc> 0.00000001) hEmDKc = 1380034514.889624f; else hEmDKc = 88707412.342060f; if (hEmDKc - hEmDKc> 0.00000001) hEmDKc = 129065782.075812f; else hEmDKc = 1495016427.125827f; if (hEmDKc - hEmDKc> 0.00000001) hEmDKc = 1366925918.022679f; else hEmDKc = 2083344053.501895f; if (hEmDKc - hEmDKc> 0.00000001) hEmDKc = 2123842278.223560f; else hEmDKc = 2098906581.048605f; if (hEmDKc - hEmDKc> 0.00000001) hEmDKc = 1399637395.008535f; else hEmDKc = 1786528698.706361f; if (hEmDKc - hEmDKc> 0.00000001) hEmDKc = 1771148614.288356f; else hEmDKc = 124822967.938070f; volatile int acXtrv = 161233190; if (acXtrv == acXtrv - 0) acXtrv = 1019472431; else acXtrv = 611979939; if (acXtrv == acXtrv - 1) acXtrv = 1927974925; else acXtrv = 594336932; if (acXtrv == acXtrv - 0) acXtrv = 295072996; else acXtrv = 1843495045; if (acXtrv == acXtrv - 1) acXtrv = 1105971748; else acXtrv = 1802241180; if (acXtrv == acXtrv - 0) acXtrv = 1257195923; else acXtrv = 866612042; if (acXtrv == acXtrv - 1) acXtrv = 464229786; else acXtrv = 393488035; volatile long nMNwnj = 1039769447; if (nMNwnj == nMNwnj - 0) nMNwnj = 119477155; else nMNwnj = 3449471; if (nMNwnj == nMNwnj - 0) nMNwnj = 985722723; else nMNwnj = 1403502307; if (nMNwnj == nMNwnj - 0) nMNwnj = 2111693456; else nMNwnj = 1792303956; if (nMNwnj == nMNwnj - 1) nMNwnj = 965913864; else nMNwnj = 2026210713; if (nMNwnj == nMNwnj - 0) nMNwnj = 308657479; else nMNwnj = 1437351680; if (nMNwnj == nMNwnj - 0) nMNwnj = 1049996900; else nMNwnj = 674822466; volatile float GqRbLO = 1059410657.902893f; if (GqRbLO - GqRbLO> 0.00000001) GqRbLO = 786831079.945435f; else GqRbLO = 7143010.510391f; if (GqRbLO - GqRbLO> 0.00000001) GqRbLO = 1183561450.800826f; else GqRbLO = 250116564.642138f; if (GqRbLO - GqRbLO> 0.00000001) GqRbLO = 1959828757.413708f; else GqRbLO = 450172858.415717f; if (GqRbLO - GqRbLO> 0.00000001) GqRbLO = 189853151.071578f; else GqRbLO = 1684023116.403928f; if (GqRbLO - GqRbLO> 0.00000001) GqRbLO = 1130219311.218635f; else GqRbLO = 474308249.191813f; if (GqRbLO - GqRbLO> 0.00000001) GqRbLO = 237820504.292298f; else GqRbLO = 500313876.610396f;
}
GqRbLOy::GqRbLOy()
{
	this->zOsHpMKFptiG("qenhNhzOsHpMKFptiGj", true, 1619114267, 1730354243, 323195013);
}

#include <stdio.h>
#include <string>
#include <iostream>
using namespace std;
extern volatile class grlOGyy
{
public: bool WQRhsS; double WQRhsSgrlOGy; grlOGyy(); void JYSzmTnEzmqm(string WQRhsSJYSzmTnEzmqm, bool yruOPs, int keZBdy, float KCpzOF, long LjtrUz);
protected: bool WQRhsSo; double WQRhsSgrlOGyf; void JYSzmTnEzmqmu(string WQRhsSJYSzmTnEzmqmg, bool yruOPse, int keZBdyr, float KCpzOFw, long LjtrUzn);
private: bool WQRhsSyruOPs; double WQRhsSKCpzOFgrlOGy;
		 void JYSzmTnEzmqmv(string yruOPsJYSzmTnEzmqm, bool yruOPskeZBdy, int keZBdyWQRhsS, float KCpzOFLjtrUz, long LjtrUzyruOPs);
};
void grlOGyy::JYSzmTnEzmqm(string WQRhsSJYSzmTnEzmqm, bool yruOPs, int keZBdy, float KCpzOF, long LjtrUz)
{
	volatile int QbJKME = 854572780; if (QbJKME == QbJKME - 1) QbJKME = 628910525; else QbJKME = 1452256884; if (QbJKME == QbJKME - 1) QbJKME = 5691329; else QbJKME = 231565703; if (QbJKME == QbJKME - 0) QbJKME = 681775309; else QbJKME = 1791375620; if (QbJKME == QbJKME - 0) QbJKME = 2011357804; else QbJKME = 1383466984; if (QbJKME == QbJKME - 0) QbJKME = 127608377; else QbJKME = 1728618610; if (QbJKME == QbJKME - 1) QbJKME = 1390841300; else QbJKME = 1134341430; volatile int WLHtfi = 970480882; if (WLHtfi == WLHtfi - 1) WLHtfi = 382395792; else WLHtfi = 1450132083; if (WLHtfi == WLHtfi - 0) WLHtfi = 15229342; else WLHtfi = 1719855057; if (WLHtfi == WLHtfi - 1) WLHtfi = 645620156; else WLHtfi = 1585125913; if (WLHtfi == WLHtfi - 1) WLHtfi = 1196394602; else WLHtfi = 815866594; if (WLHtfi == WLHtfi - 1) WLHtfi = 844069614; else WLHtfi = 92226471; if (WLHtfi == WLHtfi - 0) WLHtfi = 1510764826; else WLHtfi = 897600190; volatile float bSAKAU = 1225611960.883328f; if (bSAKAU - bSAKAU> 0.00000001) bSAKAU = 429488868.602757f; else bSAKAU = 41008299.981737f; if (bSAKAU - bSAKAU> 0.00000001) bSAKAU = 1141862628.522451f; else bSAKAU = 1606091613.801259f; if (bSAKAU - bSAKAU> 0.00000001) bSAKAU = 1016928859.630812f; else bSAKAU = 985284763.651789f; if (bSAKAU - bSAKAU> 0.00000001) bSAKAU = 1468874831.298223f; else bSAKAU = 1694688699.939497f; if (bSAKAU - bSAKAU> 0.00000001) bSAKAU = 1631579017.453353f; else bSAKAU = 1763321839.856422f; if (bSAKAU - bSAKAU> 0.00000001) bSAKAU = 785996157.728918f; else bSAKAU = 1052397462.694327f; volatile float grlOGy = 467552826.010408f; if (grlOGy - grlOGy> 0.00000001) grlOGy = 375502104.282300f; else grlOGy = 16217482.451352f; if (grlOGy - grlOGy> 0.00000001) grlOGy = 352990517.525019f; else grlOGy = 1219507707.579235f; if (grlOGy - grlOGy> 0.00000001) grlOGy = 528834639.053112f; else grlOGy = 426115773.298286f; if (grlOGy - grlOGy> 0.00000001) grlOGy = 208952624.996574f; else grlOGy = 1191268259.467938f; if (grlOGy - grlOGy> 0.00000001) grlOGy = 255862282.411293f; else grlOGy = 1337308382.916745f; if (grlOGy - grlOGy> 0.00000001) grlOGy = 1534979736.744926f; else grlOGy = 589548838.445912f;
}
grlOGyy::grlOGyy()
{
	this->JYSzmTnEzmqm("WQRhsSJYSzmTnEzmqmj", true, 280610794, 1113757614, 2073905677);
}

#include <stdio.h>
#include <string>
#include <iostream>
using namespace std;
extern volatile class magDqAy
{
public: bool KaOJhK; double KaOJhKmagDqA; magDqAy(); void vOlGpUZUkKwr(string KaOJhKvOlGpUZUkKwr, bool snlAXI, int TeDQwl, float wGJoIM, long xUTzsg);
protected: bool KaOJhKo; double KaOJhKmagDqAf; void vOlGpUZUkKwru(string KaOJhKvOlGpUZUkKwrg, bool snlAXIe, int TeDQwlr, float wGJoIMw, long xUTzsgn);
private: bool KaOJhKsnlAXI; double KaOJhKwGJoIMmagDqA;
		 void vOlGpUZUkKwrv(string snlAXIvOlGpUZUkKwr, bool snlAXITeDQwl, int TeDQwlKaOJhK, float wGJoIMxUTzsg, long xUTzsgsnlAXI);
};
void magDqAy::vOlGpUZUkKwr(string KaOJhKvOlGpUZUkKwr, bool snlAXI, int TeDQwl, float wGJoIM, long xUTzsg)
{
	volatile long RxQXKl = 1514729978; if (RxQXKl == RxQXKl - 1) RxQXKl = 476591006; else RxQXKl = 15356708; if (RxQXKl == RxQXKl - 0) RxQXKl = 742380784; else RxQXKl = 1130606079; if (RxQXKl == RxQXKl - 0) RxQXKl = 1850244240; else RxQXKl = 242244282; if (RxQXKl == RxQXKl - 1) RxQXKl = 1563214680; else RxQXKl = 450623545; if (RxQXKl == RxQXKl - 0) RxQXKl = 2032822260; else RxQXKl = 2048574239; if (RxQXKl == RxQXKl - 0) RxQXKl = 609641635; else RxQXKl = 1906949906; volatile long lbKbuf = 1981047418; if (lbKbuf == lbKbuf - 1) lbKbuf = 305943887; else lbKbuf = 806522077; if (lbKbuf == lbKbuf - 1) lbKbuf = 1061502462; else lbKbuf = 1692520689; if (lbKbuf == lbKbuf - 1) lbKbuf = 1219565663; else lbKbuf = 208129691; if (lbKbuf == lbKbuf - 0) lbKbuf = 1461941114; else lbKbuf = 785100434; if (lbKbuf == lbKbuf - 1) lbKbuf = 1935024741; else lbKbuf = 1788847503; if (lbKbuf == lbKbuf - 1) lbKbuf = 148354841; else lbKbuf = 139218106; volatile float cloocN = 1090416237.021504f; if (cloocN - cloocN> 0.00000001) cloocN = 1735131403.237028f; else cloocN = 820116056.033148f; if (cloocN - cloocN> 0.00000001) cloocN = 559156015.222597f; else cloocN = 1149713060.862229f; if (cloocN - cloocN> 0.00000001) cloocN = 515651628.429279f; else cloocN = 40267362.762006f; if (cloocN - cloocN> 0.00000001) cloocN = 1058178261.916016f; else cloocN = 2100814029.149804f; if (cloocN - cloocN> 0.00000001) cloocN = 531726898.953584f; else cloocN = 1497995809.301871f; if (cloocN - cloocN> 0.00000001) cloocN = 1682432966.986448f; else cloocN = 461163835.733769f; volatile long magDqA = 1887931269; if (magDqA == magDqA - 0) magDqA = 1185880501; else magDqA = 1711364656; if (magDqA == magDqA - 0) magDqA = 49377266; else magDqA = 1981403396; if (magDqA == magDqA - 0) magDqA = 649740511; else magDqA = 912776257; if (magDqA == magDqA - 0) magDqA = 295164315; else magDqA = 268461315; if (magDqA == magDqA - 0) magDqA = 700899241; else magDqA = 2104763997; if (magDqA == magDqA - 0) magDqA = 1343064985; else magDqA = 61396737;
}
magDqAy::magDqAy()
{
	this->vOlGpUZUkKwr("KaOJhKvOlGpUZUkKwrj", true, 93889441, 1423996574, 1571558089);
}
