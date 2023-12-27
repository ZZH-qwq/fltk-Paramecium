#pragma once

/*
 *  maze.h
 *
 *  Randomly generates maze and create the barrier
 */

namespace grid {

	class Maze {
    public:
        size_t grid_w, grid_h, maze_w = 0, maze_h = 0, unit_size = 5;
        std::uniform_int_distribution<int> rand_int;

        std::vector<std::vector<int>> walls;
        std::vector<std::vector<int>> visited;
        const int FATHER[4][2] = { {1, 0}, {0, 1}, {-1, 0}, {0, -1} };

        Fl_Button* gen_maze_bt = nullptr;
        Fl_Hor_Value_Slider* m_size_ip = nullptr;

        Maze(size_t w_, size_t h_) : grid_w(w_), grid_h(h_) {}

        void generate(int m_size = 0) {
            if (m_size) {
                unit_size = m_size;
            }
            maze_w = (grid_w - 1) / unit_size + 1, maze_h = (grid_h - 1) / unit_size + 1;
            visited = std::vector<std::vector<int>>(maze_w, std::vector<int>(maze_h));
            walls = std::vector<std::vector<int>>(maze_w * 2, std::vector<int>(maze_h * 2));
            // All set walls
            for (size_t i = 0; i < maze_w; i++) {
                for (size_t j = 0; j < maze_h; j++) {
                    walls[2 * i][2 * j + 1] = walls[2 * i + 1][2 * j] = walls[2 * i + 1][2 * j + 1] = 1;
                }
            }
            dfs(rand_int(rand_e) % maze_w, rand_int(rand_e) % maze_h);
#ifdef _DEBUG
            //print_maze();
#endif // _DEBUG
        }

        void dfs(int mx, int my) {
            visited[mx][my] = 1;
            while (!(is_visited(mx - 1, my) && is_visited(mx + 1, my) && is_visited(mx, my - 1) && is_visited(mx, my + 1))) {
                int dir;
                do {
                    dir = rand_int(rand_e) % 4;
                } while (is_visited(mx + FATHER[dir][0], my + FATHER[dir][1]));
                if (!out_bound(mx + FATHER[dir][0], my + FATHER[dir][1])) {
                    // Tear down the wall
                    walls[2 * mx + FATHER[dir][0]][2 * my + FATHER[dir][1]] = 0;
                    dfs(mx + FATHER[dir][0], my + FATHER[dir][1]);
                }
            }
        }

        std::vector<std::pair<int, int>> get_barriers() {
            std::vector<std::pair<int, int>> b;
            for (size_t i = 0; i < maze_w; i++) {
                for (size_t j = 0; j < maze_h; j++) {
                    int x = (i + 1) * unit_size - 1, y = (j + 1) * unit_size - 1;
                    if (walls[2 * i + 1][2 * j] && x < grid_w) {
                        int end = (j == maze_h - 1 ? grid_h : (j + 1) * unit_size);
                        for (int y = j * unit_size; y < end; y++) {
                            b.push_back({ x,y });
                        }
                    }
                    if (walls[2 * i][2 * j + 1] && y < grid_h) {
                        int end = (i == maze_w - 1 ? grid_w : (i + 1) * unit_size);
                        for (int x = i * unit_size; x < end; x++) {
                            b.push_back({ x,y });
                        }
                    }
                    if (x < grid_w && y < grid_h) {
                        b.push_back({ x,y });
                    }
                }
            }
            return b;
        }

        std::list<std::pair<int, int>> get_orig() {
            int o_x, o_y;
            do {
                o_x = rand_int(rand_e) % maze_w * unit_size + (unit_size - 1) / 2;
                o_y = rand_int(rand_e) % maze_h * unit_size + (unit_size - 1) / 2;
            } while (o_x >= grid_w || o_y >= grid_h);
            return std::list<std::pair<int, int>>(1, { o_x,o_y });
        }

        bool out_bound(int mx, int my) const {
            return mx < 0 || my < 0 || mx >= maze_w || my >= maze_h;
        }
        bool is_visited(int mx, int my) const {
            return out_bound(mx, my) || visited[mx][my];
        }

        void print_maze() {
            for (size_t i = 0; i < maze_w * 2; i++) {
                for (size_t j = 0; j < maze_h * 2; j++) {
                    std::cout << (walls[i][j] ? "¨€¨€" : "  ");
                }
                std::cout << std::endl;
            }
        }
	};
} // namespace grid