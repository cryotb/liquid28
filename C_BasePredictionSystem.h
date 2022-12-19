#include "SDK.h"

class C_BasePrediction008
{
public:
	void runSimulation(CPrediction *pred, int curr_cmd, float currTime, C_UserCmd *cmd, C_Entity *pBaseEnt);
	void start(C_Entity* m_pEnt, C_UserCmd* m_pUssr);
	void enddh(C_Entity* m_pEnt, C_UserCmd* m_pUssr, float oc, float of);
};

class C_PredictionData008
{
public:
	Vector old_origin;
	Vector new_origin;
	Vector new_vecview;

	int old_conds;
	int old_flags;
};

extern C_BasePrediction008 g_pBasePrediction;
extern C_PredictionData008 g_pPredData008;