#include "Utils.hpp"

#include "../SDK.hpp"

#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <ctime>
#include <locale>
#include <cstdarg>
#include <vector>
#include <Psapi.h>

#pragma comment(lib, "psapi.lib")

HANDLE _out = NULL, _old_out = NULL;
HANDLE _err = NULL, _old_err = NULL;
HANDLE _in = NULL, _old_in = NULL;

namespace Utils
{
	void AttachConsole()
	{
		_old_out = GetStdHandle(STD_OUTPUT_HANDLE);
		_old_err = GetStdHandle(STD_ERROR_HANDLE);
		_old_in = GetStdHandle(STD_INPUT_HANDLE);

		::AllocConsole() && ::AttachConsole(GetCurrentProcessId());

		_out = GetStdHandle(STD_OUTPUT_HANDLE);
		_err = GetStdHandle(STD_ERROR_HANDLE);
		_in = GetStdHandle(STD_INPUT_HANDLE);

		SetConsoleMode(_out,
			ENABLE_PROCESSED_OUTPUT | ENABLE_WRAP_AT_EOL_OUTPUT);

		SetConsoleMode(_in,
			ENABLE_EXTENDED_FLAGS |
			/*ENABLE_PROCESSED_INPUT |*/ ENABLE_QUICK_EDIT_MODE);
	}

	void DetachConsole()
	{
		if (_out && _err && _in) {
			FreeConsole();

			if (_old_out)
				SetStdHandle(STD_OUTPUT_HANDLE, _old_out);
			if (_old_err)
				SetStdHandle(STD_ERROR_HANDLE, _old_err);
			if (_old_in)
				SetStdHandle(STD_INPUT_HANDLE, _old_in);
		}
	}

	bool ConsolePrint(bool logToFile, const char *fmt, ...)
	{
		char buf[1024] = { 0 };
		va_list va;

		va_start(va, fmt);
		_vsnprintf_s(buf, 1024, fmt, va);
		va_end(va);

		std::string outRes = GetTimestamp() + " " + buf;

		OutputDebugString(outRes.c_str());

		if (logToFile)
		{
			std::ofstream file;
			file.open(std::string("Gladiatorcheatz").append(".log"), std::ios::app);
			file << outRes;
			file.close();
		}

		if (!_out)
			return false;

		return !!WriteConsoleA(_out, outRes.c_str(), static_cast<DWORD>(strlen(outRes.c_str())), nullptr, nullptr);
	}

	void Log(const char* fmt, ...)
	{
		if (!fmt) return;

		va_list va_alist;
		char logBuf[1024] = { 0 };

		va_start(va_alist, fmt);
		_vsnprintf(logBuf + strlen(logBuf), sizeof(logBuf) - strlen(logBuf), fmt, va_alist);
		va_end(va_alist);

		std::ofstream file;

		file.open(std::string("Gladiatorcheatz").append(".log"), std::ios::app);

		file << GetTimestamp() << " " << logBuf << std::endl;

		file.close();
	}

	std::string GetTimestamp()
	{
		std::time_t t = std::time(nullptr);
		std::tm tm;
		localtime_s(&tm, &t);
		std::locale loc(std::cout.getloc());

		std::basic_stringstream<char> ss;
		ss.imbue(loc);
		ss << std::put_time(&tm, "[%A %b %e %H:%M:%S %Y]");

		return ss.str();
	}

	int WaitForModules(std::int32_t timeout, const std::initializer_list<std::wstring> &modules)
	{
		bool signaled[32] = { 0 };
		bool success = false;

		std::uint32_t totalSlept = 0;

		if (timeout == 0)
		{
			for (auto& mod : modules)
			{
				if (GetModuleHandleW(std::data(mod)) == NULL)
					return WAIT_TIMEOUT;
			}
			return WAIT_OBJECT_0;
		}

		if (timeout < 0)
			timeout = INT32_MAX;

		while (true)
		{
			for (auto i = 0u; i < modules.size(); ++i)
			{
				auto& module = *(modules.begin() + i);
				if (!signaled[i] && GetModuleHandleW(std::data(module)) != NULL) {
					signaled[i] = true;

					//
					// Checks if all modules are signaled
					//
					bool done = true;
					for (auto j = 0u; j < modules.size(); ++j)
					{
						if (!signaled[j])
						{
							done = false;
							break;
						}
					}
					if (done)
					{
						success = true;
						goto exit;
					}
				}
			}
			if (totalSlept > std::uint32_t(timeout))
			{
				break;
			}
			Sleep(10);
			totalSlept += 10;
		}

	exit:
		return success ? WAIT_OBJECT_0 : WAIT_TIMEOUT;
	}

	std::uint8_t *PatternScan(void* module, const char* signature)
    {
		FUNCTION_GUARD;

        static auto pattern_to_byte = [](const char* pattern) {
            auto bytes = std::vector<int>{};
            auto start = const_cast<char*>(pattern);
            auto end = const_cast<char*>(pattern) + strlen(pattern);

            for(auto current = start; current < end; ++current) {
                if(*current == '?') {
                    ++current;
                    if(*current == '?')
                        ++current;
                    bytes.push_back(-1);
                } else {
                    bytes.push_back(strtoul(current, &current, 16));
                }
            }
            return bytes;
        };

        auto dosHeader = (PIMAGE_DOS_HEADER)module;
        auto ntHeaders = (PIMAGE_NT_HEADERS)((std::uint8_t*)module + dosHeader->e_lfanew);

        auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
        auto patternBytes = pattern_to_byte(signature);
        auto scanBytes = reinterpret_cast<std::uint8_t*>(module);

        auto s = patternBytes.size();
        auto d = patternBytes.data();

        for(auto i = 0ul; i < sizeOfImage - s; ++i) {
            bool found = true;
            for(auto j = 0ul; j < s; ++j) {
                if(scanBytes[i + j] != d[j] && d[j] != -1) {
                    found = false;
                    break;
                }
            }
            if(found) {
                return &scanBytes[i];
            }
        }

		// Afterwards call server to stop dispatch of cheat and to alert us of update.
		ConsolePrint(true, "A pattern has outtdated: %s", signature);
        return nullptr;
	}

	DWORD GetModuleBase(char *moduleName)
	{
		FUNCTION_GUARD;

		HMODULE hModule;
		MODULEINFO moduleInfo;
		DWORD dModule, dModuleSize;

		hModule = GetModuleHandle(moduleName);
		GetModuleInformation(GetCurrentProcess(), hModule, &moduleInfo, sizeof(MODULEINFO));

		dModule = (DWORD)moduleInfo.lpBaseOfDll;

		return dModule;
	}

	void RankRevealAll()
	{
		FUNCTION_GUARD;

		using ServerRankRevealAll = char(__cdecl*)(int*);

		static auto fnServerRankRevealAll = PatternScan(GetModuleHandle("client.dll"), "55 8B EC 8B 0D ? ? ? ? 68");

		int v[3] = { 0,0,0 };

		reinterpret_cast<ServerRankRevealAll>(fnServerRankRevealAll)(v);
	}

	void IsReady()
	{
		FUNCTION_GUARD;

		using IsReadyFn = void(__cdecl*)();
		static auto fnIsReady = PatternScan(GetModuleHandle("client.dll"), "55 8B EC 83 E4 F8 83 EC 08 56 8B 35 ? ? ? ? 57 83 BE");

		reinterpret_cast<IsReadyFn>(fnIsReady)();
	}

	void SetClantag(const char *tag)
	{
		FUNCTION_GUARD;

		static auto fnClantagChanged = (int(__fastcall*)(const char*, const char*))PatternScan(GetModuleHandle("engine.dll"), "53 56 57 8B DA 8B F9 FF 15");

		fnClantagChanged(tag, tag);
	}

	void LoadNamedSkys(const char *sky_name)
	{
		static auto fnLoadNamedSkys = (void(__fastcall*)(const char*))PatternScan(GetModuleHandle("engine.dll"), "55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45");
		
		fnLoadNamedSkys(sky_name);
	}

	void RandomSeed(int iSeed)
	{
		static auto ranSeed = reinterpret_cast<void(*)(int)>(GetProcAddress(GetModuleHandle("vstdlib.dll"), "RandomSeed"));
		if (ranSeed)
			ranSeed(iSeed);
	}

	float RandomFloat(float min, float max)
	{
		static auto ranFloat = reinterpret_cast<float(*)(float, float)>(GetProcAddress(GetModuleHandle("vstdlib.dll"), "RandomFloat"));
		if (ranFloat)
			return ranFloat(min, max);
		else
			return 0.f;
	}

	int RandomInt(int min, int max)
	{
		static auto ranInt = reinterpret_cast<int(*)(int, int)>(GetProcAddress(GetModuleHandle("vstdlib.dll"), "RandomInt"));
		if (ranInt)
			return ranInt(min, max);
		else
			return 0;
	}

	unsigned int FindInDataMap(datamap_t *pMap, const char *name)
	{
		FUNCTION_GUARD;

		while (pMap)
		{
			for (int i = 0; i<pMap->dataNumFields; i++)
			{
				if (pMap->dataDesc[i].fieldName == NULL)
					continue;

				if (strcmp(name, pMap->dataDesc[i].fieldName) == 0)
					return pMap->dataDesc[i].fieldOffset[TD_OFFSET_NORMAL];

				if (pMap->dataDesc[i].fieldType == FIELD_EMBEDDED)
				{
					if (pMap->dataDesc[i].td)
					{
						unsigned int offset;

						if ((offset = FindInDataMap(pMap->dataDesc[i].td, name)) != 0)
							return offset;
					}
				}
			}
			pMap = pMap->baseMap;
		}

		return 0;
	}

	float GetNetworkLatency(int type)
	{
		FUNCTION_GUARD;

		INetChannelInfo *nec = g_EngineClient->GetNetChannelInfo();
		if (nec)
			return nec->GetAvgLatency(type);
		return 0.0f;
	}
}