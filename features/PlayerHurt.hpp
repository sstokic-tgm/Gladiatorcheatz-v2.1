#pragma once

#include "../interfaces/IGameEventmanager.hpp"
#include "../Singleton.hpp"

#include <vector>

struct HitMarkerInfo
{
	float m_flExpTime;
	int m_iDmg;
};

struct EventInfo
{
	std::string m_szMessage;
	float m_flExpTime;
};

class PlayerHurtEvent : public IGameEventListener2, public Singleton<PlayerHurtEvent>
{
public:

	void FireGameEvent(IGameEvent *event);
	int  GetEventDebugID(void);

	void RegisterSelf();
	void UnregisterSelf();

	void Paint(void);

private:

	std::vector<HitMarkerInfo> hitMarkerInfo;
	std::vector<EventInfo> eventInfo;
};