#pragma once
#include "SDK.h"

class CTriggerbot
{
public:

	void Run(C_Entity* pLocal, C_UserCmd* pCommand);

};

extern CTriggerbot gTrigger; 