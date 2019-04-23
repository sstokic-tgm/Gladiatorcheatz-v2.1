#pragma once

#include "../interfaces/IGameEventmanager.hpp"
#include "../Singleton.hpp"

#include "../misc/Color.hpp"

#include <vector>

struct HitMarkerInfo
{
	float m_flExpTime;
	int m_iDmg;
};

struct MessageInfo
{
	MessageInfo(std::string text, Color color = Color(255, 255, 255, 255), int time = 5.f)
	{
		this->m_Text = text;
		this->m_Color = color;
		this->m_flTime = time;
	}

	std::string m_Text;
	Color m_Color;
	float m_flTime;
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

public:
	std::vector<MessageInfo> eventLog;
};