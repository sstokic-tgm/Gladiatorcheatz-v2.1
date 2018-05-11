#include "Visuals.hpp"
#include "../Options.hpp"
#include "../Structs.hpp"
#include "../helpers/Math.hpp"
#include "LagCompensation.hpp"

namespace Visuals
{
	vgui::HFont weapon_font;
	vgui::HFont ui_font;
	vgui::HFont watermark_font;
	vgui::HFont spectatorlist_font;
	VisualsStruct ESP_ctx;
	float ESP_Fade[64];
	C_BasePlayer* local_observed;
}

RECT GetBBox(C_BaseEntity* ent, Vector pointstransf[])
{
	RECT rect{};
	auto collideable = ent->GetCollideable();

	if (!collideable)
		return rect;

	auto min = collideable->OBBMins();
	auto max = collideable->OBBMaxs();

	const matrix3x4_t &trans = ent->m_rgflCoordinateFrame();

	Vector points[] =
	{
		Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),
		Vector(max.x, max.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(min.x, min.y, max.z),
		Vector(max.x, min.y, max.z)
	};

	Vector pointsTransformed[8];
	for (int i = 0; i < 8; i++) {
		Math::VectorTransform(points[i], trans, pointsTransformed[i]);
	}

	Vector pos = ent->m_vecOrigin();
	Vector screen_points[8] = {};

	for (int i = 0; i < 8; i++)
		if (!Math::WorldToScreen(pointsTransformed[i], screen_points[i]))
			return rect;
		else
			pointstransf[i] = screen_points[i];

	auto left = screen_points[0].x;
	auto top = screen_points[0].y;
	auto right = screen_points[0].x;
	auto bottom = screen_points[0].y;

	for (int i = 1; i < 8; i++)
	{
		if (left > screen_points[i].x)
			left = screen_points[i].x;
		if (top < screen_points[i].y)
			top = screen_points[i].y;
		if (right < screen_points[i].x)
			right = screen_points[i].x;
		if (bottom > screen_points[i].y)
			bottom = screen_points[i].y;
	}
	return RECT{ (long)left, (long)top, (long)right, (long)bottom };
}

void Visuals::RenderNadeEsp(C_BaseCombatWeapon* nade)
{
	if (!g_Options.esp_grenades)
		return;

	const model_t* model = nade->GetModel();
	if (!model)
		return;

	studiohdr_t* hdr = g_MdlInfo->GetStudiomodel(model);
	if (!hdr)
		return;

	Color Nadecolor;
	std::string entityName = hdr->szName, icon_character;
	switch (nade->GetClientClass()->m_ClassID)
	{
	case 9:
		if (entityName[16] == 's')
		{
			Nadecolor = Color(255, 255, 0, 200);
			entityName = "Flash";
			icon_character = "G";
		}
		else
		{
			Nadecolor = Color(255, 0, 0, 200);
			entityName = "Frag";
			icon_character = "H";
		}
		break;
	case 134:
		Nadecolor = Color(170, 170, 170, 200);
		entityName = "Smoke";
		icon_character = "P";
		break;
	case 98:
		Nadecolor = Color(255, 0, 0, 200);
		entityName = "Fire";
		icon_character = "P";
		break;
	case 41:
		Nadecolor = Color(255, 255, 0, 200);
		entityName = "Decoy";
		icon_character = "G";
		break;
	default:
		return;
	}

	Vector points_transformed[8];
	RECT size = GetBBox(nade, points_transformed);
	if (size.right == 0 || size.bottom == 0)
		return;

	int width, height, width_icon, height_icon;
	Visuals::GetTextSize(ui_font, entityName.c_str(), width, height);
	Visuals::GetTextSize(weapon_font, icon_character.c_str(), width_icon, height_icon);

	// + distance? just make it customizable
	switch (g_Options.esp_grenades_type)
	{
	case 1:
		g_VGuiSurface->DrawSetColor(Color(20, 20, 20, 240));
		g_VGuiSurface->DrawLine(size.left - 1, size.bottom - 1, size.left - 1, size.top + 1);
		g_VGuiSurface->DrawLine(size.right + 1, size.top + 1, size.right + 1, size.bottom - 1);
		g_VGuiSurface->DrawLine(size.left - 1, size.top + 1, size.right + 1, size.top + 1);
		g_VGuiSurface->DrawLine(size.right + 1, size.bottom - 1, size.left + -1, size.bottom - 1);

		g_VGuiSurface->DrawSetColor(Nadecolor);
		g_VGuiSurface->DrawLine(size.left, size.bottom, size.left, size.top);
		g_VGuiSurface->DrawLine(size.left, size.top, size.right, size.top);
		g_VGuiSurface->DrawLine(size.right, size.top, size.right, size.bottom);
		g_VGuiSurface->DrawLine(size.right, size.bottom, size.left, size.bottom);
	case 0:
		DrawString(ui_font, size.left + ((size.right - size.left) * 0.5), size.bottom + (size.top - size.bottom) + height * 0.5f + 2, Nadecolor, FONT_CENTER, entityName.c_str());
		break;
	case 3:
		DrawString(ui_font, size.left + ((size.right - size.left) * 0.5), size.bottom + (size.top - size.bottom) + height_icon * 0.5f + 1, Nadecolor, FONT_CENTER, entityName.c_str());
	case 2:
		DrawString(weapon_font, size.left + ((size.right - size.left) * 0.5), size.bottom + (size.top - size.bottom), Nadecolor, FONT_CENTER, icon_character.c_str());
		break;
	}
}

void Visuals::DrawhealthIcon(int x, int y, C_BasePlayer* player)
{
	Color cross_color = Color(255, 25, 50, 255);

	// If higher health than normal, flash the icon to notify.
	if (player && player->m_iHealth() > 100)
	{
		if (((float)g_GlobalVars->curtime - (int)(g_GlobalVars->curtime)) > 0.5f)
			cross_color = Color(255, 255, 0, 200);
	}

	g_VGuiSurface->DrawSetColor(cross_color); // Cross
	g_VGuiSurface->DrawFilledRect(x + 4, y + 2, x + 6, y + 10);// top-bottom
	g_VGuiSurface->DrawFilledRect(x + 1, y + 5, x + 4, y + 7);// left
	g_VGuiSurface->DrawFilledRect(x + 6, y + 5, x + 9, y + 7);// right

	g_VGuiSurface->DrawSetColor(Color(0, 0, 0, 255)); // Outline
	g_VGuiSurface->DrawLine(x + 3, y + 1, x + 6, y + 1); // top
	g_VGuiSurface->DrawLine(x + 3, y + 1, x + 3, y + 4);
	g_VGuiSurface->DrawLine(x + 6, y + 1, x + 6, y + 4);
	g_VGuiSurface->DrawLine(x + 3, y + 7, x + 3, y + 10);// bottom
	g_VGuiSurface->DrawLine(x + 6, y + 7, x + 6, y + 10);
	g_VGuiSurface->DrawLine(x + 3, y + 10, x + 6, y + 10);
	g_VGuiSurface->DrawLine(x, y + 4, x, y + 7);// left
	g_VGuiSurface->DrawLine(x, y + 4, x + 3, y + 4);
	g_VGuiSurface->DrawLine(x, y + 7, x + 3, y + 7);
	g_VGuiSurface->DrawLine(x + 9, y + 4, x + 9, y + 7);// right
	g_VGuiSurface->DrawLine(x + 6, y + 4, x + 9, y + 4);
	g_VGuiSurface->DrawLine(x + 6, y + 7, x + 9, y + 7);
}

bool Visuals::InitFont()
{
	ui_font = g_VGuiSurface->CreateFont_();
	g_VGuiSurface->SetFontGlyphSet(ui_font, "Bell Gothic", 10, 450, 0, 0, FONTFLAG_OUTLINE); 	//g_VGuiSurface->SetFontGlyphSet(ui_font, "Courier New", 14, 0, 0, 0, FONTFLAG_OUTLINE); // Styles

	watermark_font = g_VGuiSurface->CreateFont_();
	g_VGuiSurface->SetFontGlyphSet(watermark_font, "Verdana", 16, 700, 0, 0, FONTFLAG_OUTLINE | FONTFLAG_DROPSHADOW);

	weapon_font = g_VGuiSurface->CreateFont_();// 0xA1;
	g_VGuiSurface->SetFontGlyphSet(weapon_font, "Counter-Strike", 16, 500, 0, 0, FONTFLAG_OUTLINE | FONTFLAG_ANTIALIAS);

	spectatorlist_font = g_VGuiSurface->CreateFont_();
	g_VGuiSurface->SetFontGlyphSet(spectatorlist_font, "Tahoma", 14, 350, 0, 0, FONTFLAG_OUTLINE);

	return true;
}

bool Visuals::IsVisibleScan(C_BasePlayer *player)
{
	matrix3x4_t boneMatrix[MAXSTUDIOBONES];
	Vector eyePos = g_LocalPlayer->GetEyePos();

	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = g_LocalPlayer;

	if (!player->SetupBones(boneMatrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0.0f))
	{
		return false;
	}

	auto studio_model = g_MdlInfo->GetStudiomodel(player->GetModel());
	if (!studio_model)
	{
		return false;
	}

	int scan_hitboxes[] = {
		HITBOX_HEAD,
		HITBOX_LEFT_FOREARM,
		HITBOX_LEFT_UPPER_ARM,
		HITBOX_LEFT_FOOT,
		HITBOX_RIGHT_FOOT,
		HITBOX_LEFT_CALF,
		HITBOX_RIGHT_CALF,
		HITBOX_CHEST,
		HITBOX_STOMACH
	};
	
	for (int i = 0; i < ARRAYSIZE(scan_hitboxes); i++)
	{
		auto hitbox = studio_model->pHitboxSet(player->m_nHitboxSet())->pHitbox(scan_hitboxes[i]);
		if (hitbox)
		{
			auto
				min = Vector{},
				max = Vector{};

			Math::VectorTransform(hitbox->bbmin, boneMatrix[hitbox->bone], min);
			Math::VectorTransform(hitbox->bbmax, boneMatrix[hitbox->bone], max);

			ray.Init(eyePos, (min + max) * 0.5);
			g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

			if (tr.hit_entity == player || tr.fraction > 0.97f)
				return true;
		}
	}
	return false;
}

bool Visuals::ValidPlayer(C_BasePlayer *player, bool count_step)
{
	int idx = player->EntIndex();
	constexpr float frequency = 0.35f / 0.5f;
	float step = frequency * g_GlobalVars->frametime;
	if (!player->IsAlive())
		return false;

	// Don't render esp if in firstperson viewing player.
	if (player == local_observed)
	{
		if (g_LocalPlayer->m_iObserverMode() == 4)
			return false;
	}

	if (player == g_LocalPlayer)
	{
		if (!g_Input->m_fCameraInThirdPerson)
			return false;
	}

	if (count_step)
	{
		if (!player->IsDormant()) {
			if (ESP_Fade[idx] < 1.f)
				ESP_Fade[idx] += step;
		}
		else {
			if (ESP_Fade[idx] > 0.f)
				ESP_Fade[idx] -= step;
		}
		ESP_Fade[idx] = (ESP_Fade[idx] > 1.f ? 1.f : ESP_Fade[idx] < 0.f ? 0.f : ESP_Fade[idx]);
	}

	return (ESP_Fade[idx] > 0.f);
}

bool Visuals::Begin(C_BasePlayer *player)
{
	ESP_ctx.player = player;
	ESP_ctx.bEnemy = g_LocalPlayer->m_iTeamNum() != player->m_iTeamNum();
	ESP_ctx.isVisible = Visuals::IsVisibleScan(player);
	local_observed = (C_BasePlayer*)g_EntityList->GetClientEntityFromHandle(g_LocalPlayer->m_hObserverTarget());

	int idx = player->EntIndex();
	bool playerTeam = player->m_iTeamNum() == 2;

	if (!ESP_ctx.bEnemy && g_Options.esp_enemies_only)
		return false;

	if (!player->m_bGunGameImmunity())
	{
		if (ESP_ctx.isVisible)
		{
			ESP_ctx.clr_fill.SetColor(playerTeam ? g_Options.esp_player_fill_color_t_visible : g_Options.esp_player_fill_color_ct_visible);
			ESP_ctx.clr.SetColor(playerTeam ? g_Options.esp_player_bbox_color_t_visible : g_Options.esp_player_bbox_color_ct_visible);
		}
		else
		{
			ESP_ctx.clr_fill.SetColor(playerTeam ? g_Options.esp_player_fill_color_t : g_Options.esp_player_fill_color_ct);
			ESP_ctx.clr.SetColor(playerTeam ? g_Options.esp_player_bbox_color_t : g_Options.esp_player_bbox_color_ct);
			ESP_ctx.clr.SetAlpha(255);
		}
		ESP_ctx.clr.SetAlpha(ESP_ctx.clr.a() * ESP_Fade[idx]);
		ESP_ctx.clr_fill.SetAlpha(g_Options.esp_fill_amount * ESP_Fade[idx]);
		ESP_ctx.clr_text = Color(245, 245, 245, (int)(ESP_ctx.clr.a() * ESP_Fade[idx]));
	}
	else
	{
		// Set all colors to grey if immune.
		ESP_ctx.clr.SetColor(166, 169, 174, (int)(225 * ESP_Fade[idx]));
		ESP_ctx.clr_text.SetColor(166, 169, 174, (int)(225 * ESP_Fade[idx]));
		ESP_ctx.clr_fill.SetColor(166, 169, 174, (int)(g_Options.esp_fill_amount * ESP_Fade[idx]));
	}

	// Do some touch ups if local player and scoped
	if (player == g_LocalPlayer && g_LocalPlayer->m_bIsScoped())
	{
		ESP_ctx.clr.SetAlpha(ESP_ctx.clr.a() * 0.1f);
		ESP_ctx.clr_text.SetAlpha(ESP_ctx.clr_text.a() * 0.1f);
		ESP_ctx.clr_fill.SetAlpha(ESP_ctx.clr_fill.a() * 0.1f);
	}

	Vector head = player->GetAbsOrigin() + Vector(0, 0, player->GetCollideable()->OBBMaxs().z);
	Vector origin = player->GetAbsOrigin();
	origin.z -= 5;

	if (!Math::WorldToScreen(head, ESP_ctx.head_pos) ||
		!Math::WorldToScreen(origin, ESP_ctx.feet_pos))
		return false;

	auto h = fabs(ESP_ctx.head_pos.y - ESP_ctx.feet_pos.y);
	auto w = h / 1.65f;

	switch (g_Options.esp_player_boundstype)
	{
	case 0:
		ESP_ctx.bbox.left = static_cast<long>(ESP_ctx.feet_pos.x - w * 0.5f);
		ESP_ctx.bbox.right = static_cast<long>(ESP_ctx.bbox.left + w);
		ESP_ctx.bbox.bottom = (ESP_ctx.feet_pos.y > ESP_ctx.head_pos.y ? static_cast<long>(ESP_ctx.feet_pos.y) : static_cast<long>(ESP_ctx.head_pos.y));
		ESP_ctx.bbox.top = (ESP_ctx.feet_pos.y > ESP_ctx.head_pos.y ? static_cast<long>(ESP_ctx.head_pos.y) : static_cast<long>(ESP_ctx.feet_pos.y));
		break;
	case 1:
	{
		Vector points_transformed[8];
		RECT BBox = GetBBox(player, points_transformed);
		ESP_ctx.bbox = BBox;
		ESP_ctx.bbox.top = BBox.bottom;
		ESP_ctx.bbox.bottom = BBox.top;
		break;
	}
	}

	return true;
}

void Visuals::RenderFill()
{
	g_VGuiSurface->DrawSetColor(ESP_ctx.clr_fill);
	g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.left + 2, ESP_ctx.bbox.top + 2, ESP_ctx.bbox.right - 2, ESP_ctx.bbox.bottom - 2);
}

void Visuals::RenderBox()
{
	float
		length_horizontal = (ESP_ctx.bbox.right - ESP_ctx.bbox.left) * 0.2f,
		length_vertical = (ESP_ctx.bbox.bottom - ESP_ctx.bbox.top) * 0.2f;

	Color col_black = Color(0, 0, 0, (int)(255.f * ESP_Fade[ESP_ctx.player->EntIndex()]));
	switch (g_Options.esp_player_boxtype)
	{
	case 0:
		break;

	case 1:
		g_VGuiSurface->DrawSetColor(ESP_ctx.clr);
		g_VGuiSurface->DrawOutlinedRect(ESP_ctx.bbox.left, ESP_ctx.bbox.top, ESP_ctx.bbox.right, ESP_ctx.bbox.bottom);
		g_VGuiSurface->DrawSetColor(col_black);
		g_VGuiSurface->DrawOutlinedRect(ESP_ctx.bbox.left - 1, ESP_ctx.bbox.top - 1, ESP_ctx.bbox.right + 1, ESP_ctx.bbox.bottom + 1);
		g_VGuiSurface->DrawOutlinedRect(ESP_ctx.bbox.left + 1, ESP_ctx.bbox.top + 1, ESP_ctx.bbox.right - 1, ESP_ctx.bbox.bottom - 1);
		break;

	case 2:
		g_VGuiSurface->DrawSetColor(col_black);
		g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.left - 1, ESP_ctx.bbox.top - 1, ESP_ctx.bbox.left + 1 + length_horizontal, ESP_ctx.bbox.top + 2);
		g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.right - 1 - length_horizontal, ESP_ctx.bbox.top - 1, ESP_ctx.bbox.right + 1, ESP_ctx.bbox.top + 2);
		g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.left - 1, ESP_ctx.bbox.bottom - 2, ESP_ctx.bbox.left + 1 + length_horizontal, ESP_ctx.bbox.bottom + 1);
		g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.right - 1 - length_horizontal, ESP_ctx.bbox.bottom - 2, ESP_ctx.bbox.right + 1, ESP_ctx.bbox.bottom + 1);

		g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.left - 1, ESP_ctx.bbox.top + 2, ESP_ctx.bbox.left + 2, ESP_ctx.bbox.top + 1 + length_vertical);
		g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.right - 2, ESP_ctx.bbox.top + 2, ESP_ctx.bbox.right + 1, ESP_ctx.bbox.top + 1 + length_vertical);
		g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.left - 1, ESP_ctx.bbox.bottom - 1 - length_vertical, ESP_ctx.bbox.left + 2, ESP_ctx.bbox.bottom - 2);
		g_VGuiSurface->DrawFilledRect(ESP_ctx.bbox.right - 2, ESP_ctx.bbox.bottom - 1 - length_vertical, ESP_ctx.bbox.right + 1, ESP_ctx.bbox.bottom - 2);

		g_VGuiSurface->DrawSetColor(ESP_ctx.clr);
		g_VGuiSurface->DrawLine(ESP_ctx.bbox.left, ESP_ctx.bbox.top, ESP_ctx.bbox.left + length_horizontal - 1, ESP_ctx.bbox.top);
		g_VGuiSurface->DrawLine(ESP_ctx.bbox.right - length_horizontal, ESP_ctx.bbox.top, ESP_ctx.bbox.right - 1, ESP_ctx.bbox.top);
		g_VGuiSurface->DrawLine(ESP_ctx.bbox.left, ESP_ctx.bbox.bottom - 1, ESP_ctx.bbox.left + length_horizontal - 1, ESP_ctx.bbox.bottom - 1);
		g_VGuiSurface->DrawLine(ESP_ctx.bbox.right - length_horizontal, ESP_ctx.bbox.bottom - 1, ESP_ctx.bbox.right - 1, ESP_ctx.bbox.bottom - 1);

		g_VGuiSurface->DrawLine(ESP_ctx.bbox.left, ESP_ctx.bbox.top, ESP_ctx.bbox.left, ESP_ctx.bbox.top + length_vertical - 1);
		g_VGuiSurface->DrawLine(ESP_ctx.bbox.right - 1, ESP_ctx.bbox.top, ESP_ctx.bbox.right - 1, ESP_ctx.bbox.top + length_vertical - 1);
		g_VGuiSurface->DrawLine(ESP_ctx.bbox.left, ESP_ctx.bbox.bottom - length_vertical, ESP_ctx.bbox.left, ESP_ctx.bbox.bottom - 1);
		g_VGuiSurface->DrawLine(ESP_ctx.bbox.right - 1, ESP_ctx.bbox.bottom - length_vertical, ESP_ctx.bbox.right - 1, ESP_ctx.bbox.bottom - 1);
		break;
	}
}

void Visuals::RenderName()
{
	wchar_t buf[128];
	std::string name = ESP_ctx.player->GetName(),
		s_name = (name.length() > 0 ? name : "##ERROR_empty_name");

	if (MultiByteToWideChar(CP_UTF8, 0, s_name.c_str(), -1, buf, 128) > 0)
	{
		int tw, th;
		g_VGuiSurface->GetTextSize(ui_font, buf, tw, th);

		g_VGuiSurface->DrawSetTextFont(ui_font);
		g_VGuiSurface->DrawSetTextColor(ESP_ctx.clr_text);
		g_VGuiSurface->DrawSetTextPos(ESP_ctx.bbox.left + (ESP_ctx.bbox.right - ESP_ctx.bbox.left) * 0.5 - tw * 0.5, ESP_ctx.bbox.top - th + 1);
		g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
	}
}

void Visuals::DrawAngleLines()
{
	Vector src3D, dst3D, forward, src, dst;
	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;

	filter.pSkip = ESP_ctx.player;
	Math::AngleVectors(QAngle(0, ESP_ctx.player->m_flLowerBodyYawTarget(), 0), forward);
	src3D = ESP_ctx.player->m_vecOrigin();
	dst3D = src3D + (forward * 50.f);

	ray.Init(src3D, dst3D);
	g_EngineTrace->TraceRay(ray, 0, &filter, &tr);

	if (!Math::WorldToScreen(src3D, src) || !Math::WorldToScreen(tr.endpos, dst))
		return;

	g_VGuiSurface->DrawSetColor(Color::Green);
	g_VGuiSurface->DrawLine(src.x, src.y, dst.x, dst.y);


	Math::AngleVectors(QAngle(0, ESP_ctx.player->m_angEyeAngles().yaw, 0), forward);
	dst3D = src3D + (forward * 50.f);

	ray.Init(src3D, dst3D);
	g_EngineTrace->TraceRay(ray, 0, &filter, &tr);

	if (!Math::WorldToScreen(src3D, src) || !Math::WorldToScreen(tr.endpos, dst))
		return;

	g_VGuiSurface->DrawSetColor(Color::Blue);
	g_VGuiSurface->DrawLine(src.x, src.y, dst.x, dst.y);
}

void Visuals::DrawWatermark()
{
	DrawString(watermark_font, 10, 10, Color(0, 153, 204, 255), FONT_LEFT, "Gladiatorcheatz v2.0: Counter-Strike: Global Offensive");
}

void Visuals::DrawResolverModes()
{
	if (ESP_ctx.bEnemy) // on enemies only
		DrawString(ui_font, ESP_ctx.bbox.right + 5, ESP_ctx.bbox.top, Color(255, 255, 255, 255), FONT_LEFT, Global::resolverModes[ESP_ctx.player->EntIndex()].c_str());
}

void Visuals::DrawCapsuleOverlay(int idx, float duration)
{
	if (idx == g_EngineClient->GetLocalPlayer())
		return;

	auto player = C_BasePlayer::GetPlayerByIndex(idx);
	if (!player)
		return;

	if (g_Options.rage_lagcompensation && CMBacktracking::Get().m_LagRecord[player->EntIndex()].empty())
		return;

	matrix3x4_t boneMatrix_actual[MAXSTUDIOBONES];
	if (!player->SetupBones(boneMatrix_actual, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, g_EngineClient->GetLastTimeStamp()))
		return;

	studiohdr_t *studioHdr = g_MdlInfo->GetStudiomodel(player->GetModel());
	if (studioHdr)
	{
		matrix3x4_t boneMatrix[MAXSTUDIOBONES];
		std::memcpy(boneMatrix, CMBacktracking::Get().current_record[player->EntIndex()].matrix, sizeof(CMBacktracking::Get().current_record[player->EntIndex()].matrix));

		mstudiohitboxset_t *set = studioHdr->pHitboxSet(player->m_nHitboxSet());
		if (set)
		{
			for (int i = 0; i < set->numhitboxes; i++)
			{
				mstudiobbox_t *hitbox = set->pHitbox(i);
				if (hitbox)
				{
					if (hitbox->m_flRadius == -1.0f)
					{
						Vector position, position_actual;
						QAngle angles, angles_actual;
						MatrixAngles(boneMatrix[hitbox->bone], angles, position);
						MatrixAngles(boneMatrix_actual[hitbox->bone], angles_actual, position_actual);

						g_DebugOverlay->AddBoxOverlay(position, hitbox->bbmin, hitbox->bbmax, angles, 255, 0, 0, 150, duration);

						if (g_Options.esp_lagcompensated_hitboxes_type == 1)
							g_DebugOverlay->AddBoxOverlay(position_actual, hitbox->bbmin, hitbox->bbmax, angles_actual, 0, 0, 255, 150, duration);
					}
					else
					{
						Vector min, max,
							min_actual, max_actual;

						Math::VectorTransform(hitbox->bbmin, boneMatrix[hitbox->bone], min);
						Math::VectorTransform(hitbox->bbmax, boneMatrix[hitbox->bone], max);

						Math::VectorTransform(hitbox->bbmin, boneMatrix_actual[hitbox->bone], min_actual);
						Math::VectorTransform(hitbox->bbmax, boneMatrix_actual[hitbox->bone], max_actual);

						g_DebugOverlay->AddCapsuleOverlay(min, max, hitbox->m_flRadius, 255, 0, 0, 150, duration);

						if (g_Options.esp_lagcompensated_hitboxes_type == 1)
							g_DebugOverlay->AddCapsuleOverlay(min_actual, max_actual, hitbox->m_flRadius, 0, 0, 255, 150, duration);
					}
				}
			}
		}
	}
}

void Visuals::RenderHealth()
{
	int health = ESP_ctx.player->m_iHealth();
	if (health > 100)
		health = 100;

	float box_h = (float)fabs(ESP_ctx.bbox.bottom - ESP_ctx.bbox.top);
	float off = 8;

	auto height = box_h - (((box_h * health) / 100));

	int x = ESP_ctx.bbox.left - off;
	int y = ESP_ctx.bbox.top;
	int w = 4;
	int h = box_h;

	DrawhealthIcon(x - 12, y, ESP_ctx.player);

	Color col_black = Color(0, 0, 0, (int)(255.f * ESP_Fade[ESP_ctx.player->EntIndex()]));
	g_VGuiSurface->DrawSetColor(col_black);
	g_VGuiSurface->DrawFilledRect(x, y, x + w, y + h);

	g_VGuiSurface->DrawSetColor(Color((255 - health * int(2.55f)), (health * int(2.55f)), 0, (int)(180.f * ESP_Fade[ESP_ctx.player->EntIndex()])));
	g_VGuiSurface->DrawFilledRect(x + 1, y + height + 1, x + w - 1, y + h - 1);
}

void Visuals::RenderWeapon()
{
	wchar_t buf[80];
	auto clean_item_name = [](const char *name) -> const char*
	{
		if (name[0] == 'C')
			name++;

		auto start = strstr(name, "Weapon");
		if (start != nullptr)
			name = start + 6;

		return name;
	};

	auto weapon = ESP_ctx.player->m_hActiveWeapon().Get();

	if (!weapon) return;

	if (weapon->m_hOwnerEntity().IsValid())
	{
		auto name = clean_item_name(weapon->GetClientClass()->m_pNetworkName);

		if (MultiByteToWideChar(CP_UTF8, 0, name, -1, buf, 80) > 0)
		{
			int tw, th;
			g_VGuiSurface->GetTextSize(ui_font, buf, tw, th);

			g_VGuiSurface->DrawSetTextFont(ui_font);
			g_VGuiSurface->DrawSetTextColor(ESP_ctx.clr_text);
			g_VGuiSurface->DrawSetTextPos(ESP_ctx.bbox.left + (ESP_ctx.bbox.right - ESP_ctx.bbox.left) * 0.5 - tw * 0.5, ESP_ctx.bbox.bottom + 1);
			g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
		}
	}
}

void Visuals::RenderSnapline()
{
	int width, height;
	g_EngineClient->GetScreenSize(width, height);

	int screen_x = width * 0.5f,
		screen_y = height,
		target_x = ESP_ctx.bbox.left + (ESP_ctx.bbox.right - ESP_ctx.bbox.left) * 0.5,
		target_y = ESP_ctx.bbox.bottom,
		max_length = height * 0.3f;

	if (target_x == 0 ||
		target_y == 0)
		return;

	float length = sqrt(pow(target_x - screen_x, 2) + pow(target_y - screen_y, 2));
	if (length > max_length)
	{
		float
			x_normalized = (target_x - screen_x) / length,
			y_normalized = (target_y - screen_y) / length;
		target_x = screen_x + x_normalized * max_length;
		target_y = screen_y + y_normalized * max_length;
		DrawCircle(target_x + x_normalized * 3.5f, target_y + y_normalized * 3.5f, 3.5f, 12, ESP_ctx.clr);
	}

	g_VGuiSurface->DrawSetColor(ESP_ctx.clr);
	g_VGuiSurface->DrawLine(screen_x, screen_y, target_x, target_y);
}

void Visuals::RenderSkelet()
{
	studiohdr_t *studioHdr = g_MdlInfo->GetStudiomodel(ESP_ctx.player->GetModel());
	if (studioHdr)
	{
		static matrix3x4_t boneToWorldOut[128];
		if (ESP_ctx.player->SetupBones(boneToWorldOut, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, g_GlobalVars->curtime))
		//if(ESP_ctx.player->HandleBoneSetup(BONE_USED_BY_HITBOX, boneToWorldOut))
		{
			for (int i = 0; i < studioHdr->numbones; i++)
			{
				mstudiobone_t *bone = studioHdr->pBone(i);
				if (!bone || !(bone->flags & BONE_USED_BY_HITBOX) || bone->parent == -1)
					continue;

				Vector bonePos1;
				if (!Math::WorldToScreen(Vector(boneToWorldOut[i][0][3], boneToWorldOut[i][1][3], boneToWorldOut[i][2][3]), bonePos1))
					continue;

				Vector bonePos2;
				if (!Math::WorldToScreen(Vector(boneToWorldOut[bone->parent][0][3], boneToWorldOut[bone->parent][1][3], boneToWorldOut[bone->parent][2][3]), bonePos2))
					continue;

				g_VGuiSurface->DrawSetColor(ESP_ctx.clr);
				g_VGuiSurface->DrawLine((int)bonePos1.x, (int)bonePos1.y, (int)bonePos2.x, (int)bonePos2.y);
			}
		}
	}
}

void Visuals::RenderBacktrackedSkelet()
{
	if (!g_Options.rage_lagcompensation)
		return;

	auto records = &CMBacktracking::Get().m_LagRecord[ESP_ctx.player->EntIndex()];
	if (records->size() < 2)
		return;

	Vector previous_screenpos;
	for (auto record = records->begin(); record != records->end(); record++)
	{
		if (!CMBacktracking::Get().IsTickValid(TIME_TO_TICKS(record->m_flSimulationTime)))
			continue;

		Vector screen_pos;
		if (!Math::WorldToScreen(record->m_vecHeadSpot, screen_pos))
			continue;

		if (previous_screenpos.IsValid())
		{
			if (*record == CMBacktracking::Get().m_RestoreLagRecord[ESP_ctx.player->EntIndex()].first)
				g_VGuiSurface->DrawSetColor(Color(255, 255, 0, 255));
			else
				g_VGuiSurface->DrawSetColor(Color(255, 255, 255, 255));
			g_VGuiSurface->DrawLine(screen_pos.x, screen_pos.y, previous_screenpos.x, previous_screenpos.y);
		}

		previous_screenpos = screen_pos;
	}
}

void Visuals::RenderWeapon(C_BaseCombatWeapon *entity)
{
	wchar_t buf[80];
	auto clean_item_name = [](const char *name) -> const char*
	{
		if (name[0] == 'C')
			name++;

		auto start = strstr(name, "Weapon");
		if (start != nullptr)
			name = start + 6;

		return name;
	};

	if (entity->m_hOwnerEntity().IsValid() ||
		entity->m_vecOrigin() == Vector(0, 0, 0))
		return;

	Vector pointsTransformed[8];
	auto bbox = GetBBox(entity, pointsTransformed);
	if (bbox.right == 0 || bbox.bottom == 0)
		return;

	g_VGuiSurface->DrawSetColor(Color::Green);
	switch (g_Options.esp_dropped_weapons)
	{
	case 1:
		g_VGuiSurface->DrawLine(bbox.left, bbox.top, bbox.right, bbox.top);
		g_VGuiSurface->DrawLine(bbox.left, bbox.bottom, bbox.right, bbox.bottom);
		g_VGuiSurface->DrawLine(bbox.left, bbox.top, bbox.left, bbox.bottom);
		g_VGuiSurface->DrawLine(bbox.right, bbox.top, bbox.right, bbox.bottom);
		break;
	case 2:
		g_VGuiSurface->DrawLine(pointsTransformed[0].x, pointsTransformed[0].y, pointsTransformed[1].x, pointsTransformed[1].y);
		g_VGuiSurface->DrawLine(pointsTransformed[0].x, pointsTransformed[0].y, pointsTransformed[6].x, pointsTransformed[6].y);
		g_VGuiSurface->DrawLine(pointsTransformed[1].x, pointsTransformed[1].y, pointsTransformed[5].x, pointsTransformed[5].y);
		g_VGuiSurface->DrawLine(pointsTransformed[6].x, pointsTransformed[6].y, pointsTransformed[5].x, pointsTransformed[5].y);

		g_VGuiSurface->DrawLine(pointsTransformed[2].x, pointsTransformed[2].y, pointsTransformed[1].x, pointsTransformed[1].y);
		g_VGuiSurface->DrawLine(pointsTransformed[4].x, pointsTransformed[4].y, pointsTransformed[5].x, pointsTransformed[5].y);
		g_VGuiSurface->DrawLine(pointsTransformed[6].x, pointsTransformed[6].y, pointsTransformed[7].x, pointsTransformed[7].y);
		g_VGuiSurface->DrawLine(pointsTransformed[3].x, pointsTransformed[3].y, pointsTransformed[0].x, pointsTransformed[0].y);

		g_VGuiSurface->DrawLine(pointsTransformed[3].x, pointsTransformed[3].y, pointsTransformed[2].x, pointsTransformed[2].y);
		g_VGuiSurface->DrawLine(pointsTransformed[2].x, pointsTransformed[2].y, pointsTransformed[4].x, pointsTransformed[4].y);
		g_VGuiSurface->DrawLine(pointsTransformed[7].x, pointsTransformed[7].y, pointsTransformed[4].x, pointsTransformed[4].y);
		g_VGuiSurface->DrawLine(pointsTransformed[7].x, pointsTransformed[7].y, pointsTransformed[3].x, pointsTransformed[3].y);
		break;
	}

	auto name = clean_item_name(entity->GetClientClass()->m_pNetworkName);
	if (MultiByteToWideChar(CP_UTF8, 0, name, -1, buf, 80) > 0)
	{
		int w = bbox.right - bbox.left;
		int tw, th;
		g_VGuiSurface->GetTextSize(ui_font, buf, tw, th);

		g_VGuiSurface->DrawSetTextFont(ui_font);
		g_VGuiSurface->DrawSetTextColor(Color::Green);
		g_VGuiSurface->DrawSetTextPos(bbox.left + ((bbox.right - bbox.left) / 2) - (tw / 2), bbox.top + 1);
		g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
	}
}

void Visuals::RenderPlantedC4(C_BaseEntity *entity)
{
	Vector screen_points[8];
	auto bbox = GetBBox(entity, screen_points);

	if (bbox.right == 0 || bbox.bottom == 0)
		return;

	g_VGuiSurface->DrawSetColor(Color::Red);
	g_VGuiSurface->DrawLine(bbox.left, bbox.bottom, bbox.left, bbox.top);
	g_VGuiSurface->DrawLine(bbox.left, bbox.top, bbox.right, bbox.top);
	g_VGuiSurface->DrawLine(bbox.right, bbox.top, bbox.right, bbox.bottom);
	g_VGuiSurface->DrawLine(bbox.right, bbox.bottom, bbox.left, bbox.bottom);

	const wchar_t *buf = L"C4";

	int w = bbox.right - bbox.left;
	int tw, th;
	g_VGuiSurface->GetTextSize(ui_font, buf, tw, th);

	g_VGuiSurface->DrawSetTextFont(ui_font);
	g_VGuiSurface->DrawSetTextColor(Color::Red);
	g_VGuiSurface->DrawSetTextPos((bbox.left + w * 0.5f) - tw * 0.5f, bbox.bottom + 1);
	g_VGuiSurface->DrawPrintText(buf, wcslen(buf));
}

void Visuals::RenderSpectatorList()
{
	RECT scrn = GetViewport();
	int cnt = 0;

	for (int i = 1; i <= g_EntityList->GetHighestEntityIndex(); i++)
	{
		C_BasePlayer *player = C_BasePlayer::GetPlayerByIndex(i);

		if (!player || player == nullptr)
			continue;

		player_info_t player_info;
		if (player != g_LocalPlayer)
		{
			if (g_EngineClient->GetPlayerInfo(i, &player_info) && !player->IsAlive() && !player->IsDormant())
			{
				auto observer_target = player->m_hObserverTarget();
				if (!observer_target)
					continue;

				auto target = observer_target.Get();
				if (!target)
					continue;

				player_info_t player_info2;
				if (g_EngineClient->GetPlayerInfo(target->EntIndex(), &player_info2))
				{
					char player_name[255] = { 0 };
					sprintf_s(player_name, "%s => %s", player_info.szName, player_info2.szName);

					int w, h;
					GetTextSize(spectatorlist_font, player_name, w, h);

					g_VGuiSurface->DrawSetColor(Color(31, 44, 54, 140));
					DrawFilledRect(scrn.right - 260, (scrn.bottom / 2) + (16 * cnt), 260, 16);
					DrawString(spectatorlist_font, false, scrn.right - w - 4, (scrn.bottom / 2) + (16 * cnt), target->EntIndex() == g_LocalPlayer->EntIndex() ? Color(240, 70, 80, 255) : Color(255, 255, 255, 255), player_name);
					++cnt;
				}
			}
		}
	}
	DrawOutlinedRect(scrn.right - 261, (scrn.bottom / 2) - 1, 262, (16 * cnt) + 2, Color(9, 82, 128, 255));
	DrawOutlinedRect(scrn.right - 260, (scrn.bottom / 2), 260, (16 * cnt), Color(90, 90, 90, 160));
}

void Visuals::Polygon(int count, Vertex_t* Vertexs, Color color)
{
	static int Texture = g_VGuiSurface->CreateNewTextureID(true);
	unsigned char buffer[4] = { color.r(), color.g(), color.b(), color.a() };

	g_VGuiSurface->DrawSetTextureRGBA(Texture, buffer, 1, 1);
	g_VGuiSurface->DrawSetColor(Color(255, 255, 255, 255));
	g_VGuiSurface->DrawSetTexture(Texture);

	g_VGuiSurface->DrawTexturedPolygon(count, Vertexs);
}

void Visuals::PolygonOutline(int count, Vertex_t* Vertexs, Color color, Color colorLine)
{
	static int x[128];
	static int y[128];

	Polygon(count, Vertexs, color);

	for (int i = 0; i < count; i++)
	{
		x[i] = Vertexs[i].m_Position.x;
		y[i] = Vertexs[i].m_Position.y;
	}

	PolyLine(x, y, count, colorLine);
}

void Visuals::PolyLine(int count, Vertex_t* Vertexs, Color colorLine)
{
	static int x[128];
	static int y[128];

	for (int i = 0; i < count; i++)
	{
		x[i] = Vertexs[i].m_Position.x;
		y[i] = Vertexs[i].m_Position.y;
	}

	PolyLine(x, y, count, colorLine);
}

void Visuals::PolyLine(int *x, int *y, int count, Color color)
{
	g_VGuiSurface->DrawSetColor(color);
	g_VGuiSurface->DrawPolyLine(x, y, count);
}

void Visuals::Draw3DCube(float scalar, QAngle angles, Vector middle_origin, Color outline)
{
	Vector forward, right, up;
	Math::AngleVectors(angles, forward, right, up);

	Vector points[8];
	points[0] = middle_origin - (right * scalar) + (up * scalar) - (forward * scalar); // BLT
	points[1] = middle_origin + (right * scalar) + (up * scalar) - (forward * scalar); // BRT
	points[2] = middle_origin - (right * scalar) - (up * scalar) - (forward * scalar); // BLB
	points[3] = middle_origin + (right * scalar) - (up * scalar) - (forward * scalar); // BRB

	points[4] = middle_origin - (right * scalar) + (up * scalar) + (forward * scalar); // FLT
	points[5] = middle_origin + (right * scalar) + (up * scalar) + (forward * scalar); // FRT
	points[6] = middle_origin - (right * scalar) - (up * scalar) + (forward * scalar); // FLB
	points[7] = middle_origin + (right * scalar) - (up * scalar) + (forward * scalar); // FRB

	Vector points_screen[8];
	for (int i = 0; i < 8; i++)
		if (!Math::WorldToScreen(points[i], points_screen[i]))
			return;

	g_VGuiSurface->DrawSetColor(outline);

	// Back frame
	g_VGuiSurface->DrawLine(points_screen[0].x, points_screen[0].y, points_screen[1].x, points_screen[1].y);
	g_VGuiSurface->DrawLine(points_screen[0].x, points_screen[0].y, points_screen[2].x, points_screen[2].y);
	g_VGuiSurface->DrawLine(points_screen[3].x, points_screen[3].y, points_screen[1].x, points_screen[1].y);
	g_VGuiSurface->DrawLine(points_screen[3].x, points_screen[3].y, points_screen[2].x, points_screen[2].y);

	// Frame connector
	g_VGuiSurface->DrawLine(points_screen[0].x, points_screen[0].y, points_screen[4].x, points_screen[4].y);
	g_VGuiSurface->DrawLine(points_screen[1].x, points_screen[1].y, points_screen[5].x, points_screen[5].y);
	g_VGuiSurface->DrawLine(points_screen[2].x, points_screen[2].y, points_screen[6].x, points_screen[6].y);
	g_VGuiSurface->DrawLine(points_screen[3].x, points_screen[3].y, points_screen[7].x, points_screen[7].y);

	// Front frame
	g_VGuiSurface->DrawLine(points_screen[4].x, points_screen[4].y, points_screen[5].x, points_screen[5].y);
	g_VGuiSurface->DrawLine(points_screen[4].x, points_screen[4].y, points_screen[6].x, points_screen[6].y);
	g_VGuiSurface->DrawLine(points_screen[7].x, points_screen[7].y, points_screen[5].x, points_screen[5].y);
	g_VGuiSurface->DrawLine(points_screen[7].x, points_screen[7].y, points_screen[6].x, points_screen[6].y);
}

void Visuals::FillRGBA(int x, int y, int w, int h, Color c)
{
	g_VGuiSurface->DrawSetColor(c);
	g_VGuiSurface->DrawFilledRect(x, y, x + w, y + h);
}

void Visuals::BorderBox(int x, int y, int w, int h, Color color, int thickness)
{
	FillRGBA(x, y, w, thickness, color);
	FillRGBA(x, y, thickness, h, color);
	FillRGBA(x + w, y, thickness, h, color);
	FillRGBA(x, y + h, w + thickness, thickness, color);
}

__inline void Visuals::DrawFilledRect(int x, int y, int w, int h)
{
	g_VGuiSurface->DrawFilledRect(x, y, x + w, y + h);
}

void Visuals::DrawRectOutlined(int x, int y, int w, int h, Color color, Color outlinedColor, int thickness)
{
	FillRGBA(x, y, w, h, color);
	BorderBox(x - 1, y - 1, w + 1, h + 1, outlinedColor, thickness);
}

void Visuals::DrawString(unsigned long font, int x, int y, Color color, unsigned long alignment, const char* msg, ...)
{
	FUNCTION_GUARD;

	va_list va_alist;
	char buf[1024];
	va_start(va_alist, msg);
	_vsnprintf(buf, sizeof(buf), msg, va_alist);
	va_end(va_alist);
	wchar_t wbuf[1024];
	MultiByteToWideChar(CP_UTF8, 0, buf, 256, wbuf, 256);

	int r = 255, g = 255, b = 255, a = 255;
	color.GetColor(r, g, b, a);

	int width, height;
	g_VGuiSurface->GetTextSize(font, wbuf, width, height);

	if (alignment & FONT_RIGHT)
		x -= width;
	if (alignment & FONT_CENTER)
		x -= width / 2;

	g_VGuiSurface->DrawSetTextFont(font);
	g_VGuiSurface->DrawSetTextColor(r, g, b, a);
	g_VGuiSurface->DrawSetTextPos(x, y - height / 2);
	g_VGuiSurface->DrawPrintText(wbuf, wcslen(wbuf));
}

void Visuals::DrawString(unsigned long font, bool center, int x, int y, Color c, const char *fmt, ...)
{
	wchar_t *pszStringWide = reinterpret_cast< wchar_t* >(malloc((strlen(fmt) + 1) * sizeof(wchar_t)));

	mbstowcs(pszStringWide, fmt, (strlen(fmt) + 1) * sizeof(wchar_t));

	TextW(center, font, x, y, c, pszStringWide);

	free(pszStringWide);
}

void Visuals::TextW(bool center, unsigned long font, int x, int y, Color c, wchar_t *pszString)
{
	if (center)
	{
		int wide, tall;
		g_VGuiSurface->GetTextSize(font, pszString, wide, tall);
		x -= wide / 2;
		y -= tall / 2;
	}
	g_VGuiSurface->DrawSetTextColor(c);
	g_VGuiSurface->DrawSetTextFont(font);
	g_VGuiSurface->DrawSetTextPos(x, y);
	g_VGuiSurface->DrawPrintText(pszString, (int)wcslen(pszString), FONT_DRAW_DEFAULT);
}

void Visuals::DrawCircle(int x, int y, float r, int step, Color color)
{
	float Step = PI * 2.0 / step;
	for (float a = 0; a < (PI*2.0); a += Step)
	{
		float x1 = r * cos(a) + x;
		float y1 = r * sin(a) + y;
		float x2 = r * cos(a + Step) + x;
		float y2 = r * sin(a + Step) + y;
		g_VGuiSurface->DrawSetColor(color);
		g_VGuiSurface->DrawLine(x1, y1, x2, y2);
	}
}

void Visuals::DrawOutlinedRect(int x, int y, int w, int h, Color &c)
{
	g_VGuiSurface->DrawSetColor(c);
	g_VGuiSurface->DrawOutlinedRect(x, y, x + w, y + h);
}

void Visuals::GetTextSize(unsigned long font, const char *txt, int &width, int &height)
{
	FUNCTION_GUARD;

	size_t origsize = strlen(txt) + 1;
	const size_t newsize = 100;
	size_t convertedChars = 0;
	wchar_t wcstring[newsize];
	int x, y;

	mbstowcs_s(&convertedChars, wcstring, origsize, txt, _TRUNCATE);

	g_VGuiSurface->GetTextSize(font, wcstring, x, y);

	width = x;
	height = y;
}

RECT Visuals::GetViewport()
{
	RECT viewport = { 0, 0, 0, 0 };
	int w, h;
	g_EngineClient->GetScreenSize(w, h);
	viewport.right = w; viewport.bottom = h;

	return viewport;
}