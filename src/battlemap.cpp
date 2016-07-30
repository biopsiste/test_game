// Copyright 2016 Marco Di Cristina, Alessandro Fabbri
#pragma once

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>

#include <SDL.h>

#include "texture.h"
#include "geometry.h"
#include "battlemap.h"
#include "utils.h"

//Scene sprites
extern SDL_Rect gSpriteClips[], cursorSprite, highlighterSprite, HighCursorSprite, LowCursorSprite;
extern LTexture gSpriteSheetTexture;


////// CUBE methods
Cube::Cube(int _stack_index, Point _base_coords, Point _flat_coords, Point _scr_coords, std::string entry) :
  stack_index(_stack_index), base_coords(_base_coords), flat_coords(_flat_coords), screen_coords(_scr_coords) {
  std::istringstream istr(entry);
  std::string token;
  std::getline(istr, token, ','); sprite_index = std::stoi(token);
  std::getline(istr, token, ','); altitude = std::stoi(token);
  std::getline(istr, token, ','); traversability = std::stoi(token);
  //std::getline(istr, token, ','); empty = std::stoi(token);
  //std::cout << sprite_index << " " << height << " " << passable << " " << empty << std::endl; system("pause");
}

void Cube::render(const Point& cam) {
  gSpriteSheetTexture.render(screen_coords - cam, &gSpriteClips[sprite_index]);
}


////// CUBESTACK methods
CubeStack::CubeStack(Point _coords, std::string stackentry) : coords(_coords) {
  std::istringstream istr(stackentry);
  std::string token;
  int i = 0;
  while (std::getline(istr, token, '|')) {
    //std::cout << token << std::endl; //system("pause");

    auto scr_coords = tile2screen(_coords) - Point{ 0, i*int(TILE_H) / 2 };
    s.emplace_back(i, _coords, _coords - Point{ i,i }, scr_coords, token);
    i++;
  }
  //std::cout << "---" << std::endl; system("pause");
}

void CubeStack::render(const Point& cam) {
  for (auto &c : s) c.render(cam);
}

int CubeStack::altitude() { //total altitude
  return 2 * (s.size() - 1) + s.back().altitude;
}


/////// MAP methods
Map::Map(std::string filename) {
  std::ifstream ifi(filename);
  std::string line, stackentry;
  int w, h;
  std::getline(ifi, line); //ignore header
  ifi >> w >> h; //std::cout << w << " " << h << std::endl;
  std::getline(ifi, line); //ignore w h
  m.assign(w, std::vector<CubeStack>(h));
  int r = 0;
  while (std::getline(ifi, line)) {
    std::istringstream istr(line);
    int c = 0;
    while (istr >> stackentry) {
      //      std::cout << stackentry << std::endl;
            //std::cout << r << " " << c << std::endl;
      m[c][r] = CubeStack(Point{ c, r }, stackentry);
      c++;  // <--- cool!
    }
    r++;
  }

  north = tile2screen(Point{ 0, 0 });
  south = tile2screen(Point{ this->w() - 1, this->h() - 1 });
  west = tile2screen(Point{ 0,this->h() - 1 });
  east = tile2screen(Point{ this->w() - 1 , 0 });
}

// utilities
inline int Map::w() { 
  return m.size(); 
}

inline int Map::h() { 
  return m[0].size(); 
}

inline bool Map::is_in_map(const Point& p) {
  return p.x >= 0 && p.x < w() && p.y >= 0 && p.y < h();
}

inline bool Map::isSafeTile(const Point & tile) {
  if (tile.x < w() &&     // These conditions ensure that selected tile
    tile.y < h() &&       // is inside the upper indices bounds 
    ((tile.x - tile.y) < w()) &&       // These conditions ensure the existence
    ((tile.y - tile.x) < h())          // of low tile
    )
    return true;
  else
    return false;
}


// render
//void Map::renderSprite(const Point& cam, const Point& tile_index, SDL_Rect * tile) {
//  Point vert_offset{ 0, m[tile_index.x][tile_index.y].s.back().altitude*TILE_GROUND_HEIGHT_OFFSET };
//  gSpriteSheetTexture.render(m[tile_index.x][tile_index.y].s.back().screen_coords - vert_offset - cam, tile);
//
//  //re-render cubes that should be in the foreground
//  for (int j = tile_index.y + 1; j < h(); ++j) {
//    if (is_in_map({ tile_index.x, j })) m[tile_index.x][j].render(cam);
//  }
//  for (int i = tile_index.x + 1; i < w(); ++i) {
//    for (int j = 0; j < h(); ++j) {
//      if (is_in_map({ i, j })) m[i][j].render(cam);
//    }
//  }
//}

//void Map::renderCursor(const Point& cam, const Point& mouse, SDL_Rect * tile) {
//  if (mouse_map.count(mouse + cam)) {
//    Point vert_offset{ 0, mouse_map[mouse + cam]->altitude*TILE_GROUND_HEIGHT_OFFSET };
//    gSpriteSheetTexture.render(mouse_map[mouse + cam]->screen_coords - vert_offset - cam, tile);
//
//    //re-render cubes that should be in the foreground
//    Point p = mouse_map[mouse + cam]->base_coords;
//    for (int j = p.y + 1; j < h(); ++j) {
//      if (is_in_map({ p.x, j })) m[p.x][j].render(cam);
//    }
//    for (int i = p.x + 1; i < w(); ++i) {
//      for (int j = 0; j < h(); ++j) {
//        if (is_in_map({ i, j })) m[i][j].render(cam);
//      }
//    }
//  }
//  //if(mouse_map.count(mouse)) std::cout << mouse_map[mouse]->base_coords << "   " << mouse_map[mouse]->flat_coords << std::endl;
//}

void Map::renderOnTop(const Point& tile, SDL_Rect * clip) {
  if (is_in_map(tile))
    gSpriteSheetTexture.render(m[tile.x][tile.y].s.back().screen_coords -
      Point{ 0, m[tile.x][tile.y].s.back().altitude*int(TILE_H / 4) }, clip);
}

void Map::render(const Point& cam) {
  for (int i = 0; i < w(); i++) {
    for (int j = 0; j < h(); j++) {
      m[i][j].render(cam);
    }
  }
}

// pathfinding
std::vector<Point> Map::get_4neighbors(const Point& p) {
  std::vector<Point> vn;
  if (is_in_map({ p.x - 1, p.y })) vn.push_back(Point{ p.x - 1, p.y });
  if (is_in_map({ p.x + 1, p.y })) vn.push_back(Point{ p.x + 1, p.y });
  if (is_in_map({ p.x, p.y - 1 })) vn.push_back(Point{ p.x, p.y - 1 });
  if (is_in_map({ p.x, p.y + 1 })) vn.push_back(Point{ p.x, p.y + 1 });
  return vn;
}

int Map::Astar_cost(const Point& a, const Point& b) {
  return 1;
};

int Map::Astar_heuristic(const Point& a, const Point& b) {
  return abs(a.x - b.x) + abs(a.y - b.y);
};

std::vector<Point> Map::findPath_Astar(const Point& origin, const Point& destination) {
  std::vector<Point> path;
  if (!is_in_map(destination)) return path;

  std::unordered_map<Point, Point, PointHasher> came_from;
  std::unordered_map<Point, int, PointHasher> cost_so_far;
  came_from[origin] = origin;
  cost_so_far[origin] = 0;

  std::priority_queue<Astar_candidate, std::vector<Astar_candidate>, Astar_candidate_comparator> frontier;
  frontier.emplace(origin, 0);
  while (!frontier.empty()) {
    auto current = frontier.top().tile; frontier.pop();

    if (current == destination) break;

    auto ns = get_4neighbors(current);
    for (auto neighbor : ns) {
      int new_cost = cost_so_far[current] + Astar_cost(current, neighbor);
      if (!cost_so_far.count(neighbor) || new_cost < cost_so_far[neighbor]) {
        cost_so_far[neighbor] = new_cost;
        int priority = new_cost + Astar_heuristic(neighbor, destination);
        frontier.emplace(neighbor, priority);
        came_from[neighbor] = current;
      }
    }
  }

  Point current_p = destination;
  path.push_back(current_p);
  while (current_p != origin) {
    current_p = came_from[current_p];
    path.push_back(current_p);
  }
  std::reverse(path.begin(), path.end());
  return path;
}

// tile detection
Point Map::findHighest(const Point & tile) {
  Point high(tile);
  if (tile.x >= 0 && tile.y >= 0)       // I quadrant
    while (high.x > 0 && high.y > 0) --high;
  else if (tile.x < 0 && tile.y >= 0)   // II quadrant
    while (high.x < 0) ++high;
  else if (tile.x < 0 && tile.y < 0)    // III quadrant
    while (high.x < 0 || high.y < 0) ++high;
  else if (tile.x >= 0 && tile.y < 0)   // IV quadrant
    while (high.y < 0) ++high;
  else {
    //wtf
  }
  return high;
}

Point Map::findLowest(const Point & tile) {
  Point low(tile);
  while (low.x < w() - 1 && low.y < h() - 1) ++low;
  return low;
}

Point Map::mouse2basetile(const Point& mouse) {
  Point TileCandidate{ -1,-1 };

  int x = int(mouse.x);
  int y = int(mouse.y - TILE_H / 4);
  int yh = int(mouse.y);
  double xf = (x - OFFSET_X) / TILE_W + (y - OFFSET_Y) / (TILE_H / 2.) - 1;
  double yf = -(x - OFFSET_X) / TILE_W + (y - OFFSET_Y) / (TILE_H / 2.);
  double xfh = (x - OFFSET_X) / TILE_W + (yh - OFFSET_Y) / (TILE_H / 2.) - 1;
  double yfh = -(x - OFFSET_X) / TILE_W + (yh - OFFSET_Y) / (TILE_H / 2.);

  Point tile0{ myrounder(xf), myrounder(yf) },    // _0 refers to the altitude level
    tile1{ myrounder(xfh), myrounder(yfh) },
    low, high;

  if (isSafeTile(tile0) && isSafeTile(tile1)) {

    high = findHighest(tile0);
    low = findLowest(tile0);
    Point candidate0 = low;
    bool is_full = false;
    while (candidate0 != high - Point{ 1,1 }) {
      if (2 * (candidate0.y - tile0.y) - m[candidate0.x][candidate0.y].altitude() == 0) {
        is_full = true;
        break;
      }
//      std::cout << "[full] c" << candidate0 <<
//        " d" << candidate0.y - high.y <<
//        " a" << m[candidate0.x][candidate0.y].altitude() << std::endl;
      --candidate0;
    }

//    std::cout
//      << " s" << tile0 << " "
//      << " l" << low << " "
//      << " h" << high << " "
//      << "  can" << candidate0 << " f" << is_full << " "
//      << std::endl;

    high = findHighest(tile1);
    low = findLowest(tile1);
    Point candidate1 = low;
    bool is_half = false;
    while (candidate1 != high - Point{ 1,1 }) {
      if (2 * (candidate1.y - tile1.y) - m[candidate1.x][candidate1.y].altitude() == -1) {
        is_half = true;
        break;
      }
//      std::cout << "[half] c" << candidate1 <<
//        " p" << 2 * (candidate1.y - tile1.y) - m[candidate1.x][candidate1.y].altitude() <<
//        " a" << m[candidate1.x][candidate1.y].altitude() << 
//        std::endl;
      --candidate1;
    }

//        if (screen.x >= 0 && screen.x < w() && screen.y >= 0 && screen.y < h()) // safe access condition
//          std::cout << "alt " << m[screen.x][screen.y].altitude() << " ";
//        std::cout << std::endl;
//        std::cout 
//          << "sh" << tile1 << " "
//          << "lh" << low << " "
//          << "hh" << high << " "
//          << "canh" << candidate1 << " h" << is_half << " "
//          << std::endl;
//        if (screenh.x >= 0 && screenh.x < w() && screenh.y >= 0 && screenh.y < h()) // safe access condition
//          std::cout << "alth " << m[screenh.x][screenh.y].altitude() << " ";

    if (is_full && !is_half) {                    // FULL tile
//      std::cout << "FULL tile" << std::endl;
      TileCandidate = candidate0;
    }
    else if (!is_full && is_half) {               // HALF tile
//      std::cout << "HALF tile" << std::endl;
      TileCandidate = candidate1;
    }
    else if (is_full && is_half) {                // HALF tile
//      std::cout << "HALF tile" << std::endl;
      TileCandidate = candidate1;
    }
    else if (!is_full && !is_half) {              // HALF tile
//      std::cout << "NO tile" << std::endl;
    }
    else {
      // wtf
    }
//    std::cout << "\n basetile " << TileCandidate << std::endl;

  }

  return TileCandidate;
}
