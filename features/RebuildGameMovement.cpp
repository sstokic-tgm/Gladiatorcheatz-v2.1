#include "RebuildGameMovement.hpp"

#include "../Structs.hpp"
#include "../helpers/Math.hpp"

void RebuildGameMovement::SetAbsOrigin(C_BasePlayer *player, const Vector &vec)
{
	player->SetAbsOrigin(vec);
}

int RebuildGameMovement::ClipVelocity(Vector &in, Vector &normal, Vector &out, float overbounce)
{
	float	backoff;
	float	change;
	float angle;
	int		i, blocked;

	angle = normal[2];

	blocked = 0x00;         // Assume unblocked.
	if (angle > 0)			// If the plane that is blocking us has a positive z component, then assume it's a floor.
		blocked |= 0x01;	// 
	if (!angle)				// If the plane has no Z, it is vertical (wall/step)
		blocked |= 0x02;	// 

							// Determine how far along plane to slide based on incoming direction.
	backoff = in.Dot(normal) * overbounce;

	for (i = 0; i<3; i++)
	{
		change = normal[i] * backoff;
		out[i] = in[i] - change;
	}

	// iterate once to make sure we aren't still moving through the plane
	float adjust = out.Dot(normal);
	if (adjust < 0.0f)
	{
		out -= (normal * adjust);
		//		Msg( "Adjustment = %lf\n", adjust );
	}

	// Return blocking flags.
	return blocked;
}

int RebuildGameMovement::TryPlayerMove(C_BasePlayer *player, Vector *pFirstDest, trace_t *pFirstTrace)
{
	Vector  planes[5];
	numbumps[player->EntIndex()] = 4;           // Bump up to four times

	blocked[player->EntIndex()] = 0;           // Assume not blocked
	numplanes[player->EntIndex()] = 0;           //  and not sliding along any planes

	original_velocity[player->EntIndex()] = player->m_vecVelocity(); // Store original velocity
	primal_velocity[player->EntIndex()] = player->m_vecVelocity();

	allFraction[player->EntIndex()] = 0;
	time_left[player->EntIndex()] = g_GlobalVars->frametime;   // Total time for this movement operation.

	new_velocity[player->EntIndex()].Zero();

	for (bumpcount[player->EntIndex()] = 0; bumpcount[player->EntIndex()] < numbumps[player->EntIndex()]; bumpcount[player->EntIndex()]++)
	{
		if (player->m_vecVelocity().Length() == 0.0)
			break;

		// Assume we can move all the way from the current origin to the
		//  end point.
		VectorMA(player->GetAbsOrigin(), time_left[player->EntIndex()], player->m_vecVelocity(), end[player->EntIndex()]);

		// See if we can make it from origin to end point.
		if (true)
		{
			// If their velocity Z is 0, then we can avoid an extra trace here during WalkMove.
			if (pFirstDest && end[player->EntIndex()] == *pFirstDest)
				pm[player->EntIndex()] = *pFirstTrace;
			else
			{
				TracePlayerBBox(player->GetAbsOrigin(), end[player->EntIndex()], MASK_PLAYERSOLID, 8, pm[player->EntIndex()], player);
			}
		}
		else
		{
			TracePlayerBBox(player->GetAbsOrigin(), end[player->EntIndex()], MASK_PLAYERSOLID, 8, pm[player->EntIndex()], player);
		}

		allFraction[player->EntIndex()] += pm[player->EntIndex()].fraction;

		// If we started in a solid object, or we were in solid space
		//  the whole way, zero out our velocity and return that we
		//  are blocked by floor and wall.
		if (pm[player->EntIndex()].allsolid)
		{
			// C_BasePlayer is trapped in another solid
			player->m_vecVelocity() = vec3_origin[player->EntIndex()];
			return 4;
		}

		// If we moved some portion of the total distance, then
		//  copy the end position into the pmove.origin and 
		//  zero the plane counter.
		if (pm[player->EntIndex()].fraction > 0)
		{
			if (numbumps[player->EntIndex()] > 0 && pm[player->EntIndex()].fraction == 1)
			{
				// There's a precision issue with terrain tracing that can cause a swept box to successfully trace
				// when the end position is stuck in the triangle.  Re-run the test with an uswept box to catch that
				// case until the bug is fixed.
				// If we detect getting stuck, don't allow the movement
				trace_t stuck;
				TracePlayerBBox(pm[player->EntIndex()].endpos, pm[player->EntIndex()].endpos, MASK_PLAYERSOLID, 8, stuck, player);
				if (stuck.startsolid || stuck.fraction != 1.0f)
				{
					//Msg( "Player will become stuck!!!\n" );
					player->m_vecVelocity() = vec3_origin[player->EntIndex()];
					break;
				}
			}

			// actually covered some distance
			SetAbsOrigin(player, pm[player->EntIndex()].endpos);
			original_velocity[player->EntIndex()] = player->m_vecVelocity();
			numplanes[player->EntIndex()] = 0;
		}

		// If we covered the entire distance, we are done
		//  and can return.
		if (pm[player->EntIndex()].fraction == 1)
		{
			break;		// moved the entire distance
		}

		// If the plane we hit has a high z component in the normal, then
		//  it's probably a floor
		if (pm[player->EntIndex()].plane.normal[2] > 0.7)
		{
			blocked[player->EntIndex()] |= 1;		// floor
		}
		// If the plane has a zero z component in the normal, then it's a 
		//  step or wall
		if (!pm[player->EntIndex()].plane.normal[2])
		{
			blocked[player->EntIndex()] |= 2;		// step / wall
		}

		// Reduce amount of m_flFrameTime left by total time left * fraction
		//  that we covered.
		time_left[player->EntIndex()] -= time_left[player->EntIndex()] * pm[player->EntIndex()].fraction;

		// Did we run out of planes to clip against?
		if (numplanes[player->EntIndex()] >= 5)
		{
			// this shouldn't really happen
			//  Stop our movement if so.
			player->m_vecVelocity() = vec3_origin[player->EntIndex()];
			//Con_DPrintf("Too many planes 4\n");

			break;
		}

		// Set up next clipping plane
		planes[numplanes[player->EntIndex()]] = pm[player->EntIndex()].plane.normal;
		numplanes[player->EntIndex()]++;

		// modify original_velocity so it parallels all of the clip planes
		//

		// reflect player velocity 
		// Only give this a try for first impact plane because you can get yourself stuck in an acute corner by jumping in place
		//  and pressing forward and nobody was really using this bounce/reflection feature anyway...
		if (numplanes[player->EntIndex()] == 1 &&
			player->m_fFlags() & FL_ONGROUND)
		{
			for (i[player->EntIndex()] = 0; i[player->EntIndex()] < numplanes[player->EntIndex()]; i[player->EntIndex()]++)
			{
				if (planes[i[player->EntIndex()]][2] > 0.7)
				{
					// floor or slope
					ClipVelocity(original_velocity[player->EntIndex()], planes[i[player->EntIndex()]], new_velocity[player->EntIndex()], 1);
					original_velocity[player->EntIndex()] = new_velocity[player->EntIndex()];
				}
				else
				{
					ClipVelocity(original_velocity[player->EntIndex()], planes[i[player->EntIndex()]], new_velocity[player->EntIndex()], 1.0 + g_CVar->FindVar("sv_bounce")->GetFloat() * (1 - player->m_surfaceFriction()));
				}
			}

			player->m_vecVelocity() = new_velocity[player->EntIndex()];
			original_velocity[player->EntIndex()] = new_velocity[player->EntIndex()];
		}
		else
		{
			for (i[player->EntIndex()] = 0; i[player->EntIndex()] < numplanes[player->EntIndex()]; i[player->EntIndex()]++)
			{


				for (j[player->EntIndex()] = 0; j[player->EntIndex()]<numplanes[player->EntIndex()]; j[player->EntIndex()]++)
					if (j[player->EntIndex()] != i[player->EntIndex()])
					{
						// Are we now moving against this plane?
						if (player->m_vecVelocity().Dot(planes[j[player->EntIndex()]]) < 0)
							break;	// not ok
					}
				if (j[player->EntIndex()] == numplanes[player->EntIndex()])  // Didn't have to clip, so we're ok
					break;
			}

			// Did we go all the way through plane set
			if (i[player->EntIndex()] != numplanes[player->EntIndex()])
			{	// go along this plane
				// pmove.velocity is set in clipping call, no need to set again.
				;
			}
			else
			{	// go along the crease
				if (numplanes[player->EntIndex()] != 2)
				{
					player->m_vecVelocity() = vec3_origin[player->EntIndex()];
					break;
				}

				dir[player->EntIndex()] = planes[0].Cross(planes[1]);
				dir[player->EntIndex()].NormalizeInPlace();
				d[player->EntIndex()] = dir[player->EntIndex()].Dot(player->m_vecVelocity());
				VectorMultiply(dir[player->EntIndex()], d[player->EntIndex()], player->m_vecVelocity());
			}

			//
			// if original velocity is against the original velocity, stop dead
			// to avoid tiny occilations in sloping corners
			//
			d[player->EntIndex()] = player->m_vecVelocity().Dot(primal_velocity[player->EntIndex()]);
			if (d[player->EntIndex()] <= 0)
			{
				//Con_DPrintf("Back\n");
				player->m_vecVelocity() = vec3_origin[player->EntIndex()];
				break;
			}
		}
	}

	if (allFraction == 0)
	{
		player->m_vecVelocity() = vec3_origin[player->EntIndex()];
	}

	// Check if they slammed into a wall
	float fSlamVol = 0.0f;

	float fLateralStoppingAmount = primal_velocity[player->EntIndex()].Length2D() - player->m_vecVelocity().Length2D();
	if (fLateralStoppingAmount > 580.f * 2.0f)
	{
		fSlamVol = 1.0f;
	}
	else if (fLateralStoppingAmount > 580.f)
	{
		fSlamVol = 0.85f;
	}

	return blocked[player->EntIndex()];
}

void RebuildGameMovement::Accelerate(C_BasePlayer *player, Vector &wishdir, float wishspeed, float accel)
{
	// See if we are changing direction a bit
	currentspeed[player->EntIndex()] = player->m_vecVelocity().Dot(wishdir);

	// Reduce wishspeed by the amount of veer.
	addspeed[player->EntIndex()] = wishspeed - currentspeed[player->EntIndex()];

	// If not going to add any speed, done.
	if (addspeed[player->EntIndex()] <= 0)
		return;

	// Determine amount of accleration.
	accelspeed[player->EntIndex()] = accel * g_GlobalVars->frametime * wishspeed * player->m_surfaceFriction();

	// Cap at addspeed
	if (accelspeed[player->EntIndex()] > addspeed[player->EntIndex()])
		accelspeed[player->EntIndex()] = addspeed[player->EntIndex()];

	// Adjust velocity.
	for (i[player->EntIndex()] = 0; i[player->EntIndex()]<3; i[player->EntIndex()]++)
	{
		player->m_vecVelocity()[i[player->EntIndex()]] += accelspeed[player->EntIndex()] * wishdir[i[player->EntIndex()]];
	}
}

void RebuildGameMovement::AirAccelerate(C_BasePlayer *player, Vector &wishdir, float wishspeed, float accel)
{

	wishspd[player->EntIndex()] = wishspeed;

	// Cap speed
	if (wishspd[player->EntIndex()] > 30.f)
		wishspd[player->EntIndex()] = 30.f;

	// Determine veer amount
	currentspeed[player->EntIndex()] = player->m_vecVelocity().Dot(wishdir);

	// See how much to add
	addspeed[player->EntIndex()] = wishspd[player->EntIndex()] - currentspeed[player->EntIndex()];

	// If not adding any, done.
	if (addspeed <= 0)
		return;

	// Determine acceleration speed after acceleration
	accelspeed[player->EntIndex()] = accel * wishspeed * g_GlobalVars->frametime * player->m_surfaceFriction();

	// Cap it
	if (accelspeed[player->EntIndex()] > addspeed[player->EntIndex()])
		accelspeed[player->EntIndex()] = addspeed[player->EntIndex()];

	// Adjust pmove vel.
	for (i[player->EntIndex()] = 0; i[player->EntIndex()]<3; i[player->EntIndex()]++)
	{
		player->m_vecVelocity()[i[player->EntIndex()]] += accelspeed[player->EntIndex()] * wishdir[i[player->EntIndex()]];
		g_MoveHelper->SetHost(player);
		g_MoveHelper->m_outWishVel[i[player->EntIndex()]] += accelspeed[player->EntIndex()] * wishdir[i[player->EntIndex()]];

	}
}

void RebuildGameMovement::AirMove(C_BasePlayer *player)
{
	Math::AngleVectors(player->m_angEyeAngles(), forward[player->EntIndex()], right[player->EntIndex()], up[player->EntIndex()]);  // Determine movement angles

																																						 // Copy movement amounts
	g_MoveHelper->SetHost(player);
	fmove[player->EntIndex()] = g_MoveHelper->m_flForwardMove;
	smove[player->EntIndex()] = g_MoveHelper->m_flSideMove;

	// Zero out z components of movement vectors
	forward[player->EntIndex()][2] = 0;
	right[player->EntIndex()][2] = 0;
	Math::NormalizeVector(forward[player->EntIndex()]);  // Normalize remainder of vectors
	Math::NormalizeVector(right[player->EntIndex()]);    // 

	for (i[player->EntIndex()] = 0; i[player->EntIndex()]<2; i[player->EntIndex()]++)       // Determine x and y parts of velocity
		wishvel[player->EntIndex()][i[player->EntIndex()]] = forward[player->EntIndex()][i[player->EntIndex()]] * fmove[player->EntIndex()] + right[player->EntIndex()][i[player->EntIndex()]] * smove[player->EntIndex()];

	wishvel[player->EntIndex()][2] = 0;             // Zero out z part of velocity

	wishdir[player->EntIndex()] = wishvel[player->EntIndex()]; // Determine maginitude of speed of move
	wishspeed[player->EntIndex()] = wishdir[player->EntIndex()].Normalize();

	//
	// clamp to server defined max speed
	//
	if (wishspeed != 0 && (wishspeed[player->EntIndex()] > player->m_flMaxspeed()))
	{
		VectorMultiply(wishvel[player->EntIndex()], player->m_flMaxspeed() / wishspeed[player->EntIndex()], wishvel[player->EntIndex()]);
		wishspeed[player->EntIndex()] = player->m_flMaxspeed();
	}

	AirAccelerate(player, wishdir[player->EntIndex()], wishspeed[player->EntIndex()], g_CVar->FindVar("sv_airaccelerate")->GetFloat());

	// Add in any base velocity to the current velocity.
	VectorAdd(player->m_vecVelocity(), player->m_vecBaseVelocity(), player->m_vecVelocity());
	trace_t trace;
	TryPlayerMove(player, &dest[player->EntIndex()], &trace);

	// Now pull the base velocity back out.   Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
	VectorSubtract(player->m_vecVelocity(), player->m_vecBaseVelocity(), player->m_vecVelocity());
}

void RebuildGameMovement::StepMove(C_BasePlayer *player, Vector &vecDestination, trace_t &trace)
{
	Vector vecEndPos;
	vecEndPos = vecDestination;

	// Try sliding forward both on ground and up 16 pixels
	//  take the move that goes farthest
	Vector vecPos, vecVel;
	vecPos = player->GetAbsOrigin();
	vecVel = player->m_vecVelocity();

	// Slide move down.
	TryPlayerMove(player, &vecEndPos, &trace);

	// Down results.
	Vector vecDownPos, vecDownVel;
	vecDownPos = player->GetAbsOrigin();
	vecDownVel = player->m_vecVelocity();

	// Reset original values.
	SetAbsOrigin(player, vecPos);
	player->m_vecVelocity() = vecVel;

	// Move up a stair height.
	vecEndPos = player->GetAbsOrigin();

	vecEndPos.z += player->m_flStepSize() + 0.03125;


	TracePlayerBBox(player->GetAbsOrigin(), vecEndPos, MASK_PLAYERSOLID, 8, trace, player);
	if (!trace.startsolid && !trace.allsolid)
	{
		SetAbsOrigin(player, trace.endpos);
	}

	TryPlayerMove(player, &dest[player->EntIndex()], &trace);

	// Move down a stair (attempt to).
	vecEndPos = player->GetAbsOrigin();

	vecEndPos.z -= player->m_flStepSize() + 0.03125;


	TracePlayerBBox(player->GetAbsOrigin(), vecEndPos, MASK_PLAYERSOLID, 8, trace, player);

	// If we are not on the ground any more then use the original movement attempt.
	if (trace.plane.normal[2] < 0.7)
	{
		SetAbsOrigin(player, vecDownPos);
		player->m_vecVelocity() = vecDownVel;

		float flStepDist = player->GetAbsOrigin().z - vecPos.z;
		if (flStepDist > 0.0f)
		{
			g_MoveHelper->SetHost(player);
			g_MoveHelper->m_outStepHeight += flStepDist;
			g_MoveHelper->SetHost(nullptr);
		}
		return;
	}

	// If the trace ended up in empty space, copy the end over to the origin.
	if (!trace.startsolid && !trace.allsolid)
	{
		player->SetAbsOrigin(trace.endpos);
	}

	// Copy this origin to up.
	Vector vecUpPos;
	vecUpPos = player->GetAbsOrigin();

	// decide which one went farther
	float flDownDist = (vecDownPos.x - vecPos.x) * (vecDownPos.x - vecPos.x) + (vecDownPos.y - vecPos.y) * (vecDownPos.y - vecPos.y);
	float flUpDist = (vecUpPos.x - vecPos.x) * (vecUpPos.x - vecPos.x) + (vecUpPos.y - vecPos.y) * (vecUpPos.y - vecPos.y);
	if (flDownDist > flUpDist)
	{
		SetAbsOrigin(player, vecDownPos);
		player->m_vecVelocity() = vecDownVel;
	}
	else
	{
		// copy z value from slide move
		player->m_vecVelocity() = vecDownVel;
	}

	float flStepDist = player->GetAbsOrigin().z - vecPos.z;
	if (flStepDist > 0)
	{
		g_MoveHelper->SetHost(player);
		g_MoveHelper->m_outStepHeight += flStepDist;
		g_MoveHelper->SetHost(nullptr);
	}
}

void RebuildGameMovement::TracePlayerBBox(const Vector &start, const Vector &end, unsigned int fMask, int collisionGroup, trace_t& pm, C_BasePlayer *player)
{
	Ray_t ray;
	CTraceFilter filter;
	filter.pSkip = reinterpret_cast<void*>(player);

	ray.Init(start, end, player->GetCollideable()->OBBMins(), player->GetCollideable()->OBBMaxs());
	g_EngineTrace->TraceRay(ray, fMask, &filter, &pm);
}

void RebuildGameMovement::WalkMove(C_BasePlayer *player)
{
	Math::AngleVectors(player->m_angEyeAngles(), forward[player->EntIndex()], right[player->EntIndex()], up[player->EntIndex()]);  // Determine movement angles
																																						 // Copy movement amounts
	g_MoveHelper->SetHost(player);
	fmove[player->EntIndex()] = g_MoveHelper->m_flForwardMove;
	smove[player->EntIndex()] = g_MoveHelper->m_flSideMove;
	g_MoveHelper->SetHost(nullptr);


	if (forward[player->EntIndex()][2] != 0)
	{
		forward[player->EntIndex()][2] = 0;
		Math::NormalizeVector(forward[player->EntIndex()]);
	}

	if (right[player->EntIndex()][2] != 0)
	{
		right[player->EntIndex()][2] = 0;
		Math::NormalizeVector(right[player->EntIndex()]);
	}


	for (i[player->EntIndex()] = 0; i[player->EntIndex()]<2; i[player->EntIndex()]++)       // Determine x and y parts of velocity
		wishvel[player->EntIndex()][i[player->EntIndex()]] = forward[player->EntIndex()][i[player->EntIndex()]] * fmove[player->EntIndex()] + right[player->EntIndex()][i[player->EntIndex()]] * smove[player->EntIndex()];

	wishvel[player->EntIndex()][2] = 0;             // Zero out z part of velocity

	wishdir[player->EntIndex()] = wishvel[player->EntIndex()]; // Determine maginitude of speed of move
	wishspeed[player->EntIndex()] = wishdir[player->EntIndex()].Normalize();

	//
	// Clamp to server defined max speed
	//
	g_MoveHelper->SetHost(player);
	if ((wishspeed[player->EntIndex()] != 0.0f) && (wishspeed[player->EntIndex()] > g_MoveHelper->m_flMaxSpeed))
	{
		VectorMultiply(wishvel[player->EntIndex()], player->m_flMaxspeed() / wishspeed[player->EntIndex()], wishvel[player->EntIndex()]);
		wishspeed[player->EntIndex()] = player->m_flMaxspeed();
	}
	g_MoveHelper->SetHost(nullptr);
	// Set pmove velocity
	player->m_vecVelocity()[2] = 0;
	Accelerate(player, wishdir[player->EntIndex()], wishspeed[player->EntIndex()], g_CVar->FindVar("sv_accelerate")->GetFloat());
	player->m_vecVelocity()[2] = 0;

	// Add in any base velocity to the current velocity.
	VectorAdd(player->m_vecVelocity(), player->m_vecBaseVelocity(), player->m_vecVelocity());

	spd[player->EntIndex()] = player->m_vecVelocity().Length();

	if (spd[player->EntIndex()] < 1.0f)
	{
		player->m_vecVelocity().Zero();
		// Now pull the base velocity back out.   Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
		VectorSubtract(player->m_vecVelocity(), player->m_vecBaseVelocity(), player->m_vecVelocity());
		return;
	}

	// first try just moving to the destination	
	dest[player->EntIndex()][0] = player->GetAbsOrigin()[0] + player->m_vecVelocity()[0] * g_GlobalVars->frametime;
	dest[player->EntIndex()][1] = player->GetAbsOrigin()[1] + player->m_vecVelocity()[1] * g_GlobalVars->frametime;
	dest[player->EntIndex()][2] = player->GetAbsOrigin()[2];

	// first try moving directly to the next spot
	TracePlayerBBox(player->GetAbsOrigin(), dest[player->EntIndex()], MASK_PLAYERSOLID, 8, pm[player->EntIndex()], player);

	// If we made it all the way, then copy trace end as new player position.
	g_MoveHelper->SetHost(player);
	g_MoveHelper->m_outWishVel += wishdir[player->EntIndex()] * wishspeed[player->EntIndex()];
	g_MoveHelper->SetHost(nullptr);

	if (pm[player->EntIndex()].fraction == 1)
	{
		player->SetAbsOrigin(pm[player->EntIndex()].endpos);
		// Now pull the base velocity back out.   Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
		VectorSubtract(player->m_vecVelocity(), player->m_vecBaseVelocity(), player->m_vecVelocity());

		return;
	}

	// Don't walk up stairs if not on ground.
	if (!(player->m_fFlags() & FL_ONGROUND))
	{
		// Now pull the base velocity back out.   Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
		VectorSubtract(player->m_vecVelocity(), player->m_vecBaseVelocity(), player->m_vecVelocity());
		return;
	}

	StepMove(player, dest[player->EntIndex()], pm[player->EntIndex()]);

	// Now pull the base velocity back out.   Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
	VectorSubtract(player->m_vecVelocity(), player->m_vecBaseVelocity(), player->m_vecVelocity());
}

void RebuildGameMovement::FinishGravity(C_BasePlayer *player)
{
	float ent_gravity;

	ent_gravity = g_CVar->FindVar("sv_gravity")->GetFloat();

	// Get the correct velocity for the end of the dt 
	player->m_vecVelocity()[2] -= (ent_gravity * g_CVar->FindVar("sv_gravity")->GetFloat() * g_GlobalVars->frametime * 0.5);

	CheckVelocity(player);
}

void RebuildGameMovement::FullWalkMove(C_BasePlayer *player)
{

	StartGravity(player);

	// Fricion is handled before we add in any base velocity. That way, if we are on a conveyor, 
	//  we don't slow when standing still, relative to the conveyor.
	if (player->m_fFlags() & FL_ONGROUND)
	{
		player->m_vecVelocity()[2] = 0.0;
		Friction(player);
	}

	// Make sure velocity is valid.
	CheckVelocity(player);

	if (player->m_fFlags() & FL_ONGROUND)
	{
		WalkMove(player);
	}
	else
	{
		AirMove(player);  // Take into account movement when in air.
	}

	// Make sure velocity is valid.
	CheckVelocity(player);

	// Add any remaining gravitational component.
	FinishGravity(player);


	// If we are on ground, no downward velocity.
	if (player->m_fFlags() & FL_ONGROUND)
	{
		player->m_vecVelocity()[2] = 0;
	}

	CheckFalling(player);
}

void RebuildGameMovement::Friction(C_BasePlayer *player)
{
	// Calculate speed
	speed[player->EntIndex()] = player->m_vecVelocity().Length();

	// If too slow, return
	if (speed[player->EntIndex()] < 0.1f)
	{
		return;
	}

	drop[player->EntIndex()] = 0;

	// apply ground friction
	if (player->m_fFlags() & FL_ONGROUND)  // On an C_BasePlayer that is the ground
	{
		friction[player->EntIndex()] = g_CVar->FindVar("sv_friction")->GetFloat() * player->m_surfaceFriction();

		//  Bleed off some speed, but if we have less than the bleed
		//  threshold, bleed the threshold amount.


		control[player->EntIndex()] = (speed[player->EntIndex()] < g_CVar->FindVar("sv_stopspeed")->GetFloat()) ? g_CVar->FindVar("sv_stopspeed")->GetFloat() : speed[player->EntIndex()];

		// Add the amount to the drop amount.
		drop[player->EntIndex()] += control[player->EntIndex()] * friction[player->EntIndex()] * g_GlobalVars->frametime;
	}

	// scale the velocity
	newspeed[player->EntIndex()] = speed[player->EntIndex()] - drop[player->EntIndex()];
	if (newspeed[player->EntIndex()] < 0)
		newspeed[player->EntIndex()] = 0;

	if (newspeed[player->EntIndex()] != speed[player->EntIndex()])
	{
		// Determine proportion of old speed we are using.
		newspeed[player->EntIndex()] /= speed[player->EntIndex()];
		// Adjust velocity according to proportion.
		VectorMultiply(player->m_vecVelocity(), newspeed[player->EntIndex()], player->m_vecVelocity());
	}

	player->m_vecVelocity() -= (1.f - newspeed[player->EntIndex()]) * player->m_vecVelocity();
}


void RebuildGameMovement::CheckFalling(C_BasePlayer *player)
{
	// this function really deals with landing, not falling, so early out otherwise
	if (player->m_flFallVelocity() <= 0)
		return;

	if (!player->m_iHealth() && player->m_flFallVelocity() >= 303.0f)
	{
		bool bAlive = true;
		float fvol = 0.5;

		//
		// They hit the ground.
		//
		if (player->m_vecVelocity().z < 0.0f)
		{
			// Player landed on a descending object. Subtract the velocity of the ground C_BasePlayer.
			player->m_flFallVelocity() += player->m_vecVelocity().z;
			player->m_flFallVelocity() = max(0.1f, player->m_flFallVelocity());
		}

		if (player->m_flFallVelocity() > 526.5f)
		{
			fvol = 1.0;
		}
		else if (player->m_flFallVelocity() > 526.5f / 2)
		{
			fvol = 0.85;
		}
		else if (player->m_flFallVelocity() < 173)
		{
			fvol = 0;
		}

	}

	// let any subclasses know that the player has landed and how hard

	//
	// Clear the fall velocity so the impact doesn't happen again.
	//
	player->m_flFallVelocity() = 0;
}

const int nanmask = 255 << 23;
#define	IS_NAN(x) (((*(int *)&x)&nanmask)==nanmask)

void RebuildGameMovement::CheckVelocity(C_BasePlayer *player)
{
	Vector org = player->GetAbsOrigin();

	for (i[player->EntIndex()] = 0; i[player->EntIndex()] < 3; i[player->EntIndex()]++)
	{
		// See if it's bogus.
		if (IS_NAN(player->m_vecVelocity()[i[player->EntIndex()]]))
		{
			player->m_vecVelocity()[i[player->EntIndex()]] = 0;
		}

		if (IS_NAN(org[i[player->EntIndex()]]))
		{
			org[i[player->EntIndex()]] = 0;
			player->SetAbsOrigin(org);
		}

		// Bound it.
		if (player->m_vecVelocity()[i[player->EntIndex()]] > g_CVar->FindVar("sv_maxvelocity")->GetFloat())
		{
			player->m_vecVelocity()[i[player->EntIndex()]] = g_CVar->FindVar("sv_maxvelocity")->GetFloat();
		}
		else if (player->m_vecVelocity()[i[player->EntIndex()]] < -g_CVar->FindVar("sv_maxvelocity")->GetFloat())
		{
			player->m_vecVelocity()[i[player->EntIndex()]] = -g_CVar->FindVar("sv_maxvelocity")->GetFloat();
		}
	}
}
void RebuildGameMovement::StartGravity(C_BasePlayer *player)
{
	if (!player || !player->m_iHealth())
		return;

	Vector pVel = player->m_vecVelocity();

	pVel[2] -= (g_CVar->FindVar("sv_gravity")->GetFloat() * 0.5f * g_GlobalVars->interval_per_tick);
	pVel[2] += (player->m_vecBaseVelocity()[2] * g_GlobalVars->interval_per_tick);

	player->m_vecVelocity() = pVel;

	Vector tmp = player->m_vecBaseVelocity();
	tmp[2] = 0.f;
	player->m_vecVelocity() = tmp;
}