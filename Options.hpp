#pragma once

#include "interfaces/IInputSystem.h"

#define OPTION(type, var, value) type var = value

class Options
{
public:

	OPTION(bool, misc_autoaccept, false);
	OPTION(bool, misc_revealAllRanks, false);
	OPTION(bool, esp_enemies_only, false);
	OPTION(bool, esp_farther, false);
	OPTION(float, esp_fill_amount, 0.f);
	float esp_player_fill_color_t[4] = { 1.f, 0.f, 0.3f, 1.f };
	float esp_player_fill_color_ct[4] = { 0.f, 0.2f, 1.f, 1.f };
	float esp_player_fill_color_t_visible[4] = { 1.f, 1.f, 0.0f, 1.f };
	float esp_player_fill_color_ct_visible[4] = { 0.f, 0.7f, 1.f, 0.85f };
	OPTION(int, esp_player_boundstype, 0);
	OPTION(int, esp_player_boxtype, 0);
	float esp_player_bbox_color_t[4] = { 1.f, 0.f, 0.3f, 1.f };
	float esp_player_bbox_color_ct[4] = { 0.f, 0.2f, 1.f, 1.f };
	float esp_player_bbox_color_t_visible[4] = { 1.f, 1.f, 0.0f, 1.f };
	float esp_player_bbox_color_ct_visible[4] = { 0.f, 0.7f, 1.f, 0.85f };
	OPTION(bool, esp_player_name, false);
	OPTION(bool, esp_player_health, false);
	OPTION(bool, esp_player_weapons, false);
	OPTION(bool, esp_player_snaplines, false);
	OPTION(bool, esp_player_chams, false);
	OPTION(int, esp_player_chams_type, 0);
	float esp_player_chams_color_t_visible[4] = { 1.f, 1.f, 0.0f, 1.f };
	float esp_player_chams_color_ct_visible[4] = { 0.15f, 0.7f, 1.f, 1.0f };
	float esp_player_chams_color_t[4] = { 1.f, 0.f, 0.3f, 1.f };
	float esp_player_chams_color_ct[4] = { 0.f, 0.2f, 1.f, 1.f };
	OPTION(bool, esp_player_skelet, false);
	OPTION(bool, esp_player_anglelines, false);
	OPTION(int, esp_dropped_weapons, 0);
	OPTION(bool, esp_planted_c4, false);
	OPTION(bool, esp_grenades, false);
	OPTION(int, esp_grenades_type, 0);
	OPTION(float, visuals_others_player_fov, 0);
	OPTION(float, visuals_others_player_fov_viewmodel, 0);
	OPTION(bool, visuals_others_watermark, true);
	OPTION(bool, visuals_others_grenade_pred, false);
	OPTION(int, visuals_others_sky, 0);
	OPTION(bool, glow_enabled, false);
	OPTION(bool, glow_players, false);
	float glow_player_color_t[4] = { 1.f, 0.f, 0.3f, 1.f };
	float glow_player_color_ct[4] = { 0.f, 0.2f, 1.f, 1.f };
	float glow_player_color_t_visible[4] = { 1.f, 1.f, 0.0f, 1.f };
	float glow_player_color_ct_visible[4] = { 0.f, 0.7f, 1.f, 0.85f };
	OPTION(int, glow_players_style, 0);
	OPTION(bool, glow_others, false);
	OPTION(int, glow_others_style, 0);
	OPTION(bool, visuals_others_hitmarker, false);
	OPTION(bool, visuals_others_bulletimpacts, false);
	float visuals_others_bulletimpacts_color[4] = { 0.012f, 0.788f, 0.663f, 1.f };
	OPTION(bool, removals_scope, false);
	OPTION(bool, removals_novisualrecoil, false);
	OPTION(bool, removals_postprocessing, false);
	OPTION(bool, removals_flash, false);
	OPTION(bool, removals_smoke, false);
	OPTION(int, removals_smoke_type, 0);

#ifdef NIGHTMODE
	OPTION(bool, visuals_others_nightmode, false);
	float visuals_others_nightmode_color[4] = { 0.15f, 0.15f, 0.15f, 1.f };
#endif


	OPTION(bool, misc_bhop, false);
	OPTION(bool, misc_autostrafe, false);
	OPTION(bool, misc_auto_pistol, false);
	OPTION(bool, misc_chatspamer, false);
	OPTION(bool, misc_thirdperson, false);
	OPTION(ButtonCode_t, misc_thirdperson_bind, BUTTON_CODE_NONE);
	OPTION(bool, misc_fakewalk, false);
	OPTION(ButtonCode_t, misc_fakewalk_bind, BUTTON_CODE_NONE);
	OPTION(bool, misc_fakelag_enabled, false);
	OPTION(int, misc_fakelag_value, 0);
	OPTION(int, misc_fakelag_activation_type, 0);
	OPTION(bool, misc_fakelag_adaptive, false);
	OPTION(bool, misc_animated_clantag, false);
	OPTION(bool, misc_spectatorlist, false);
	OPTION(bool, misc_instant_defuse_plant, false);
	OPTION(bool, misc_logevents, false);
	OPTION(bool, legit_enabled, false);
	OPTION(ButtonCode_t, legit_aimkey1, BUTTON_CODE_NONE);
	OPTION(ButtonCode_t, legit_aimkey2, BUTTON_CODE_NONE);
	OPTION(bool, legit_rcs, false);
	OPTION(bool, legit_trigger, false);
	OPTION(bool, legit_trigger_with_aimkey, false);
	OPTION(int, legit_preaim, 0);
	OPTION(int, legit_aftershots, 3);
	OPTION(int, legit_afteraim, 0);
	OPTION(float, legit_smooth_factor, 1.f);
	OPTION(float, legit_fov, 1.f);
	OPTION(bool, rage_enabled, false);
	OPTION(ButtonCode_t, rage_aimkey, BUTTON_CODE_NONE);
	OPTION(bool, rage_usekey, false);
	OPTION(bool, rage_silent, false);
	OPTION(bool, rage_norecoil, false);
	OPTION(bool, rage_autoshoot, false);
	OPTION(bool, rage_autoscope, false);
	OPTION(bool, rage_autocrouch, false);
	OPTION(bool, rage_autostop, false);
	OPTION(bool, rage_autobaim, false);
	OPTION(bool, rage_autocockrevolver, false);
	OPTION(int, rage_baim_after_x_shots, 0);
	OPTION(bool, rage_lagcompensation, false);
	OPTION(int, rage_lagcompensation_type, 0);
	OPTION(bool, rage_fixup_entities, false);
	OPTION(float, rage_mindmg, 0.f);
	OPTION(float, rage_hitchance_amount, 0.f);
	OPTION(int, rage_hitbox, 0);
	OPTION(bool, rage_multipoint, false);
	OPTION(float, rage_pointscale, 0.7f);

	bool rage_multiHitboxes[14] =
	{
		true, true, true, true, true,
		true, true, true, true, true,
		true, true, true, true
	};

	OPTION(bool, rage_prioritize, false);
	OPTION(bool, esp_backtracked_player_skelet, false);
	OPTION(bool, esp_lagcompensated_hitboxes, false);
	OPTION(int, esp_lagcompensated_hitboxes_type, 0);
	OPTION(int, hvh_antiaim_x, 0);
	OPTION(int, hvh_antiaim_y, 0);
	OPTION(int, hvh_antiaim_y_fake, 0);
	OPTION(bool, hvh_antiaim_lby_breaker, false);
	OPTION(bool, hvh_show_real_angles, false);
	OPTION(bool, hvh_resolver, false);
	OPTION(bool, hvh_resolver_override, false);
	OPTION(ButtonCode_t, hvh_resolver_override_key, BUTTON_CODE_NONE);
	OPTION(bool, skinchanger_enabled, false);
	OPTION(bool, debug_window, false);
	OPTION(bool, debug_showposes, false);
	OPTION(bool, debug_showactivities, false);
	OPTION(bool, debug_headbox, false);
	OPTION(bool, debug_fliponkey, false);
	OPTION(ButtonCode_t, debug_flipkey, BUTTON_CODE_NONE);

};

extern const char *opt_EspType[];
extern const char *opt_BoundsType[];
extern const char *opt_WeaponBoxType[];
extern const char *opt_GrenadeESPType[];
extern const char *opt_AimHitboxSpot[];
extern const char *opt_GlowStyles[];
extern const char *opt_Chams[];
extern const char *opt_AimSpot[];
extern const char *opt_MultiHitboxes[14];
extern const char *opt_AApitch[];
extern const char *opt_AAyaw[];
extern const char *opt_AAfakeyaw[];
extern const char *opt_Skynames[];
extern const char *opt_nosmoketype[];
extern int realAimSpot[];
extern int realHitboxSpot[];
extern const char *opt_LagCompType[];
extern bool input_shouldListen;
extern ButtonCode_t* input_receivedKeyval;
extern bool menuOpen;

extern Options g_Options;