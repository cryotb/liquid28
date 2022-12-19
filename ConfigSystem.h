#pragma once
#include "SDK.h"
class ConfigSystem
{
public:
	void load_cfg(string c, string n, float &v);
	void writ_cfg(string c, string n, UINT v);
	void rel_cfgh();
	ConfigSystem();
	~ConfigSystem();
};

extern ConfigSystem pConfigSystemHb;