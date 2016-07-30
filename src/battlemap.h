// Copyright 2016 Marco Di Cristina, Alessandro Fabbri
#pragma once

//////// CUBE
struct Cube {
  int altitude;                   //0: base level, 1: half-tile height, 2: full height
  int sprite_index;
  int traversability;             // 0, ... , N to achieve the slow/haste effects of terrain, N is for wall
  Point base_coords, flat_coords, screen_coords;
  int stack_index;

  Cube() : altitude{}, sprite_index{}, traversability{} {}
  Cube(int _h, int _si, int _traversability) : altitude(_h), sprite_index(_si), traversability(_traversability) {}
  Cube(int _stack_index, Point _base_coords, Point _flat_coords, Point _scr_coords, std::string entry);

  void render(const Point& cam);
};

//////// CUBESTACK
class CubeStack {
public:
  std::vector<Cube> s;
  Point coords;

  CubeStack() {}
  CubeStack(Point _coords, std::string stackentry);

  void render(const Point& cam);
  int altitude();
};

//////// Map object (maybe a name too common...add namespace?)
class Map {
  std::vector<std::vector<CubeStack>> m;

public:
  Point north, south, west, east;

  // Constructor
  Map(std::string filename);

  // Utilities
  inline int w();
  inline int h();
  inline bool is_in_map(const Point& p);
  inline bool isSafeTile(const Point & tile);

  // Tile detection
  Point findLowest(const Point & tile);
  Point findHighest(const Point & tile);
  Point mouse2basetile(const Point& mouse);

  // Rendering
  void render(const Point& cam);
//  void renderSprite(const Point& cam, const Point& tile_index, SDL_Rect * tile);
//  void renderCursor(const Point& cam, const Point& mouse, SDL_Rect * tile);
  void renderOnTop(const Point& tileIndices, SDL_Rect * tile);

  // Pathfinding
  std::vector<Point> get_4neighbors(const Point& p);
  int Astar_cost(const Point& a, const Point& b);
  int Astar_heuristic(const Point& a, const Point& b);

  struct Astar_candidate {
    Point tile; int priority;
    Astar_candidate(Point _tile, int _priority) : tile(_tile), priority(_priority) {}
  };

  struct Astar_candidate_comparator {
    bool operator() (const Astar_candidate& a, const Astar_candidate& b) const {
      return a.priority > b.priority;
    }
  };

  std::vector<Point> findPath_Astar(const Point& origin, const Point& destination);
};















