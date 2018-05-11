#pragma once

struct surfacephysicsparams_t
{
	float    friction;
	float    elasticity;
	float    density;
	float    thickness;
	float    dampening;
};

struct surfaceaudioparams_t
{
	float    reflectivity;             // like elasticity, but how much sound should be reflected by this surface
	float    hardnessFactor;           // like elasticity, but only affects impact sound choices
	float    roughnessFactor;          // like friction, but only affects scrape sound choices   
	float    roughThreshold;           // surface roughness > this causes "rough" scrapes, < this causes "smooth" scrapes
	float    hardThreshold;            // surface hardness > this causes "hard" impacts, < this causes "soft" impacts
	float    hardVelocityThreshold;    // collision velocity > this causes "hard" impacts, < this causes "soft" impacts   
	float    highPitchOcclusion;       //a value betweeen 0 and 100 where 0 is not occluded at all and 100 is silent (except for any additional reflected sound)
	float    midPitchOcclusion;
	float    lowPitchOcclusion;
};

struct surfacesoundnames_t
{
	unsigned short    walkStepLeft;
	unsigned short    walkStepRight;
	unsigned short	  runStepLeft;
	unsigned short	  runStepRight;
	unsigned short    impactSoft;
	unsigned short    impactHard;
	unsigned short    scrapeSmooth;
	unsigned short    scrapeRough;
	unsigned short    bulletImpact;
	unsigned short    rolling;
	unsigned short    breakSound;
	unsigned short    strainSound;
};

struct surfacegameprops_t
{
public:
	float maxSpeedFactor;
	float jumpFactor;
	float flPenetrationModifier;
	float flDamageModifier;
	unsigned short material;
	byte climbable;
};

struct surfacedata_t
{
	surfacephysicsparams_t    physics;
	surfaceaudioparams_t    audio;
	surfacesoundnames_t        sounds;
	surfacegameprops_t        game;
};

class IMultiplayerPhysics
{
public:
	virtual int		GetMultiplayerPhysicsMode(void) = 0;
	virtual float	GetMass(void) = 0;
	virtual bool	IsAsleep(void) = 0;
};

class IBreakableWithPropData
{
public:
	// Damage modifiers
	virtual void            SetDmgModBullet(float flDmgMod) = 0;
	virtual void            SetDmgModClub(float flDmgMod) = 0;
	virtual void            SetDmgModExplosive(float flDmgMod) = 0;
	virtual float           GetDmgModBullet(void) = 0;
	virtual float           GetDmgModClub(void) = 0;
	virtual float           GetDmgModExplosive(void) = 0;
	// Explosive
	virtual void            SetExplosiveRadius(float flRadius) = 0;
	virtual void            SetExplosiveDamage(float flDamage) = 0;
	virtual float           GetExplosiveRadius(void) = 0;
	virtual float           GetExplosiveDamage(void) = 0;
	// Physics damage tables
	virtual void            SetPhysicsDamageTable(char* iszTableName) = 0;
	virtual char*           GetPhysicsDamageTable(void) = 0;
	// Breakable chunks
	virtual void            SetBreakableModel(char* iszModel) = 0;
	virtual char*           GetBreakableModel(void) = 0;
	virtual void            SetBreakableSkin(int iSkin) = 0;
	virtual int                     GetBreakableSkin(void) = 0;
	virtual void            SetBreakableCount(int iCount) = 0;
	virtual int                     GetBreakableCount(void) = 0;
	virtual void            SetMaxBreakableSize(int iSize) = 0;
	virtual int                     GetMaxBreakableSize(void) = 0;
	// LOS blocking
	virtual void            SetPropDataBlocksLOS(bool bBlocksLOS) = 0;
	virtual void            SetPropDataIsAIWalkable(bool bBlocksLOS) = 0;
	// Interactions
	virtual void            SetInteraction(propdata_interactions_t Interaction) = 0;
	virtual bool            HasInteraction(propdata_interactions_t Interaction) = 0;
	// Multi player physics mode
	virtual void            SetPhysicsMode(int iMode) = 0;
	virtual int                     GetPhysicsMode() = 0;
	// Multi player breakable spawn behavior
	virtual void            SetMultiplayerBreakMode(mp_break_t mode) = 0;
	virtual mp_break_t      GetMultiplayerBreakMode(void) const = 0;
	// Used for debugging
	virtual void            SetBasePropData(char* iszBase) = 0;
	virtual char*           GetBasePropData(void) = 0;
};

class IPhysicsSurfaceProps
{
public:
    virtual ~IPhysicsSurfaceProps(void) {}
    virtual int             ParseSurfaceData(const char *pFilename, const char *pTextfile) = 0;
    virtual int             SurfacePropCount(void) const = 0;
    virtual int             GetSurfaceIndex(const char *pSurfacePropName) const = 0;
    virtual void            GetPhysicsProperties(int surfaceDataIndex, float *density, float *thickness, float *friction, float *elasticity) const = 0;
    virtual surfacedata_t*  GetSurfaceData(int surfaceDataIndex) = 0;
    virtual const char*     GetString(unsigned short stringTableIndex) const = 0;
    virtual const char*     GetPropName(int surfaceDataIndex) const = 0;
    virtual void            SetWorldMaterialIndexTable(int *pMapArray, int mapSize) = 0;
    virtual void            GetPhysicsParameters(int surfaceDataIndex, surfacephysicsparams_t *pParamsOut) const = 0;
};
