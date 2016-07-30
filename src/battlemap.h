#pragma once
#include <unordered_map>
//////// Cube object
// 
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

//////// Cube stack object
// 
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
//
class Map {
  //std::vector<MapLayer> m;
  std::vector<std::vector<CubeStack>> m;
public:
  std::unordered_map<Point, Cube*, PointHasher> mouse_map;
  Point north, south, west, east;

  // Constructor
  Map(std::string filename);

  // Utilities
  int w() { return m.size(); }
  int h() { return m[0].size(); }
  bool is_in_map(const Point& p) {
    return p.x >= 0 && p.x < w() && p.y >= 0 && p.y < h();
  }
  bool isSafeTile(const Point & tile);

  // Geometry
  Point Map::findLowest(const Point & tile);
  Point Map::findHighest(const Point & tile);
  Point mouse2basetile(const Point& mouse);
  Point mouse2tile_piuficodiquellodibio(Point mouse);

  ////// Rendering
  void render(const Point& cam);
  void renderSprite(const Point& cam, const Point& tile_index, SDL_Rect * tile);
  void renderCursor(const Point& cam, const Point& mouse, SDL_Rect * tile);
  void renderOnTop(const Point& tileIndices, SDL_Rect * tile);

  // A* algo
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















