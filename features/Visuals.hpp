#pragma once
#include "../helpers/Math.hpp"

class C_BasePlayer;
class C_BaseEntity;
class C_BaseCombatWeapon;
class C_PlantedC4;
class Color;
class ClientClass;

namespace Visuals
{
	struct VisualsStruct
	{
		C_BasePlayer *player;
		bool bEnemy;
		bool isVisible;
		Color clr;
		Color clr_text;
		Color clr_fill;
		Vector head_pos;
		Vector feet_pos;
		RECT bbox;
		float fl_opacity;
	};

	extern VisualsStruct ESP_ctx;
	extern float ESP_Fade[64];
	extern C_BasePlayer* local_observed;

	extern unsigned long weapon_font;
	extern unsigned long ui_font;
	extern unsigned long watermark_font;
	extern unsigned long spectatorlist_font;

	bool Begin(C_BasePlayer *player);
	bool ValidPlayer(C_BasePlayer *player, bool = true);
	bool IsVisibleScan(C_BasePlayer *player);

	void RenderBox();
	void RenderFill();
	void RenderName();
	void RenderHealth();
	void RenderWeapon();
	void RenderSnapline();
	void RenderSkelet();
	void RenderBacktrackedSkelet();
	void RenderWeapon(C_BaseCombatWeapon *entity);
	void RenderNadeEsp(C_BaseCombatWeapon* nade);
	void RenderPlantedC4(C_BaseEntity *entity);
	void RenderSpectatorList();
	void DrawAngleLines();
	void DrawWatermark();
	void DrawResolverModes();
	void DrawCapsuleOverlay(int idx, float duration);
	bool InitFont();

	void DrawhealthIcon(int x, int y, C_BasePlayer* player = nullptr);

	void Draw3DCube(float scalar, QAngle angles, Vector middle_origin, Color outline);
	void Polygon(int count, Vertex_t* Vertexs, Color color);
	void PolygonOutline(int count, Vertex_t* Vertexs, Color color, Color colorLine);
	void PolyLine(int count, Vertex_t* Vertexs, Color colorLine);
	void PolyLine(int *x, int *y, int count, Color color);
	void FillRGBA(int x, int y, int w, int h, Color c);
	void BorderBox(int x, int y, int w, int h, Color color, int thickness);
	void DrawFilledRect(int x, int y, int w, int h);
	void DrawString(unsigned long font, int x, int y, Color color, unsigned long alignment, const char* msg, ...);
	void DrawString(unsigned long font, bool center, int x, int y, Color c, const char *fmt, ...);
	void TextW(bool center, unsigned long font, int x, int y, Color c, wchar_t *pszString);
	void DrawRectOutlined(int x, int y, int w, int h, Color color, Color outlinedColor, int thickness);
	void DrawOutlinedRect(int x, int y, int w, int h, Color &c);
	void GetTextSize(unsigned long font, const char *txt, int &width, int &height);
	void DrawCircle(int x, int y, float r, int step, Color color);
	RECT GetViewport();
}