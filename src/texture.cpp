// Copyright 2016 Marco Di Cristina, Alessandro Fabbri
#pragma once

#include "texture.h"
#include <SDL_image.h>

extern SDL_Renderer *gRenderer;
extern SDL_Window *gWindow;
extern SDL_Rect gSpriteClips[], cursorSprite, highlighterSprite, HighCursorSprite, LowCursorSprite;
extern PTexture gSpriteSheetTexture;

PTexture::PTexture(){
  //Initialize
  mTexture = NULL;
  mWidth = 0;
  mHeight = 0;
}

PTexture::~PTexture() {
  //Deallocate
  free();
}

bool PTexture::loadFromFile(std::string path) {
  //Get rid of preexisting texture
  free();

  //The final texture
  SDL_Texture* newTexture = NULL;

  //Load image at specified path
  SDL_Surface* loadedSurface = IMG_Load(path.c_str());
  if (loadedSurface == NULL) {
    printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
  }
  else {
    //Color key image
    SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

    //Create texture from surface pixels
    newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
    if (newTexture == NULL) {
      printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
    }
    else {
      //Get image dimensions
      mWidth = loadedSurface->w;
      mHeight = loadedSurface->h;
    }

    //Get rid of old loaded surface
    SDL_FreeSurface(loadedSurface);
  }

  //Return success
  mTexture = newTexture;
  return mTexture != NULL;
}

void PTexture::free() {
  //Free texture if it exists
  if (mTexture != NULL) {
    SDL_DestroyTexture(mTexture);
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
  }
}

void PTexture::render(const Point &p, SDL_Rect* clip /* = NULL */ ) {
  //Set rendering space and render to screen
  SDL_Rect renderQuad = { p.x, p.y, mWidth, mHeight };

  //Set clip rendering dimensions
  if (clip != NULL) {
    renderQuad.w = clip->w;
    renderQuad.h = clip->h;
  }

  //Render to screen
  SDL_RenderCopy(gRenderer, mTexture, clip, &renderQuad);
}


bool init() {
  //Initialization flag
  bool success = true;

  //Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
    success = false;
  }
  else {
    //Set texture filtering to linear
    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
      printf("Warning: Linear texture filtering not enabled!");
    }

    //Create window
    gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
    if (gWindow == NULL) {
      printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
      success = false;
    }
    else {
      //Create renderer for window
      gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
      if (gRenderer == NULL) {
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        success = false;
      }
      else {
        //Initialize renderer color
        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

        //Initialize PNG loading
        int imgFlags = IMG_INIT_PNG;
        if (!(IMG_Init(imgFlags) & imgFlags)) {
          printf("SDL_image could not initialize! SDL_mage Error: %s\n", IMG_GetError());
          success = false;
        }
      }
    }
  }

  return success;
}

bool loadMedia(std::string path) {
  //Loading success flag
  bool success = true;

  //Load sprite sheet texture
  if (!gSpriteSheetTexture.loadFromFile(path)) {
    printf("Failed to load sprite sheet texture!\n");
    success = false;
  }
  else {
    for (int i = 0; i < TILESET_TILES; ++i) {
      int r = i / TILESET_COLUMNS, c = i % TILESET_COLUMNS;
      gSpriteClips[i].x = int( TILE_W * c );
      gSpriteClips[i].y = int( TILE_H * r );
      gSpriteClips[i].w = int( TILE_W );
      gSpriteClips[i].h = int( TILE_H );
    }

    cursorSprite = gSpriteClips[TILESET_CURSOR_TILE_INDEX];
    HighCursorSprite = gSpriteClips[TILESET_CURSOR_TILE_INDEX-1];
    LowCursorSprite = gSpriteClips[TILESET_CURSOR_TILE_INDEX];
		highlighterSprite = gSpriteClips[TILESET_HIGHLIGHTER_TILE_INDEX];
	}

  return success;
}

void close() {
  //Free loaded images
  gSpriteSheetTexture.free();

  //Destroy window	
  SDL_DestroyRenderer(gRenderer);
  SDL_DestroyWindow(gWindow);
  gWindow = NULL;
  gRenderer = NULL;

  //Quit SDL subsystems
  IMG_Quit();
  SDL_Quit();
}



///////////// useful render function
void renderTile(const Point& cam, const Point& tile_index, SDL_Rect * tile) {
  gSpriteSheetTexture.render(tile2screen(tile_index) - cam, tile);
}

