#include "Glow.h"
#include "Util.h"

#define REDCOLORFLOAT (float)0.490000, (float)0.660000, (float)0.770000
#define BLUCOLORFLOAT (float)0.740000, (float)0.230000, (float)0.230000

bool C_Glow::update;

C_Glow pGlow_t;

int C_Glow::registerGlowObject(C_Entity *ent, float r, float g, float b, float a, bool bRenderWhenOccluded, bool bRenderWhenUnoccluded, int nSplitScreenSlot)
{
	try
	{
		// assumes the glow object is real

		if (!pGlow)
			return 0;

		if (!ent)
			return 0;

		// assumes the glow object is real

		typedef int(__thiscall *registerFn)(CGlowObjectManager *, C_Entity *, Vector &, float, bool, bool, int);
		static DWORD dwFn = gSignatures.GetClientSignature("55 8B EC 51 53 56 8B F1 57 8B 5E 14");
		static registerFn Register = (registerFn)dwFn;

		return Register(pGlow, ent, Vector{ r, g, b }, a, bRenderWhenOccluded, bRenderWhenUnoccluded, nSplitScreenSlot);
	}
	catch (...)
	{

	}
}

int AcquirePlayerCount_t()
{
	static int m_ipCountReturnFn = 0;

	for (int i = 0; i <= Iface.Engine->GetMaxClients(); i++)
	{
		C_Entity* cur = GetBaseEntity(1);

		if (!cur)
			continue;

		++m_ipCountReturnFn;
	}

	return m_ipCountReturnFn;
}

#define END_OF_FREE_LIST -1
#define ENTRY_IN_USE -2

void C_Glow::fsn()
{
	static bool pRegsFn[33];
	static float pUpdateTimeFn = 0.0f;

	static int oldcount = AcquirePlayerCount_t();

	for (int index = 0; index < pGlow->m_GlowObjectDefinitions.Count(); index++) {
		GlowObjectDefinition_t& glowobject = pGlow->m_GlowObjectDefinitions[index];

		if (glowobject.m_nNextFreeSlot != ENTRY_IN_USE)
			continue;

		glowobject.m_vGlowColor = Vector(0.f, 1.f, 0.f);
		pGlow->m_GlowObjectDefinitions.RemoveAll();
	}

	for (int i = 0; i <= Iface.Engine->GetMaxClients(); i++)
	{
		C_Entity* cur = GetBaseEntity(i);

		if (!cur)
			continue;

		if (cur->GetLifeState() != LIFE_ALIVE)
			continue;

		if (cur->IsDormant() == true)
			continue;

		if (cur->GetIndex() == me)
			continue;

		if (cur->GetTeamNum() == GetBaseEntity(me)->GetTeamNum())
			continue;

		registerGlowObject(cur, 255.f, 244, 113, 66, true, true, 0);
	}

}

void C_Glow::init()
{
	DWORD dwGlowLoc = gSignatures.GetClientSignature("8B 0D ? ? ? ? A1 ? ? ? ? 56 8B 37") + 0x2;
	XASSERT(dwGlowLoc);
	pGlow = *(CGlowObjectManager**)dwGlowLoc;
	XASSERT(pGlow);
}