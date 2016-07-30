// Copyright 2016 Marco Di Cristina, Alessandro Fabbri
#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include "jsoncons/json.hpp"

#include <string>

#include "units.h"
#include "animation.h"
#include "defines.h"

extern SDL_Rect gSpriteClips[];
extern SDL_Window *gWindow;

Units::Units(const Point &p) {
  CurrentTile = p;
  TickCounter = 0;
  ClipsIndices = { 1 };
  TimerType = TIMER_NONE;
}

Units::Units(std::string json_path) {
  jsoncons::json junit;
  try {
    junit = jsoncons::json::parse_file(json_path);
  }
  catch (std::exception &e) {
    std::cout << "ERROR: " << e.what() << std::endl; CLI_PAUSE
    exit(21);
  }

  CurrentTile = Point{ junit.has_member("tile") ? junit["tile"][0].as<int>() : 0,
    junit.has_member("tile") ? junit["tile"][1].as<int>() : 0 };

  if (junit.has_member("tile_path") && junit["tile_path"].is_array()) {
    jsoncons::json jtemp = junit["tile_path"];
    for (size_t i = 0; i < jtemp.size(); i++) {
      TilePath.push_back(Point{ jtemp[i][0].as<int>(), jtemp[i][1].as<int>() });
    }
  }
  else
    TilePath.push_back(Point{ 0, 0 });

  CurrentSprite = gSpriteClips[junit.has_member("clip") ? junit["clip"].as<int>() : 0];

  if(junit.has_member("clips_indices") && junit["clips_indices"].is_array()) {
    jsoncons::json jtemp = junit["clips_indices"];
    for(size_t i = 0; i < jtemp.size(); i++) {
      ClipsIndices.push_back(jtemp[i].as<int>());
    }
    CurrentSprite = gSpriteClips[ClipsIndices.front()];
  } else
    ClipsIndices.push_back(0);

  TimerType = TIMER_NONE;
  TickCounter = 0;
}

//void Units::render(Map &map, const Point &cam) {
//  map.renderSprite(cam, CurrentTile, &CurrentSprite);
//}

void Units::UpdateSprite() {
  CurrentSprite = gSpriteClips[ClipsIndices[TickCounter++ % ClipsIndices.size()]];
}

void Units::UpdateTile() {
  CurrentTile = TilePath[TickCounter++ % TilePath.size()];
}

void Units::AddTimer(Uint32 dt_ms, char type) {
  TimerType = type;
  UnitsTimer = SDL_AddTimer(dt_ms, units_cb, this);
}

Uint32 units_cb(Uint32 interval, void* param) {
  Units * uptr = (Units *)param;

  switch (uptr->TimerType) {
  case TIMER_ANIMATION:
    uptr->UpdateSprite();
    break;
  case TIMER_MOTION:
    uptr->UpdateTile();
    break;
  case TIMER_NONE:
    break;
  default:
    std::cout << "You can't read this!" << std::endl;
    break;
  }

  return interval;
}

void Units::initStats() {
  max_hp = 100; max_mp = 50;
  hp = max_hp; mp = max_mp;
}

void Units::updateStatusBar() {
	int winH, winW; SDL_GetWindowSize(gWindow, &winW, &winH);
	gui.hpBar_outline = { 20, winH - 100, 200, 20 };
	gui.mpBar_outline = { 20, winH -  50, 200, 20 };

	int hpsize = double(hp) / max_hp * 200. + 0.5;
	int mpsize = double(mp) / max_mp * 200. + 0.5;
	gui.hpBar = { 20, winH - 100, hpsize, 20 };
	gui.mpBar = { 20, winH - 50, mpsize, 20 };

	std::string hplabel = std::to_string(hp) + "/" + std::to_string(max_hp);
	std::string mplabel = std::to_string(mp) + "/" + std::to_string(max_mp);
	gui.hp_label.setText(hplabel);
	gui.mp_label.setText(mplabel);
}
