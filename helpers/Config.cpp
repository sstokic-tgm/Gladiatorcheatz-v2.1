#include "../SDK.hpp"

#include "Config.hpp"
#include "json.hpp"

#include "../Options.hpp"

#include <fstream>
#include <experimental/filesystem> // hack

nlohmann::json config;

void Config::CreateConfigFolder(std::string path)
{
	if (!std::experimental::filesystem::create_directory(path)) return;
}

bool Config::FileExists(std::string file)
{
	return std::experimental::filesystem::exists(file);
}

void Config::SaveConfig(const std::string path)
{
	std::ofstream output_file(path);

	if (!output_file.good())
		return;

	Save(g_Options.misc_autoaccept, "misc_autoaccept");
	Save(g_Options.misc_revealAllRanks, "misc_revealAllRanks");
	Save(g_Options.misc_bhop, "misc_bhop");
	Save(g_Options.misc_autostrafe, "misc_autostrafe");
	Save(g_Options.misc_auto_pistol, "misc_auto_pistol");
	Save(g_Options.misc_chatspamer, "misc_chatspamer");
	Save(g_Options.misc_thirdperson, "misc_thirdperson");
	Save(g_Options.misc_thirdperson_bind, "misc_thirdperson_bind");
	Save(g_Options.misc_fakewalk, "misc_fakewalk");
	Save(g_Options.misc_fakewalk_bind, "misc_fakewalk_bind");
	Save(g_Options.misc_fakelag_enabled, "misc_fakelag_enabled");
	Save(g_Options.misc_fakelag_value, "misc_fakelag_value");
	Save(g_Options.misc_fakelag_activation_type, "misc_fakelag_activation_type");
	Save(g_Options.misc_fakelag_adaptive, "misc_fakelag_adaptive");
	Save(g_Options.misc_animated_clantag, "misc_animated_clantag");
	Save(g_Options.misc_spectatorlist, "misc_spectatorlist");
	Save(g_Options.misc_logevents, "misc_logevents");
	Save(g_Options.removals_flash, "removals_flash");
	Save(g_Options.removals_smoke, "removals_smoke");
	Save(g_Options.removals_smoke_type, "removals_smoke_type");
	Save(g_Options.removals_scope, "removals_scope");
	Save(g_Options.removals_novisualrecoil, "removals_novisualrecoil");
	Save(g_Options.removals_postprocessing, "removals_postprocessing");
	Save(g_Options.esp_enemies_only, "esp_enemies_only");
	Save(g_Options.esp_farther, "esp_farther");
	Save(g_Options.esp_fill_amount, "esp_fill_amount");
	SaveArray(g_Options.esp_player_fill_color_t, "esp_player_fill_color_t");
	SaveArray(g_Options.esp_player_fill_color_ct, "esp_player_fill_color_ct");
	SaveArray(g_Options.esp_player_fill_color_t_visible, "esp_player_fill_color_t_visible");
	SaveArray(g_Options.esp_player_fill_color_ct_visible, "esp_player_fill_color_ct_visible");
	Save(g_Options.esp_player_boundstype, "esp_player_boundstype");
	Save(g_Options.esp_player_boxtype, "esp_player_boxtype");
	SaveArray(g_Options.esp_player_bbox_color_t, "esp_player_bbox_color_t");
	SaveArray(g_Options.esp_player_bbox_color_ct, "esp_player_bbox_color_ct");
	SaveArray(g_Options.esp_player_bbox_color_t_visible, "esp_player_bbox_color_t_visible");
	SaveArray(g_Options.esp_player_bbox_color_ct_visible, "esp_player_bbox_color_ct_visible");
	Save(g_Options.esp_player_name, "esp_player_name");
	Save(g_Options.esp_player_health, "esp_player_health");
	Save(g_Options.esp_player_weapons, "esp_player_weapons");
	Save(g_Options.esp_player_snaplines, "esp_player_snaplines");
	Save(g_Options.esp_player_chams, "esp_player_chams");
	Save(g_Options.esp_player_chams_type, "esp_player_chams_type");
	SaveArray(g_Options.esp_player_chams_color_t_visible, "esp_player_chams_color_t_visible");
	SaveArray(g_Options.esp_player_chams_color_ct_visible, "esp_player_chams_color_ct_visible");
	SaveArray(g_Options.esp_player_chams_color_t, "esp_player_chams_color_t");
	SaveArray(g_Options.esp_player_chams_color_ct, "esp_player_chams_color_ct");
	Save(g_Options.esp_player_skelet, "esp_player_skelet");
	Save(g_Options.esp_player_anglelines, "esp_player_anglelines");
	Save(g_Options.esp_dropped_weapons, "esp_dropped_weapons");
	Save(g_Options.esp_planted_c4, "esp_planted_c4");
	Save(g_Options.esp_grenades, "esp_grenades");
	Save(g_Options.esp_grenades_type, "esp_grenades_type");
	Save(g_Options.esp_backtracked_player_skelet, "esp_backtracked_player_skelet");
	Save(g_Options.esp_lagcompensated_hitboxes, "esp_lagcompensated_hitboxes");
	Save(g_Options.esp_lagcompensated_hitboxes_type, "esp_lagcompensated_hitboxes_type");
	Save(g_Options.visuals_others_player_fov, "visuals_others_player_fov");
	Save(g_Options.visuals_others_player_fov_viewmodel, "visuals_others_player_fov_viewmodel");
	Save(g_Options.visuals_others_watermark, "visuals_others_watermark");
	Save(g_Options.visuals_others_grenade_pred, "visuals_others_grenade_pred");
	Save(g_Options.visuals_others_hitmarker, "visuals_others_hitmarker");
	Save(g_Options.visuals_others_bulletimpacts, "visuals_others_bulletimpacts");
	SaveArray(g_Options.visuals_others_bulletimpacts_color, "visuals_others_bulletimpacts_color");
	Save(g_Options.visuals_others_sky, "visuals_others_sky");
	Save(g_Options.glow_enabled, "glow_enabled");
	Save(g_Options.glow_players, "glow_players");
	SaveArray(g_Options.glow_player_color_t, "glow_player_color_t");
	SaveArray(g_Options.glow_player_color_ct, "glow_player_color_ct");
	SaveArray(g_Options.glow_player_color_t_visible, "glow_player_color_t_visible");
	SaveArray(g_Options.glow_player_color_ct_visible, "glow_player_color_ct_visible");
	Save(g_Options.glow_players_style, "glow_players_style");
	Save(g_Options.glow_others, "glow_others");
	Save(g_Options.glow_others_style, "glow_others_style");
	Save(g_Options.legit_enabled, "legit_enabled");
	Save(g_Options.legit_aimkey1, "legit_aimkey1");
	Save(g_Options.legit_aimkey2, "legit_aimkey2");
	Save(g_Options.legit_rcs, "legit_rcs");
	Save(g_Options.legit_trigger, "legit_trigger");
	Save(g_Options.legit_trigger_with_aimkey, "legit_trigger_with_aimkey");
	Save(g_Options.legit_preaim, "legit_preaim");
	Save(g_Options.legit_aftershots, "legit_aftershots");
	Save(g_Options.legit_afteraim, "legit_afteraim");
	Save(g_Options.legit_smooth_factor, "legit_smooth_factor");
	Save(g_Options.legit_fov, "legit_fov");
	Save(g_Options.rage_enabled, "rage_enabled");
	Save(g_Options.rage_aimkey, "rage_aimkey");
	Save(g_Options.rage_usekey, "rage_usekey");
	Save(g_Options.rage_silent, "rage_silent");
	Save(g_Options.rage_norecoil, "rage_norecoil");
	Save(g_Options.rage_autoshoot, "rage_autoshoot");
	Save(g_Options.rage_autoscope, "rage_autoscope");
	Save(g_Options.rage_autocrouch, "rage_autocrouch");
	Save(g_Options.rage_autostop, "rage_autostop");
	Save(g_Options.rage_autobaim, "rage_autobaim");
	Save(g_Options.rage_autocockrevolver, "rage_autocockrevolver");
	Save(g_Options.rage_baim_after_x_shots, "rage_baim_after_x_shots");
	Save(g_Options.rage_lagcompensation, "rage_lagcompensation");
	Save(g_Options.rage_lagcompensation_type, "rage_lagcompensation_type");
	Save(g_Options.rage_fixup_entities, "rage_fixup_entities");
	Save(g_Options.rage_mindmg, "rage_mindmg");
	Save(g_Options.rage_hitchance_amount, "rage_hitchance_amount");
	Save(g_Options.rage_hitbox, "rage_hitbox");
	Save(g_Options.rage_prioritize, "rage_prioritize");
	Save(g_Options.rage_multipoint, "rage_multipoint");
	Save(g_Options.rage_pointscale, "rage_pointscale");
	SaveArray(g_Options.rage_multiHitboxes, "rage_multiHitboxes");
	Save(g_Options.hvh_antiaim_x, "hvh_antiaim_x");
	Save(g_Options.hvh_antiaim_y, "hvh_antiaim_y");
	Save(g_Options.hvh_antiaim_y_fake, "hvh_antiaim_y_fake");
	Save(g_Options.hvh_antiaim_lby_breaker, "hvh_antiaim_lby_breaker");
	Save(g_Options.hvh_show_real_angles, "hvh_show_real_angles");
	Save(g_Options.hvh_resolver, "hvh_resolver");
	Save(g_Options.hvh_resolver_override, "hvh_resolver_override");
	Save(g_Options.hvh_resolver_override_key, "hvh_resolver_override_key");
	Save(g_Options.skinchanger_enabled, "skinchanger_enabled");

	output_file << std::setw(4) << config << std::endl;
	output_file.close();
}

void Config::LoadConfig(const std::string path)
{
	std::ifstream input_file(path);

	if (!input_file.good())
		return;

	try
	{
		config << input_file;
	}
	catch (...)
	{
		input_file.close();
		return;
	}

	Load(g_Options.misc_autoaccept, "misc_autoaccept");
	Load(g_Options.misc_revealAllRanks, "misc_revealAllRanks");
	Load(g_Options.misc_bhop, "misc_bhop");
	Load(g_Options.misc_autostrafe, "misc_autostrafe");
	Load(g_Options.misc_auto_pistol, "misc_auto_pistol");
	Load(g_Options.misc_chatspamer, "misc_chatspamer");
	Load(g_Options.misc_thirdperson, "misc_thirdperson");
	Load(g_Options.misc_thirdperson_bind, "misc_thirdperson_bind");
	Load(g_Options.misc_fakewalk, "misc_fakewalk");
	Load(g_Options.misc_fakewalk_bind, "misc_fakewalk_bind");
	Load(g_Options.misc_fakelag_enabled, "misc_fakelag_enabled");
	Load(g_Options.misc_fakelag_value, "misc_fakelag_value");
	Load(g_Options.misc_fakelag_activation_type, "misc_fakelag_activation_type");
	Load(g_Options.misc_fakelag_adaptive, "misc_fakelag_adaptive");
	Load(g_Options.misc_animated_clantag, "misc_animated_clantag");
	Load(g_Options.misc_spectatorlist, "misc_spectatorlist");
	Load(g_Options.misc_logevents, "misc_logevents");
	Load(g_Options.removals_flash, "removals_flash");
	Load(g_Options.removals_smoke, "removals_smoke");
	Load(g_Options.removals_smoke_type, "removals_smoke_type");
	Load(g_Options.removals_scope, "removals_scope");
	Load(g_Options.removals_novisualrecoil, "removals_novisualrecoil");
	Load(g_Options.removals_postprocessing, "removals_postprocessing");
	Load(g_Options.esp_enemies_only, "esp_enemies_only");
	Load(g_Options.esp_farther, "esp_farther");
	Load(g_Options.esp_fill_amount, "esp_fill_amount");
	LoadArray(g_Options.esp_player_fill_color_t, "esp_player_fill_color_t");
	LoadArray(g_Options.esp_player_fill_color_ct, "esp_player_fill_color_ct");
	LoadArray(g_Options.esp_player_fill_color_t_visible, "esp_player_fill_color_t_visible");
	LoadArray(g_Options.esp_player_fill_color_ct_visible, "esp_player_fill_color_ct_visible");
	Load(g_Options.esp_player_boundstype, "esp_player_boundstype");
	Load(g_Options.esp_player_boxtype, "esp_player_boxtype");
	LoadArray(g_Options.esp_player_bbox_color_t, "esp_player_bbox_color_t");
	LoadArray(g_Options.esp_player_bbox_color_ct, "esp_player_bbox_color_ct");
	LoadArray(g_Options.esp_player_bbox_color_t_visible, "esp_player_bbox_color_t_visible");
	LoadArray(g_Options.esp_player_bbox_color_ct_visible, "esp_player_bbox_color_ct_visible");
	Load(g_Options.esp_player_name, "esp_player_name");
	Load(g_Options.esp_player_health, "esp_player_health");
	Load(g_Options.esp_player_weapons, "esp_player_weapons");
	Load(g_Options.esp_player_snaplines, "esp_player_snaplines");
	Load(g_Options.esp_player_chams, "esp_player_chams");
	Load(g_Options.esp_player_chams_type, "esp_player_chams_type");
	LoadArray(g_Options.esp_player_chams_color_t_visible, "esp_player_chams_color_t_visible");
	LoadArray(g_Options.esp_player_chams_color_ct_visible, "esp_player_chams_color_ct_visible");
	LoadArray(g_Options.esp_player_chams_color_t, "esp_player_chams_color_t");
	LoadArray(g_Options.esp_player_chams_color_ct, "esp_player_chams_color_ct");
	Load(g_Options.esp_player_skelet, "esp_player_skelet");
	Load(g_Options.esp_player_anglelines, "esp_player_anglelines");
	Load(g_Options.esp_dropped_weapons, "esp_dropped_weapons");
	Load(g_Options.esp_planted_c4, "esp_planted_c4");
	Load(g_Options.esp_grenades, "esp_grenades");
	Load(g_Options.esp_grenades_type, "esp_grenades_type");
	Load(g_Options.esp_backtracked_player_skelet, "esp_backtracked_player_skelet");
	Load(g_Options.esp_lagcompensated_hitboxes, "esp_lagcompensated_hitboxes");
	Load(g_Options.esp_lagcompensated_hitboxes_type, "esp_lagcompensated_hitboxes_type");
	Load(g_Options.visuals_others_player_fov, "visuals_others_player_fov");
	Load(g_Options.visuals_others_player_fov_viewmodel, "visuals_others_player_fov_viewmodel");
	Load(g_Options.visuals_others_watermark, "visuals_others_watermark");
	Load(g_Options.visuals_others_grenade_pred, "visuals_others_grenade_pred");
	Load(g_Options.visuals_others_hitmarker, "visuals_others_hitmarker");
	Load(g_Options.visuals_others_bulletimpacts, "visuals_others_bulletimpacts");
	LoadArray(g_Options.visuals_others_bulletimpacts_color, "visuals_others_bulletimpacts_color");
	Load(g_Options.visuals_others_sky, "visuals_others_sky");
	Load(g_Options.glow_enabled, "glow_enabled");
	Load(g_Options.glow_players, "glow_players");
	LoadArray(g_Options.glow_player_color_t, "glow_player_color_t");
	LoadArray(g_Options.glow_player_color_ct, "glow_player_color_ct");
	LoadArray(g_Options.glow_player_color_t_visible, "glow_player_color_t_visible");
	LoadArray(g_Options.glow_player_color_ct_visible, "glow_player_color_ct_visible");
	Load(g_Options.glow_players_style, "glow_players_style");
	Load(g_Options.glow_others, "glow_others");
	Load(g_Options.glow_others_style, "glow_others_style");
	Load(g_Options.legit_enabled, "legit_enabled");
	Load(g_Options.legit_aimkey1, "legit_aimkey1");
	Load(g_Options.legit_aimkey2, "legit_aimkey2");
	Load(g_Options.legit_rcs, "legit_rcs");
	Load(g_Options.legit_trigger, "legit_trigger");
	Load(g_Options.legit_trigger_with_aimkey, "legit_trigger_with_aimkey");
	Load(g_Options.legit_preaim, "legit_preaim");
	Load(g_Options.legit_aftershots, "legit_aftershots");
	Load(g_Options.legit_afteraim, "legit_afteraim");
	Load(g_Options.legit_smooth_factor, "legit_smooth_factor");
	Load(g_Options.legit_fov, "legit_fov");
	Load(g_Options.rage_enabled, "rage_enabled");
	Load(g_Options.rage_aimkey, "rage_aimkey");
	Load(g_Options.rage_usekey, "rage_usekey");
	Load(g_Options.rage_silent, "rage_silent");
	Load(g_Options.rage_norecoil, "rage_norecoil");
	Load(g_Options.rage_autoshoot, "rage_autoshoot");
	Load(g_Options.rage_autoscope, "rage_autoscope");
	Load(g_Options.rage_autocrouch, "rage_autocrouch");
	Load(g_Options.rage_autostop, "rage_autostop");
	Load(g_Options.rage_autobaim, "rage_autobaim");
	Load(g_Options.rage_autocockrevolver, "rage_autocockrevolver");
	Load(g_Options.rage_baim_after_x_shots, "rage_baim_after_x_shots");
	Load(g_Options.rage_lagcompensation, "rage_lagcompensation");
	Load(g_Options.rage_lagcompensation_type, "rage_lagcompensation_type");
	Load(g_Options.rage_fixup_entities, "rage_fixup_entities");
	Load(g_Options.rage_mindmg, "rage_mindmg");
	Load(g_Options.rage_hitchance_amount, "rage_hitchance_amount");
	Load(g_Options.rage_hitbox, "rage_hitbox");
	Load(g_Options.rage_prioritize, "rage_prioritize");
	Load(g_Options.rage_multipoint, "rage_multipoint");
	Load(g_Options.rage_pointscale, "rage_pointscale");
	LoadArray(g_Options.rage_multiHitboxes, "rage_multiHitboxes");
	Load(g_Options.hvh_antiaim_x, "hvh_antiaim_x");
	Load(g_Options.hvh_antiaim_y, "hvh_antiaim_y");
	Load(g_Options.hvh_antiaim_y_fake, "hvh_antiaim_y_fake");
	Load(g_Options.hvh_antiaim_lby_breaker, "hvh_antiaim_lby_breaker");
	Load(g_Options.hvh_show_real_angles, "hvh_show_real_angles");
	Load(g_Options.hvh_resolver, "hvh_resolver");
	Load(g_Options.hvh_resolver_override, "hvh_resolver_override");
	Load(g_Options.hvh_resolver_override_key, "hvh_resolver_override_key");
	Load(g_Options.skinchanger_enabled, "skinchanger_enabled");

	input_file.close();
}

std::vector<std::string> Config::GetAllConfigs()
{
	namespace fs = std::experimental::filesystem;

	std::string fPath = std::string(Global::my_documents_folder) + "\\gladiatorcheatz\\";

	std::vector<ConfigFile> config_files = GetAllConfigsInFolder(fPath, ".glad");
	std::vector<std::string> config_file_names;

	for (auto config = config_files.begin(); config != config_files.end(); config++)
		config_file_names.emplace_back(config->GetName());

	std::sort(config_file_names.begin(), config_file_names.end());

	return config_file_names;
}

std::vector<ConfigFile> Config::GetAllConfigsInFolder(const std::string path, const std::string ext)
{
	namespace fs = std::experimental::filesystem;

	std::vector<ConfigFile> config_files;

	if (fs::exists(path) && fs::is_directory(path))
	{
		for (auto it = fs::recursive_directory_iterator(path); it != fs::recursive_directory_iterator(); it++)
		{
			if (fs::is_regular_file(*it) && it->path().extension() == ext)
			{
				std::string fPath = path + it->path().filename().string();

				std::string tmp_f_name = it->path().filename().string();
				size_t pos = tmp_f_name.find(".");
				std::string fName = (std::string::npos == pos) ? tmp_f_name : tmp_f_name.substr(0, pos);

				ConfigFile new_config(fName, fPath);
				config_files.emplace_back(new_config);
			}
		}
	}
	return config_files;
}

template<typename T>
void Config::Load(T &value, std::string str)
{
	if (config[str].empty())
		return;

	value = config[str].get<T>();
}

void Config::LoadArray(float_t value[4], std::string str)
{
	if (config[str]["0"].empty() || config[str]["1"].empty() || config[str]["2"].empty() || config[str]["3"].empty())
		return;

	value[0] = config[str]["0"].get<float_t>();
	value[1] = config[str]["1"].get<float_t>();
	value[2] = config[str]["2"].get<float_t>();
	value[3] = config[str]["3"].get<float_t>();
}

void Config::LoadArray(bool value[14], std::string str)
{
	if (config[str]["0"].empty() || config[str]["1"].empty() || config[str]["2"].empty() || config[str]["3"].empty()
		|| config[str]["4"].empty() || config[str]["5"].empty() || config[str]["6"].empty() || config[str]["7"].empty()
		|| config[str]["8"].empty() || config[str]["9"].empty() || config[str]["10"].empty() || config[str]["11"].empty()
		|| config[str]["12"].empty() || config[str]["13"].empty())
		return;

	value[0] = config[str]["0"].get<bool>();
	value[1] = config[str]["1"].get<bool>();
	value[2] = config[str]["2"].get<bool>();
	value[3] = config[str]["3"].get<bool>();
	value[4] = config[str]["4"].get<bool>();
	value[5] = config[str]["5"].get<bool>();
	value[6] = config[str]["6"].get<bool>();
	value[7] = config[str]["7"].get<bool>();
	value[8] = config[str]["8"].get<bool>();
	value[9] = config[str]["9"].get<bool>();
	value[10] = config[str]["10"].get<bool>();
	value[11] = config[str]["11"].get<bool>();
	value[12] = config[str]["12"].get<bool>();
	value[13] = config[str]["13"].get<bool>();
}

template<typename T>
void Config::Save(T &value, std::string str)
{
	config[str] = value;
}

void Config::SaveArray(float_t value[4], std::string str)
{
	config[str]["0"] = value[0];
	config[str]["1"] = value[1];
	config[str]["2"] = value[2];
	config[str]["3"] = value[3];
}

void Config::SaveArray(bool value[14], std::string str)
{
	config[str]["0"] = value[0];
	config[str]["1"] = value[1];
	config[str]["2"] = value[2];
	config[str]["3"] = value[3];
	config[str]["4"] = value[4];
	config[str]["5"] = value[5];
	config[str]["6"] = value[6];
	config[str]["7"] = value[7];
	config[str]["8"] = value[8];
	config[str]["9"] = value[9];
	config[str]["10"] = value[10];
	config[str]["11"] = value[11];
	config[str]["12"] = value[12];
	config[str]["13"] = value[13];
}