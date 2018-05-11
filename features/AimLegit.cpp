#include "AimLegit.h"

#include <random>

#include "../Options.hpp"
#include "../helpers/Math.hpp"
#include "../helpers/Utils.hpp"
#include "LagCompensation.hpp"

LineGoesThroughSmokeFn LineGoesThroughSmoke;

void AimLegit::Work(CUserCmd *usercmd)
{
	this->usercmd = usercmd;

	if (!g_LocalPlayer->m_hActiveWeapon().Get())
		return;

	if (g_LocalPlayer->m_hActiveWeapon().Get()->IsWeaponNonAim() || g_LocalPlayer->m_hActiveWeapon().Get()->m_iClip1() < 1)
		return;

	if (g_Options.legit_enabled && (g_InputSystem->IsButtonDown(static_cast<ButtonCode_t>(g_Options.legit_aimkey1)) || g_InputSystem->IsButtonDown(static_cast<ButtonCode_t>(g_Options.legit_aimkey2))) && !g_LocalPlayer->m_hActiveWeapon().Get()->IsWeaponNonAim())
	{
		GetBestTarget();
		if (HasTarget())
			TargetRegion();
	}

	if (g_Options.legit_trigger || (g_Options.legit_trigger_with_aimkey && (g_InputSystem->IsButtonDown(static_cast<ButtonCode_t>(g_Options.legit_aimkey1)) || g_InputSystem->IsButtonDown(static_cast<ButtonCode_t>(g_Options.legit_aimkey2)))))
		Triggerbot();

	if (g_Options.legit_rcs)
		RecoilControlSystem();

	if (!g_Options.rage_silent) // I dont feel here good, cause of ragebot and stuff and the part inside CM cause of that
		g_EngineClient->SetViewAngles(usercmd->viewangles);
}

void AimLegit::GetBestTarget()
{
	float maxDistance = 8192.f;
	float nearest;
	int index = -1;

	float fov = g_Options.legit_fov;

	int firedShots = g_LocalPlayer->m_iShotsFired();

	for (int i = 1; i < g_GlobalVars->maxClients; i++)
	{
		auto player = C_BasePlayer::GetPlayerByIndex(i);

		if (!CheckTarget(player))
			continue;

		QAngle viewangles = usercmd->viewangles;

		Vector targetpos;

		if (firedShots > g_Options.legit_aftershots)
			targetpos = player->GetBonePos(realAimSpot[g_Options.legit_afteraim]);
		else if (firedShots < g_Options.legit_aftershots)
			targetpos = player->GetBonePos(realAimSpot[g_Options.legit_preaim]);

		nearest = Math::GetFov(viewangles, Math::CalcAngle(g_LocalPlayer->GetEyePos(), targetpos));

		if (nearest > fov)
			continue;

		float distance = Math::GetDistance(g_LocalPlayer->m_vecOrigin(), player->m_vecOrigin());

		if (fabsf(fov - nearest) < 5)
		{
			if (distance < maxDistance)
			{
				fov = nearest;
				maxDistance = distance;
				index = i;
			}
		}
		else if (nearest < fov)
		{
			fov = nearest;
			maxDistance = distance;
			index = i;
		}
	}
	SetTarget(index);
}

bool AimLegit::CheckTarget(C_BasePlayer *player)
{
	if (!player || player == nullptr)
		return false;

	if (player == g_LocalPlayer)
		return false;

	if (player->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
		return false;

	if (player->IsDormant())
		return false;

	if (player->m_bGunGameImmunity())
		return false;

	if (!player->IsAlive())
		return false;

	int firedShots = g_LocalPlayer->m_iShotsFired();
	Vector targetpos;

	CGameTrace tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = g_LocalPlayer;
	auto start = g_LocalPlayer->GetEyePos();

	if (firedShots > g_Options.legit_aftershots)
		targetpos = player->GetBonePos(realAimSpot[g_Options.legit_afteraim]);
	else if (firedShots < g_Options.legit_aftershots)
		targetpos = player->GetBonePos(realAimSpot[g_Options.legit_preaim]);

	ray.Init(start, targetpos);
	g_EngineTrace->TraceRay(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

	if (tr.hit_entity != player)
		return false;

	if (IsBehindSmoke(player->GetEyePos(), targetpos))
		return false;

	return true;
}

bool AimLegit::IsBehindSmoke(Vector src, Vector rem)
{
	LineGoesThroughSmoke = (LineGoesThroughSmokeFn)(Offsets::lgtSmoke);

	return LineGoesThroughSmoke(src, rem, true);
}

void AimLegit::TargetRegion()
{
	auto player = C_BasePlayer::GetPlayerByIndex(m_iTarget);

	int firedShots = g_LocalPlayer->m_iShotsFired();

	QAngle aimPunchAngle = g_LocalPlayer->m_aimPunchAngle();

	std::random_device r3nd0m;
	std::mt19937 r3nd0mGen(r3nd0m());

	std::uniform_real<float> r3nd0mXAngle(1.7f, 1.9f);
	std::uniform_real<float> r3nd0mYAngle(1.7f, 1.9f);

	aimPunchAngle.pitch *= r3nd0mXAngle(r3nd0mGen);
	aimPunchAngle.yaw *= r3nd0mYAngle(r3nd0mGen);
	aimPunchAngle.roll = 0.0f;

	Math::NormalizeAngles(aimPunchAngle);

	QAngle viewangles = usercmd->viewangles;

	Vector targetpos;

	if (firedShots > g_Options.legit_aftershots)
		targetpos = player->GetBonePos(realAimSpot[g_Options.legit_afteraim]);
	else if (firedShots < g_Options.legit_aftershots)
		targetpos = player->GetBonePos(realAimSpot[g_Options.legit_preaim]);

	QAngle angle = Math::CalcAngle(g_LocalPlayer->GetEyePos(), targetpos);
	angle.pitch -= aimPunchAngle.pitch;
	angle.yaw -= aimPunchAngle.yaw;

	Math::SmoothAngle(viewangles, angle, g_Options.legit_smooth_factor);

	Math::NormalizeAngles(angle);

	usercmd->viewangles = angle;
}

void AimLegit::Triggerbot()
{
	Vector rem, forward,
		src = g_LocalPlayer->GetEyePos();

	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = g_LocalPlayer;

	QAngle viewangles = usercmd->viewangles;

	viewangles += g_LocalPlayer->m_aimPunchAngle() * 2.f;

	Math::AngleVectors(viewangles, forward);

	forward *= g_LocalPlayer->m_hActiveWeapon().Get()->GetWeapInfo()->m_fRange;

	rem = src + forward;

	ray.Init(src, rem);
	g_EngineTrace->TraceRay(ray, 0x46004003, &filter, &tr);

	if (!tr.hit_entity)
		return;

	bool dh = false;

	if (tr.hitgroup == HITGROUP_HEAD || tr.hitgroup == HITGROUP_CHEST || tr.hitgroup == HITGROUP_STOMACH || (tr.hitgroup == HITGROUP_LEFTARM || tr.hitgroup == HITGROUP_RIGHTARM) || (tr.hitgroup == HITGROUP_LEFTLEG || tr.hitgroup == HITGROUP_RIGHTLEG))
		dh = true;

	auto player = reinterpret_cast<C_BasePlayer*>(tr.hit_entity);
	if (player && !player->IsDormant() && player->m_iHealth() > 0 && player->IsPlayer())
	{
		if (player->m_iTeamNum() != g_LocalPlayer->m_iTeamNum())
		{
			if (g_Options.legit_trigger_with_aimkey)
			{
				if(dh && (!(usercmd->buttons & IN_ATTACK) && (!g_InputSystem->IsButtonDown(static_cast<ButtonCode_t>(g_Options.legit_aimkey1)) || !g_InputSystem->IsButtonDown(static_cast<ButtonCode_t>(g_Options.legit_aimkey2))))) // if you don't attack currently
					usercmd->buttons |= IN_ATTACK;

				static bool already_shot = false;
				if (g_LocalPlayer->m_hActiveWeapon().Get()->IsPistol())
				{
					if (usercmd->buttons & IN_ATTACK)
						if (already_shot)
							usercmd->buttons &= ~IN_ATTACK;

					already_shot = usercmd->buttons & IN_ATTACK ? true : false;
				}
			}
			else
			{
				if (dh && (!(usercmd->buttons & IN_ATTACK) && !g_InputSystem->IsButtonDown(ButtonCode_t::MOUSE_LEFT))) // if you don't attack currently
					usercmd->buttons |= IN_ATTACK;

				static bool already_shot = false;
				if (g_LocalPlayer->m_hActiveWeapon().Get()->IsPistol())
				{
					if (usercmd->buttons & IN_ATTACK)
						if (already_shot)
							usercmd->buttons &= ~IN_ATTACK;

					already_shot = usercmd->buttons & IN_ATTACK ? true : false;
				}
			}
		}
	}
}

QAngle oldPunch;

void AimLegit::RecoilControlSystem()
{
	int firedShots = g_LocalPlayer->m_iShotsFired();

	if (usercmd->buttons & IN_ATTACK)
	{
		QAngle aimPunchAngle = g_LocalPlayer->m_aimPunchAngle();

		std::random_device r3nd0m;
		std::mt19937 r3nd0mGen(r3nd0m());

		std::uniform_real<float> r3nd0mXAngle(1.7f, 1.9f);
		std::uniform_real<float> r3nd0mYAngle(1.7f, 1.9f);

		aimPunchAngle.pitch *= r3nd0mXAngle(r3nd0mGen);
		aimPunchAngle.yaw *= r3nd0mYAngle(r3nd0mGen);
		aimPunchAngle.roll = 0.0f;

		Math::NormalizeAngles(aimPunchAngle);

		if (firedShots > 2)
		{
			QAngle viewangles = usercmd->viewangles;

			QAngle viewangles_mod = aimPunchAngle;

			viewangles_mod -= oldPunch;
			viewangles_mod.roll = 0.0f;

			Math::NormalizeAngles(viewangles_mod);

			viewangles -= viewangles_mod;
			viewangles.roll = 0.0f;

			Math::NormalizeAngles(viewangles);

			usercmd->viewangles = viewangles;
		}
		oldPunch = aimPunchAngle;
	}
	else
		oldPunch.Init();
}