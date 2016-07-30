#include "gui.h"
#include "defines.h"

extern SDL_Window *gWindow;
extern SDL_Renderer *gRenderer;

void BattleStatus::render() {
	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
	SDL_RenderFillRect(gRenderer, &hpBar);
	SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0xFF, 0xFF);
	SDL_RenderFillRect(gRenderer, &mpBar);

	SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderDrawRect(gRenderer, &hpBar_outline);
	SDL_RenderDrawRect(gRenderer, &mpBar_outline);

	int winH, winW; SDL_GetWindowSize(gWindow, &winW, &winH);

	hp_label.render({ 20, winH - 120 });
	mp_label.render({ 20, winH -  70 });
}

BattleStatus::BattleStatus() {
	if(!hp_label.loadFormat(TTF_PATH_LAZY, 15) || !mp_label.loadFormat(TTF_PATH_LAZY, 15)) {
		printf("Failed to load text media!\n"); CLI_PAUSE
		exit(4);
	}
}
