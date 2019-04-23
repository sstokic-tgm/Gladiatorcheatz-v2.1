#pragma once
#include "../Singleton.hpp"

#include "../misc/Color.hpp"

#include <vector>

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

class Logger : public Singleton<Logger>
{
public:
	void Paint();

	void add(std::string message, Color color)
	{
		eventLog.push_back(MessageInfo(message, color));
	}

private:
	std::vector<MessageInfo> eventLog;
};