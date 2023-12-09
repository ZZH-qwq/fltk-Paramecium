#pragma once

/*
*  barrier.h
* 
*  A drawing board that holds information about barriers
*/

#include "grid.h"

namespace grid {

	class Barrier {
	public:
        std::vector<std::pair<int, int>> segs;
        size_t grid_w, grid_h;

		bool has_temp = false;
		double st_x = 0, st_y = 0, ed_x = 0, ed_y = 0;

        Barrier(size_t w_, size_t h_) :grid_w(w_), grid_h(h_) {}

        void add_barrier_for(Grid* g) {
            g->add_barrier(line_intersection());
            g->add_barrier(st_x, st_y);
            g->add_barrier(ed_x, ed_y);
            clear_line();
            g->clear_status();
        }

        int remove_barrier_for(Grid* g, int x, int y) {
            if (g->barrier[x][y] == 0) {
                return 0;
            }
            g->remove_barrier(x, y);
            g->clear_status();
            return 1;
        }

        const std::vector<std::pair<int, int>>& line_intersection() {
            segs = std::move(line_intersection(st_x, st_y, ed_x, ed_y));
            return segs;
        }

		// Calculate the intersection of a line with a grid
        static std::vector<std::pair<int, int>> line_intersection(double x1, double y1, double x2, double y2) {
            std::vector<std::pair<int, int>> segs;
            double xmin = std::min(x1, x2);
            double ymin = std::min(y1, y2);
            double xmax = std::max(x1, x2);
            double ymax = std::max(y1, y2);
            if ((x1 == x2 && std::round(x1) == x1) || (y1 == y2 && std::round(y1) == y1)) {
                return segs;
            }
            if (x1 == x2 && std::round(x1) != x1) {
                // Vertical line
                std::vector<double> sp;
                for (double i = ymin; i <= ymax; i++) {
                    sp.push_back(i);
                }
                sp.push_back(ymax);
                for (int t = 0; t < sp.size() - 1; t++) {
                    segs.push_back({ std::floor(x1) ,std::max(std::floor(sp[t + 1]), std::floor(sp[t])) });
                }
                return segs;
            }
            double k = (y1 - y2) / (x1 - x2), b = y1 - k * x1;
            std::set<std::pair<double, double>> sp;
            for (int i = std::ceil(xmin); i <= std::floor(xmax); i++) {
#ifdef _DEBUG
                //std::cout << "line point x:" << i << " " << k * i + b << std::endl;
#endif
                sp.insert({ i,k * i + b });
            }
            for (int j = std::ceil(ymin); j <= std::floor(ymax); j++) {
#ifdef _DEBUG
                //std::cout << "line point y:" << (j - b) / k << " " << j << std::endl;
#endif
                sp.insert({ (j - b) / k,j });
            }
            for (auto it = sp.cbegin(); !sp.empty() && it != --sp.cend();) {
                auto it2 = it++;
                segs.push_back({ std::max(std::ceil(it->first),std::ceil(it2->first)) - 1,std::max(std::ceil(it->second),std::ceil(it2->second)) - 1 });
            }
            return segs;
		}

        void clear_line() {
            has_temp = false;
        }
	};
} // namespace grid