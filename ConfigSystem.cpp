#include "ConfigSystem.h"
#include <string>

ConfigSystem pConfigSystemHb;

ConfigSystem::ConfigSystem()
{
}


ConfigSystem::~ConfigSystem()
{
}

void ConfigSystem::load_cfg(string c, string n, float &v)
{
	v = GetPrivateProfileIntA(c.c_str(), n.c_str(), 0, "C:/cfg.ini");
}

void ConfigSystem::writ_cfg(string c, string n, UINT v)
{
	WritePrivateProfileString(c.c_str(), n.c_str(), to_string(v).c_str(), "C:/cfg.ini");

	pConfigSystemHb.rel_cfgh();
}

void ConfigSystem::rel_cfgh()
{
	//Config;
	pConfigSystemHb.load_cfg("Aim", "acik", g_Options.aimbot_active);
	pConfigSystemHb.load_cfg("Aim", "autoshot", g_Options.aimbot_autoshoot);
	pConfigSystemHb.load_cfg("Aim", "hitscn", g_Options.aimbot_hitscan);
	pConfigSystemHb.load_cfg("Aim", "hbx", g_Options.aimbot_hitbox);
	pConfigSystemHb.load_cfg("Aim", "slt", g_Options.aimbot_silent);

	//Config;
	pConfigSystemHb.load_cfg("Vis", "acik", g_Options.esp_active);
	pConfigSystemHb.load_cfg("Vis", "i", g_Options.esp_enemyonly);
	pConfigSystemHb.load_cfg("Vis", "bx", g_Options.esp_box);
	pConfigSystemHb.load_cfg("Vis", "nam", g_Options.esp_name);
	pConfigSystemHb.load_cfg("Vis", "bons", g_Options.esp_bones);
	pConfigSystemHb.load_cfg("Vis", "Ht", g_Options.esp_health);
	pConfigSystemHb.load_cfg("Vis", "St", g_Options.esp_sight);

	//Config;
	pConfigSystemHb.load_cfg("Mis", "nt", g_Options.misc_noisemaker_spam);
	pConfigSystemHb.load_cfg("Mis", "at", g_Options.misc_autostrafe);
	pConfigSystemHb.load_cfg("Mis", "bhp", g_Options.misc_bunnyhop);
}