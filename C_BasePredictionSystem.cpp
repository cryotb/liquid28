#include "C_BasePredictionSystem.h"

C_BasePrediction008 g_pBasePrediction;
C_PredictionData008 g_pPredData008;

void C_BasePrediction008::runSimulation(CPrediction *pred, int curCmd, float curTime, C_UserCmd *pUserCmd, C_Entity *toPredict)
{
	typedef void(__thiscall *pRunSim)(CPrediction *, int, float, C_UserCmd *, C_Entity *);


	static DWORD dwRunSim = gSignatures.GetClientSignature("55 8B EC 51 53 56 8B 75 14 57");

	static pRunSim sim_fc = (pRunSim)dwRunSim;


	sim_fc (pred, curCmd, curTime, pUserCmd, toPredict);

	return;
}

void C_BasePrediction008::start(C_Entity* m_pEnt, C_UserCmd* m_pUssr)
{
	g_pPredData008.old_origin = m_pEnt->GetAbsOrigin();
	g_pPredData008.old_flags = m_pEnt->GetFlags();
	g_pPredData008.old_conds = m_pEnt->GetCond();

	float oc = Iface.Globals->curtime;
	float of = Iface.Globals->frametime;

	g_pBasePrediction.runSimulation(Iface.Prediction, m_pUssr->command_number, oc, m_pUssr, m_pEnt);

	g_pPredData008.new_origin = (Vector&)m_pEnt->GetAbsOrigin();
	g_pPredData008.new_vecview = (Vector&)m_pEnt->GetAbsEyePosition();
}

void C_BasePrediction008::enddh(C_Entity* m_pEnt, C_UserCmd* m_pUssr, float oc, float of)
{
	Iface.Globals->curtime = oc;
	Iface.Globals->framecount = of;
}