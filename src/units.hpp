#pragma once

class Units {
public:
  Point Tile;
  size_t TickCounter;
  int SpriteAltitude;
  SDL_Rect CurrentSprite;
  std::vector<int> ClipsIndices;

  // Animation
  SDL_TimerID AnimationTimer;

  // Constructor
  Units(const Point &p);

  // Methods
  void AddTimer();
  void UpdateSprite();
  void render(Map &map, const Point &cam);
};

Units::Units(const Point &p){
  Tile = p;
  TickCounter = 0;

  // modify to parse a json encoding unit properties
  ClipsIndices = { 40, 44, 42, 46 };
  SpriteAltitude = 1;
}

void Units::render(Map &map, const Point &cam) {
  map.renderSprite(cam, Tile, &CurrentSprite);
}

void Units::UpdateSprite() {
  CurrentSprite = gSpriteClips[ ClipsIndices[ TickCounter++ % ClipsIndices.size()]];
}

void Units::AddTimer() {
  AnimationTimer = SDL_AddTimer(ANIMATION_DT_MS, user_event_cb, NULL);
}


