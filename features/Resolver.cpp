#include "Resolver.hpp"

#include "../Options.hpp"

#include "LagCompensation.hpp"

void Resolver::Log()
{
	for (int i = 1; i <= g_GlobalVars->maxClients; i++)
	{
		auto &record = arr_infos[i];

		C_BasePlayer *player = C_BasePlayer::GetPlayerByIndex(i);
		if (!player || !player->IsAlive() || player->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
		{
			record.m_bActive = false;
			continue;
		}

		if (player->IsDormant())
			continue;

		if(record.m_flSimulationTime == player->m_flSimulationTime())
			continue;

		record.SaveRecord(player);
		record.m_bActive = true;
	}
}

void Resolver::Resolve()
{
	for (int i = 1; i <= g_GlobalVars->maxClients; i++)
	{
		auto &record = arr_infos[i];
		if (!record.m_bActive)
			continue;

		C_BasePlayer *player = C_BasePlayer::GetPlayerByIndex(i);
		if (!player || !player->IsAlive() || player->IsDormant() || player == g_LocalPlayer)
			continue;

		if (record.m_flVelocity == 0.f && player->m_vecVelocity().Length2D() != 0.f)
		{
			Math::VectorAngles(player->m_vecVelocity(), record.m_angDirectionFirstMoving);
			record.m_nCorrectedFakewalkIdx = 0;
		}

		auto firedShots = g_LocalPlayer->m_iShotsFired();

		if (g_Options.debug_fliponkey)
		{
			float_t new_yaw = player->m_flLowerBodyYawTarget();
			if (g_InputSystem->IsButtonDown(g_Options.debug_flipkey))
				new_yaw += 180.f;
			new_yaw = Math::ClampYaw(new_yaw);
			player->m_angEyeAngles().yaw = new_yaw;
			return;
		}

		if (g_Options.hvh_resolver_override && g_InputSystem->IsButtonDown(g_Options.hvh_resolver_override_key))
		{
			Override(); //needs an improvement sometimes fucked up xD

			Global::resolverModes[player->EntIndex()] = "Overriding";

			return;
		}

		AnimationLayer curBalanceLayer, prevBalanceLayer;

		ResolveInfo curtickrecord;
		curtickrecord.SaveRecord(player);

		if ((player->m_fFlags() & FL_ONGROUND) && (IsFakewalking(player, curtickrecord) || (player->m_vecVelocity().Length2D() > 0.1f && player->m_vecVelocity().Length2D() < 45.f && !(player->m_fFlags() & FL_DUCKING)))) //Fakewalk, shiftwalk check // We have to rework the fakewalk resolving, it sucks :D
		{
			float_t new_yaw = ResolveFakewalk(player, curtickrecord);
			new_yaw = Math::ClampYaw(new_yaw);

			player->m_angEyeAngles().yaw = new_yaw;

			Global::resolverModes[player->EntIndex()] = "Fakewalking";

			continue;
		}
		if (IsEntityMoving(player))
		{
			float_t new_yaw = player->m_flLowerBodyYawTarget();
			new_yaw = Math::ClampYaw(new_yaw);

			player->m_angEyeAngles().yaw = new_yaw;

			record.m_flStandingTime = player->m_flSimulationTime();
			record.m_flMovingLBY = player->m_flLowerBodyYawTarget();
			record.m_bIsMoving = true;

			Global::resolverModes[player->EntIndex()] = "Moving";

			continue;
		}
		if (IsAdjustingBalance(player, curtickrecord, &curBalanceLayer))
		{
			if (fabsf(LBYDelta(curtickrecord)) > 35.f)
			{
				float_t new_yaw = player->m_flLowerBodyYawTarget() + record.m_flLbyDelta;
				new_yaw = Math::ClampYaw(new_yaw);

				player->m_angEyeAngles().yaw = new_yaw;

				Global::resolverModes[player->EntIndex()] = "Fakehead (delta > 35)";
			}
			if (IsAdjustingBalance(player, record, &prevBalanceLayer))
			{
				if ((prevBalanceLayer.m_flCycle != curBalanceLayer.m_flCycle) || curBalanceLayer.m_flWeight == 1.f)
				{
					float
						flAnimTime = curBalanceLayer.m_flCycle,
						flSimTime = player->m_flSimulationTime();

					if (flAnimTime < 0.01f && prevBalanceLayer.m_flCycle > 0.01f && g_Options.rage_lagcompensation && CMBacktracking::Get().IsTickValid(TIME_TO_TICKS(flSimTime - flAnimTime)))
					{
						CMBacktracking::Get().SetOverwriteTick(player, QAngle(player->m_angEyeAngles().pitch, player->m_flLowerBodyYawTarget(), 0), (flSimTime - flAnimTime), 2);
					}

					float_t new_yaw = player->m_flLowerBodyYawTarget();
					new_yaw = Math::ClampYaw(new_yaw);

					player->m_angEyeAngles().yaw = new_yaw;

					Global::resolverModes[player->EntIndex()] = "Breaking LBY";

					continue;
				}
				else if (curBalanceLayer.m_flWeight == 0.f && (prevBalanceLayer.m_flCycle > 0.92f && curBalanceLayer.m_flCycle > 0.92f)) // breaking lby with delta < 120
				{
					if (player->m_flSimulationTime() >= record.m_flStandingTime + 0.22f && record.m_bIsMoving)
					{
						record.m_flLbyDelta = record.m_flLowerBodyYawTarget - player->m_flLowerBodyYawTarget();

						float_t new_yaw = player->m_flLowerBodyYawTarget() + record.m_flLbyDelta;
						new_yaw = Math::ClampYaw(new_yaw);

						player->m_angEyeAngles().yaw = new_yaw;

						record.m_bIsMoving = false;

						Global::resolverModes[player->EntIndex()] = "Breaking LBY (delta < 120)";

						continue;
					}

					if (player->m_flSimulationTime() >= record.m_flStandingTime + 1.32f && std::fabsf(record.m_flLbyDelta) < 35.f)
					{
						record.m_flLbyDelta = record.m_flMovingLBY - player->m_flLowerBodyYawTarget();
						float_t new_yaw = player->m_flLowerBodyYawTarget() + record.m_flLbyDelta;
						new_yaw = Math::ClampYaw(new_yaw);

						player->m_angEyeAngles().yaw = new_yaw;

						record.m_bIsMoving = false;

						Global::resolverModes[player->EntIndex()] = "LBY delta < 35";

						continue;
					}
				}
			}
			else
			{
				float_t new_yaw = player->m_flLowerBodyYawTarget();
				new_yaw = Math::ClampYaw(new_yaw);

				player->m_angEyeAngles().yaw = new_yaw;

				Global::resolverModes[player->EntIndex()] = "Other";

				continue;
			}
		}
		if (player->m_flSimulationTime() >= record.m_flStandingTime + 0.22f && record.m_bIsMoving)
		{
			record.m_flLbyDelta = record.m_flLowerBodyYawTarget - player->m_flLowerBodyYawTarget();

			float_t new_yaw = player->m_flLowerBodyYawTarget() + record.m_flLbyDelta;
			new_yaw = Math::ClampYaw(new_yaw);

			player->m_angEyeAngles().yaw = new_yaw;

			record.m_bIsMoving = false;

			Global::resolverModes[player->EntIndex()] = "Breaking LBY (delta < 120)";

			continue;
		}
		if (player->m_flSimulationTime() >= record.m_flStandingTime + 1.32f && std::fabsf(record.m_flLbyDelta) < 35.f)
		{
			record.m_flLbyDelta = record.m_flMovingLBY - player->m_flLowerBodyYawTarget();
			float_t new_yaw = player->m_flLowerBodyYawTarget() + record.m_flLbyDelta;
			new_yaw = Math::ClampYaw(new_yaw);

			player->m_angEyeAngles().yaw = new_yaw;

			record.m_bIsMoving = false;

			Global::resolverModes[player->EntIndex()] = "LBY delta < 35";

			continue;
		}

		float_t new_yaw = player->m_flLowerBodyYawTarget() + record.m_flLbyDelta;
		new_yaw = Math::ClampYaw(new_yaw);

		player->m_angEyeAngles().yaw = new_yaw;
	}
}

void Resolver::Override()
{
	if (!g_Options.hvh_resolver_override)
		return;

	if (!g_InputSystem->IsButtonDown(g_Options.hvh_resolver_override_key))
		return;

	int w, h, cx, cy;

	g_EngineClient->GetScreenSize(w, h);

	cx = w / 2;
	cy = h / 2;

	Vector crosshair = Vector(cx, cy, 0);

	C_BasePlayer * nearest_player = nullptr;
	float bestFoV = 0;
	Vector bestHead2D;

	for (int i = 1; i <= g_GlobalVars->maxClients; i++) //0 is always the world entity
	{
		C_BasePlayer *player = (C_BasePlayer*)g_EntityList->GetClientEntity(i);

		if (!CMBacktracking::Get().IsPlayerValid(player)) //ghetto
			continue;

		Vector headPos3D = player->GetBonePos(HITBOX_HEAD), headPos2D;

		if (!Math::WorldToScreen(headPos3D, headPos2D))
			continue;

		float FoV2D = crosshair.DistTo(headPos2D);

		if (!nearest_player || FoV2D < bestFoV)
		{
			nearest_player = player;
			bestFoV = FoV2D;
			bestHead2D = headPos2D;
		}
	}

	if (nearest_player) //use pointers and avoid double calling of GetClientEntity
	{
		int minX = cx - (w / 10), maxX = cx + (w / 10);

		if (bestHead2D.x < minX || bestHead2D.x > maxX)
			return;

		int totalWidth = maxX - minX;

		int playerX = bestHead2D.x - minX;

		int yawCorrection = -(((playerX * 360) / totalWidth) - 180);

		float_t new_yaw = yawCorrection;

		Math::ClampYaw(new_yaw);

		nearest_player->m_angEyeAngles().yaw += new_yaw;
	}
}

float_t Resolver::ResolveFakewalk(C_BasePlayer *player, ResolveInfo &curtickrecord)
{
	auto &record = arr_infos[player->EntIndex()];

	float_t yaw;
	int32_t correctedFakewalkIdx = record.m_nCorrectedFakewalkIdx;

	if (correctedFakewalkIdx < 2)
	{
		yaw = record.m_angDirectionFirstMoving.yaw + 180.f;
	}
	else if (correctedFakewalkIdx < 4)
	{
		yaw = player->m_flLowerBodyYawTarget() + record.m_flLbyDelta;
	}
	else if (correctedFakewalkIdx < 6)
	{
		yaw = record.m_angDirectionFirstMoving.yaw;
	}
	else
	{
		QAngle dir;
		Math::VectorAngles(curtickrecord.m_vecVelocity, dir);

		yaw = dir.yaw;
	}

	return yaw;
}

bool Resolver::IsEntityMoving(C_BasePlayer *player)
{
	return (player->m_vecVelocity().Length2D() > 0.1f && player->m_fFlags() & FL_ONGROUND);
} 

bool Resolver::IsAdjustingBalance(C_BasePlayer *player, ResolveInfo &record, AnimationLayer *layer)
{
	for (int i = 0; i < record.m_iLayerCount; i++)
	{
		const int activity = player->GetSequenceActivity(record.animationLayer[i].m_nSequence);
		if (activity == 979)
		{
			*layer = record.animationLayer[i];
			return true;
		}
	}
	return false;
}

bool Resolver::IsAdjustingStopMoving(C_BasePlayer *player, ResolveInfo &record, AnimationLayer *layer)
{
	for (int i = 0; i < record.m_iLayerCount; i++)
	{
		const int activity = player->GetSequenceActivity(record.animationLayer[i].m_nSequence);
		if (activity == 980)
		{
			*layer = record.animationLayer[i];
			return true;
		}
	}
	return false;
}

bool Resolver::IsFakewalking(C_BasePlayer *player, ResolveInfo &record)
{
	bool
		bFakewalking = false,
		stage1 = false,			// stages needed cause we are iterating all layers, eitherwise won't work :)
		stage2 = false,
		stage3 = false;

	for (int i = 0; i < record.m_iLayerCount; i++)
	{
		if (record.animationLayer[i].m_nSequence == 26 && record.animationLayer[i].m_flWeight < 0.4f)
			stage1 = true;
		if (record.animationLayer[i].m_nSequence == 7 && record.animationLayer[i].m_flWeight > 0.001f)
			stage2 = true;
		if (record.animationLayer[i].m_nSequence == 2 && record.animationLayer[i].m_flWeight == 0)
			stage3 = true;
	}

	if (stage1 && stage2)
		if (stage3 || (player->m_fFlags() & FL_DUCKING)) // since weight from stage3 can be 0 aswell when crouching, we need this kind of check, cause you can fakewalk while crouching, thats why it's nested under stage1 and stage2
			bFakewalking = true;
		else
			bFakewalking = false;
	else
		bFakewalking = false;

	return bFakewalking;
}