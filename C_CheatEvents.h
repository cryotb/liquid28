#include "SDK.h"
#include "CDrawManager.h"

class C_CheatEvents
{
public:
	void execute();
	void raise(string name, int state);

	static string cache[256];
};

extern C_CheatEvents pCheatEvents;