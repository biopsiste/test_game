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

//Scene sprites
extern SDL_Rect gSpriteClips[], cursorSprite, highlighterSprite, HighCursorSprite, LowCursorSprite;
extern LTexture gSpriteSheetTexture;

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


CubeStack::CubeStack(Point _coords, std::string stackentry) : coords(_coords) {
  std::istringstream istr(stackentry);
  std::string token;
  int i = 0;
  while(std::getline(istr, token, '|')) {
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


Map::Map(std::string filename) {
  std::ifstream ifi(filename);
  std::string line, stackentry; int w, h;
  std::getline(ifi, line); //ignore header
  ifi >> w >> h; //std::cout << w << " " << h << std::endl;
  std::getline(ifi, line); //ignore w h
  m.assign(w, std::vector<CubeStack>(h));
  int i = 0;
  while (std::getline(ifi, line)) {
    //std::cout << line << std::endl; system("pause");
    std::istringstream istr(line);
    int j = 0;
    while (istr >> stackentry) {
      //std::cout << stackentry << std::endl;
      //std::cout << i << " " << j << std::endl;
      m[i][j] = CubeStack({ i,j }, stackentry);
      j++;
    }
    i++;
  }

  //build mouse_map
  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      auto upper_left = m[i][j].s.back().screen_coords;
      for(int xx = upper_left.x; xx < upper_left.x + TILE_W; ++xx) {
        for(int yy = upper_left.y; yy < upper_left.y + TILE_H; ++yy) {
          int y = int(yy - TILE_H / 2. + m[i][j].s.back().altitude*TILE_GROUND_HEIGHT_OFFSET);
          double dI = (xx - OFFSET_X) / TILE_W + (y - OFFSET_Y) / (TILE_H / 2.); if(dI > 0) dI--; auto I = lrint(dI);
          double dJ = -(xx - OFFSET_X) / TILE_W + (y - OFFSET_Y) / (TILE_H / 2.); auto J = lrint(dJ);
          if(m[i][j].s.back().flat_coords == Point{ I,J }) mouse_map[{xx, yy}] = &m[i][j].s.back();
          else if(yy >= upper_left.y + TILE_H - (m[i][j].s.back().altitude + 1)*TILE_GROUND_HEIGHT_OFFSET) mouse_map.erase({ xx, yy });
        }
      }
    }
  }
  std::cout << "mouse_map size " << mouse_map.size() << std::endl;
  north = tile2screen(Point{ 0, 0 });
  south = tile2screen(Point{ this->w() - 1, this->h() - 1 });
  west = tile2screen(Point{ 0,this->h() - 1 });
  east = tile2screen(Point{ this->w() - 1 , 0 });

}

Point Map::mouse2basetile(const Point& mouse) {
  if (mouse_map.count(mouse)) return mouse_map[mouse]->base_coords;
  return{ -999,-999 };
}

void Map::renderSprite(const Point& cam, const Point& tile_index, SDL_Rect * tile) {
  Point vert_offset{ 0, m[tile_index.x][tile_index.y].s.back().altitude*TILE_GROUND_HEIGHT_OFFSET };
  gSpriteSheetTexture.render(m[tile_index.x][tile_index.y].s.back().screen_coords - vert_offset - cam, tile);

  //re-render cubes that should be in the foreground
  for (int j = tile_index.y + 1; j < h(); ++j) {
    if (is_in_map({ tile_index.x, j })) m[tile_index.x][j].render(cam);
  }
  for (int i = tile_index.x + 1; i < w(); ++i) {
    for (int j = 0; j < h(); ++j) {
      if (is_in_map({ i, j })) m[i][j].render(cam);
    }
  }
}

void Map::renderCursor(const Point& cam, const Point& mouse, SDL_Rect * tile) {
  if (mouse_map.count(mouse + cam)) {
    Point vert_offset{ 0, mouse_map[mouse + cam]->altitude*TILE_GROUND_HEIGHT_OFFSET };
    gSpriteSheetTexture.render(mouse_map[mouse + cam]->screen_coords - vert_offset - cam, tile);

    //re-render cubes that should be in the foreground
    Point p = mouse_map[mouse + cam]->base_coords;
    for (int j = p.y + 1; j < h(); ++j) {
      if (is_in_map({ p.x, j })) m[p.x][j].render(cam);
    }
    for (int i = p.x + 1; i < w(); ++i) {
      for (int j = 0; j < h(); ++j) {
        if (is_in_map({ i, j })) m[i][j].render(cam);
      }
    }
  }
  //if(mouse_map.count(mouse)) std::cout << mouse_map[mouse]->base_coords << "   " << mouse_map[mouse]->flat_coords << std::endl;
}

void Map::render(const Point& cam) {
  for (int i = 0; i < w(); i++) {
    for (int j = 0; j < h(); j++) {
      m[i][j].render(cam);
    }
  }
}

std::vector<Point> Map::get_4neighbors(const Point& p) {
  std::vector<Point> vn;
  if (is_in_map({ p.x - 1, p.y })) vn.push_back(Point{ p.x - 1, p.y });
  if (is_in_map({ p.x + 1, p.y })) vn.push_back(Point{ p.x + 1, p.y });
  if (is_in_map({ p.x, p.y - 1 })) vn.push_back(Point{ p.x, p.y - 1 });
  if (is_in_map({ p.x, p.y + 1 })) vn.push_back(Point{ p.x, p.y + 1 });
  return vn;
}

//cost function, always 1 for now
int Map::Astar_cost(const Point& a, const Point& b) {
  return 1;
};

//manhattan heuristic
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

