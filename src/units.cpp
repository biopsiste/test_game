#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include "jsoncons/json.hpp"

#include "units.h"
#include "animation.h"

extern SDL_Rect gSpriteClips[];

Units::Units(const Point &p) {
  Tile = p;
  TickCounter = 0;

  // modify to parse a json encoding unit properties
  ClipsIndices = { 40, 44, 42, 46 };
}

Units::Units(std::string json_path) {
  jsoncons::json junit = jsoncons::json::parse_file(json_path);

  Tile = Point{ junit.has_member("tile") ? junit["tile"][0].as<int>() : 0,
    junit.has_member("tile") ? junit["tile"][1].as<int>() : 0 };
  TickCounter = 0;
  if(junit.has_member("clips_indices") && junit["clips_indices"].is_array()) {
    jsoncons::json jtemp = junit["clips_indices"];
    for(size_t i = 0; i < jtemp.size(); i++) {
      ClipsIndices.push_back(jtemp[i].as<int>());
    }
  } else
    ClipsIndices.push_back(0);
}

void Units::render(Map &map, const Point &cam) {
  map.renderSprite(cam, Tile, &CurrentSprite);
}

void Units::UpdateSprite() {
  CurrentSprite = gSpriteClips[ClipsIndices[TickCounter++ % ClipsIndices.size()]];
}

void Units::AddTimer() {
  AnimationTimer = SDL_AddTimer(ANIMATION_DT_MS, user_event_cb, NULL);
}
