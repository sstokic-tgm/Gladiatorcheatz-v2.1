#include "PredictionSystem.hpp"

#include "../helpers/Utils.hpp"

void PredictionSystem::Start(CUserCmd *userCMD, C_BasePlayer* player)
{
	*predictionRandomSeed = MD5_PseudoRandom(userCMD->command_number) & 0x7FFFFFFF;
	predictionPlayer = player;

	m_flOldCurTime = g_GlobalVars->curtime;
	m_flOldFrametime = g_GlobalVars->frametime;

	g_GlobalVars->curtime = player->m_nTickBase() * g_GlobalVars->interval_per_tick;
	g_GlobalVars->frametime = g_GlobalVars->interval_per_tick;

	//Here we're doing CBasePlayer::UpdateButtonState // NOTE: hard to tell when offsets changed, think of more longterm solution or just dont do this.
	moveData.m_nButtons = userCMD->buttons;
	int buttonsChanged = userCMD->buttons ^ *reinterpret_cast<int*>(uintptr_t(player) + 0x31E8);
	*reinterpret_cast<int*>(uintptr_t(player) + 0x31DC) = (uintptr_t(player) + 0x31E8);
	*reinterpret_cast<int*>(uintptr_t(player) + 0x31E8) = userCMD->buttons;
	*reinterpret_cast<int*>(uintptr_t(player) + 0x31E0) = userCMD->buttons & buttonsChanged;  //m_afButtonPressed ~ The changed ones still down are "pressed"
	*reinterpret_cast<int*>(uintptr_t(player) + 0x31E4) = buttonsChanged & ~userCMD->buttons; //m_afButtonReleased ~ The ones not down are "released"

	g_GameMovement->StartTrackPredictionErrors(player);

	memset(&moveData, 0, sizeof(CMoveData));
	g_MoveHelper->SetHost(player);
	g_Prediction->SetupMove(player, userCMD, g_MoveHelper, &moveData);
	g_GameMovement->ProcessMovement(player, &moveData);
	g_Prediction->FinishMove(player, userCMD, &moveData);
}

void PredictionSystem::End(C_BasePlayer* player)
{
	g_GameMovement->FinishTrackPredictionErrors(player);
	g_MoveHelper->SetHost(nullptr);

	*predictionRandomSeed = -1;
	predictionPlayer = nullptr;

	g_GlobalVars->curtime = m_flOldCurTime;
	g_GlobalVars->frametime = m_flOldFrametime;
}