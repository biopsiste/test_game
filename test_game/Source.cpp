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

Uint32 my_callbackfunc(Uint32 interval, void *param) {
	SDL_Event event;
	SDL_UserEvent userevent;

	/* In this example, our callback pushes an SDL_USEREVENT event
	into the queue, and causes our callback to be called again at the
	same interval: */

	userevent.type = SDL_USEREVENT;
	userevent.code = 0;
	userevent.data1 = NULL;
	userevent.data2 = NULL;

	event.type = SDL_USEREVENT;
	event.user = userevent;

	SDL_PushEvent(&event);
	return(interval);
}


int main(int argc, char* args[]) {
	//int* p;	auto path = findPath(p, Point{ 0,0 }, Point{ 0,5 }); //to test pathing

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

			Point mouse_tile, last_mouse_tile{ -100, -100 };
			Point sprite_tile{ 1, 1 };
      // MAIN LOOP

			SDL_TimerID sprite_move_timer = SDL_AddTimer(1000, my_callbackfunc, &sprite_tile); //call callback every 1 sec

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

					// move sprite, commented
					if(e.type == SDL_USEREVENT) {
						//insert new coords here, example:
						//sprite_tile.x++;
						//sprite_tile.y++;
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
				mouse_tile = mouse2tile(Point{ mouseX, mouseY });
				if(mouse_tile != last_mouse_tile) {
					cout <<"mouse in tile  " << mouse_tile.x << "  " << mouse_tile.y << endl;
					last_mouse_tile = mouse_tile;
				}
        renderCursor(mouse_tile, &cursorSprite);

        // Render unit
        renderTile(sprite_tile, &unitSprite);

        //Update screen
        SDL_RenderPresent(gRenderer);
      }
    }
  }

  //Free resources and close SDL
  close();

  return 0;
}
