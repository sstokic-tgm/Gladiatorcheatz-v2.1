#pragma once

#include "../Singleton.hpp"
#include "../Structs.hpp"
#include "../Options.hpp"

class ServerSound : public Singleton<ServerSound>
{
public:
	// Call before and after ESP.
	void Start();
	void Finish();

private:

	void AdjustPlayerBegin(C_BasePlayer* player);
	void AdjustPlayerFinish();
	void SetupAdjustPlayer(C_BasePlayer* player, SndInfo_t& sound);

	bool ValidSound(SndInfo_t& sound);

	struct SoundPlayer
	{
		void Override(SndInfo_t& sound) {
			m_iIndex = sound.m_nSoundSource;
			m_vecOrigin = *sound.m_pOrigin;
			m_iReceiveTime = GetTickCount();
		}

		int m_iIndex = 0;
		int m_iReceiveTime = 0;
		Vector m_vecOrigin = Vector(0, 0, 0);
		/* Restore data */
		int m_nFlags = 0;
		C_BasePlayer* player = nullptr;
		Vector m_vecAbsOrigin = Vector(0, 0, 0);
		bool m_bDormant = false;
	} m_cSoundPlayers[64];
	CUtlVector<SndInfo_t> m_utlvecSoundBuffer;
	CUtlVector<SndInfo_t> m_utlCurSoundList;
	std::vector<SoundPlayer> m_arRestorePlayers;
};