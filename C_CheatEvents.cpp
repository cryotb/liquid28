#include "C_CheatEvents.h"

C_CheatEvents pCheatEvents;

string C_CheatEvents::cache[256] = {
	"",
	"",
	"",
	"",
};

void C_CheatEvents::execute()
{
	int vthis = 35;

	if (GetAsyncKeyState(VK_RSHIFT))
	{
		for each(string s in cache)
		{
			if (s.empty() != true)
			{
				pDrawing_t.DrawString(1, 5, vthis, Color::Red(), s.c_str());
				vthis += 15;
			}
		}
	}
}

void C_CheatEvents::raise(string name, int state)
{
	int i = 0;
	for each(string s in cache)
	{
		if (s.empty())
			cache[i] = name;

			++i;
	}
}