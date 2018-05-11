#pragma once

#include <Windows.h>
#include <memory>
#include <array>
#include <deque>
#include <map>
#include <shlobj.h>

#include "IfaceMngr.hpp"
#include "misc/Enums.hpp"
#include "VFunc.hpp"

#include "math/VMatrix.hpp"
#include "math/QAngle.hpp"
#include "math/Vector.hpp"
#include "misc/Studio.hpp"

#include "interfaces/IAppSystem.hpp"
#include "interfaces/IBaseClientDll.hpp"
#include "interfaces/IClientEntity.hpp"
#include "interfaces/IClientEntityList.hpp"
#include "interfaces/IClientMode.hpp"
#include "interfaces/IConVar.hpp"
#include "interfaces/ICvar.hpp"
#include "interfaces/IEngineTrace.hpp"
#include "interfaces/IVEngineClient.hpp"
#include "interfaces/IVDebugOverlay.hpp"
#include "interfaces/ISurface.hpp"
#include "interfaces/CInput.hpp"
#include "interfaces/IVModelInfoClient.hpp"
#include "interfaces/IVModelRender.hpp"
#include "interfaces/IRenderView.hpp"
#include "interfaces/IGameEventmanager.hpp"
#include "interfaces/IMaterialSystem.hpp"
#include "interfaces/IMoveHelper.hpp"
#include "interfaces/IMDLCache.hpp"
#include "interfaces/IPrediction.hpp"
#include "interfaces/IPanel.hpp"
#include "interfaces/IEngineSound.hpp"
#include "interfaces/IViewRender.hpp"
#include "interfaces/CClientState.hpp"
#include "interfaces/IPhysics.hpp"
#include "interfaces/IInputSystem.h"
#include "interfaces/memalloc.h"
#include "interfaces/IViewRenderBeams.hpp"
#include "interfaces/ILocalize.hpp"

#include "misc/Convar.hpp"
#include "misc/CUserCmd.hpp"
#include "misc/glow_outline_effect.hpp"
#include "misc/datamap.hpp"

#include "NetMngr.hpp"

#include "helpers/Config.hpp"

#include "features/BoneAccessor.hpp"

struct IDirect3DDevice9;

extern IVEngineClient			*g_EngineClient;
extern IBaseClientDLL			*g_CHLClient;
extern IClientEntityList		*g_EntityList;
extern CGlobalVarsBase			*g_GlobalVars;
extern IEngineTrace				*g_EngineTrace;
extern ICvar					*g_CVar;
extern IPanel					*g_VGuiPanel;
extern IClientMode				*g_ClientMode;
extern IVDebugOverlay			*g_DebugOverlay;
extern ISurface					*g_VGuiSurface;
extern CInput					*g_Input;
extern IVModelInfoClient		*g_MdlInfo;
extern IVModelRender			*g_MdlRender;
extern IVRenderView				*g_RenderView;
extern IMaterialSystem			*g_MatSystem;
extern IGameEventManager2		*g_GameEvents;
extern IMoveHelper				*g_MoveHelper;
extern IMDLCache				*g_MdlCache;
extern IPrediction				*g_Prediction;
extern CGameMovement			*g_GameMovement;
extern IEngineSound				*g_EngineSound;
extern CGlowObjectManager		*g_GlowObjManager;
extern CClientState				*g_ClientState;
extern IPhysicsSurfaceProps		*g_PhysSurface;
extern IInputSystem			    *g_InputSystem;
extern DWORD					*g_InputInternal;
extern IMemAlloc				*g_pMemAlloc;
extern IViewRenderBeams			*g_RenderBeams;
extern ILocalize				*g_Localize;

#include "misc/EHandle.hpp"

extern C_BasePlayer *g_LocalPlayer;

namespace Offsets
{
	extern DWORD invalidateBoneCache;
	extern DWORD smokeCount;
	extern DWORD playerResource;
	extern DWORD bOverridePostProcessingDisable;
	extern DWORD getSequenceActivity;
	extern DWORD lgtSmoke;
	extern DWORD dwCCSPlayerRenderablevftable;
	extern DWORD reevauluate_anim_lod;
}

namespace Global
{
	extern char my_documents_folder[MAX_PATH];

	extern float smt;
	extern QAngle visualAngles;
	extern bool	bSendPacket;
	extern bool bAimbotting;
	extern bool bVisualAimbotting;
	extern QAngle vecVisualAimbotAngs;
	extern CUserCmd *userCMD;

	extern char *szLastFunction;
	extern HMODULE hmDll;

	extern bool bFakelag;
	extern float flFakewalked;
	extern Vector vecUnpredictedVel;

	extern float flFakeLatencyAmount;
	extern float flEstFakeLatencyOnServer;

	extern matrix3x4_t traceHitboxbones[128];

	extern std::array<std::string, 64> resolverModes;
}

namespace Proxies
{
	void didSmokeEffect(const CRecvProxyData *pData, void *pStruct, void *pOut);
	void nSequence(const CRecvProxyData *pData, void *pStruct, void *pOut);
}

extern RecvVarProxyFn o_didSmokeEffect;
extern RecvVarProxyFn o_nSequence;

extern int32_t originalCorrectedFakewalkIdx;
extern int32_t tickHitPlayer;
extern int32_t tickHitWall;
extern int32_t originalShotsMissed;

#define TIME_TO_TICKS(dt) ((int)( 0.5f + (float)(dt) / g_GlobalVars->interval_per_tick))
#define TICKS_TO_TIME(t) (g_GlobalVars->interval_per_tick * (t) )

#ifdef _DEBUG
#define FUNCTION_GUARD Global::szLastFunction = __FUNCTION__
#else
#define FUNCTION_GUARD std::runtime_error("ERROR! Module shipped in release with debug information.\n");
#endif