#pragma once

//Screen dimension constants
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 672;

const int tileX = 0;
const int tileY = 32;
const float tileWidth = 64.;
const float tileHeight = 64.;
const int tileHeightOffset = 32;

inline void clamp(int& i, int a = 0, int b = 9) {
  if (i < a) i = a; else if (i > b) i = b;
}

struct Point {
  int x, y;
};

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

Point tile2screen(Point p) {
  return Point{
    int(tileWidth * p.x / 2),
    int(tileHeight * p.y / 2)
  };
}

Point screen2tile(Point p) {
  return Point{
    int(p.x / tileWidth),
    int(p.y / tileHeight)
  };
}

Point mouse2tile(Point p) {
  return Point{
    int(p.x / tileWidth - p.y / (tileHeight / 2) + 1),
    int(p.x / tileWidth + p.y / (tileHeight / 2) + 1)
  };
}

///////////////// ALLE

#define      CART_COORD       1
#define      ISO_COORD        2
#define      CART_IND         3
#define      ISO_IND          4

class smartPoint {
public:
  int cartX, cartY;
  int isoX, isoY;
  int cartI, cartJ;
  int isoI, isoJ;

  smartPoint(int a, int b, int c = CART_COORD) {
    switch (c) {
    case CART_COORD:
      cartX = a;
      cartY = b;
      isoX = cartX - cartY;
      isoY = 0.5*(cartX + cartY);
      break;
    case ISO_COORD:
      isoX = a;
      isoY = b;
      cartX = (2 * isoY + isoX) / 2;
      cartY = (2 * isoY - isoX) / 2;
      break;
    default:
      break;
    }
  }

};



