//Using SDL, SDL_image, standard math, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h> 
#include <stdio.h>
#include <string>
#include <iostream>

#include "geometry.h"
#include "texture.h"
#include "buttons.h"
#include "animation.h"

#define TILESET_1_PATH      "../resources/iso-64x64-outside_numeri_transp.png"
#define TILESET_2_PATH      "../resources/basic_ground_tiles_64_halftile.png"
#define MESSAGE             "no text wrapping built-in (hit X to quit or D to hide/show)"

using namespace std;

int main(int argc, char* args[]) {
  //Start up SDL and create window
  if (!init()) {
    printf("Failed to initialize!\n");
  }
  else {
    //Load media
    if (!loadMedia(TILESET_2_PATH)) {
      printf("Failed to load media!\n");
      exit(-1);
    }

    //Initialize SDL_ttf 
    if (TTF_Init() == -1) {
      printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
      exit(-2);
    }

    SDL_Color textWhite = { 255,255,255 };
    SDL_Color textRed = { 255,0,0 };
    SDL_Color textGreen = { 0,255,0 };
    SDL_Color currentColor = textWhite;
    if ( !gTextTexture.loadTextMedia("../resources/sample.ttf", MESSAGE, currentColor) ) {
      printf("Failed to load text media!\n");
      exit(-3);
    }
    if (!gTextTexture2.loadTextMedia("../resources/sample.ttf", "multiline text", textGreen)) {
      printf("Failed to load text media!\n");
      exit(-3);
    }

    //Event handler
    SDL_Event e;

    // General variables
    int mouseX = 0, mouseY = 0;
    bool show_text = true;
    bool quit = false;

    Point mouse_tile, last_mouse_tile{ -100, -100 };
		Point sprite_tile{ 1, 1 };
		//Point highl_tile{ 2, 2 };
		vector<Point> bestpath;

    const unsigned char* currentKeyStates;

    // Set buttons position
    for (int i = 0; i < TOTAL_BUTTONS; ++i) {
      gButtons[i].setPosition(50 + i*BUTTON_WIDTH, 50);
    }


    // register a timer
    //SDL_TimerID sprite_move_timer = SDL_AddTimer(1000, move_timer_cb, &sprite_tile); //call callback every 1000 msec


    // MAIN LOOP
    while (!quit) {
      // Handle events on queue
      while (SDL_PollEvent(&e) != 0) {
        // User requests quit
        if (e.type == SDL_QUIT) {
          quit = true;
        }

        // Keyboard event sample:
        // sdl_scancodes
        // https://wiki.libsdl.org/SDL_Scancode
        currentKeyStates = SDL_GetKeyboardState(NULL);
        if (currentKeyStates[SDL_SCANCODE_X]) {    // hit X to exit
          quit = true;
          break;
        }
        if (e.type == SDL_KEYDOWN) {
          switch (e.key.keysym.sym) {
          case SDLK_d:                     // hit D to hide/display text
            show_text = (show_text == true) ? false : true;
            break;
          case SDLK_c:                     // change text color holding C
            currentColor = textRed;
            break;
          default:
            break;
          }
        }
        if (e.type == SDL_KEYUP) {
          switch (e.key.keysym.sym) {
          case SDLK_c:                     // change text color holding C
            currentColor = textWhite;
            break;
          default:
            break;
          }
        }

        // Get mouse position
        if (e.type == SDL_MOUSEMOTION) {
          SDL_GetMouseState(&mouseX, &mouseY);
        }

        // move sprite, commented
        if (e.type == SDL_USEREVENT) {    // timer callback points here
          //insert new coords here, example:
          //sprite_tile.x++;
          //sprite_tile.y++;
        }

        // Handle button events 
        for (int i = 0; i < TOTAL_BUTTONS; ++i) {
          gButtons[i].handleEvent(&e);
        }
      }

      //Clear screen
      SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xFF);
      SDL_RenderClear(gRenderer);

      //compute mouse tile and best path
      mouse_tile = mouse2tile({ mouseX, mouseY });
      if(mouse_tile != last_mouse_tile) {
        //cout << "mouse in tile  " << mouse_tile.x << "  " << mouse_tile.y << endl;
        last_mouse_tile = mouse_tile;
        bestpath = findPath_Astar(sprite_tile, mouse_tile);
      }

      // Render ground 
      for (int i = 0; i < MAP_W; i++) {
        for (int j = 0; j < MAP_H; j++) {
          renderTile(i, j, &gSpriteClips[1]);
        }
      }

      //Render buttons 
      for (int i = 0; i < TOTAL_BUTTONS; ++i) {
        gButtons[i].render();
      }

			// render path
			for(auto& p : bestpath) renderTile(p, &highlighterSprite);

      // Render cursor
      renderCursor(mouse_tile, &cursorSprite);

      // Render unit
			renderTile(sprite_tile, &unitSprite);

      // Render text
      if (show_text) {
        gTextTexture.setText(MESSAGE, currentColor);
        gTextTexture.render(SCREEN_WIDTH / 15, 3 * SCREEN_HEIGHT / 4);
      }
      //gTextTexture2.setText("multiline text", textGreen);
      //gTextTexture2.render(SCREEN_WIDTH / 15, 7 * SCREEN_HEIGHT / 8.);

      //Update screen
      SDL_RenderPresent(gRenderer);
    }
  }

  //Free resources and close SDL
  close();

  return 0;
}
