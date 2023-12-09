#pragma once

/*
*  grid.h
* 
*  A basic grid with information on origin points, barriers, distances, etc.
*  Constructing information via BFS
*/

#include "drawings.h"

namespace grid {

	template <typename T>
	bool operator<(const std::pair<T, T>& a, const std::pair<T, T>& b) {
		return a.first < b.first || a.first == b.first && a.second < b.second;
	}

	class Grid {
	public:
		struct Node {
			int distance, fx, fy;
		};

		struct Intermediate {
			int stage, cx, cy, f;
		};

		const int FATHER[9][2] = { {0,0},{1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1} };

		std::vector<std::vector<int>> status;
		std::vector<std::vector<int>> expected;
		std::vector<std::vector<int>> barrier;
		std::vector<std::vector<Node>> nodes;
		size_t grid_w, grid_h;

		std::list<std::pair<int, int>> orig;
		std::list<Intermediate> queue;
		std::list<std::pair<int, int>> updated;
		int d_min = 1, d_max = 10;

		bool show_border = false;
		bool step_flag = false, redraw_flag = true;

		// for test only
		int count = 0;

		Grid(size_t w_, size_t h_) :grid_w(w_), grid_h(h_),
			status(w_, std::vector<int>(h_, 0)), expected(w_, std::vector<int>(h_, 0)), barrier(w_, std::vector<int>(h_, 0)),
			nodes(w_, std::vector<Node>(h_)), d_max(w_ / 2) {

			// for test only
			nodes[0][0] = { d_min,-1,-1 };
			queue.push_back({ 0,0,0,0 });
			orig.push_back({ 0,0 });
		}

		// A single step for BFS
		void bfs_step() {
#ifdef _DEBUG
			//std::cout << ++count << std::endl;
#endif
			assert(!queue.empty());
			int cx = queue.front().cx, cy = queue.front().cy;
			if (status[cx][cy]) {
				queue.pop_front();
				return;
			}
			if (queue.front().stage != 0) {
				// Not finished
				queue.push_back({ queue.front().stage - 1,cx,cy,queue.front().f });
				queue.pop_front();
				return;
			}
			// Finished
			status[cx][cy] = 1;
			updated.push_back({ cx,cy });
			int f = queue.front().f, fx = cx + FATHER[f][0], fy = cy + FATHER[f][1];
			if (f != 0) {
				// Not origin
				int new_d = nodes[fx][fy].distance + (f % 2 ? 2 : 3);
				nodes[cx][cy] = { new_d,fx,fy };
				if (new_d > 1.5 * d_max) {
					d_max = new_d;
					redraw_flag = true;
				}
			}
			if (cx < grid_w - 1 && cy < grid_h - 1) {
				push_pos(cx + 1, cy + 1, 6);
			}
			if (cx > 0 && cy < grid_h - 1) {
				push_pos(cx - 1, cy + 1, 8);
			}
			if (cx < grid_w - 1 && cy > 0) {
				push_pos(cx + 1, cy - 1, 4);
			}
			if (cx > 0 && cy > 0) {
				push_pos(cx - 1, cy - 1, 2);
			}
			if (cx < grid_w - 1) {
				push_pos(cx + 1, cy, 5);
			}
			if (cx > 0) {
				push_pos(cx - 1, cy, 1);
			}
			if (cy < grid_h - 1) {
				push_pos(cx, cy + 1, 7);
			}
			if (cy > 0) {
				push_pos(cx, cy - 1, 3);
			}
			queue.pop_front();
		}

		void push_pos(int cx, int cy, int f) {
			if (status[cx][cy] == 0 && barrier[cx][cy] == 0) {
				int fx = cx + FATHER[f][0], fy = cy + FATHER[f][1];
				if (!(f % 2) && barrier[fx][cy] && barrier[cx][fy]) {
					return;
				}
				int expected_d = nodes[fx][fy].distance + (f % 2 ? 2 : 3);
				if (expected[cx][cy] != 0 && expected[cx][cy] <= expected_d) {
					return;
				}
				queue.push_back({ (f % 2 ? 1 : 2),cx,cy,f });
				expected[cx][cy] = expected_d;
			}
		};

		void set_orig(std::list<std::pair<int, int>>&& o) {
			orig = std::forward<std::list<std::pair<int, int>>>(o);
			clear_status();
			for (auto it = orig.begin(); it != orig.end();) {
				if (barrier[it->first][it->second] != 0) {
					it = orig.erase(it);
				} else {
					it++;
				}
			}
		}

		void set_barrier(std::vector<std::vector<int>>&& b) {
			barrier = std::forward<std::vector<std::vector<int>>>(b);
			clear_status();
		}

		void add_barrier(const std::vector<std::pair<int, int>>& b) {
			for (const auto& i : b) {
				barrier[i.first][i.second] = 1;
			}
		}

		void add_barrier(int x, int y) { barrier[x][y] = 1; }

		void remove_barrier(int x, int y) { barrier[x][y] = 0; }

		void clear_status() {
			status = std::vector<std::vector<int>>(grid_w, std::vector<int>(grid_h));
			expected = std::vector<std::vector<int>>(grid_w, std::vector<int>(grid_h));
			redraw_flag = true;
			d_max = grid_w / 2;
			for (auto& p : orig) {
				int px = p.first, py = p.second;
				nodes[px][py] = { d_min,-1,-1 };
				barrier[px][py] = 0;
				queue.push_back({ 0,px,py,0 });
			}
		}

		void print_flow() {
			size_t x = 0, y = 0;
			for (auto& col : nodes) {
				y = 0;
				for (auto& i : col) {
					if (barrier[x][y] == 0) {
						//std::cout << i.distance << "\t";
						if (i.fx == x) {
							std::cout << "- ";
						} else if (i.fy == y) {
							std::cout << "| ";
						} else if (i.fx > x && i.fy > y || i.fx < x && i.fy < y) {
							std::cout << "\\ ";
						} else {
							std::cout << "/ ";
						}
					} else {
						if (barrier[x][y]) {
							std::cout << "x ";
						} else {
							std::cout << "  ";
						}
					}
					y++;
				}
				std::cout << std::endl;
				x++;
			}
			std::cout << std::endl;
		}

		void print_dist() {
			for (auto& col : nodes) {
				for (auto& i : col) {
					if (i.distance > 0) {
						std::cout << i.distance << "\t";
					} else {
						std::cout << "  " << "\t";
					}
				}
				std::cout << std::endl;
			}
			std::cout << std::endl;
		}
	};
} // namespace grid
