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

#define OFFSET_X      400.
#define OFFSET_Y      150.

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

        //Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
          //User requests quit
          if (e.type == SDL_QUIT) {
            quit = true;
          }

          //Get mouse position
          if (e.type == SDL_MOUSEMOTION) {
            SDL_GetMouseState(&mouseX, &mouseY);
          }

          //Handle button events 
          for( int i = 0; i < TOTAL_BUTTONS; ++i ) { 
            gButtons[ i ].handleEvent( &e ); 
          } 
        }

        //Clear screen
        SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0xFF);
        SDL_RenderClear(gRenderer);

        for (int i = 0; i < 5; i++) {
          for (int j = 0; j < 5; j++) {
            int X = (i - j) * tileWidth / 2 + OFFSET_X;
            int Y = (i + j) * tileHeight / 4 + OFFSET_Y;
            gSpriteSheetTexture.render(Point{ X , Y }, &cursor);
          }
        }

        //Render buttons 
        for( int i = 0; i < TOTAL_BUTTONS; ++i ) { 
          gButtons[ i ].render(); 
        } 

        auto mo = smartPoint(mouseX, mouseY - tileHeight / 2);
        int I = (mo.cartX - OFFSET_X) / tileWidth + (mo.cartY - OFFSET_Y) / (tileHeight / 2.);
        int J = -(mo.cartX - OFFSET_X) / tileWidth + (mo.cartY - OFFSET_Y) / (tileHeight / 2.);
        cout << "x " << mo.cartX << " y " << mo.cartY <<
           " tile  " << I << " " << J << endl;
//          " u " << mo.isoX << " v " << mo.isoY <<
//          " iso BIN  " << abs(mo.isoX) / isoW << "   " << abs(mo.isoY) / isoH << endl;
        int XX = (I - J) * tileWidth / 2 + OFFSET_X;
        int YY = (I + J) * tileHeight / 4 + OFFSET_Y;
        gSpriteSheetTexture.render(Point{ XX , YY }, &gSpriteClips[27]);

        //Update screen
        SDL_RenderPresent(gRenderer);
      }
    }
  }

  //Free resources and close SDL
  close();

  return 0;
}
