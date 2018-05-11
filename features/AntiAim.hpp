#pragma once

#include "../Singleton.hpp"
#include "../Structs.hpp"

class CUserCmd;
class CRecvProxyData;
class Vector;

enum AA_PITCH
{
	AA_PITCH_OFF,
	AA_PITCH_DYNAMIC,
	AA_PITCH_EMOTION,
	AA_PITCH_STRAIGHT,
	AA_PITCH_UP
};

enum AA_YAW
{
	AA_YAW_OFF,
	AA_YAW_BACKWARDS,
	AA_YAW_BACKWARDS_JITTER,
	AA_YAW_MANUALLBY,
	AA_YAW_MANUALLBYJITTER,
};

enum AA_FAKEYAW
{
	AA_FAKEYAW_OFF,
	AA_FAKEYAW_BACKWARDS,
	AA_FAKEYAW_BACKWARDS_JITTER,
	AA_FAKEYAW_EVADE,
	AA_FAKEYAW_MANUALLBY,
	AA_FAKEYAW_MANUALLBYJITTER,
};

class AntiAim : public Singleton<AntiAim>
{

public:

	void Work(CUserCmd *usercmd);
	void UpdateLBYBreaker(CUserCmd *usercmd);
	void Fakewalk(CUserCmd *usercmd);

private:

	float GetPitch();
	float GetYaw();
	float GetFakeYaw();
	void Accelerate(C_BasePlayer *player, Vector &wishdir, float wishspeed, float accel, Vector &outVel);
	void WalkMove(C_BasePlayer *player, Vector &outVel);
	void Friction(Vector &outVel);

	CUserCmd *usercmd = nullptr;

	bool m_bBreakLowerBody = false;
	float_t m_flSpawnTime = 0.f;
	float_t m_flNextBodyUpdate = 0.f;
	CBaseHandle *m_ulEntHandle = nullptr;

	C_CSGOPlayerAnimState *m_serverAnimState = nullptr;
};