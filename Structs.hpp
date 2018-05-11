#pragma once

#include "SDK.hpp"
#include "helpers/Utils.hpp"

#define NETVAR(type, name, table, netvar)                           \
    type& name##() const {                                          \
        static int _##name = NetMngr::Get().getOffs(table, netvar);     \
        return *(type*)((uintptr_t)this + _##name);                 \
	}

#define NETVARADDOFFS(type, name, table, netvar, offs)                           \
    type& name##() const {                                          \
        static int _##name = NetMngr::Get().getOffs(table, netvar) + offs;     \
        return *(type*)((uintptr_t)this + _##name);                 \
	}

#define PNETVAR(type, name, table, netvar)                           \
    type* name##() const {                                          \
        static int _##name = NetMngr::Get().getOffs(table, netvar);     \
        return (type*)((uintptr_t)this + _##name);                 \
	}

enum CSWeaponType
{
	WEAPONTYPE_KNIFE = 0,
	WEAPONTYPE_PISTOL,
	WEAPONTYPE_SUBMACHINEGUN,
	WEAPONTYPE_RIFLE,
	WEAPONTYPE_SHOTGUN,
	WEAPONTYPE_SNIPER_RIFLE,
	WEAPONTYPE_MACHINEGUN,
	WEAPONTYPE_C4,
	WEAPONTYPE_PLACEHOLDER,
	WEAPONTYPE_GRENADE,
	WEAPONTYPE_UNKNOWN
};

class VarMapEntry_t
{
public:
	unsigned short type;
	unsigned short m_bNeedsToInterpolate;	// Set to false when this var doesn't
											// need Interpolate() called on it anymore.
	void *data;
	void *watcher;
};

struct VarMapping_t
{
	CUtlVector<VarMapEntry_t> m_Entries;
	int m_nInterpolatedEntries;
	float m_lastInterpolationTime;
};

struct Item_t;
struct WeaponName_t;
struct QualityName_t;
class AnimationLayer;
class C_CSGOPlayerAnimState;
class C_BaseEntity;
class CStudioHdr;

class WeapInfo_t
{
public:

	char pad00[0xC8];
	__int32 weapon_type;
	char padCC[0x20];
	__int32 m_iDamage;
	float m_fArmorRatio;
	char padF4[0x4];
	float m_fPenetration;
	char padFC[0x8];
	float m_fRange;
	float m_fRangeModifier;
	char pad10C[0x10];
	bool m_bHasSilencer;
};

class C_BaseEntity : public IClientEntity
{
public:

	static __forceinline C_BaseEntity* GetEntityByIndex(int index)
	{
		return static_cast<C_BaseEntity*>(g_EntityList->GetClientEntity(index));
	}
	static __forceinline C_BaseEntity* get_entity_from_handle(CBaseHandle h)
	{
		return static_cast<C_BaseEntity*>(g_EntityList->GetClientEntityFromHandle(h));
	}

	datamap_t *GetDataDescMap()
	{
		typedef datamap_t*(__thiscall *o_GetPredDescMap)(void*);
		return VT::vfunc<o_GetPredDescMap>(this, 15)(this);
	}

	datamap_t *GetPredDescMap()
	{
		typedef datamap_t*(__thiscall *o_GetPredDescMap)(void*);
		return VT::vfunc<o_GetPredDescMap>(this, 17)(this);
	}

	NETVAR(int32_t, m_nModelIndex, "CBaseEntity", "m_nModelIndex");
	NETVAR(int32_t, m_iTeamNum, "CBaseEntity", "m_iTeamNum");
	NETVAR(Vector, m_vecOrigin, "CBaseEntity", "m_vecOrigin");
	NETVAR(CHandle<C_BasePlayer>, m_hOwnerEntity, "CBaseEntity", "m_hOwnerEntity");
	NETVAR(int32_t, m_CollisionGroup, "CBaseEntity", "m_CollisionGroup");
	NETVAR(CHandle<C_BaseEntity>, m_hWeaponWorldModel, "CBaseCombatWeapon", "m_hWeaponWorldModel");

	const matrix3x4_t& m_rgflCoordinateFrame()
	{
		static auto _m_rgflCoordinateFrame = NetMngr::Get().getOffs("CBaseEntity", "m_CollisionGroup") - 0x30;
		return *(matrix3x4_t*)((uintptr_t)this + _m_rgflCoordinateFrame);
	}

	void SetModelIndex(int index)
	{
		VT::vfunc<void(__thiscall*)(C_BaseEntity*, int)>(this, 75)(this, index);
	}

	bool IsPlayer();
	bool IsWeapon();
	bool IsPlantedC4();
	bool IsDefuseKit();

	unsigned int PhysicsSolidMaskForEntity();
};

class C_BaseAttributableItem : public C_BaseEntity
{
public:

	NETVAR(int32_t, m_iItemDefinitionIndex, "CBaseAttributableItem", "m_iItemDefinitionIndex");
	NETVAR(int32_t, m_nFallbackStatTrak, "CBaseAttributableItem", "m_nFallbackStatTrak");
	NETVAR(int32_t, m_nFallbackPaintKit, "CBaseAttributableItem", "m_nFallbackPaintKit");
	NETVAR(int32_t, m_nFallbackSeed, "CBaseAttributableItem", "m_nFallbackSeed");
	NETVAR(float_t, m_flFallbackWear, "CBaseAttributableItem", "m_flFallbackWear");
	NETVAR(int32_t, m_iAccountID, "CBaseAttributableItem", "m_iAccountID");
	NETVAR(int32_t, m_iItemIDHigh, "CBaseAttributableItem", "m_iItemIDHigh");
	NETVAR(int32_t, m_iEntityQuality, "CBaseAttributableItem", "m_iEntityQuality");
	PNETVAR(char, m_szCustomName, "CBaseAttributableItem", "m_szCustomName");
};

class C_BaseCombatWeapon : public C_BaseAttributableItem
{
public:

	NETVAR(float_t, m_flNextPrimaryAttack, "CBaseCombatWeapon", "m_flNextPrimaryAttack");
	NETVAR(int32_t, m_iItemDefinitionIndex, "CBaseCombatWeapon", "m_iItemDefinitionIndex");
	NETVAR(int32_t, m_iClip1, "CBaseCombatWeapon", "m_iClip1");
	NETVAR(int32_t, m_iViewModelIndex, "CBaseCombatWeapon", "m_iViewModelIndex");
	NETVAR(int32_t, m_iWorldModelIndex, "CBaseCombatWeapon", "m_iWorldModelIndex");
	NETVAR(float_t, m_fAccuracyPenalty, "CWeaponCSBase", "m_fAccuracyPenalty");
	NETVAR(int32_t, m_zoomLevel, "CWeaponCSBaseGun", "m_zoomLevel");
	NETVAR(bool, m_bPinPulled, "CBaseCSGrenade", "m_bPinPulled");
	NETVAR(float_t, m_fThrowTime, "CBaseCSGrenade", "m_fThrowTime");
	NETVAR(float_t, m_flPostponeFireReadyTime, "CWeaponCSBase", "m_flPostponeFireReadyTime");
	NETVAR(float_t, m_fLastShotTime, "CWeaponCSBase", "m_fLastShotTime");

	WeapInfo_t *GetWeapInfo();
	bool HasBullets();
	bool CanFire();
	bool IsReloading();
	bool IsRifle();
	bool IsPistol();
	bool IsSniper();
	bool IsGrenade();
	float GetInaccuracy();
	float GetSpread();
	void UpdateAccuracyPenalty();
	bool IsWeaponNonAim();
	bool CanFirePostPone();
	bool IsInThrow();
	char* GetWeaponIcon();
};

class C_BasePlayer : public C_BaseEntity
{
public:

	static __forceinline C_BasePlayer* GetPlayerByUserId(int id)
	{
		return static_cast<C_BasePlayer*>(GetEntityByIndex(g_EngineClient->GetPlayerForUserID(id)));
	}
	static __forceinline C_BasePlayer* GetPlayerByIndex(int i)
	{
		return static_cast<C_BasePlayer*>(GetEntityByIndex(i));
	}
	static __forceinline C_BasePlayer *GetLocalPlayer(const bool set = false)
	{
		static C_BasePlayer *local = nullptr;

		if (!local || set)
			local = GetPlayerByIndex(g_EngineClient->GetLocalPlayer());

		return local;
	}

	NETVAR(bool, m_bHasDefuser, "CCSPlayer", "m_bHasDefuser");
	NETVAR(bool, m_bGunGameImmunity, "CCSPlayer", "m_bGunGameImmunity");
	NETVAR(int32_t, m_iShotsFired, "CCSPlayer", "m_iShotsFired");
	NETVAR(QAngle, m_angEyeAngles, "CCSPlayer", "m_angEyeAngles[0]");
	NETVAR(QAngle, m_angRotation, "CCSPlayer", "m_angRotation");
	NETVAR(int32_t, m_ArmorValue, "CCSPlayer", "m_ArmorValue");
	NETVAR(bool, m_bHasHelmet, "CCSPlayer", "m_bHasHelmet");
	NETVAR(bool, m_bHasHeavyArmor, "CCSPlayer", "m_bHasHeavyArmor");
	NETVAR(bool, m_bIsScoped, "CCSPlayer", "m_bIsScoped");;
	NETVAR(float_t, m_flLowerBodyYawTarget, "CCSPlayer", "m_flLowerBodyYawTarget");
	NETVAR(float_t, m_flFlashDuration, "CCSPlayer", "m_flFlashDuration");
	NETVAR(int32_t, m_iHealth, "CBasePlayer", "m_iHealth");
	NETVAR(int32_t, m_lifeState, "CBasePlayer", "m_lifeState");
	NETVAR(int32_t, m_fFlags, "CBasePlayer", "m_fFlags");
	NETVAR(int32_t, m_nHitboxSet, "CBasePlayer", "m_nHitboxSet");
	NETVAR(int32_t, m_nTickBase, "CBasePlayer", "m_nTickBase");
	NETVAR(Vector, m_vecViewOffset, "CBasePlayer", "m_vecViewOffset[0]");
	NETVAR(QAngle, m_viewPunchAngle, "CBasePlayer", "m_viewPunchAngle");
	NETVAR(QAngle, m_aimPunchAngle, "CBasePlayer", "m_aimPunchAngle");
	NETVAR(CHandle<C_BaseViewModel>, m_hViewModel, "CBasePlayer", "m_hViewModel[0]");
	NETVAR(Vector, m_vecVelocity, "CBasePlayer", "m_vecVelocity[0]");
	NETVAR(float_t, m_flSimulationTime, "CBaseEntity", "m_flSimulationTime");
	NETVARADDOFFS(float_t, m_flOldSimulationTime, "CBaseEntity", "m_flSimulationTime", 0x4);
	NETVAR(float_t, m_flDuckSpeed, "CCSPlayer", "m_flDuckSpeed");
	NETVAR(float_t, m_flDuckAmount, "CCSPlayer", "m_flDuckAmount");
	NETVAR(bool, m_bDucked, "CCSPlayer", "m_bDucked");
	NETVAR(bool, m_bDucking, "CCSPlayer", "m_bDucking");
	NETVAR(float_t, m_flFallVelocity, "CBasePlayer", "m_flFallVelocity");
	NETVAR(float_t, m_flStepSize, "CBaseEntity", "m_flStepSize");
	NETVAR(float_t, m_flNextAttack, "CBaseCombatCharacter", "m_flNextAttack");
	NETVAR(int, m_iObserverMode, "CBasePlayer", "m_iObserverMode");
	NETVAR(bool, m_bClientSideAnimation, "CBasePlayer", "m_bClientSideAnimation");
	NETVAR(CHandle<C_BasePlayer>, m_hObserverTarget, "CBasePlayer", "m_hObserverTarget");
	NETVAR(CHandle<C_BaseCombatWeapon>, m_hActiveWeapon, "CBaseCombatCharacter", "m_hActiveWeapon");
	CHandle<C_BaseCombatWeapon> *m_hMyWeapons() const
	{
		static int _m_hMyWeapons = NetMngr::Get().getOffs("CBaseCombatCharacter", "m_hMyWeapons");
		return (CHandle<C_BaseCombatWeapon>*)((uintptr_t)this + (_m_hMyWeapons / 2));
	}
	PNETVAR(CHandle<C_BaseAttributableItem>, m_hMyWearables, "CBaseCombatCharacter", "m_hMyWearables");

	float_t &m_surfaceFriction()
	{
		static unsigned int _m_surfaceFriction = Utils::FindInDataMap(GetPredDescMap(), "m_surfaceFriction");
		return *(float_t*)((uintptr_t)this + _m_surfaceFriction);
	}

	Vector &m_vecBaseVelocity()
	{
		static unsigned int _m_vecBaseVelocity = Utils::FindInDataMap(GetPredDescMap(), "m_vecBaseVelocity");
		return *(Vector*)((uintptr_t)this + _m_vecBaseVelocity);
	}

	float_t &m_flMaxspeed()
	{
		static unsigned int _m_flMaxspeed = Utils::FindInDataMap(GetPredDescMap(), "m_flMaxspeed");
		return *(float_t*)((uintptr_t)this + _m_flMaxspeed);
	}

	QAngle &m_angAbsRotation()
	{
		return *(QAngle*)((DWORD)&m_angRotation() - 12);
	}

	float_t m_flSpawnTime();

	std::array<float, 24> &m_flPoseParameter();
	QAngle &visuals_Angles();
	int32_t GetMoveType();

	// Move to nice spots, if not moved, remind me to move em because I haven't tested if this is neccesarry yet since setabsangles should update matrix anyways
	void SetPoseAngles(float_t yaw, float_t pitch);

	void InvalidateBoneCache();
	int GetNumAnimOverlays();
	AnimationLayer *GetAnimOverlays();
	AnimationLayer *GetAnimOverlay(int i);
	int GetSequenceActivity(int sequence);
	C_CSGOPlayerAnimState *GetPlayerAnimState();

	static void UpdateAnimationState(C_CSGOPlayerAnimState *state, QAngle angle);
	static void ResetAnimationState(C_CSGOPlayerAnimState *state);
	void CreateAnimationState(C_CSGOPlayerAnimState *state);

	CBoneAccessor *GetBoneAccessor();
	CStudioHdr *GetModelPtr();
	void StandardBlendingRules(CStudioHdr *hdr, Vector *pos, Quaternion *q, float_t curtime, int32_t boneMask);
	void BuildTransformations(CStudioHdr *hdr, Vector *pos, Quaternion *q, const matrix3x4_t &cameraTransform, int32_t boneMask, byte *computed);

	bool HandleBoneSetup(int32_t boneMask, matrix3x4_t *boneOut, float_t curtime);

	const Vector &WorldSpaceCenter();
	Vector GetEyePos();
	player_info_t GetPlayerInfo();
	std::string GetName(bool console_safe = false);
	bool IsAlive();
	bool HasC4();
	Vector GetBonePos(int bone);
	int GetBoneByName(const char *boneName);
	Vector GetHitboxPos(int hitbox);
	bool IsLocalInTarget(C_BasePlayer *player);
	VarMapping_t *VarMapping();
	void SetAbsOrigin(const Vector &origin);
	void SetAbsAngles(const QAngle &angles);
	void UpdateClientSideAnimation();
	int GetPing();
};

class C_BaseViewModel : public C_BaseEntity
{
public:

	NETVAR(int32_t, m_nModelIndex, "CBaseViewModel", "m_nModelIndex");
	NETVAR(int32_t, m_nViewModelIndex, "CBaseViewModel", "m_nViewModelIndex");
	NETVAR(CHandle<C_BaseCombatWeapon>, m_hWeapon, "CBaseViewModel", "m_hWeapon");
	NETVAR(CHandle<C_BasePlayer>, m_hOwner, "CBaseViewModel", "m_hOwner");
};

class AnimationLayer
{
public:
	char  pad_0000[20];
	// These should also be present in the padding, don't see the use for it though
	//float	m_flLayerAnimtime;
	//float	m_flLayerFadeOuttime;
	uint32_t m_nOrder; //0x0014
	uint32_t m_nSequence; //0x0018
	float_t m_flPrevCycle; //0x001C
	float_t m_flWeight; //0x0020
	float_t m_flWeightDeltaRate; //0x0024
	float_t m_flPlaybackRate; //0x0028
	float_t m_flCycle; //0x002C
	void *m_pOwner; //0x0030 // player's thisptr
	char  pad_0038[4]; //0x0034
}; //Size: 0x0038

class C_CSGOPlayerAnimState
{
public:

	int32_t &m_iLastClientSideAnimationUpdateFramecount()
	{
		return *(int32_t*)((uintptr_t)this + 0x70);
	}

	float_t &m_flEyeYaw()
	{
		return *(float_t*)((uintptr_t)this + 0x78);
	}

	float_t &m_flEyePitch()
	{
		return *(float_t*)((uintptr_t)this + 0x7C);
	}

	float_t &m_flGoalFeetYaw()
	{
		return *(float_t*)((uintptr_t)this + 0x80);
	}

	float_t &m_flCurrentFeetYaw()
	{
		return *(float_t*)((uintptr_t)this + 0x84);
	}

	bool &m_bCurrentFeetYawInitialized()
	{
		return *(bool*)((uintptr_t)this + 0x88);
	}

	Vector &m_vecVelocity()
	{
		// Only on ground velocity
		return *(Vector*)((uintptr_t)this + 0xC8);
	}

	float_t m_flVelocity()
	{
		return *(float_t*)((uintptr_t)this + 0xEC);
	}

	char pad_0x0000[0x344]; //0x0000
}; //Size=0x344

class C_EconItemView
{
public:
	char _pad_0x0000[0x14];
	CUtlVector<IRefCounted*>	m_CustomMaterials; //0x0014
	char _pad_0x0028[0x1F8];
	CUtlVector<IRefCounted*>	m_VisualsDataProcessors; //0x0220
};

class C_AttributeManager
{
public:
	char	_pad_0x0000[0x18];
	__int32						m_iReapplyProvisionParity; //0x0018 
	uintptr_t						m_hOuter; //0x001C 
	char	_pad_0x0020[0x4];
	__int32						m_ProviderType; //0x0024 
	char	_pad_0x0028[0x18];
	C_EconItemView				m_Item; //0x0040 
};

class C_WeaponCSBase : public IClientEntity
{
public:
	char	_pad_0x0000[0x09CC];
	CUtlVector<IRefCounted*>	m_CustomMaterials; //0x09DC
	char	_pad_0x09F0[0x2380];
	C_AttributeManager			m_AttributeManager; //0x2D70
	char	_pad_0x2D84[0x2F9];
	bool						m_bCustomMaterialInitialized; //0x32DD
};

struct Item_t
{
	constexpr Item_t(const char* model, const char* icon = nullptr) : model(model), icon(icon) {}

	const char* model;
	const char* icon;
};

struct WeaponName_t
{
	constexpr WeaponName_t(int definition_index, const char* name) : definition_index(definition_index), name(name) {}

	int definition_index = 0;
	const char* name = nullptr;
};

struct QualityName_t
{
	constexpr QualityName_t(int index, const char* name) : index(index), name(name) {}

	int index = 0;
	const char* name = nullptr;
};

extern const std::map<size_t, Item_t> k_weapon_info;
extern const std::vector<WeaponName_t> k_knife_names;
extern const std::vector<WeaponName_t> k_glove_names;
extern const std::vector<WeaponName_t> k_weapon_names;
extern const std::vector<QualityName_t> k_quality_names;