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

// Jsoncons
#include "jsoncons/json.hpp"

// GOTYAY include
#include "geometry.hpp"
#include "texture.hpp"
#include "buttons.hpp"
#include "animation.hpp"
#include "textbox.hpp"
#include "battlemap.hpp"
#include "units.hpp"

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

  // Load sprite media
  if (!loadMedia(TILESET_PATH)) {
    printf("Failed to load media!\n"); 
    exit(3);
  }

  // Load text media  
  SDL_Color currentColor = textWhite;
  LTextTexture label;
  if (!label.loadFormat(TTF_PATH_LAZY, 25)) {
    printf("Failed to load text media!\n");
    exit(4);
  }
  LMultiLineTextTexture menuTextBox;
  if (!menuTextBox.loadFormat(TTF_PATH_LAZY, 20)) {
    printf("Failed to load text media!\n");
    exit(5);
  }
  std::string menuText = R"(MENU:
1) 'X' to quit
2) 'D' to toggle
on/off this menu
3) 'C' (hold) to change 
menu color
4) 'A' toggle on/off
unit animation
5) coming soon
)";
    menuTextBox.setText(menuText);

  // Event handler and variables
  SDL_Event e;
  bool show_menu = true;
  bool unit_animation_on = true;
  bool quit = false; 
  const unsigned char* currentKeyStates;

  // Level map object
  Map map("../resources/test_map.txt");

  // Unit variables
  Units unit(Point{map.w()-1, 0});
  unit.AddTimer();

  // Multipurpose Point variables
  Point mouse_tile{}, last_mouse_tile{ -100, -100 }, mouse_point{};
  Point camera{ 0, 0 };
  vector<Point> bestpath;

  // Set buttons position
  for (int i = 0; i < TOTAL_BUTTONS; ++i) {
    gButtons[i].setPosition(50 + i*BUTTON_WIDTH, 50);
  }

  //// MAIN LOOP
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
          show_menu = (show_menu == true) ? false : true;
          break;
        case SDLK_a:                     // toggle animation on/off
          unit_animation_on = (unit_animation_on == true) ? false : true;
          break;
        case SDLK_c:                     // change text color holding C
          currentColor = textRed;
          break;

        // move camera
        case SDLK_DOWN:
          camera.y += 10; if (camera.y + winH > map.south.y + 64 + 30) camera.y = map.south.y + 64 + 30 - winH;
          break;
        case SDLK_UP:
          camera.y -= 10; if (camera.y < map.north.y - 30) camera.y = map.north.y - 30;
          break;
        case SDLK_LEFT:
          camera.x -= 10; if (camera.x < map.west.x - 30) camera.x = map.west.x - 30;
          break;
        case SDLK_RIGHT:
          camera.x += 10; if (camera.x + winW > map.east.x + 64 + 30) camera.x = map.east.x + 64 + 30 - winW;
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
        SDL_GetMouseState(&(mouse_point.x), &(mouse_point.y));
      }

      // Timer callback points here
      if (e.type == SDL_USEREVENT) {
        if( unit_animation_on )
          unit.UpdateSprite();
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

    // Render map
    map.render(camera);

#ifdef SHOW_BUTTONS
    // Render buttons 
    for (int i = 0; i < TOTAL_BUTTONS; ++i) {
      gButtons[i].render();
    }
#endif

#ifdef SHOW_PATH
    // Render path
    for (auto& p : bestpath) map.renderSprite(camera, p, &highlighterSprite);
#endif

    // Render unit
    unit.render(map, camera);

    // Render cursor
    map.renderCursor(camera, mouse_point, &LowCursorSprite);

#ifdef SHOW_TEXT
    // Single Line
    label.setText("Single Line Text", textGreen);
    label.render(Point{ 5 * SCREEN_WIDTH / 7, SCREEN_HEIGHT / 8 });

    // Multiline
    if (show_menu) {
      menuTextBox.setText(menuText, currentColor);
      menuTextBox.render(Point{ 5 * SCREEN_WIDTH / 7, 2 * SCREEN_HEIGHT / 8 });
    }
#endif

    //Update screen
    SDL_RenderPresent(gRenderer);
  }

  //Free resources and close SDL
  close();

  return 0;
}
