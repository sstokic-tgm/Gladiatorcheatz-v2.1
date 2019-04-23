#include "Logger.hpp"

#include "../Structs.hpp"
#include "Visuals.hpp"

#include "../Options.hpp"

#include <algorithm>

void Logger::Paint()
{
	if (eventLog.size() > 6) //remove if size is > 6
		eventLog.erase(eventLog.begin() + 1);

	int x = 8, y = 7;

	for (size_t i = 0; i < eventLog.size(); ++i) // valve code at it's finest (lmao) https://github.com/VSES/SourceEngine2007/blob/master/se2007/engine/console.cpp#L978-L1010
	{
		eventLog[i].m_flTime -= g_GlobalVars->frametime;

		int fontTall = g_VGuiSurface->GetFontTall(Visuals::log_font) + 1;

		if (eventLog[i].m_flTime < .5f)
		{
			float f = std::clamp(eventLog[i].m_flTime, 0.0f, .5f) / .5f;

			if (i == 0)
			{
				eventLog[i].m_Color.SetAlpha((int)(f * 255.0f));

				if (f < 0.2f)
				{
					y -= fontTall * (1.0f - f / 0.2f) - 2;
				}
			}
		}
		else
		{
			eventLog[i].m_Color.SetAlpha(255);
		}

		if (i == 0 && eventLog[i].m_Color.a() <= .1f)
		{
			eventLog.erase(eventLog.begin() + i);
			eventLog[i].m_flTime = 0.75f;
		}

		Visuals::DrawString(Visuals::log_font, x, y + (i * 16), eventLog[i].m_Color, FONT_LEFT, eventLog[i].m_Text.c_str());
	}
}