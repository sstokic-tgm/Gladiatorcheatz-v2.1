#include "Miscellaneous.hpp"

#include "../Structs.hpp"
#include "../Options.hpp"
#include "../helpers/Math.hpp"

#include "AntiAim.hpp"
#include "AimRage.hpp"
#include "PredictionSystem.hpp"

#include <chrono>

template<class T, class U>
T Clamp(T in, U low, U high)
{
	if (in <= low)
		return low;

	if (in >= high)
		return high;

	return in;
}

std::vector<std::string> msgs =
{
	"Gladiatorcheatz owns me and all",
	"Your p2c sucks my sword",
	"Are you not entertained? Are you not entertained? Is this not why you are here?",
	"You would fight me?",
	"Why not? Do you think I am afraid?",
	"I think you've been afraid all your life.",
	"Strength and honor.",
	"The gods have seen it fit yet again to spread cheeks and jam cock in ass.",
	"A Gladiator does not fear death. He embraces it. Caresses it. Fucks it.",
	"Each time he enters the arena, he slips his cock in the mouth of the beast and prays to thrust home before the jaws snap shut.",
	"You squealed like a girl when I nailed you to the cross.",
	"And your wife... moaned like a whore when I ravaged her again and again... and again.",
	"I am required to kill, so I kill."
};

void Miscellaneous::Bhop(CUserCmd *userCMD)
{
	if (g_LocalPlayer->GetMoveType() == MOVETYPE_NOCLIP || g_LocalPlayer->GetMoveType() == MOVETYPE_LADDER) return;
	if (userCMD->buttons & IN_JUMP && !(g_LocalPlayer->m_fFlags() & FL_ONGROUND)) {
		userCMD->buttons &= ~IN_JUMP;
	}
}

void Miscellaneous::AutoStrafe(CUserCmd *userCMD)
{
	if (g_LocalPlayer->GetMoveType() == MOVETYPE_NOCLIP || g_LocalPlayer->GetMoveType() == MOVETYPE_LADDER || !g_LocalPlayer->IsAlive()) return;

	// If we're not jumping or want to manually move out of the way/jump over an obstacle don't strafe.
	if (!g_InputSystem->IsButtonDown(ButtonCode_t::KEY_SPACE) ||
		g_InputSystem->IsButtonDown(ButtonCode_t::KEY_A) ||
		g_InputSystem->IsButtonDown(ButtonCode_t::KEY_D) ||
		g_InputSystem->IsButtonDown(ButtonCode_t::KEY_S) ||
		g_InputSystem->IsButtonDown(ButtonCode_t::KEY_W))
		return;

	if (!(g_LocalPlayer->m_fFlags() & FL_ONGROUND)) {
		if (userCMD->mousedx > 1 || userCMD->mousedx < -1) {
			userCMD->sidemove = clamp(userCMD->mousedx < 0.f ? -400.f : 400.f, -400, 400);
		}
		else {
			if (g_LocalPlayer->m_vecVelocity().Length2D() == 0 || g_LocalPlayer->m_vecVelocity().Length2D() == NAN || g_LocalPlayer->m_vecVelocity().Length2D() == INFINITE)
			{
				userCMD->forwardmove = 400;
				return;
			}
			userCMD->forwardmove = clamp(5850.f / g_LocalPlayer->m_vecVelocity().Length2D(), -400, 400);
			if (userCMD->forwardmove < -400 || userCMD->forwardmove > 400)
				userCMD->forwardmove = 0;
			userCMD->sidemove = clamp((userCMD->command_number % 2) == 0 ? -400.f : 400.f, -400, 400);
			if (userCMD->sidemove < -400 || userCMD->sidemove > 400)
				userCMD->sidemove = 0;
		}
	}
}

void Miscellaneous::Fakelag(CUserCmd *userCMD)
{
	if (!g_Options.misc_fakelag_enabled)
		return;

	int choke = std::min<int>(g_Options.misc_fakelag_adaptive ? static_cast<int>(std::ceilf(64 / (g_LocalPlayer->m_vecVelocity().Length() * g_GlobalVars->interval_per_tick))) : g_Options.misc_fakelag_value, 14);

	if (Global::bAimbotting && userCMD->buttons & IN_ATTACK)
		return;
	if (g_Options.misc_fakelag_activation_type == 1 && g_LocalPlayer->m_vecVelocity().Length() < 3.0f)
		return;
	if (g_Options.misc_fakelag_activation_type == 2 && (g_LocalPlayer->m_fFlags() & FL_ONGROUND))
		return;

	if (g_Options.misc_fakelag_adaptive && choke > 13)
		return;

	if (!(Global::flFakewalked == PredictionSystem::Get().GetOldCurTime()))
		Global::bSendPacket = (choked > choke);

	if (Global::bSendPacket)
		choked = 0;
	else
		choked++;

	Global::bFakelag = true;
}

void Miscellaneous::ChangeName(const char *name)
{
	ConVar *cv = g_CVar->FindVar("name");
	*(int*)((DWORD)&cv->m_fnChangeCallbacks + 0xC) = 0;
	cv->SetValue(name);
}

void Miscellaneous::NameChanger()
{
	if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return;

	if (changes == -1)
		return;

	long curTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	static long timestamp = curTime;

	if ((curTime - timestamp) < 150)
		return;

	timestamp = curTime;
	++changes;

	if (changes >= 5)
	{
		std::string name = "\n";
		char chars[3] = { '\n', '\0', '\t' };

		for (int i = 0; i < 127; i++)
			name += chars[rand() % 2];

		ChangeName(name.c_str());

		changes = -1;

		return;
	}
	ChangeName(setStrRight("Gladiatorcheatz", strlen("Gladiatorcheatz") + changes));
}

const char *Miscellaneous::setStrRight(std::string txt, unsigned int value)
{
	txt.insert(txt.length(), value - txt.length(), ' ');

	return txt.c_str();
}

void Miscellaneous::ChatSpamer()
{
	if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return;

	long curTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
	static long timestamp = curTime;

	if ((curTime - timestamp) < 850)
		return;

	if (g_Options.misc_chatspamer)
	{
		if (msgs.empty())
			return;

		std::srand(time(NULL));

		std::string msg = msgs[rand() % msgs.size()];

		std::string str;
		str.append("say ");
		str.append(msg);

		g_EngineClient->ExecuteClientCmd(str.c_str());
	}
	timestamp = curTime;
}

void Miscellaneous::ClanTag()
{
	if (!g_EngineClient->IsInGame() || !g_EngineClient->IsConnected())
		return;

	if (!g_Options.misc_animated_clantag)
		return;

	static size_t lastTime = 0;

	if (GetTickCount() > lastTime)
	{
		gladTag += gladTag.at(0);
		gladTag.erase(0, 1);

		Utils::SetClantag(gladTag.c_str());

		lastTime = GetTickCount() + 650;
	}
}

void Miscellaneous::ThirdPerson()
{
	static size_t lastTime = 0;

	if (g_InputSystem->IsButtonDown(g_Options.misc_thirdperson_bind))
	{
		if (GetTickCount() > lastTime) {
			g_Options.misc_thirdperson = !g_Options.misc_thirdperson;

			lastTime = GetTickCount() + 650;
		}
	}

	g_Input->m_fCameraInThirdPerson = g_Options.misc_thirdperson && g_LocalPlayer && g_LocalPlayer->IsAlive();
}

void Miscellaneous::PunchAngleFix_RunCommand(void* base_player)
{
	if (g_LocalPlayer &&  g_LocalPlayer->IsAlive() && g_LocalPlayer == (C_BasePlayer*)base_player)
		m_aimPunchAngle[AimRage::Get().GetTickbase() % 128] = g_LocalPlayer->m_aimPunchAngle();
}

void Miscellaneous::PunchAngleFix_FSN()
{
	if (g_LocalPlayer && g_LocalPlayer->IsAlive())
	{
		QAngle new_punch_angle = m_aimPunchAngle[AimRage::Get().GetTickbase() % 128];

		if (!new_punch_angle.IsValid())
			return;

		g_LocalPlayer->m_aimPunchAngle() = new_punch_angle;
	}
}

template<class T, class U>
T Miscellaneous::clamp(T in, U low, U high)
{
	if (in <= low)
		return low;

	if (in >= high)
		return high;

	return in;
}

void Miscellaneous::FixMovement(CUserCmd *usercmd, QAngle &wish_angle)
{
	Vector view_fwd, view_right, view_up, cmd_fwd, cmd_right, cmd_up;
	auto viewangles = usercmd->viewangles;
	viewangles.Normalize();

	Math::AngleVectors(wish_angle, view_fwd, view_right, view_up);
	Math::AngleVectors(viewangles, cmd_fwd, cmd_right, cmd_up);

	const float v8 = sqrtf((view_fwd.x * view_fwd.x) + (view_fwd.y * view_fwd.y));
	const float v10 = sqrtf((view_right.x * view_right.x) + (view_right.y * view_right.y));
	const float v12 = sqrtf(view_up.z * view_up.z);

	const Vector norm_view_fwd((1.f / v8) * view_fwd.x, (1.f / v8) * view_fwd.y, 0.f);
	const Vector norm_view_right((1.f / v10) * view_right.x, (1.f / v10) * view_right.y, 0.f);
	const Vector norm_view_up(0.f, 0.f, (1.f / v12) * view_up.z);

	const float v14 = sqrtf((cmd_fwd.x * cmd_fwd.x) + (cmd_fwd.y * cmd_fwd.y));
	const float v16 = sqrtf((cmd_right.x * cmd_right.x) + (cmd_right.y * cmd_right.y));
	const float v18 = sqrtf(cmd_up.z * cmd_up.z);

	const Vector norm_cmd_fwd((1.f / v14) * cmd_fwd.x, (1.f / v14) * cmd_fwd.y, 0.f);
	const Vector norm_cmd_right((1.f / v16) * cmd_right.x, (1.f / v16) * cmd_right.y, 0.f);
	const Vector norm_cmd_up(0.f, 0.f, (1.f / v18) * cmd_up.z);

	const float v22 = norm_view_fwd.x * usercmd->forwardmove;
	const float v26 = norm_view_fwd.y * usercmd->forwardmove;
	const float v28 = norm_view_fwd.z * usercmd->forwardmove;
	const float v24 = norm_view_right.x * usercmd->sidemove;
	const float v23 = norm_view_right.y * usercmd->sidemove;
	const float v25 = norm_view_right.z * usercmd->sidemove;
	const float v30 = norm_view_up.x * usercmd->upmove;
	const float v27 = norm_view_up.z * usercmd->upmove;
	const float v29 = norm_view_up.y * usercmd->upmove;

	usercmd->forwardmove = ((((norm_cmd_fwd.x * v24) + (norm_cmd_fwd.y * v23)) + (norm_cmd_fwd.z * v25))
		+ (((norm_cmd_fwd.x * v22) + (norm_cmd_fwd.y * v26)) + (norm_cmd_fwd.z * v28)))
		+ (((norm_cmd_fwd.y * v30) + (norm_cmd_fwd.x * v29)) + (norm_cmd_fwd.z * v27));
	usercmd->sidemove = ((((norm_cmd_right.x * v24) + (norm_cmd_right.y * v23)) + (norm_cmd_right.z * v25))
		+ (((norm_cmd_right.x * v22) + (norm_cmd_right.y * v26)) + (norm_cmd_right.z * v28)))
		+ (((norm_cmd_right.x * v29) + (norm_cmd_right.y * v30)) + (norm_cmd_right.z * v27));
	usercmd->upmove = ((((norm_cmd_up.x * v23) + (norm_cmd_up.y * v24)) + (norm_cmd_up.z * v25))
		+ (((norm_cmd_up.x * v26) + (norm_cmd_up.y * v22)) + (norm_cmd_up.z * v28)))
		+ (((norm_cmd_up.x * v30) + (norm_cmd_up.y * v29)) + (norm_cmd_up.z * v27));

	usercmd->forwardmove = clamp(usercmd->forwardmove, -450.f, 450.f);
	usercmd->sidemove = clamp(usercmd->sidemove, -450.f, 450.f);
	usercmd->upmove = clamp(usercmd->upmove, -320.f, 320.f);
}

void Miscellaneous::AutoPistol(CUserCmd *usercmd)
{
	if (!g_Options.misc_auto_pistol)
		return;

	if (!g_LocalPlayer)
		return;

	C_BaseCombatWeapon* local_weapon = g_LocalPlayer->m_hActiveWeapon().Get();
	if (!local_weapon)
		return;

	if (!local_weapon->IsPistol())
		return;

	float cur_time = AimRage::Get().GetTickbase() * g_GlobalVars->interval_per_tick;
	if (cur_time >= local_weapon->m_flNextPrimaryAttack() && cur_time >= g_LocalPlayer->m_flNextAttack())
		return;

	usercmd->buttons &= (local_weapon->m_iItemDefinitionIndex() == WEAPON_REVOLVER ? ~IN_ATTACK2 : ~IN_ATTACK);
}

void Miscellaneous::AntiAim(CUserCmd *usercmd)
{
	if (!g_LocalPlayer || !g_LocalPlayer->m_hActiveWeapon().Get())
		return;

	const bool can_shoot = g_LocalPlayer->m_hActiveWeapon().Get()->CanFire();

	if ((!(usercmd->buttons & IN_ATTACK) || !(can_shoot)) && !(usercmd->buttons & IN_USE))
	{
		if (g_Options.hvh_antiaim_y || g_Options.hvh_antiaim_x)
		{
			Global::bAimbotting = false;
			AntiAim::Get().Work(usercmd);
		}
	}
}