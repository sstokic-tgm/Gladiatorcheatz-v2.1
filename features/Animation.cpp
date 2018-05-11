#include "Animation.hpp"

void Animation::UpdatePlayerAnimations(int32_t idx)
{
	C_BasePlayer *player = C_BasePlayer::GetPlayerByIndex(idx);
	if (!player || !player->IsAlive())
		return;

	auto &info = arr_infos[idx];

	bool
		allocate = (info.m_playerAnimState == nullptr),
		change = (!allocate) && (&player->GetRefEHandle() != info.m_ulEntHandle),
		reset = (!allocate && !change) && (player->m_flSpawnTime() != info.m_flSpawnTime);

	// player changed, free old animation state.
	if (change)
		g_pMemAlloc->Free(info.m_playerAnimState);

	// need to reset? (on respawn)
	if (reset)
	{
		// reset animation state.
		C_BasePlayer::ResetAnimationState(info.m_playerAnimState);

		// note new spawn time.
		info.m_flSpawnTime = player->m_flSpawnTime();
	}

	// need to allocate or create new due to player change.
	if (allocate || change)
	{
		C_CSGOPlayerAnimState *state = (C_CSGOPlayerAnimState*)g_pMemAlloc->Alloc(sizeof(C_CSGOPlayerAnimState));

		if (state != nullptr)
			player->CreateAnimationState(state);

		// used to detect if we need to recreate / reset.
		info.m_ulEntHandle = const_cast<CBaseHandle*>(&player->GetRefEHandle());
		info.m_flSpawnTime = player->m_flSpawnTime();

		// note anim state for future use.
		info.m_playerAnimState = state;
	}

	if (!info.m_playerAnimState)
		return;

	std::array<float_t, 24> backup_poses = player->m_flPoseParameter();

	AnimationLayer backup_layers[15];
	std::memcpy(backup_layers, player->GetAnimOverlays(), (sizeof(AnimationLayer) * player->GetNumAnimOverlays()));

	// fixing legs and few other things missing here
	C_BasePlayer::UpdateAnimationState(info.m_playerAnimState, player->m_angEyeAngles());

	info.m_flPoseParameters = player->m_flPoseParameter();
	std::memcpy(info.m_AnimationLayer, player->GetAnimOverlays(), (sizeof(AnimationLayer) * player->GetNumAnimOverlays()));

	player->m_flPoseParameter() = backup_poses;
	std::memcpy(player->GetAnimOverlays(), backup_layers, (sizeof(AnimationLayer) * player->GetNumAnimOverlays()));
}

AnimationInfo &Animation::GetPlayerAnimationInfo(int32_t idx)
{
	return arr_infos[idx];
}