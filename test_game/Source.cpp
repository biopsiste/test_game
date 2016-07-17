//Using SDL, SDL_image, standard math, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <iostream>

#include "geometry.h"
#include "texture.h"
#include "buttons.h"

using namespace std;

int main(int argc, char* args[]) {
  //Start up SDL and create window
  if (!init()) {
    printf("Failed to initialize!\n");
  }
  else {
    //Load media
    if (!loadMedia()) {
      printf("Failed to load media!\n");
      exit(-1);
    }
    else {
      //Main loop flag
      bool quit = false;

      //Event handler
      SDL_Event e;

      // Set buttons position
      for (int i = 0; i < TOTAL_BUTTONS; ++i) {
        gButtons[i].setPosition(50 + i*BUTTON_WIDTH, 50 );
      }

      //While application is running
      int mouseX = 0, mouseY = 0;
      int cartX = 0, cartY = 0;
      int isoX = 0, isoY = 0;

      // MAIN LOOP
      while (!quit) {

        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
          // User requests quit
          if (e.type == SDL_QUIT) {
            quit = true;
          }

          // Get mouse position
          if (e.type == SDL_MOUSEMOTION) {
            SDL_GetMouseState(&mouseX, &mouseY);
          }

          // Handle button events 
          for( int i = 0; i < TOTAL_BUTTONS; ++i ) { 
            gButtons[ i ].handleEvent( &e ); 
          } 
        }

        //Clear screen
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xFF);
        SDL_RenderClear(gRenderer);

        // Render ground 
        for (int i = 0; i < MAP_W; i++) {
          for (int j = 0; j < MAP_H; j++) {
            renderTile(i, j, &gSpriteClips[25]);
          }
        }

        //Render buttons 
        for( int i = 0; i < TOTAL_BUTTONS; ++i ) { 
          gButtons[ i ].render(); 
        } 

        // Render cursor
        renderCursor(mouse2tile(Point{ mouseX, mouseY }), &cursorSprite);

        // Render unit
        renderTile(1,1, &unitSprite);

        //Update screen
        SDL_RenderPresent(gRenderer);
      }
    }
  }

  //Free resources and close SDL
  close();

  return 0;
}
