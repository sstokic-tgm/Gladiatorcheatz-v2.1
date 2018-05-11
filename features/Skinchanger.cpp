#include "../Structs.hpp"

#include "Skinchanger.hpp"

#include "../helpers/json.hpp"

#include <fstream>
#include <experimental/filesystem> // hack

const std::map<size_t, Item_t> k_weapon_info =
{
	{ WEAPON_KNIFE,{ "models/weapons/v_knife_default_ct.mdl", "knife_default_ct" } },
	{ WEAPON_KNIFE_T,{ "models/weapons/v_knife_default_t.mdl", "knife_t" } },
	{ WEAPON_KNIFE_BAYONET,{ "models/weapons/v_knife_bayonet.mdl", "bayonet" } },
	{ WEAPON_KNIFE_FLIP,{ "models/weapons/v_knife_flip.mdl", "knife_flip" } },
	{ WEAPON_KNIFE_GUT,{ "models/weapons/v_knife_gut.mdl", "knife_gut" } },
	{ WEAPON_KNIFE_KARAMBIT,{ "models/weapons/v_knife_karam.mdl", "knife_karambit" } },
	{ WEAPON_KNIFE_M9_BAYONET,{ "models/weapons/v_knife_m9_bay.mdl", "knife_m9_bayonet" } },
	{ WEAPON_KNIFE_TACTICAL,{ "models/weapons/v_knife_tactical.mdl", "knife_tactical" } },
	{ WEAPON_KNIFE_FALCHION,{ "models/weapons/v_knife_falchion_advanced.mdl", "knife_falchion" } },
	{ WEAPON_KNIFE_SURVIVAL_BOWIE,{ "models/weapons/v_knife_survival_bowie.mdl", "knife_survival_bowie" } },
	{ WEAPON_KNIFE_BUTTERFLY,{ "models/weapons/v_knife_butterfly.mdl", "knife_butterfly" } },
	{ WEAPON_KNIFE_PUSH,{ "models/weapons/v_knife_push.mdl", "knife_push" } },
	{ GLOVE_STUDDED_BLOODHOUND,{ "models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl" } },
	{ GLOVE_T_SIDE,{ "models/weapons/v_models/arms/glove_fingerless/v_glove_fingerless.mdl" } },
	{ GLOVE_CT_SIDE,{ "models/weapons/v_models/arms/glove_hardknuckle/v_glove_hardknuckle.mdl" } },
	{ GLOVE_SPORTY,{ "models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl" } },
	{ GLOVE_SLICK,{ "models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl" } },
	{ GLOVE_LEATHER_WRAP,{ "models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl" } },
	{ GLOVE_MOTORCYCLE,{ "models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl" } },
	{ GLOVE_SPECIALIST,{ "models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl" } }
};

const std::vector<WeaponName_t> k_knife_names =
{
	{ 0, "Default" },
	{ WEAPON_KNIFE_BAYONET, "Bayonet" },
	{ WEAPON_KNIFE_FLIP, "Flip Knife" },
	{ WEAPON_KNIFE_GUT, "Gut Knife" },
	{ WEAPON_KNIFE_KARAMBIT, "Karambit" },
	{ WEAPON_KNIFE_M9_BAYONET, "M9 Bayonet" },
	{ WEAPON_KNIFE_TACTICAL, "Huntsman Knife" },
	{ WEAPON_KNIFE_FALCHION, "Falchion Knife" },
	{ WEAPON_KNIFE_SURVIVAL_BOWIE, "Bowie Knife" },
	{ WEAPON_KNIFE_BUTTERFLY, "Butterfly Knife" },
	{ WEAPON_KNIFE_PUSH, "Shadow Daggers" }
};

const std::vector<WeaponName_t> k_glove_names =
{
	{ 0, "Default" },
	{ GLOVE_STUDDED_BLOODHOUND, "Bloodhound" },
	{ GLOVE_T_SIDE, "Default (Terrorists)" },
	{ GLOVE_CT_SIDE, "Default (Counter-Terrorists)" },
	{ GLOVE_SPORTY, "Sporty" },
	{ GLOVE_SLICK, "Slick" },
	{ GLOVE_LEATHER_WRAP, "Handwrap" },
	{ GLOVE_MOTORCYCLE, "Motorcycle" },
	{ GLOVE_SPECIALIST, "Specialist" }
};

const std::vector<WeaponName_t> k_weapon_names =
{
	{ WEAPON_KNIFE, "Knife" },
	{ GLOVE_T_SIDE, "Glove" },
	{ 7, "AK-47" },
	{ 8, "AUG" },
	{ 9, "AWP" },
	{ 63, "CZ-75" },
	{ 1, "Desert Eagle" },
	{ 2, "Dual Berettas" },
	{ 10, "FAMAS" },
	{ 3, "Five-SeveN" },
	{ 11, "G3SG1" },
	{ 13, "Galil AR" },
	{ 4, "Glock-18" },
	{ 14, "M249" },
	{ 60, "M4A1-S" },
	{ 16, "M4A4" },
	{ 17, "MAC-10" },
	{ 27, "MAG-7" },
	{ 33, "MP7" },
	{ 34, "MP9" },
	{ 28, "Negev" },
	{ 35, "Nova" },
	{ 32, "P2000" },
	{ 36, "P250" },
	{ 19, "P90" },
	{ 26, "PP-Bizon" },
	{ 64, "R8 Revolver" },
	{ 29, "Sawed-Off" },
	{ 38, "SCAR-20" },
	{ 40, "SSG 08" },
	{ 39, "SG-553" },
	{ 30, "Tec-9" },
	{ 24, "UMP-45" },
	{ 61, "USP-S" },
	{ 25, "XM1014" },
};

const std::vector<QualityName_t> k_quality_names =
{
	{ 0, "Default" },
	{ 1, "Genuine" },
	{ 2, "Vintage" },
	{ 3, "Unusual" },
	{ 5, "Community" },
	{ 6, "Developer" },
	{ 7, "Self-Made" },
	{ 8, "Customized" },
	{ 9, "Strange" },
	{ 10, "Completed" },
	{ 12, "Tournament" }
};

enum ESequence
{
	SEQUENCE_DEFAULT_DRAW = 0,
	SEQUENCE_DEFAULT_IDLE1 = 1,
	SEQUENCE_DEFAULT_IDLE2 = 2,
	SEQUENCE_DEFAULT_LIGHT_MISS1 = 3,
	SEQUENCE_DEFAULT_LIGHT_MISS2 = 4,
	SEQUENCE_DEFAULT_HEAVY_MISS1 = 9,
	SEQUENCE_DEFAULT_HEAVY_HIT1 = 10,
	SEQUENCE_DEFAULT_HEAVY_BACKSTAB = 11,
	SEQUENCE_DEFAULT_LOOKAT01 = 12,

	SEQUENCE_BUTTERFLY_DRAW = 0,
	SEQUENCE_BUTTERFLY_DRAW2 = 1,
	SEQUENCE_BUTTERFLY_LOOKAT01 = 13,
	SEQUENCE_BUTTERFLY_LOOKAT03 = 15,

	SEQUENCE_FALCHION_IDLE1 = 1,
	SEQUENCE_FALCHION_HEAVY_MISS1 = 8,
	SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP = 9,
	SEQUENCE_FALCHION_LOOKAT01 = 12,
	SEQUENCE_FALCHION_LOOKAT02 = 13,

	SEQUENCE_DAGGERS_IDLE1 = 1,
	SEQUENCE_DAGGERS_LIGHT_MISS1 = 2,
	SEQUENCE_DAGGERS_LIGHT_MISS5 = 6,
	SEQUENCE_DAGGERS_HEAVY_MISS2 = 11,
	SEQUENCE_DAGGERS_HEAVY_MISS1 = 12,

	SEQUENCE_BOWIE_IDLE1 = 1,
};

inline int RandomSequence(int low, int high)
{
	return rand() % (high - low + 1) + low;
}

// Map of animation fixes
// unfortunately can't be constexpr
const static std::unordered_map<std::string, int(*)(int)> animation_fix_map
{
	{ "models/weapons/v_knife_butterfly.mdl", [](int sequence) -> int
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_DRAW:
			return RandomSequence(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
		case SEQUENCE_DEFAULT_LOOKAT01:
			return RandomSequence(SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03);
		default:
			return sequence + 1;
		}
	} },
	{ "models/weapons/v_knife_falchion_advanced.mdl", [](int sequence) -> int
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_IDLE2:
			return SEQUENCE_FALCHION_IDLE1;
		case SEQUENCE_DEFAULT_HEAVY_MISS1:
			return RandomSequence(SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP);
		case SEQUENCE_DEFAULT_LOOKAT01:
			return RandomSequence(SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02);
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1:
			return sequence;
		default:
			return sequence - 1;
		}
	} },
	{ "models/weapons/v_knife_push.mdl", [](int sequence) -> int
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_IDLE2:
			return SEQUENCE_DAGGERS_IDLE1;
		case SEQUENCE_DEFAULT_LIGHT_MISS1:
		case SEQUENCE_DEFAULT_LIGHT_MISS2:
			return RandomSequence(SEQUENCE_DAGGERS_LIGHT_MISS1, SEQUENCE_DAGGERS_LIGHT_MISS5);
		case SEQUENCE_DEFAULT_HEAVY_MISS1:
			return RandomSequence(SEQUENCE_DAGGERS_HEAVY_MISS2, SEQUENCE_DAGGERS_HEAVY_MISS1);
		case SEQUENCE_DEFAULT_HEAVY_HIT1:
		case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
		case SEQUENCE_DEFAULT_LOOKAT01:
			return sequence + 3;
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1:
			return sequence;
		default:
			return sequence + 2;
		}
	} },
	{ "models/weapons/v_knife_survival_bowie.mdl", [](int sequence) -> int
	{
		switch (sequence)
		{
		case SEQUENCE_DEFAULT_DRAW:
		case SEQUENCE_DEFAULT_IDLE1:
			return sequence;
		case SEQUENCE_DEFAULT_IDLE2:
			return SEQUENCE_BOWIE_IDLE1;
		default:
			return sequence - 1;
		}
	} }
};

void Proxies::nSequence(const CRecvProxyData *pData, void *pStruct, void *pOut)
{
	auto local = C_BasePlayer::GetPlayerByIndex(g_EngineClient->GetLocalPlayer());

	if (!local || !local->IsAlive())
		return o_nSequence(pData, pStruct, pOut);

	CRecvProxyData *proxy_data = const_cast<CRecvProxyData*>(pData);
	C_BaseViewModel *view_model = static_cast<C_BaseViewModel*>(pStruct);

	if (view_model && view_model->m_hOwner() && view_model->m_hOwner().IsValid())
	{
		auto owner = view_model->m_hOwner().Get();

		if (owner == local)
		{
			// Get the filename of the current view model.
			auto knife_model = g_MdlInfo->GetModel(view_model->m_nModelIndex());
			auto model_name = g_MdlInfo->GetModelName(knife_model);

			if (animation_fix_map.count(model_name))
				proxy_data->m_Value.m_Int = animation_fix_map.at(model_name)(proxy_data->m_Value.m_Int);
		}
	}

	o_nSequence(proxy_data, pStruct, pOut);
}

bool IsKnife(int i)
{
	return (i >= WEAPON_KNIFE_BAYONET && i < GLOVE_STUDDED_BLOODHOUND) || i == WEAPON_KNIFE_T || i == WEAPON_KNIFE;
}

void clearRefCountedVector(CUtlVector<IRefCounted*>& vec)
{
	for (auto &elem : vec)
	{
		if (elem)
		{
			elem->unreference();
			elem = nullptr;
		}
	}
	vec.RemoveAll();
}

void ForceItemUpdate(C_WeaponCSBase *item)
{
	if (!item)
		return;

	C_EconItemView &view = item->m_AttributeManager.m_Item;

	item->m_bCustomMaterialInitialized = (reinterpret_cast<C_BaseAttributableItem*>(item)->m_nFallbackPaintKit() <= 0);

	item->m_CustomMaterials.RemoveAll(); // clear vector, but don't unreference items
	view.m_CustomMaterials.RemoveAll();
	clearRefCountedVector(view.m_VisualsDataProcessors); // prevent memory leak

	item->PostDataUpdate(0);
	item->OnDataChanged(0);
}

static void EraseOverrideIfExistsByIndex(int definition_index)
{
	// We have info about the item not needed to be overridden
	if (k_weapon_info.count(definition_index))
	{
		auto &icon_override_map = Skinchanger::Get().GetIconOverrideMap();

		const auto &original_item = k_weapon_info.at(definition_index);

		// We are overriding its icon when not needed
		if (original_item.icon && icon_override_map.count(original_item.icon))
			icon_override_map.erase(icon_override_map.at(original_item.icon)); // Remove the leftover override
	}
}

static void ApplyConfigOnAttributableItem(C_BaseAttributableItem *item, const EconomyItem_t *config, unsigned xuid_low)
{
	if (config->definition_index != GLOVE_T_SIDE && config->definition_override_index != GLOVE_T_SIDE)
	{
		auto world_model_handle = item->m_hWeaponWorldModel();

		if (!world_model_handle.IsValid())
			return;

		auto view_model_weapon = world_model_handle.Get();

		if (!view_model_weapon)
			return;

		view_model_weapon->m_nModelIndex() = item->m_nModelIndex() + 1;
	}

	// Force fallback values to be used.
	item->m_iItemIDHigh() = -1;

	// Set the owner of the weapon to our lower XUID. (fixes StatTrak)
	item->m_iAccountID() = xuid_low;

	if (config->entity_quality_index)
		item->m_iEntityQuality() = config->entity_quality_index;

	if (config->custom_name[0])
		strcpy(item->m_szCustomName(), config->custom_name);

	if (config->paint_kit_index)
		item->m_nFallbackPaintKit() = config->paint_kit_index;

	if (config->seed)
		item->m_nFallbackSeed() = config->seed;

	if (config->stat_trak)
		item->m_nFallbackStatTrak() = config->stat_trak;

	item->m_flFallbackWear() = config->wear;

	auto &definition_index = item->m_iItemDefinitionIndex();

	auto &icon_override_map = Skinchanger::Get().GetIconOverrideMap();

	if (config->definition_override_index // We need to override defindex
		&& config->definition_override_index != definition_index // It is not yet overridden
		&& k_weapon_info.count(config->definition_override_index)) // We have info about what we gonna override it to
	{
		unsigned old_definition_index = definition_index;

		definition_index = config->definition_override_index;

		const auto &replacement_item = k_weapon_info.at(config->definition_override_index);

		// Set the weapon model index -- required for paint kits to work on replacement items after the 29/11/2016 update.
		//item->GetModelIndex() = g_model_info->GetModelIndex(k_weapon_info.at(config->definition_override_index).model);
		item->SetModelIndex(g_MdlInfo->GetModelIndex(replacement_item.model));
		item->GetClientNetworkable()->PreDataUpdate(0);

		// We didn't override 0, but some actual weapon, that we have data for
		if (old_definition_index && k_weapon_info.count(old_definition_index))
		{
			const auto &original_item = k_weapon_info.at(old_definition_index);

			if (original_item.icon && replacement_item.icon)
				icon_override_map[original_item.icon] = replacement_item.icon;
		}
	}
	else
	{
		EraseOverrideIfExistsByIndex(definition_index);
	}
}

static CreateClientClassFn GetWearableCreateFn()
{
	auto clazz = g_CHLClient->GetAllClasses();

	while (strcmp(clazz->m_pNetworkName, "CEconWearable"))
		clazz = clazz->m_pNext;

	return clazz->m_pCreateFn;
}

void Skinchanger::Work()
{
	auto local_index = g_EngineClient->GetLocalPlayer();
	auto local = C_BasePlayer::GetPlayerByIndex(local_index);
	if (!local)
		return;

	player_info_t player_info;

	if (!g_EngineClient->GetPlayerInfo(local_index, &player_info))
		return;

	// Handle glove config
	{
		auto wearables = local->m_hMyWearables();

		auto glove_config = this->GetByDefinitionIndex(GLOVE_T_SIDE);

		static auto glove_handle = CBaseHandle(0);

		auto glove = wearables[0].Get();

		if (!glove) // There is no glove
		{
			// Try to get our last created glove
			auto our_glove = reinterpret_cast<C_BaseAttributableItem*>(g_EntityList->GetClientEntityFromHandle(glove_handle));

			if (our_glove) // Our glove still exists
			{
				wearables[0] = glove_handle;
				glove = our_glove;
			}
		}

		if (!local->IsAlive())
		{
			// We are dead but we have a glove, destroy it
			if (glove)
			{
				glove->GetClientNetworkable()->SetDestroyedOnRecreateEntities();
				glove->GetClientNetworkable()->Release();
			}
			return;
		}

		if (glove_config && glove_config->definition_override_index)
		{
			// We don't have a glove, but we should
			if (!glove)
			{
				static auto create_wearable_fn = GetWearableCreateFn();

				auto entry = g_EntityList->GetHighestEntityIndex() + 1;
				auto serial = rand() % 0x1000;

				create_wearable_fn(entry, serial);
				glove = reinterpret_cast<C_BaseAttributableItem*>(g_EntityList->GetClientEntity(entry));

				// He he
				{
					static auto set_abs_origin_fn = reinterpret_cast< void(__thiscall*)(void*, const Vector&) >
						(Utils::PatternScan(GetModuleHandle("client.dll"), "55 8B EC 83 E4 F8 51 53 56 57 8B F1"));

					static const Vector new_pos = { 10000.f, 10000.f, 10000.f };

					set_abs_origin_fn(glove, new_pos);
				}

				wearables[0].Init(entry, serial, 16);

				// Let's store it in case we somehow lose it.
				glove_handle = wearables[0];
			}

			// Thanks, Beakers
			*reinterpret_cast<int*>(uintptr_t(glove) + 0x64) = -1;

			ApplyConfigOnAttributableItem(glove, glove_config, player_info.xuid_low);
		}
	}

	// Handle weapon configs
	{
		auto weapons = local->m_hMyWeapons();

		for (size_t i = 0; weapons[i].IsValid(); i++)
		{
			auto weapon = weapons[i].Get();

			if (!weapon)
				continue;

			auto& definition_index = weapon->m_iItemDefinitionIndex();

			// All knives are terrorist knives.
			if (auto active_conf = this->GetByDefinitionIndex(IsKnife(definition_index) ? WEAPON_KNIFE : definition_index))
				ApplyConfigOnAttributableItem(weapon, active_conf, player_info.xuid_low);
			else
				EraseOverrideIfExistsByIndex(definition_index);
		}
	}

	auto view_model_handle = local->m_hViewModel();

	if (!view_model_handle.IsValid())
		return;

	auto view_model = view_model_handle.Get();

	if (!view_model)
		return;

	auto view_model_weapon_handle = view_model->m_hWeapon();

	if (!view_model_weapon_handle.IsValid())
		return;

	auto view_model_weapon = view_model_weapon_handle.Get();

	if (!view_model_weapon)
		return;

	if (k_weapon_info.count(view_model_weapon->m_iItemDefinitionIndex()))
	{
		auto& override_model = k_weapon_info.at(view_model_weapon->m_iItemDefinitionIndex()).model;
		view_model->m_nModelIndex() = g_MdlInfo->GetModelIndex(override_model);
	}

	if (bForceFullUpdate)
	{
		ForceItemUpdates();
		bForceFullUpdate = false;
	}
}

void Skinchanger::ForceItemUpdates()
{
	auto local = C_BasePlayer::GetPlayerByIndex(g_EngineClient->GetLocalPlayer());
	if (local)
	{
		auto weapons = local->m_hMyWeapons();

		for (size_t i = 0; weapons[i].IsValid(); i++)
		{
			auto weapon = weapons[i].Get();

			ForceItemUpdate(reinterpret_cast<C_WeaponCSBase*>(weapon));
		}
	}
}

using json = nlohmann::json;

void to_json(json& j, const EconomyItem_t& item)
{
	j = json
	{
		{ "name", item.name },
		{ "enabled", item.enabled },
		{ "definition_index", item.definition_index },
		{ "entity_quality_index", item.entity_quality_index },
		{ "paint_kit_index", item.paint_kit_index },
		{ "definition_override_index", item.definition_override_index },
		{ "seed", item.seed },
		{ "stat_trak", item.stat_trak },
		{ "wear", item.wear },
		{ "custom_name", item.custom_name },
	};
}

void from_json(const json& j, EconomyItem_t& item)
{
	strcpy_s(item.name, j.at("name").get<std::string>().c_str());
	item.enabled = j.at("enabled").get<bool>();
	item.definition_index = j.at("definition_index").get<int>();
	item.entity_quality_index = j.at("entity_quality_index").get<int>();
	item.paint_kit_index = j.at("paint_kit_index").get<int>();
	item.definition_override_index = j.at("definition_override_index").get<int>();
	item.seed = j.at("seed").get<int>();
	item.stat_trak = j.at("stat_trak").get<int>();
	item.wear = j.at("wear").get<float>();
	strcpy_s(item.custom_name, j.at("custom_name").get<std::string>().c_str());
	item.UpdateIds();
}

void Skinchanger::SaveSkins()
{
	std::string fPath = std::string(Global::my_documents_folder) + "\\gladiatorcheatz\\" + "skinchanger.json";

	std::ofstream(fPath) << json(m_items);
}

void Skinchanger::LoadSkins()
{
	auto compareFunction = [](const EconomyItem_t& a, const EconomyItem_t& b) { return std::string(a.name) < std::string(b.name); };
	
	std::string fPath = std::string(Global::my_documents_folder) + "\\gladiatorcheatz\\" + "skinchanger.json";

	if (!Config::Get().FileExists(fPath))
		return;

	try
	{
		m_items = json::parse(std::ifstream(fPath)).get<std::vector<EconomyItem_t>>();
		std::sort(m_items.begin(), m_items.end(), compareFunction);
		ForceItemUpdates();
	}
	catch (const std::exception&) {}
}

EconomyItem_t *Skinchanger::GetByDefinitionIndex(int definition_index)
{
	for (auto& x : m_items)
		if (x.enabled && x.definition_index == definition_index)
			return &x;

	return nullptr;
}