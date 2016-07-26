#pragma once
#include <vector>

#include <SDL.h>

//#include "animations.h"
#include "geometry.h"
#include "battlemap.h"

class Units {
public:
  Point Tile;
  size_t TickCounter;
  SDL_Rect CurrentSprite;
  std::vector<int> ClipsIndices;

  // Animation
  SDL_TimerID AnimationTimer;

  // Constructor
  Units() {};
  Units(const Point &p);
  Units(std::string json_path);

  // Methods
  void AddTimer();
  void UpdateSprite();
  void render(Map &map, const Point &cam);
};

