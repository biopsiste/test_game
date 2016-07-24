// Copyright 2016 Marco Di Cristina, Alessandro Fabbri

// STL include
#include <iostream>
#include <cstdio>
#include <string>
#include <vector>

// SDL include
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h> 

// GOTYAY include
#include "geometry.hpp"
#include "texture.hpp"
#include "buttons.hpp"
#include "animation.hpp"
#include "textbox.hpp"
#include "battlemap.hpp"

// defines (to be moved or removed)
#define MESSAGE             "no text wrapping built-in (hit X to quit or D to hide/show)"

//#define SHOW_PATH
#define SHOW_TEXT
//#define SHOW_BUTTONS

// The buttons
LButton gButtons[TOTAL_BUTTONS];

using namespace std;

int main(int argc, char* args[]) {
  //Start up SDL and create window
  if (!init()) {
    printf("Failed to initialize!\n");
    exit(1);
  }


  //Initialize SDL_ttf 
  if (TTF_Init() == -1) {
    printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
    exit(2);
  }

  //Load media
  if (!loadMedia(TILESET_PATH)) {
    printf("Failed to load media!\n"); 
  cin.get();
    exit(3);
  }


  SDL_Color currentColor = textWhite;
  LTextTexture label;
  if (!label.loadFormat(TTF_PATH_LAZY, 25)) {
    printf("Failed to load text media!\n");
    exit(4);
  }

  // Event handler
  SDL_Event e;

  // General variables
  int mouseX = 0, mouseY = 0;
  bool show_text = true;
  bool quit = false;

  Map map("../resources/test_map.txt");
  Point mouse_tile{}, last_mouse_tile{ -100, -100 }, mouse_point{};
  Point sprite_tile{ 1, 1 };
  Point camera{ 0, 0 }, north = tile2screen({ 0, 0 }), south = tile2screen({ map.w() - 1, map.h() - 1 }), west = tile2screen({ 0, map.h() - 1 }), east = tile2screen({ map.w() - 1 , 0 });
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
        int winH, winW; SDL_GetWindowSize(gWindow, &winW, &winH);

        switch (e.key.keysym.sym) {
        case SDLK_d:                     // hit D to hide/display text
          show_text = (show_text == true) ? false : true;
          break;
        case SDLK_c:                     // change text color holding C
          currentColor = textRed;
          break;

          // move camera
        case SDLK_DOWN:
          camera.y += 10; if (camera.y + winH > south.y + 64 + 30) camera.y = south.y + 64 + 30 - winH;
          break;
        case SDLK_UP:
          camera.y -= 10; if (camera.y < north.y - 30) camera.y = north.y - 30;
          break;
        case SDLK_LEFT:
          camera.x -= 10; if (camera.x < west.x - 30) camera.x = west.x - 30;
          break;
        case SDLK_RIGHT:
          camera.x += 10; if (camera.x + winW > east.x + 64 + 30) camera.x = east.x + 64 + 30 - winW;
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
        mouse_point = { mouseX, mouseY };
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
    
    //mouse_tile = mouse2tile(mouse_point + camera);
    mouse_tile = map.mouse2basetile(mouse_point + camera);
    //auto mouse_tile_high = mouse2tile_high(mouse_point + camera);
    if (mouse_tile != last_mouse_tile) {
      //cout << "mouse in tile  " << mouse_tile.x << "  " << mouse_tile.y << endl;
      //auto sx = tile2screen({ 0, MAP_H - 1 }), dx = tile2screen({ MAP_W - 1 , 0 }), su = tile2screen({ 0, 0 }), giu = tile2screen({ MAP_W - 1, MAP_H - 1 });
      //cout << "su (" << su.x << "," << su.y << ") " << "sx (" << sx.x << "," << sx.y << ") " << "giu (" << giu.x << "," << giu.y << ") " << "dx (" << dx.x << "," << dx.y << ")" << endl;
      last_mouse_tile = mouse_tile;
#ifdef SHOW_PATH
      bestpath = map.findPath_Astar(sprite_tile, mouse_tile);
#endif
    }

    // Render ground 
    //for (int i = 0; i < MAP_W; i++) {
    //  for (int j = 0; j < MAP_H; j++) {
    //    SDL_Rect *currentSprite;
    //    if (i < MAP_W / 2 && j < MAP_H / 2) currentSprite = &gSpriteClips[2];
    //    else currentSprite = &gSpriteClips[15];
    //    renderTile(camera, { i, j }, currentSprite);
    //  }
    //}
    map.render(camera);

#ifdef SHOW_BUTTONS
    //Render buttons 
    for (int i = 0; i < TOTAL_BUTTONS; ++i) {
      gButtons[i].render();
    }
#endif

#ifdef SHOW_PATH
    // render path
    for (auto& p : bestpath) map.renderSprite(camera, p, &highlighterSprite);
#endif

    // Render cursor
    map.renderCursor(camera, mouse_point, &LowCursorSprite);
    //renderCursor(camera, mouse_tile, &LowCursorSprite);
    //renderCursor(camera, mouse_tile_high, &HighCursorSprite);

    // Render unit
    renderTile(camera, sprite_tile, &unitSprite);


#ifdef SHOW_TEXT
    label.loadText("Single Line Text", textGreen);
    label.render(5 * SCREEN_WIDTH / 7, SCREEN_HEIGHT / 8);
#endif

    //Update screen
    SDL_RenderPresent(gRenderer);
  }

  //Free resources and close SDL
  close();

  return 0;
}
