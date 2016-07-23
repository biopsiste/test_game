#pragma once

class TileTower {
  int i, j;
  std::vector<SDL_Rect> content;
  int quota;

  void render();
};

class TerrainMap {
  int w, h;
  std::vector< std::vector<TileTower>> tiles;

  void render();
};

