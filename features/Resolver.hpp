#pragma once

#include "../Singleton.hpp"

#include "../Structs.hpp"

#include "../helpers/Math.hpp"

#include <deque>

class QAngle;
class C_BasePlayer;

struct ResolveInfo
{
	ResolveInfo()
	{
		m_bActive = false;

		m_flVelocity = 0.f;
		m_vecVelocity = Vector(0, 0, 0);
		m_angEyeAngles = QAngle(0, 0, 0);
		m_flSimulationTime = 0.f;
		m_flLowerBodyYawTarget = 0.f;

		m_flStandingTime = 0.f;
		m_flMovingLBY = 0.f;
		m_flLbyDelta = 180.f;
		m_bIsMoving = false;

		m_angDirectionFirstMoving = QAngle(0, 0, 0);
		m_nCorrectedFakewalkIdx = 0;
	}

	void SaveRecord(C_BasePlayer *player)
	{
		m_flLowerBodyYawTarget = player->m_flLowerBodyYawTarget();
		m_flSimulationTime = player->m_flSimulationTime();
		m_flVelocity = player->m_vecVelocity().Length2D();
		m_vecVelocity = player->m_vecVelocity();
		m_angEyeAngles = player->m_angEyeAngles();

		m_iLayerCount = player->GetNumAnimOverlays();
		for (int i = 0; i < m_iLayerCount; i++)
			animationLayer[i] = player->GetAnimOverlays()[i];
	}

	bool operator==(const ResolveInfo &other)
	{
		return other.m_flSimulationTime == m_flSimulationTime;
	}

	bool m_bActive;

	float_t m_flVelocity;
	Vector m_vecVelocity;
	QAngle m_angEyeAngles;
	float_t m_flSimulationTime;
	float_t m_flLowerBodyYawTarget;

	int32_t m_iLayerCount = 0;
	AnimationLayer animationLayer[15];

	float_t m_flStandingTime;
	float_t m_flMovingLBY;
	float_t m_flLbyDelta;
	bool m_bIsMoving;

	QAngle m_angDirectionFirstMoving;
	int32_t m_nCorrectedFakewalkIdx;

	int32_t m_nShotsMissed = 0;
};

class Resolver : public Singleton<Resolver>
{

public:

	void Log();
	void Resolve();

	void Override();

	ResolveInfo arr_infos[65];

private:
	
	float_t ResolveFakewalk(C_BasePlayer *player, ResolveInfo &curtickrecord);
	bool IsEntityMoving(C_BasePlayer *player);
	bool IsAdjustingBalance(C_BasePlayer *player, ResolveInfo &record, AnimationLayer *layer);
	bool IsAdjustingStopMoving(C_BasePlayer *player, ResolveInfo &record, AnimationLayer *layer);
	bool IsFakewalking(C_BasePlayer *player, ResolveInfo &record);

	const inline float_t GetDelta(float_t a, float_t b)
	{
		return fabsf(Math::ClampYaw(a - b));
	}

	const inline float_t LBYDelta(const ResolveInfo &v)
	{
		return v.m_angEyeAngles.yaw - v.m_flLowerBodyYawTarget;
	}
};