#pragma once

#include "../Singleton.hpp"
#include "../SDK.hpp"

class RebuildGameMovement : public Singleton<RebuildGameMovement>
{
private:

	int bumpcount[64], numbumps[64];
	Vector dir[64];
	float d[64];
	int numplanes[64];
	Vector primal_velocity[64], original_velocity[64];
	Vector new_velocity[64];
	int i[64], j[64];
	trace_t	pm[64];
	Vector end[64];
	Vector vec3_origin[64];
	float time_left[64], allFraction[64];
	int blocked[64];

	float addspeed[64], accelspeed[64], currentspeed[64];
	float wishspd[64];
	Vector wishvel[64];
	float spd[64];
	float fmove[64], smove[64];
	Vector wishdir[64];
	float wishspeed[64];

	Vector dest[64];
	Vector forward[64], right[64], up[64];
	float speed[64], newspeed[64], control[64];
	float friction[64];
	float drop[64];
	float ent_gravity[64];

private:

public:

	void SetAbsOrigin(C_BasePlayer *C_BasePlayer, const Vector &vec);
	void FullWalkMove(C_BasePlayer *C_BasePlayer);
	void CheckVelocity(C_BasePlayer *C_BasePlayer);
	void FinishGravity(C_BasePlayer *C_BasePlayer);
	void StepMove(C_BasePlayer *C_BasePlayer, Vector &vecDestination, trace_t &trace);
	int ClipVelocity(Vector &in, Vector &normal, Vector &out, float overbounce);
	int TryPlayerMove(C_BasePlayer *C_BasePlayer, Vector *pFirstDest, trace_t *pFirstTrace);
	void Accelerate(C_BasePlayer *C_BasePlayer, Vector &wishdir, float wishspeed, float accel);
	void Friction(C_BasePlayer *C_BasePlayer);
	void AirAccelerate(C_BasePlayer *C_BasePlayer, Vector &wishdir, float wishspeed, float accel);
	void AirMove(C_BasePlayer *C_BasePlayer);
	void WalkMove(C_BasePlayer *C_BasePlayer);
	void CheckFalling(C_BasePlayer *C_BasePlayer);
	void StartGravity(C_BasePlayer *C_BasePlayer);
	void TracePlayerBBox(const Vector &start, const Vector &end, unsigned int fMask, int collisionGroup, trace_t &pm, C_BasePlayer *C_BasePlayer);
};