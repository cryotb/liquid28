#include "D3D.hpp"

Direct3D_Hooks g_D3DHooks;

struct d3d
{
	POINT coords;
	const char* text;
};

void Direct3D_Hooks::EndScene(IDirect3DDevice9* pDevice)
{
	if (Iface.Engine->IsInGame() && g_Options.d3d_lines)
	{
		//Code taken from ur visuals;
		C_Entity* cur = GetBaseEntity(me);

		if (!cur)
			return;

		if (cur->GetLifeState() != LIFE_ALIVE || cur->IsDormant())
			return;


		Vector angs;

		cur->GetWorldSpaceCenter(angs);

		Vector forward;


		AngleVectors(Vector(g::real_angles.pitch, g::real_angles.yaw, 0), &forward);

		forward *= 100;

		Vector vFinal = angs + forward;

		Vector a;
		Vector b;

		if (pDrawing.WorldToScreen(angs, a) && pDrawing.WorldToScreen(vFinal, b))
		{
			g_D3dSdk.DrawLine(pDevice, a.x, a.y, b.x, b.y, 1.9, g_D3dSdk.BLUE);
		}
	}

	if (Iface.Engine->IsInGame() != true)
	{
		g_D3dSdk.DrawString(pDevice, "Liquid", 5, 95, g_D3dSdk.WHITE);
		g_D3dSdk.DrawString(pDevice, "@		Developer Release(d)", 5, 110, g_D3dSdk.WHITE);
		g_D3dSdk.DrawString(pDevice, "@		Version: 1.0", 5, 125, g_D3dSdk.WHITE);
	}
}