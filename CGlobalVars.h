#include "SDK.h"

class CGlobalVariables
{
public:
	int iAimbotIndex = -1;
	
	float settings_switch;
	float iMenu_Pos_X = 300;
	float iMenu_Pos_Y = 75;
	float szsubstest;

	float aimbot_switch;
	float aimbot_active;
	float aimbot_key;
	float aimbot_hitscan;
	float aimbot_hitbox;
	float aimbot_autoshoot;
	float aimbot_silent;

	float triggerbot_switch;
	float triggerbot_active;
	float triggerbot_key;
	float triggerbot_headonly;

	float acc_switch;
	float acc_no_scope;
	float acc_no_zoom;

	float acc_custom_fov;
	float acc_zoom_fov;
	float acc_nzoo_fov;
	float acc_multicore;

	float esp_switch;
	float esp_active;
	float esp_enemyonly;
	float esp_box;
	float esp_name;
	float esp_class;
	float esp_health;
	float esp_bones;
	float esp_sight;
	float esp_local;
	float esp_debug;
	float d3d_lines;

	float misc_switch;
	float misc_bunnyhop;
	float misc_autostrafe;
	float misc_noisemaker_spam;
	float misc_thirdperson;
	float misc_cheats;
	float misc_resolver;
	float misc_fakelag;
	float misc_paa;
	float misc_yaa;

	float esp_height;
	float esp_width;

	float fakhs;
	float reals;

	float misc_caa;

	float playerlist_switch;
	float PlayerMode[64] = {
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
		1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	};
};

class COffsets
{
public:
	int iKeyEventOffset = 20, iCreateMoveOffset = 21, iPaintTraverseOffset = 41;
};