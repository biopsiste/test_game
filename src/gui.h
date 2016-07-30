#pragma once

//#include "texture.h"
//#include "units.h"
#include <SDL.h>
#include "textbox.h"

struct BattleStatus {
	SDL_Rect hpBar_outline, mpBar_outline, hpBar, mpBar;
	LTextTexture hp_label, mp_label;

	BattleStatus();

	void render();
};