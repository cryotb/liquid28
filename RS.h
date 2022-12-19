#include "SDK.h"

class g_pEvents : public IGameEventListener2
{
public:
	void run();
	void FireGameEvent(IGameEvent* baseFn);
	bool miss[33];
	void fsn();
	void cmove();
};

extern g_pEvents pRs;