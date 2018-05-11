#include "../SDK.hpp"
#include "IEngineTrace.hpp"

// VS is gay, so I was required to put it in a .cpp file ;(

bool CGameTrace::DidHitWorld() const
{
	return hit_entity == g_EntityList->GetClientEntity(0);
}

bool CGameTrace::DidHitNonWorldEntity() const
{
	return hit_entity != nullptr && !DidHitWorld();
}