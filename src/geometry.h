#pragma once

//Geometry constants
#define SCREEN_WIDTH              1280
#define SCREEN_HEIGHT             672

#define OFFSET_X                  350.
#define OFFSET_Y                  30.

#define TILE_W                    64.
#define TILE_H                    64.
#define TILE_GROUND_HEIGHT_OFFSET 16

struct Point {
  int x, y;

  uint32_t hash() const;
  bool operator==(const Point& b) const;
  bool operator!=(const Point& b) const;
  Point& operator+=(const Point& b);
  Point& operator++();
  friend Point operator+(Point th, const Point& b);
  friend Point operator-(Point th, const Point& b);
  Point& operator-=(const Point& b);
  Point& operator--();


  friend std::ostream& operator<<(std::ostream& os, const Point& p);
};
struct PointHasher {
  uint32_t operator()(const Point& p) const;
};

Point tile2screen(int i, int j);
Point tile2screen(const Point& tile);

Point mouse2tile(const Point& mouse);
Point mouse2tile_high(const Point& mouse);

inline bool are_4neighbors(const Point& a, const Point& b);
