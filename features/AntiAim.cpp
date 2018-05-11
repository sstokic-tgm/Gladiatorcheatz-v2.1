#include "AntiAim.hpp"

#include "../Structs.hpp"
#include "../Options.hpp"

#include "AimRage.hpp"
#include "Resolver.hpp"
#include "RebuildGameMovement.hpp"
#include "Miscellaneous.hpp"
#include "PredictionSystem.hpp"

#include "../helpers/Utils.hpp"
#include "../helpers/Math.hpp"

#include <time.h>

void AntiAim::Work(CUserCmd *usercmd)
{
	this->usercmd = usercmd;

	if (usercmd->buttons & IN_USE)
		return;

	auto weapon = g_LocalPlayer->m_hActiveWeapon().Get();

	if (!weapon)
		return;

	if (weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER)
	{
		if (usercmd->buttons & IN_ATTACK2)
			return;

		if (weapon->CanFirePostPone() && (usercmd->buttons & IN_ATTACK))
			return;
	}
	else if (weapon->GetWeapInfo()->weapon_type == WEAPONTYPE_GRENADE)
	{
		if (weapon->IsInThrow())
			return;
	}
	else
	{
		if (weapon->GetWeapInfo()->weapon_type == WEAPONTYPE_KNIFE && ((usercmd->buttons & IN_ATTACK) || (usercmd->buttons & IN_ATTACK2)))
			return;
		else if ((usercmd->buttons & IN_ATTACK) && (weapon->m_iItemDefinitionIndex() != WEAPON_C4 || g_Options.hvh_antiaim_x != AA_PITCH_OFF))
			return;
	}

	if (g_LocalPlayer->GetMoveType() == MOVETYPE_NOCLIP || g_LocalPlayer->GetMoveType() == MOVETYPE_LADDER)
		return;

	if (!Global::bFakelag)
		Global::bSendPacket = usercmd->command_number % 2;

	usercmd->viewangles.pitch = GetPitch();

	if (!Global::bSendPacket && g_Options.hvh_antiaim_lby_breaker)
	{
		if (m_bBreakLowerBody)
		{
			Global::bSendPacket = true;
			usercmd->viewangles.yaw += 114.0f;
		}
	}

	if (!Global::bSendPacket)
		usercmd->viewangles.yaw = GetYaw();
	else
		usercmd->viewangles.yaw = GetFakeYaw();
}

void AntiAim::UpdateLBYBreaker(CUserCmd *usercmd)
{
	bool
		allocate = (m_serverAnimState == nullptr),
		change = (!allocate) && (&g_LocalPlayer->GetRefEHandle() != m_ulEntHandle),
		reset = (!allocate && !change) && (g_LocalPlayer->m_flSpawnTime() != m_flSpawnTime);

	// player changed, free old animation state.
	if (change)
		g_pMemAlloc->Free(m_serverAnimState);

	// need to reset? (on respawn)
	if (reset)
	{
		// reset animation state.
		C_BasePlayer::ResetAnimationState(m_serverAnimState);

		// note new spawn time.
		m_flSpawnTime = g_LocalPlayer->m_flSpawnTime();
	}

	// need to allocate or create new due to player change.
	if (allocate || change)
	{
		// only works with games heap alloc.
		C_CSGOPlayerAnimState *state = (C_CSGOPlayerAnimState*)g_pMemAlloc->Alloc(sizeof(C_CSGOPlayerAnimState));

		if (state != nullptr)
			g_LocalPlayer->CreateAnimationState(state);

		// used to detect if we need to recreate / reset.
		m_ulEntHandle = const_cast<CBaseHandle*>(&g_LocalPlayer->GetRefEHandle());
		m_flSpawnTime = g_LocalPlayer->m_flSpawnTime();

		// note anim state for future use.
		m_serverAnimState = state;
	}

	float_t curtime = TICKS_TO_TIME(AimRage::Get().GetTickbase());
	if (!g_ClientState->chokedcommands && m_serverAnimState)
	{
		C_BasePlayer::UpdateAnimationState(m_serverAnimState, usercmd->viewangles);

		// calculate delta.
		float_t delta = std::abs(Math::ClampYaw(usercmd->viewangles.yaw - g_LocalPlayer->m_flLowerBodyYawTarget()));

		// walking, delay next update by .22s.
		if (m_serverAnimState->m_flVelocity() > 0.1f && (g_LocalPlayer->m_fFlags() & FL_ONGROUND))
			m_flNextBodyUpdate = curtime + 0.22f;

		else if (curtime >= m_flNextBodyUpdate)
		{
			if (delta > 35.f)
				; // server will update lby.

			m_flNextBodyUpdate = curtime + 1.1f;
		}
	}

	// if was jumping and then onground and bsendpacket true, we're gonna update.
	m_bBreakLowerBody = (g_LocalPlayer->m_fFlags() & FL_ONGROUND) && ((m_flNextBodyUpdate - curtime) <= g_GlobalVars->interval_per_tick);
}

float AntiAim::GetPitch()
{
	switch (g_Options.hvh_antiaim_x)
	{
	case AA_PITCH_OFF:

		return usercmd->viewangles.pitch;
		break;

	case AA_PITCH_DYNAMIC:

		return g_LocalPlayer->m_hActiveWeapon().Get()->IsSniper() ? (g_LocalPlayer->m_hActiveWeapon().Get()->m_zoomLevel() != 0 ? 87.f : 85.f) : 88.99f;
		break;

	case AA_PITCH_EMOTION:

		return 88.99f;
		break;

	case AA_PITCH_STRAIGHT:

		return 0.f;
		break;

	case AA_PITCH_UP:

		return -88.99f;
		break;
	}

	return usercmd->viewangles.pitch;
}

float AntiAim::GetYaw()
{
	static bool left = false;
	static bool right = false;
	static bool backwards = true;

	static bool flip = false;
	flip = !flip;

	float_t pos = usercmd->viewangles.yaw;

	switch (g_Options.hvh_antiaim_y)
	{
	case AA_YAW_BACKWARDS:

		return pos + 180.0f;
		break;

	case AA_YAW_BACKWARDS_JITTER:

		return pos + 180.0f + Utils::RandomFloat(-25.5f, 25.5f);
		break;

	case AA_YAW_MANUALLBY:

		if (GetAsyncKeyState(VK_LEFT)) { left = true; right = false;  backwards = false; }
		else if (GetAsyncKeyState(VK_RIGHT)) { left = false; right = true; backwards = false; }
		else if (GetAsyncKeyState(VK_DOWN)) { left = false; right = false; backwards = true; }

		if (left) // left real
			return pos + 90.f;

		else if (right) // right real
			return pos - 90.f;

		else if (backwards) // backwards
			return pos + 180.f;
		break;

	case AA_YAW_MANUALLBYJITTER:

		if (GetAsyncKeyState(VK_LEFT)) { left = true; right = false;  backwards = false; }
		else if (GetAsyncKeyState(VK_RIGHT)) { left = false; right = true; backwards = false; }
		else if (GetAsyncKeyState(VK_DOWN)) { left = false; right = false; backwards = true; }

		if (left)
			return pos + (90.f + Utils::RandomFloat(-25.5f, 25.5f));

		else if (right)
			return pos - (90.f + Utils::RandomFloat(-25.5f, 25.5f));

		else if (backwards)
			return pos + 180.0f + Utils::RandomFloat(-25.5f, 25.5f);
		break;
	}

	return pos;
}

float AntiAim::GetFakeYaw()
{
	static bool left = false;
	static bool right = false;
	static bool backwards = true;

	static bool flip = false;
	flip = !flip;

	float_t pos = usercmd->viewangles.yaw;

	switch (g_Options.hvh_antiaim_y_fake)
	{
	case AA_FAKEYAW_BACKWARDS:

		return pos + 180.0f;
		break;

	case AA_YAW_BACKWARDS_JITTER:

		return pos + 180.0f + Utils::RandomFloat(-25.5f, 25.5f);
		break;

	case AA_FAKEYAW_EVADE:

		return flip ? pos + 80.f : pos - 80.f;
		break;

	case AA_FAKEYAW_MANUALLBY:
		
		if (GetAsyncKeyState(VK_LEFT)) { left = true; right = false;  backwards = false; }
		else if (GetAsyncKeyState(VK_RIGHT)) { left = false; right = true; backwards = false; }
		else if (GetAsyncKeyState(VK_DOWN)) { left = false; right = false; backwards = true; }

		if (right) // right fake
			return pos + 90.f;

		else if (left) // left fake
			return pos - 90.f;

		else if (backwards) // backwards (spin) fake
			return fmod(g_GlobalVars->curtime / 1.f * 360.f, 360.f) + 180;
		break;

	case AA_FAKEYAW_MANUALLBYJITTER:

		if (GetAsyncKeyState(VK_LEFT)) { left = true; right = false;  backwards = false; }
		else if (GetAsyncKeyState(VK_RIGHT)) { left = false; right = true; backwards = false; }
		else if (GetAsyncKeyState(VK_DOWN)) { left = false; right = false; backwards = true; }

		if (right)
			return pos + (90.f + Utils::RandomFloat(-25.5f, 25.5f));

		else if (left)
			return pos - (90.f + Utils::RandomFloat(-25.5f, 25.5f));

		else if (backwards)
			return fmod(g_GlobalVars->curtime / 0.5f * 360.f, 360.f) + 180;
		break;
	}

	return pos;
}

void AntiAim::Accelerate(C_BasePlayer *player, Vector &wishdir, float wishspeed, float accel, Vector &outVel)
{
	// See if we are changing direction a bit
	float currentspeed = outVel.Dot(wishdir);

	// Reduce wishspeed by the amount of veer.
	float addspeed = wishspeed - currentspeed;

	// If not going to add any speed, done.
	if (addspeed <= 0)
		return;

	// Determine amount of accleration.
	float accelspeed = accel * g_GlobalVars->frametime * wishspeed * player->m_surfaceFriction();

	// Cap at addspeed
	if (accelspeed > addspeed)
		accelspeed = addspeed;

	// Adjust velocity.
	for (int i = 0; i < 3; i++)
		outVel[i] += accelspeed * wishdir[i];
}

void AntiAim::WalkMove(C_BasePlayer *player, Vector &outVel)
{
	Vector forward, right, up, wishvel, wishdir, dest;
	float_t fmove, smove, wishspeed;

	Math::AngleVectors(player->m_angEyeAngles(), forward, right, up);  // Determine movement angles
	// Copy movement amounts
	g_MoveHelper->SetHost(player);
	fmove = g_MoveHelper->m_flForwardMove;
	smove = g_MoveHelper->m_flSideMove;
	g_MoveHelper->SetHost(nullptr);

	if (forward[2] != 0)
	{
		forward[2] = 0;
		Math::NormalizeVector(forward);
	}

	if (right[2] != 0)
	{
		right[2] = 0;
		Math::NormalizeVector(right);
	}

	for (int i = 0; i < 2; i++)	// Determine x and y parts of velocity
		wishvel[i] = forward[i] * fmove + right[i] * smove;

	wishvel[2] = 0;	// Zero out z part of velocity

	wishdir = wishvel; // Determine maginitude of speed of move
	wishspeed = wishdir.Normalize();

	// Clamp to server defined max speed
	g_MoveHelper->SetHost(player);
	if ((wishspeed != 0.0f) && (wishspeed > g_MoveHelper->m_flMaxSpeed))
	{
		VectorMultiply(wishvel, player->m_flMaxspeed() / wishspeed, wishvel);
		wishspeed = player->m_flMaxspeed();
	}
	g_MoveHelper->SetHost(nullptr);
	// Set pmove velocity
	outVel[2] = 0;
	Accelerate(player, wishdir, wishspeed, g_CVar->FindVar("sv_accelerate")->GetFloat(), outVel);
	outVel[2] = 0;

	// Add in any base velocity to the current velocity.
	VectorAdd(outVel, player->m_vecBaseVelocity(), outVel);

	float spd = outVel.Length();

	if (spd < 1.0f)
	{
		outVel.Init();
		// Now pull the base velocity back out. Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
		VectorSubtract(outVel, player->m_vecBaseVelocity(), outVel);
		return;
	}

	g_MoveHelper->SetHost(player);
	g_MoveHelper->m_outWishVel += wishdir * wishspeed;
	g_MoveHelper->SetHost(nullptr);

	// Don't walk up stairs if not on ground.
	if (!(player->m_fFlags() & FL_ONGROUND))
	{
		// Now pull the base velocity back out.   Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
		VectorSubtract(outVel, player->m_vecBaseVelocity(), outVel);
		return;
	}

	// Now pull the base velocity back out. Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
	VectorSubtract(outVel, player->m_vecBaseVelocity(), outVel);
}

void AntiAim::Fakewalk(CUserCmd *usercmd)
{
	if (!g_InputSystem->IsButtonDown(g_Options.misc_fakewalk_bind))
		return;

	Global::flFakewalked = PredictionSystem::Get().GetOldCurTime();

	Global::bSendPacket = true;

	Vector velocity = Global::vecUnpredictedVel;

	int32_t ticks_to_update = TIME_TO_TICKS(m_flNextBodyUpdate - TICKS_TO_TIME(AimRage::Get().GetTickbase())) - 1;
	int32_t ticks_to_stop;
	for (ticks_to_stop = 0; ticks_to_stop < 15; ticks_to_stop++)
	{
		if (velocity.Length2D() < 0.1f)
			break;

		if (g_LocalPlayer->m_fFlags() & FL_ONGROUND)
		{
			velocity[2] = 0.0f;
			Friction(velocity);
			WalkMove(g_LocalPlayer, velocity);
		}
	}

	const int32_t max_ticks = std::min<int32_t>(7, ticks_to_update);
	const int32_t chocked = Miscellaneous::Get().GetChocked();
	int32_t ticks_left = max_ticks - chocked;

	if (chocked < max_ticks || ticks_to_stop)
		Global::bSendPacket = false;
	else
		Global::bSendPacket = true;

	if (ticks_to_stop > ticks_left || !chocked || Global::bSendPacket)
	{
		float_t speed = velocity.Length();

		if (speed > 13.f)
		{
			QAngle direction;
			Math::VectorAngles(velocity, direction);

			direction.yaw = usercmd->viewangles.yaw - direction.yaw;

			Vector forward;
			Math::AngleVectors(direction, forward);
			Vector negated_direction = forward * -speed;

			usercmd->forwardmove = negated_direction.x;
			usercmd->sidemove = negated_direction.y;
		}
		else
		{
			usercmd->forwardmove = 0.f;
			usercmd->sidemove = 0.f;
		}
	}
}

void AntiAim::Friction(Vector &outVel)
{
	float speed, newspeed, control;
	float friction;
	float drop;

	speed = outVel.Length();

	if (speed <= 0.1f)
		return;

	drop = 0;

	// apply ground friction
	if (g_LocalPlayer->m_fFlags() & FL_ONGROUND)
	{
		friction = g_CVar->FindVar("sv_friction")->GetFloat() * g_LocalPlayer->m_surfaceFriction();

		// Bleed off some speed, but if we have less than the bleed
		// threshold, bleed the threshold amount.
		control = (speed < g_CVar->FindVar("sv_stopspeed")->GetFloat()) ? g_CVar->FindVar("sv_stopspeed")->GetFloat() : speed;

		// Add the amount to the drop amount.
		drop += control * friction * g_GlobalVars->frametime;
	}

	newspeed = speed - drop;
	if (newspeed < 0)
		newspeed = 0;

	if (newspeed != speed)
	{
		// Determine proportion of old speed we are using.
		newspeed /= speed;
		// Adjust velocity according to proportion.
		VectorMultiply(outVel, newspeed, outVel);
	}
}