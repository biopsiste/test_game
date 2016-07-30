// Copyright 2016 Marco Di Cristina, Alessandro Fabbri
#pragma once

#include <string>

#include <SDL.h>

#include "geometry.h"

#define USE_TILESET                      2

#if USE_TILESET == 1
//tileset 1
#define TILESET_PATH                     "../resources/tilesets/iso-64x64-outside_numeri_transp.png"

#define TILESET_COLUMNS                  10
#define TILESET_ROWS                     16
#define TILESET_TILES                    160

#define TILESET_UNIT_TILE_INDEX          119
#define TILESET_CURSOR_TILE_INDEX        159
#define TILESET_HIGHLIGHTER_TILE_INDEX   44
#elif USE_TILESET == 2
//tileset 2
#define TILESET_PATH                     "../resources/tilesets/basic_ground_tiles_64_halftile.png"

#define TILESET_COLUMNS                  8
#define TILESET_ROWS                     7
#define TILESET_TILES                    56

#define TILESET_UNIT_TILE_INDEX          47
#define TILESET_CURSOR_TILE_INDEX        8
#define TILESET_HIGHLIGHTER_TILE_INDEX   16
#define TILESET_SELECTOR_TILE_INDEX      24
#endif

#define UNIT_SPRITE_NUM                  4


////// Texture wrapper class
class LTexture {
public:
  //The actual hardware texture
  SDL_Texture* mTexture;
  SDL_Renderer* mRenderer;

  //Image dimensions
  int mWidth;
  int mHeight;

  //Initializes variables
  LTexture();
  //Deallocates memory
  ~LTexture();

  // Loads image at specified path
  bool loadFromFile(std::string path);

  //Deallocates texture
  void free();

  //Renders texture at given point
  void render(const Point &p, SDL_Rect* clip = NULL);
};

bool init();

bool loadMedia(std::string path);

void close();

///////////// useful render function
void renderTile(const Point& cam, const Point& tile_index, SDL_Rect * tile);