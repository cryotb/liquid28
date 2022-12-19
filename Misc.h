#pragma once
#include "SDK.h"

class CMisc
{
public:

	void Run(C_Entity* pLocal, C_UserCmd* pCommand);

private:

	//Other funcs

	void NoisemakerSpam(PVOID kv);
};

extern CMisc gMisc;