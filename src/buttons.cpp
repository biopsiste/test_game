// Copyright 2016 Marco Di Cristina, Alessandro Fabbri
#pragma once

#include "texture.h"
#include "buttons.h"

extern SDL_Rect gSpriteClips[], cursorSprite, highlighterSprite, HighCursorSprite, LowCursorSprite;
extern LTexture gSpriteSheetTexture;

LButton::LButton() { 
  mPosition.x = 0; 
  mPosition.y = 0; 
  mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT; 
} 

void LButton::setPosition(int x, int y) { 
  mPosition.x = x; 
  mPosition.y = y; 
}

void LButton::handleEvent(SDL_Event* e) { 
  //If mouse event happened 
  if( e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP ) { 
    //Get mouse position 
    int x, y; 
    SDL_GetMouseState( &x, &y );

    //Check if mouse is inside the button area
    bool inside = true; 
    
    if( x < mPosition.x ) {    //Mouse is left of the button 
      inside = false; 
    } 
    else if( x > mPosition.x + BUTTON_WIDTH ) {    //Mouse is right of the button 
      inside = false; 
    }
    else if( y < mPosition.y ) {  //Mouse above the button 
      inside = false; 
    } 
    else if( y > mPosition.y + BUTTON_HEIGHT ) {   //Mouse below the button 
      inside = false; 
    }

    
    if( !inside ) {    //Mouse is outside button 
      mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT; 
    } 
    else {     //Mouse is inside button 
      //Set mouse over sprite 
      switch( e->type ) { 
      case SDL_MOUSEMOTION: 
        mCurrentSprite = BUTTON_SPRITE_MOUSE_OVER_MOTION; 
        break; 
      case SDL_MOUSEBUTTONDOWN: 
        mCurrentSprite = BUTTON_SPRITE_MOUSE_DOWN; 
        break; 
      case SDL_MOUSEBUTTONUP: 
        mCurrentSprite = BUTTON_SPRITE_MOUSE_UP; 
        break; 
      } 
    } 
  } 
}

void LButton::render() { 
  //Show current button sprite 
  gSpriteSheetTexture.render({ mPosition.x, mPosition.y }, &gSpriteClips[mCurrentSprite]);
}


