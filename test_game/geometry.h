#pragma once
#include <vector>

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
	bool operator==(const Point& b) const {
		return x == b.x && y == b.y;
	}
	bool operator!=(const Point& b) const {
		return !(*this == b);
	}
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

inline bool is_in_map(const Point& p) {
	return p.x >= 0 && p.x < MAP_W && p.y >= 0 && p.y < MAP_H;
}

inline bool is_in_map(const int& px, const int& py) {
	return px >= 0 && px < MAP_W && py >= 0 && py < MAP_H;
}

inline bool are_4neighbors(const Point& a, const Point& b) {
	return (a.x == b.x && abs(a.y - b.y) == 1) || (a.y == b.y && abs(a.x - b.x) == 1);
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

struct TileCandidate {
	int x, y, counter;
	TileCandidate(int _x, int _y, int _c) : x(_x), y(_y), counter(_c) {}
};

std::vector<Point> findPath(int* map, Point& origin, Point& destination) {
	std::vector<TileCandidate> main_list;
	main_list.emplace_back(destination.x, destination.y, 0);

	int xx, yy, cc;
	int i = 0;

	bool done = false;
	while(!done) {
		cc = main_list[i].counter + 1;

		std::vector<TileCandidate> local4 = {
			{ main_list[i].x - 1, main_list[i].y, cc },
			{ main_list[i].x + 1, main_list[i].y, cc },
			{ main_list[i].x, main_list[i].y - 1, cc },
			{ main_list[i].x, main_list[i].y + 1, cc }
		};
		bool ar_ok_to_push[] = { true, true, true, true };

		for(int j = 0; j < 4; ++j) {
			if(is_in_map(local4[j].x, local4[j].y)) {
				for(auto& p : main_list) if(p.x == local4[j].x && p.y == local4[j].y && p.counter <= cc) { ar_ok_to_push[j] = false; break; }
			} else ar_ok_to_push[j] = false;
		}

		for(int j = 0; j < 4; ++j) if(ar_ok_to_push[j]) {
			main_list.emplace_back(local4[j].x, local4[j].y, cc);
			if(origin.x == local4[j].x && origin.y == local4[j].y) { done = true; break; }
		}

		//cout << i << endl;
		i++;
	}

	std::vector<Point> path;

	int last_c = main_list.back().counter;
	path.push_back(Point{ main_list.back().x, main_list.back().y });

	for(int j = main_list.size() - 1; j >= 1; --j) {
		if(main_list[j].counter < last_c && are_4neighbors(path.back(), Point{ main_list[j].x, main_list[j].y })) {
			last_c = main_list[j].counter;
			path.push_back(Point{ main_list[j].x, main_list[j].y });
		}
	}

	path.push_back(Point{ main_list.front().x, main_list.front().y });

	return path;
}

