#include "BulletImpact.hpp"

#include "../Structs.hpp"
#include "Visuals.hpp"
#include "AimRage.hpp"
#include "Resolver.hpp"

#include "../Options.hpp"
#include "../helpers/Math.hpp"

void BulletImpactEvent::FireGameEvent(IGameEvent *event)
{
	if (!g_LocalPlayer || !event)
		return;

	if (g_Options.visuals_others_bulletimpacts)
	{
		if (g_EngineClient->GetPlayerForUserID(event->GetInt("userid")) == g_EngineClient->GetLocalPlayer() && g_LocalPlayer && g_LocalPlayer->IsAlive())
		{
			float x = event->GetFloat("x"), y = event->GetFloat("y"), z = event->GetFloat("z");
			bulletImpactInfo.push_back({ g_GlobalVars->curtime, Vector(x, y, z) });
		}
	}

	if (g_Options.hvh_resolver)
	{
		int32_t userid = g_EngineClient->GetPlayerForUserID(event->GetInt("userid"));

		if (userid == g_EngineClient->GetLocalPlayer())
		{
			if (AimRage::Get().prev_aimtarget != NULL)
			{
				auto player = C_BasePlayer::GetPlayerByIndex(AimRage::Get().prev_aimtarget);
				if (!player)
					return;

				int32_t idx = player->EntIndex();
				auto &player_recs = Resolver::Get().arr_infos[idx];

				if (!player->IsDormant())
				{
					int32_t tickcount = g_GlobalVars->tickcount;

					if (tickcount != tickHitWall)
					{
						tickHitWall = tickcount;
						originalShotsMissed = player_recs.m_nShotsMissed;
						originalCorrectedFakewalkIdx = player_recs.m_nCorrectedFakewalkIdx;

						if (tickcount != tickHitPlayer)
						{
							tickHitWall = tickcount;
							++player_recs.m_nShotsMissed;

							if (++player_recs.m_nCorrectedFakewalkIdx > 3)
								player_recs.m_nCorrectedFakewalkIdx = 0;
						}
					}
				}
			}
		}
	}
}

int BulletImpactEvent::GetEventDebugID(void)
{
	return EVENT_DEBUG_ID_INIT;
}

void BulletImpactEvent::RegisterSelf()
{
	g_GameEvents->AddListener(this, "bullet_impact", false);
}

void BulletImpactEvent::UnregisterSelf()
{
	g_GameEvents->RemoveListener(this);
}

void BulletImpactEvent::Paint(void)
{
	if (!g_Options.visuals_others_bulletimpacts)
		return;

	if (!g_EngineClient->IsInGame() || !g_LocalPlayer || !g_LocalPlayer->IsAlive())
	{
		bulletImpactInfo.clear();
		return;
	}

	std::vector<BulletImpactInfo> &impacts = bulletImpactInfo;

	if (impacts.empty())
		return;

	Color current_color(g_Options.visuals_others_bulletimpacts_color);

	for (size_t i = 0; i < impacts.size(); i++)
	{
		auto current_impact = impacts.at(i);

		BeamInfo_t beamInfo;

		beamInfo.m_nType = TE_BEAMPOINTS;
		beamInfo.m_pszModelName = "sprites/physbeam.vmt";
		beamInfo.m_nModelIndex = -1;
		beamInfo.m_flHaloScale = 0.0f;
		beamInfo.m_flLife = 0.8f;
		beamInfo.m_flWidth = 1.5f;
		beamInfo.m_flEndWidth = 1.5f;
		beamInfo.m_flFadeLength = 0.0f;
		beamInfo.m_flAmplitude = 2.0f;
		beamInfo.m_flBrightness = 255.f;
		beamInfo.m_flSpeed = 0.2f;
		beamInfo.m_nStartFrame = 0;
		beamInfo.m_flFrameRate = 0.f;
		beamInfo.m_flRed = current_color.r();
		beamInfo.m_flGreen = current_color.g();
		beamInfo.m_flBlue = current_color.b();
		beamInfo.m_nSegments = 2;
		beamInfo.m_bRenderable = true;
		beamInfo.m_nFlags = FBEAM_ONLYNOISEONCE | FBEAM_NOTILE | FBEAM_HALOBEAM;

		beamInfo.m_vecStart = g_LocalPlayer->GetEyePos();
		beamInfo.m_vecEnd = current_impact.m_vecHitPos;

		auto beam = g_RenderBeams->CreateBeamPoints(beamInfo);
		if (beam)
			g_RenderBeams->DrawBeam(beam);

		g_DebugOverlay->AddBoxOverlay(current_impact.m_vecHitPos, Vector(-3, -3, -3), Vector(3, 3, 3), QAngle(0, 0, 0), current_color.r(), current_color.g(), current_color.b(), current_color.a(), 0.8f);

		impacts.erase(impacts.begin() + i);
	}
}