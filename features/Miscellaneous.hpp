#pragma once

#include <string>

#include "../Singleton.hpp"
#include "../helpers/Utils.hpp"
#include "../math/QAngle.hpp"

class CUserCmd;
class QAngle;
class CViewSetup;

class Miscellaneous : public Singleton<Miscellaneous>
{
public:

	void Bhop(CUserCmd *userCMD);
	void AutoStrafe(CUserCmd *userCMD);
	void Fakelag(CUserCmd *userCMD);
	inline int32_t GetChocked() { return choked; }
	void ChangeName(const char *name);
	void NameChanger();
	void ChatSpamer();
	void ClanTag();
	void ThirdPerson();

	void FixMovement(CUserCmd *usercmd, QAngle &wish_angle);
	void AntiAim(CUserCmd *usercmd);
	void AutoPistol(CUserCmd *usercmd);

	void PunchAngleFix_RunCommand(void* base_player);
	void PunchAngleFix_FSN();

	int changes = -1;

	template<class T, class U>
	T clamp(T in, U low, U high);

private:

	const char *setStrRight(std::string txt, unsigned int value);
	std::string gladTag = "Gladiatorcheatz ";

	QAngle m_aimPunchAngle[128];

	int32_t choked = 0;
};