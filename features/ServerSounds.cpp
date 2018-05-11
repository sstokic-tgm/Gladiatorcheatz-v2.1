#include "ServerSounds.hpp"
#include "Visuals.hpp"

void ServerSound::Start()
{
	m_utlCurSoundList.RemoveAll();
	g_EngineSound->GetActiveSounds(m_utlCurSoundList);

	// No active sounds.
	if (!m_utlCurSoundList.Count())
		return;

	// Accumulate sounds for esp correction
	for (int iter = 0; iter < m_utlCurSoundList.Count(); iter++)
	{
		SndInfo_t& sound = m_utlCurSoundList[iter];
		if (sound.m_nSoundSource == 0 || // World
			sound.m_nSoundSource > 64)   // Most likely invalid
			continue;

		C_BasePlayer* player = (C_BasePlayer*)g_EntityList->GetClientEntity(sound.m_nSoundSource);

		if (!player || player == g_LocalPlayer || sound.m_pOrigin->IsZero())
			continue;

		if (!ValidSound(sound))
			continue;

		/*
			Missing scoreboard life/team checks.
		*/

		SetupAdjustPlayer(player, sound);

		m_cSoundPlayers[sound.m_nSoundSource].Override(sound);
	}

	for (int iter = 1; iter < g_EntityList->GetHighestEntityIndex(); iter++)
	{
		C_BasePlayer* player = (C_BasePlayer*)g_EntityList->GetClientEntity(iter);
		if (!player || !player->IsDormant() || !player->IsAlive())
			continue;

		AdjustPlayerBegin(player);
	}

	m_utlvecSoundBuffer = m_utlCurSoundList;
}

void ServerSound::SetupAdjustPlayer(C_BasePlayer* player, SndInfo_t& sound)
{
	Vector src3D, dst3D;
	trace_t tr;
	Ray_t ray;
	CTraceFilter filter;

	filter.pSkip = player;
	src3D = (*sound.m_pOrigin) + Vector(0, 0, 1); // So they dont dig into ground incase shit happens /shrug
	dst3D = src3D - Vector(0, 0, 100);
	ray.Init(src3D, dst3D);

	g_EngineTrace->TraceRay(ray, MASK_PLAYERSOLID, &filter, &tr);

	// step = (tr.fraction < 0.20)
	// shot = (tr.fraction > 0.20)
	// stand = (tr.fraction > 0.50)
	// crouch = (tr.fraction < 0.50)

	/* Corrects origin and important flags. */

	// Player stuck, idk how this happened
	if (tr.allsolid)
	{
		m_cSoundPlayers[sound.m_nSoundSource].m_iReceiveTime = -1;
	}

	*sound.m_pOrigin = ((tr.fraction < 0.97) ? tr.endpos : *sound.m_pOrigin);
	m_cSoundPlayers[sound.m_nSoundSource].m_nFlags = player->m_fFlags();
	m_cSoundPlayers[sound.m_nSoundSource].m_nFlags |= (tr.fraction < 0.50f ? FL_DUCKING : 0) | (tr.fraction != 1 ? FL_ONGROUND : 0);   // Turn flags on
	m_cSoundPlayers[sound.m_nSoundSource].m_nFlags &= (tr.fraction > 0.50f ? ~FL_DUCKING : 0) | (tr.fraction == 1 ? ~FL_ONGROUND : 0); // Turn flags off
}

void ServerSound::Finish()
{
	// Do any finishing code here. If we add smtn like sonar radar this will be useful.

	AdjustPlayerFinish();
}

void ServerSound::AdjustPlayerFinish()
{
	// Restore and clear saved players for next loop.
	for (auto& RestorePlayer : m_arRestorePlayers)
	{
		auto player = RestorePlayer.player;
		player->m_fFlags() = RestorePlayer.m_nFlags;
		player->m_vecOrigin() = RestorePlayer.m_vecOrigin;
		player->SetAbsOrigin(RestorePlayer.m_vecAbsOrigin);
		*(bool*)((DWORD)player + 233) = RestorePlayer.m_bDormant; // dormant check
	}
	m_arRestorePlayers.clear();
}

void ServerSound::AdjustPlayerBegin(C_BasePlayer* player)
{
	// Adjusts player's origin and other vars so we can show full-ish esp.
	if (!g_Options.esp_farther)
		return;

	constexpr int EXPIRE_DURATION = 450; // miliseconds-ish?
	auto& sound_player = m_cSoundPlayers[player->EntIndex()];
	bool sound_expired = GetTickCount() - sound_player.m_iReceiveTime > EXPIRE_DURATION;
	if (sound_expired && Visuals::ESP_Fade[player->EntIndex()] == 0.f)
		return;

	SoundPlayer current_player;
	current_player.player = player;
	current_player.m_bDormant = true;
	current_player.m_nFlags = player->m_fFlags();
	current_player.m_vecOrigin = player->m_vecOrigin();
	current_player.m_vecAbsOrigin = player->GetAbsOrigin();
	m_arRestorePlayers.emplace_back(current_player);

	if (!sound_expired)
		*(bool*)((DWORD)player + 233) = false; // dormant check
	player->m_fFlags() = sound_player.m_nFlags;
	player->m_vecOrigin() = sound_player.m_vecOrigin;
	player->SetAbsOrigin(sound_player.m_vecOrigin);
}

bool ServerSound::ValidSound(SndInfo_t& sound)
{
	// Use only server dispatched sounds.
	if (!sound.m_bFromServer)
		return false;

	// We don't want the sound to keep following client's predicted origin.
	for (int iter = 0; iter < m_utlvecSoundBuffer.Count(); iter++)
	{
		SndInfo_t& cached_sound = m_utlvecSoundBuffer[iter];
		if (cached_sound.m_nGuid == sound.m_nGuid)
		{
			return false;
		}
	}

	return true;
}