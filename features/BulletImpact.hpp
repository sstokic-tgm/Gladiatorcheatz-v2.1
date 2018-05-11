#pragma once

#include "../interfaces/IGameEventmanager.hpp"
#include "../Singleton.hpp"

#include "../math/Vector.hpp"

#include <vector>

struct BulletImpactInfo
{
	float m_flExpTime;
	Vector m_vecHitPos;
};

class BulletImpactEvent : public IGameEventListener2, public Singleton<BulletImpactEvent>
{
public:

	void FireGameEvent(IGameEvent *event);
	int  GetEventDebugID(void);

	void RegisterSelf();
	void UnregisterSelf();

	void Paint(void);

private:

	std::vector<BulletImpactInfo> bulletImpactInfo;
};