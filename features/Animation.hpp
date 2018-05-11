#pragma once

#include "../Singleton.hpp"

#include "../Structs.hpp"

struct AnimationInfo
{
	AnimationInfo()
	{
		m_flSpawnTime = 0.f;
		m_ulEntHandle = nullptr;
		m_playerAnimState = nullptr;
	}

	std::array<float_t, 24> m_flPoseParameters;
	AnimationLayer m_AnimationLayer[15];

	float_t m_flSpawnTime;
	CBaseHandle *m_ulEntHandle;

	C_CSGOPlayerAnimState *m_playerAnimState;
};

class Animation : public Singleton<Animation>
{

public:

	void UpdatePlayerAnimations(int32_t idx);

	AnimationInfo &GetPlayerAnimationInfo(int32_t idx);

private:

	AnimationInfo arr_infos[65];
};