#pragma once

#include "../Structs.hpp"
#include "../Singleton.hpp"

class AimLegit : public Singleton<AimLegit>
{
public:
	void Work(CUserCmd *usercmd);

private:

	inline void SetTarget(int idx) { m_iTarget = idx; }
	inline bool HasTarget() { return m_iTarget != -1; }

	void GetBestTarget();
	bool CheckTarget(C_BasePlayer *player);
	bool IsBehindSmoke(Vector src, Vector rem);
	void TargetRegion();

	void Triggerbot();
	void RecoilControlSystem();
	
	int m_iTarget = -1;

	CUserCmd *usercmd;
};

using LineGoesThroughSmokeFn = bool(__cdecl*)(Vector, Vector, int16_t);
extern LineGoesThroughSmokeFn LineGoesThroughSmoke;