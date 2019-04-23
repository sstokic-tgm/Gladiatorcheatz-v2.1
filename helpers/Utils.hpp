#pragma once

#include "../features/Logger.hpp"

#include <Windows.h>
#include <string>
#include <initializer_list>

struct datamap_t;

namespace Utils
{
	void AttachConsole();
	void DetachConsole();
	bool ConsolePrint(bool logToFile, const char *fmt, ...);
	void Log(const char* fmt, ...);
	std::string GetTimestamp();
	int WaitForModules(std::int32_t timeout, const std::initializer_list<std::wstring> &modules);
	std::uint8_t *PatternScan(void* module, const char* signature);
	DWORD GetModuleBase(char *moduleName);
	void RankRevealAll();
	void IsReady();
	bool IsDangerZone();
	void EventLog(std::string message, Color messageColor = Color::White, bool console = true, std::string consolePrefix = "[Gladiatorcheatz] ", Color consoleColor = Color(50, 122, 239));
	void SetClantag(const char *tag);
	void LoadNamedSkys(const char *sky_name);
	void RandomSeed(int iSeed);
	float RandomFloat(float min, float max);
	int RandomInt(int min, int max);
	unsigned int FindInDataMap(datamap_t *pMap, const char *name);
	float GetNetworkLatency(int type);
}