#pragma once

//Geometry constants
#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 672

#define OFFSET_X      400.
#define OFFSET_Y      150.
#define TILE_W        64.
#define TILE_H        64.

#define MAP_W         10
#define MAP_H         7

struct Point {
  int x, y;
};

Point tile2screen(int i, int j) {
  int x = int( (i - j) * TILE_W / 2 + OFFSET_X + 0.5 );
  int y = int( (i + j) * TILE_H / 4 + OFFSET_Y + 0.5 );
  return Point{ x, y };
}

Point tile2screen(Point tile) {
  return tile2screen(tile.x, tile.y);
}

Point mouse2tile(Point mouse) {
  int x = int( mouse.x );
  int y = int( mouse.y - TILE_H / 2. );
  int I = int( (x - OFFSET_X) / TILE_W + (y - OFFSET_Y) / (TILE_H / 2.) - 1 + 0.5 );
  int J = int( -(x - OFFSET_X) / TILE_W + (y - OFFSET_Y) / (TILE_H / 2.) + 0.5 );
  return Point{I, J};
}



////////////////// deprecated ???

inline void clamp(int& i, int a = 0, int b = 9) {
  if (i < a) i = a; else if (i > b) i = b;
}

Point isometric2cartesian(Point p) {
  return Point{
    (2 * p.y + p.x) / 2,
    (2 * p.y - p.x) / 2
  };
}

Point cartesian2isometric(Point p) {
  return Point{
    p.x - p.y,
    (p.x + p.y) / 2
  };
}

