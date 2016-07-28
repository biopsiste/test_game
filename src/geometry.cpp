#pragma once
#include <ostream>
#include "geometry.h"

uint32_t Point::hash() const {
  //fast
  return x * 1812433253 + y;

  //a bit slower, but no collision if x, y are uint32_t
  //return x >= y ?
  //	uint64_t(x) * uint64_t(x) + uint64_t(x) + uint64_t(y) :
  //	uint64_t(x) + uint64_t(y) * uint64_t(y);
}

bool Point::operator==(const Point& b) const {
  return x == b.x && y == b.y;
}
bool Point::operator!=(const Point& b) const {
  return !(*this == b);
}
Point& Point::operator+=(const Point& b) {
  this->x += b.x; this->y += b.y;
  return *this;
}
Point& Point::operator++() {
  *this += Point{ 1,1 };
  return *this;
}
Point operator+(Point th, const Point& b) {
  th += b;
  return th;
}
Point& Point::operator-=(const Point& b) {
  this->x -= b.x; this->y -= b.y;
  return *this;
}
Point operator-(Point th, const Point& b) {
  th -= b;
  return th;
}
Point& Point::operator--() {
  *this -= Point{ 1,1 };
  return *this;
}
std::ostream& operator<<(std::ostream& os, const Point& p) {
  os << "[" << p.x << "," << p.y << "]";
  return os;
}

uint32_t PointHasher::operator()(const Point& p) const {
  return p.hash();
}

Point tile2screen(int i, int j) {
  int x = int((i - j) * TILE_W / 2 + OFFSET_X /*+ 0.5*/);
  int y = int((i + j) * TILE_H / 4 + OFFSET_Y /*+ 0.5*/);
  return Point{ x, y };
}
Point tile2screen(const Point& tile) {
  return tile2screen(tile.x, tile.y);
}

Point mouse2tile(const Point& mouse) {
  int x = int(mouse.x);
  int y = int(mouse.y - TILE_H / 2. + TILE_GROUND_HEIGHT_OFFSET);
  int I = int((x - OFFSET_X) / TILE_W + (y - OFFSET_Y) / (TILE_H / 2.) - 1 + 0.5);
  int J = int(-(x - OFFSET_X) / TILE_W + (y - OFFSET_Y) / (TILE_H / 2.) + 0.5);
  return Point{ I, J };
}
Point mouse2tile_high(const Point& mouse) {
  int x = int(mouse.x);
  int y = int(mouse.y - TILE_H / 2. + 2 * TILE_GROUND_HEIGHT_OFFSET);
  int I = int((x - OFFSET_X) / TILE_W + (y - OFFSET_Y) / (TILE_H / 2.) - 1 + 0.5);
  int J = int(-(x - OFFSET_X) / TILE_W + (y - OFFSET_Y) / (TILE_H / 2.) + 0.5);
  return Point{ I, J };
}

inline bool are_4neighbors(const Point& a, const Point& b) {
  return (a.x == b.x && abs(a.y - b.y) == 1) || (a.y == b.y && abs(a.x - b.x) == 1);
}


