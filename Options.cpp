#include "Options.hpp"

const char *opt_EspType[] = { "Off", "Bounding", "Corners" };
const char *opt_BoundsType[] = { "Static", "Dynamic" };
const char *opt_WeaponBoxType[] = { "Off", "Bounding", "3D" };
const char *opt_GrenadeESPType[] = { "Text", "Box + Text", "Icon", "Icon + Name hint" };
const char *opt_GlowStyles[] = { "Outline outer", "Cover", "Outline inner" };
const char *opt_Chams[] = { "Textured", "Textured XQZ", "Flat", "Flat XQZ" };
const char *opt_AimHitboxSpot[] = { "Head", "Neck", "Body", "Pelvis" };
const char *opt_AimSpot[] = { "Head", "Neck", "Body", "Pelvis" };
const char *opt_MultiHitboxes[14] = { "Head", "Pelvis", "Upper Chest", "Chest", "Neck", "Left Forearm", "Right Forearm", "Hands", "Left Thigh", "Right Thigh", "Left Calf", "Right Calf", "Left Foot", "Right Foot" };
const char *opt_AApitch[] = { "Off", "Dynamic", "Emotion (Down)", "Straight", "Up" };
const char *opt_AAyaw[] = { "Off", "Backwards", "Backwards Jitter", "Keybased", "Keybased Jitter" };
const char *opt_AAfakeyaw[] = { "Off", "Backwards", "Backwards Jitter", "Evade", "Keybased", "Keybased Jitter" };
const char *opt_LagCompType[] = { "Only best records", "Best and newest", "All records (fps warning)" };
const char *opt_Skynames[] = { "Default", "cs_baggage_skybox_", "cs_tibet", "embassy", "italy", "jungle", "nukeblank", "office", "sky_csgo_cloudy01", "sky_csgo_night02", "sky_csgo_night02b", "sky_dust", "sky_venice", "vertigo", "vietnam" };
const char *opt_nosmoketype[] = { "Remove", "Wireframe" };
int realAimSpot[] = { 8, 7, 6, 0 };
int realHitboxSpot[] = { 0, 1, 2, 3 };
bool input_shouldListen = false;
ButtonCode_t* input_receivedKeyval = nullptr;
bool pressedKey[256] = {};
bool menuOpen = true;

Options g_Options;