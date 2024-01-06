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

	std::default_random_engine rand_e;

	class Grid {
	public:
		struct Intermediate {
			int cx, cy, f;
		};

		const int FATHER[9][2] = { {0,0},{1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1} };

		std::vector<std::vector<int>> status;
		std::vector<std::vector<int>> distance;
		std::vector<std::vector<int>> barrier;
		std::vector<std::vector<int>> father;
		size_t grid_w, grid_h;

		std::list<std::pair<int, int>> orig;
		std::deque<std::list<Intermediate>> queue;
		std::list<std::pair<int, int>> updated;

		const int len_straight = 2, len_diagonal = 3;
		int d_min = 1, d_max = 10;
		int curr_d = d_min;

		bool show_border = false, stabled = false, finished = false;
		bool step_flag = false, redraw_flag = true, flush_flag = true;

		// for test only
		int count = 0;

		Grid(size_t w_, size_t h_) :grid_w(w_), grid_h(h_),
			status(w_, std::vector<int>(h_, 0)), distance(w_, std::vector<int>(h_, 0)), barrier(w_, std::vector<int>(h_, 0)),
			father(w_, std::vector<int>(h_)), d_max(w_ / 2) {

			// for test only
			father[0][0] = 0;
			distance[0][0] = d_min;
			queue = std::deque<std::list<Intermediate>>(1);
			queue.front().push_back({ 0,0,0 });
			orig.push_back({ 0,0 });
		}

		// A single step for BFS
		void bfs_step() {
#ifdef _DEBUG
			//std::cout << ++count << std::endl;
#endif
			assert(!queue.empty());
			while (queue.front().empty()) {
				curr_d++;
				queue.pop_front();
				if (queue.empty()) {
					return;
				}
			}
			auto& q = queue.front();
			int cx = q.front().cx, cy = q.front().cy;
			if (status[cx][cy]) {
				q.pop_front();
				return;
			}
			// Finished
			status[cx][cy] = 1;
			updated.push_back({ cx,cy });
			int f = q.front().f;
			if (f != 0) {
				// Not origin
				father[cx][cy] = f;
				if (distance[cx][cy] > 1.5 * d_max) {
					d_max = distance[cx][cy];
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
			q.pop_front();
		}

		void push_pos(int cx, int cy, int f) {
			if (status[cx][cy] == 0 && barrier[cx][cy] == 0) {
				if (!(f % 2) && barrier[size_t(cx) + FATHER[f][0]][cy] && barrier[cx][size_t(cy) + FATHER[f][1]]) {
					return;
				}
				int len = f % 2 ? len_straight : len_diagonal, expected_d = curr_d + len;
				if (distance[cx][cy] != 0 && distance[cx][cy] <= expected_d) {
					return;
				}
				while (queue.size() <= len) {
					queue.push_back({});
				}
				queue[len].push_back({ cx,cy,f });
				distance[cx][cy] = expected_d;
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

		void clear_barrier() { barrier = std::vector<std::vector<int>>(grid_w, std::vector<int>(grid_h, 0)); }

		void clear_orig() { orig.clear(); }

		void clear_status() {
			status = std::vector<std::vector<int>>(grid_w, std::vector<int>(grid_h));
			distance = std::vector<std::vector<int>>(grid_w, std::vector<int>(grid_h));
			redraw_flag = true;
			finished = false;
			d_max = grid_w / 2;
			queue = std::deque<std::list<Intermediate>>(1);
			curr_d = d_min;
			for (auto& p : orig) {
				int px = p.first, py = p.second;
				distance[px][py] = d_min;
				father[px][py] = 0;
				barrier[px][py] = 0;
				queue.front().push_back({ px,py,0 });
			}
		}

		bool is_barrier(int grid_x, int grid_y) const {
			return grid_x >= grid_w || grid_x < 0 || grid_y >= grid_h || grid_y < 0 || barrier[grid_x][grid_y] != 0;
		}
		bool is_origin(int grid_x, int grid_y) const {
			return !is_barrier(grid_x, grid_y) && distance[grid_x][grid_y] == d_min;
		}
		int get_distance(int grid_x, int grid_y) const { return distance[grid_x][grid_y]; }

		void print_flow() {
			for (size_t j = 0; j < grid_h; j++) {
				for (size_t i = 0; i < grid_w; i++) {
					if (barrier[i][j]) {
						std::cout << "x ";
					} else if (status[i][j]) {
						if (i == 3 || i == 7) {
							std::cout << "- ";
						} else if (i == 1 || i == 5) {
							std::cout << "| ";
						} else if (i == 2 || i == 6) {
							std::cout << "\\ ";
						} else {
							std::cout << "/ ";
						}
					} else {
						std::cout << "  ";
					}
				}
				std::cout << std::endl;
			}
			std::cout << std::endl;
		}

		void print_dist() {
			for (size_t j = 0; j < grid_h; j++) {
				for (size_t i = 0; i < grid_w; i++) {
					if (barrier[i][j]) {
						std::cout << "x" << "\t";
					} else if (status[i][j]) {
						std::cout << distance[i][j] << "\t";
					} else {
						std::cout << " " << "\t";
					}
				}
				std::cout << std::endl;
			}
			std::cout << std::endl;
		}
	};
} // namespace grid
