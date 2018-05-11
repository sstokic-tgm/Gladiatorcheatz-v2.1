#pragma once
#include <Windows.h>
#include <string>
#include <vector>

#include "imgui\imgui.h"
#include "imgui\DX9\imgui_impl_dx9.h"

struct IDirect3DDevice9;

namespace GladiatorMenu
{
	void GUI_Init(HWND, IDirect3DDevice9*);

	void openMenu();

	void mainWindow();
	
	void legitTab();
	void ragebotTab();
	void visualTab();
	void miscTab();
	void hvhTab();
	void skinchangerTab();

	extern bool d3dinit;
	extern ImFont* cheat_font;
	extern ImFont* title_font;
}

extern bool pressedKey[256];