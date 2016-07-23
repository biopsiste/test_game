#pragma once
#include <vector>
#include <queue>
#include <unordered_map>

//Geometry constants
#define SCREEN_WIDTH              1280
#define SCREEN_HEIGHT             672

#define OFFSET_X                  400.
#define OFFSET_Y                  30.

#define TILE_W                    64.
#define TILE_H                    64.
#define TILE_GROUND_HEIGHT_OFFSET 16.

#define MAP_W                     30
#define MAP_H                     20

struct Point {
	int x, y;

	uint32_t hash() const {
		//fast
		return x * 1812433253 + y;

		//a bit slower, but no collision if x, y are uint32_t
		//return x >= y ?
		//	uint64_t(x) * uint64_t(x) + uint64_t(x) + uint64_t(y) :
		//	uint64_t(x) + uint64_t(y) * uint64_t(y);
	}
	bool operator==(const Point& b) const {
		return x == b.x && y == b.y;
	}
	bool operator!=(const Point& b) const {
		return !(*this == b);
	}
  Point& operator+=(const Point& b) {
    this->x += b.x; this->y += b.y;
    return *this;
  }
  friend Point operator+(Point th, const Point& b) {
    th += b;
    return th;
  }
  Point& operator-=(const Point& b) {
    this->x -= b.x; this->y -= b.y;
    return *this;
  }
  friend Point operator-(Point th, const Point& b) {
    th -= b;
    return th;
  }

};
struct PointHasher {
	uint32_t operator()(const Point& p) const {
		return p.hash();
	}
};

Point tile2screen(int i, int j) {
  int x = int((i - j) * TILE_W / 2 + OFFSET_X /*+ 0.5*/);
  int y = int((i + j) * TILE_H / 4 + OFFSET_Y /*+ 0.5*/);
  return Point{ x, y };
}
Point tile2screen(const Point& tile) {
  return tile2screen(tile.x, tile.y);
}

Point mouse2tile(const Point& mouse) {
  int x = int( mouse.x );
  int y = int( mouse.y - TILE_H / 2. + TILE_GROUND_HEIGHT_OFFSET);
  int I = int( (x - OFFSET_X) / TILE_W + (y - OFFSET_Y) / (TILE_H / 2.) - 1 + 0.5 );
  int J = int( -(x - OFFSET_X) / TILE_W + (y - OFFSET_Y) / (TILE_H / 2.) + 0.5 );
  return Point{I, J};
}
Point mouse2tile_high(const Point& mouse) {
  int x = int(mouse.x);
  int y = int(mouse.y - TILE_H / 2. + 2*TILE_GROUND_HEIGHT_OFFSET);
  int I = int((x - OFFSET_X) / TILE_W + (y - OFFSET_Y) / (TILE_H / 2.) - 1 + 0.5);
  int J = int(-(x - OFFSET_X) / TILE_W + (y - OFFSET_Y) / (TILE_H / 2.) + 0.5);
  return Point{ I, J };
}

inline bool is_in_map(const int& px, const int& py) {
	return px >= 0 && px < MAP_W && py >= 0 && py < MAP_H;
}
inline bool is_in_map(const Point& p) {
	return is_in_map(p.x, p.y);
}

inline bool are_4neighbors(const Point& a, const Point& b) {
	return (a.x == b.x && abs(a.y - b.y) == 1) || (a.y == b.y && abs(a.x - b.x) == 1);
}

std::vector<Point> get_4neighbors(const Point& p) {
	std::vector<Point> vn;
	if(is_in_map(p.x - 1, p.y)) vn.push_back(Point{ p.x - 1, p.y });
	if(is_in_map(p.x + 1, p.y)) vn.push_back(Point{ p.x + 1, p.y });
	if(is_in_map(p.x, p.y - 1)) vn.push_back(Point{ p.x, p.y - 1 });
	if(is_in_map(p.x, p.y + 1)) vn.push_back(Point{ p.x, p.y + 1 });
	return vn;
}

struct Path {
	std::vector<Point> path;

	Path(std::vector<Point>& p) : path(p) {}

	void add(const Point& p) {
		path.push_back(p);
	}
	void add(const int& px, const int& py) {
		path.push_back(Point{ px, py });
	}

	size_t length() {
		return path.size();
	}
};

//cost function, always 1 for now
int Astar_cost(const Point& a, const Point& b) {
	return 1;
};

//manhattan heuristic
int Astar_heuristic(const Point& a, const Point& b) {
	return abs(a.x - b.x) + abs(a.y - b.y);
};

struct Astar_candidate {
	Point tile; int priority;
	Astar_candidate(Point _tile, int _priority) : tile(_tile), priority(_priority) {}
};

struct Astar_candidate_comparator {
	bool operator() (const Astar_candidate& a, const Astar_candidate& b) const {
		return a.priority > b.priority;
	}
};

std::vector<Point> findPath_Astar(/*int* map,*/const Point& origin, const Point& destination) {
	std::vector<Point> path;
  if(!is_in_map(destination)) return path;

	std::unordered_map<Point, Point, PointHasher> came_from;
	std::unordered_map<Point, int, PointHasher> cost_so_far;
	came_from[origin] = origin;
	cost_so_far[origin] = 0;

	std::priority_queue<Astar_candidate, std::vector<Astar_candidate>, Astar_candidate_comparator> frontier;
	frontier.emplace(origin, 0);
	while(!frontier.empty()) {
		auto current = frontier.top().tile; frontier.pop();

		if(current == destination) break;

		auto ns = get_4neighbors(current);
		for(auto neighbor : ns) {
			int new_cost = cost_so_far[current] + Astar_cost(current, neighbor);
			if(!cost_so_far.count(neighbor) || new_cost < cost_so_far[neighbor]) {
				cost_so_far[neighbor] = new_cost;
				int priority = new_cost + Astar_heuristic(neighbor, destination);
				frontier.emplace(neighbor, priority);
				came_from[neighbor] = current;
			}
		}
	}

	Point current_p = destination;
	path.push_back(current_p);
	while(current_p != origin) {
		current_p = came_from[current_p];
		path.push_back(current_p);
	}
	std::reverse(path.begin(), path.end());
	return path;
}
