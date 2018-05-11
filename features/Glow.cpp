#include "Glow.hpp"

#include "../Structs.hpp"
#include "../Options.hpp"
#include "Visuals.hpp"

void Glow::RenderGlow()
{
	for (auto i = 0; i < g_GlowObjManager->m_GlowObjectDefinitions.Count(); i++)
	{
		auto &glowObject = g_GlowObjManager->m_GlowObjectDefinitions[i];
		auto entity = reinterpret_cast<C_BasePlayer*>(glowObject.m_pEntity);

		if (glowObject.IsUnused())
			continue;

		if (!entity)
			continue;

		auto class_id = entity->GetClientClass()->m_ClassID;
		Color color;

		switch (class_id)
		{
		case ClassId_CCSPlayer:
		{
			bool is_enemy = entity->m_iTeamNum() != g_LocalPlayer->m_iTeamNum();
			bool playerTeam = entity->m_iTeamNum() == 2;

			if (!g_Options.glow_players || !Visuals::ValidPlayer(entity, false))
				continue;

			if (!is_enemy && g_Options.esp_enemies_only)
				continue;

			glowObject.m_nGlowStyle = g_Options.glow_players_style;

			bool vis = Visuals::IsVisibleScan(entity);
			float* cur_color = (playerTeam ? (vis ? g_Options.glow_player_color_t_visible : g_Options.glow_player_color_t) : (vis ? g_Options.glow_player_color_ct_visible : g_Options.glow_player_color_ct));

			color.SetColor(cur_color);

			break;
		}
		
		case ClassId_CBaseAnimating:

			if (!g_Options.glow_others)
				continue;

			glowObject.m_nGlowStyle = g_Options.glow_others_style;

			color = Color::Blue;

			break;

		case ClassId_CPlantedC4:

			if (!g_Options.glow_others)
				continue;

			glowObject.m_nGlowStyle = g_Options.glow_others_style;

			color = Color::Red;

			break;

		default:
		{
			if (entity->IsWeapon())
			{
				if (!g_Options.glow_others)
					continue;

				glowObject.m_nGlowStyle = g_Options.glow_others_style;

				color = Color(70, 255, 70, 255);
			}
		}
		}

		glowObject.m_flRed = color.r() / 255.0f;
		glowObject.m_flGreen = color.g() / 255.0f;
		glowObject.m_flBlue = color.b() / 255.0f;
		glowObject.m_flAlpha = color.a() / 255.0f;
		glowObject.m_bRenderWhenOccluded = true;
		glowObject.m_bRenderWhenUnoccluded = false;
	}
}

void Glow::ClearGlow()
{
	for (auto i = 0; i < g_GlowObjManager->m_GlowObjectDefinitions.Count(); i++)
	{
		auto &glowObject = g_GlowObjManager->m_GlowObjectDefinitions[i];
		auto entity = reinterpret_cast<C_BasePlayer*>(glowObject.m_pEntity);

		if (glowObject.IsUnused())
			continue;

		if (!entity || entity->IsDormant())
			continue;

		glowObject.m_flAlpha = 0.0f;
	}
}