#pragma once
// STL include
#include <vector>

// SDL include
#include <SDL.h>

// GOTYAY include
#include "geometry.h"
#include "battlemap.h"

#define TIMER_ANIMATION    'a'
#define TIMER_MOTION       'm'
#define TIMER_NONE         'n'


class Units {
public:
  Point CurrentTile;
  size_t TickCounter;
  SDL_Rect CurrentSprite;
  std::vector<int> ClipsIndices;
  std::vector<Point> TilePath;

  // Animation
  char TimerType;
  SDL_TimerID UnitsTimer;

  // Constructor
  Units() {};
  Units(const Point &p);
  Units(std::string json_path);

  // Methods
  void AddTimer(Uint32 dt_ms, char type);
  void UpdateSprite();
  void UpdateTile();
  void render(Map &map, const Point &cam);
};

// Timer global callback
Uint32 units_cb(Uint32 interval, void* param);


