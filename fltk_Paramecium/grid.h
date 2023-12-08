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

		std::vector<std::vector<int>> status;
		std::vector<std::vector<int>> barrier;
		std::vector<std::vector<Node>> nodes;
		size_t grid_w, grid_h;

		std::list<std::pair<int, int>> orig;
		std::list<std::pair<int, int>> queue;
		std::list<std::pair<int, int>> updated;
		int d_min = 10, d_max = 10;

		bool show_border = false;
		bool step_flag = false, redraw_flag = true;

		Grid(size_t w_, size_t h_) :grid_w(w_), grid_h(h_),
			status(w_, std::vector<int>(h_, 0)), barrier(w_, std::vector<int>(h_, 0)),
			nodes(w_, std::vector<Node>(h_)), d_max(w_ * 2) {
		}

		// A single step for BFS
		void bfs_step() {
			assert(!queue.empty());
			auto [cx, cy] = queue.front();
			if (cx < grid_w - 1) {
				renew_distance(cx, cy, cx + 1, cy);
				push_pos(cx + 1, cy, cx, cy);
			}
			if (cx > 0) {
				renew_distance(cx, cy, cx - 1, cy);
				push_pos(cx - 1, cy, cx, cy);
			}
			if (cy < grid_h - 1) {
				renew_distance(cx, cy, cx, cy + 1);
				push_pos(cx, cy + 1, cx, cy);
			}
			if (cy > 0) {
				renew_distance(cx, cy, cx, cy - 1);
				push_pos(cx, cy - 1, cx, cy);
			}
			if (cx < grid_w - 1 && cy < grid_h - 1) {
				renew_distance(cx, cy, cx + 1, cy + 1);
			}
			if (cx > 0 && cy < grid_h - 1) {
				renew_distance(cx, cy, cx - 1, cy + 1);
			}
			if (cx < grid_w - 1 && cy > 0) {
				renew_distance(cx, cy, cx + 1, cy - 1);
			}
			if (cx > 0 && cy > 0) {
				renew_distance(cx, cy, cx - 1, cy - 1);
			}
			queue.pop_front();
			//print_flow();
		}

		void push_pos(int px, int py, int fx, int fy) {
			if (!status[px][py] && barrier[px][py] == 0) {
				status[px][py] = 1;
				nodes[px][py].distance = nodes[fx][fy].distance + 10;
				//d_max = std::max(d_max, nodes[px][py].distance);
				if (nodes[px][py].distance > 1.5 * d_max) {
					d_max = nodes[px][py].distance;
					redraw_flag = true;
				}
				nodes[px][py].fx = fx, nodes[px][py].fy = fy;
				queue.push_back({ px,py });
				updated.push_back({ px,py });
			}
		};

		void renew_distance(int cx, int cy, int fx, int fy) {
			if (status[fx][fy] && barrier[fx][fy] == 0) {
				int new_d = nodes[fx][fy].distance;
				if (cx == fx || cy == fy) {
					new_d += 10;
				} else if (barrier[fx][cy] == 0 || barrier[cx][fy] == 0) {
					new_d += 14;
				} else {
					return;
				}
				if (new_d < nodes[cx][cy].distance) {
					nodes[cx][cy].distance = new_d;
					nodes[cx][cy].fx = fx, nodes[cx][cy].fy = fy;
					updated.push_back({ cx,cy });
				}
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

		void clear_status() {
			status = std::vector<std::vector<int>>(grid_w, std::vector<int>(grid_h));
			redraw_flag = true;
			queue = orig;
			d_max = grid_w * 5;
			for (auto& p : orig) {
				int px = p.first, py = p.second;
				nodes[px][py] = { 10,-1,-1 };
				barrier[px][py] = 0;
				status[px][py] = 1;
			}
		}
	};
} // namespace grid
