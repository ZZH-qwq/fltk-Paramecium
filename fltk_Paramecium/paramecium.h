#pragma once

/*
*  paramecium.h
* 
*  Act as a Paramecium
*/

#include "grid.h"

namespace paramecium {

	class Paramecium {
	public:
		grid::Grid* g;
		int step_count = 500, retry_count = 20, min_steps = step_count - 50;
		double step_length = 0.5, rotate_radius = 0.5, rotate_mean = 0, rotate_variance = 0.1;

		bool redraw_flag = true, enable_simulate = false, step_flag = false, has_temp = false;
		bool resimulate_flag = true;

		std::default_random_engine e;
		std::uniform_real_distribution<double> init_r;
		std::normal_distribution<double> dr;

		Paramecium() : init_r(0, 2 * M_PI), dr(rotate_mean, rotate_variance) {}

		enum Step_type { Forward, Backward, Rotate, Failed, Found };
		struct Step_Info {
			Step_type type;
			double amount;
			double x, y, rad;
		};
		std::list<Step_Info> steps;

		void simulate(Step_Info s) {
			if (!g->queue.empty()) {
				steps.clear();
				return;
			}
			if (s.type == Found || s.type == Failed || steps.size() > min_steps) {
				return;
			}
			double px = s.x, py = s.y, rad = s.rad;
			int gx = std::floor(px), gy = std::floor(py);
			int curr_dist = 0, prev_dist = curr_dist;
			if (!g->is_barrier(gx, gy)) {
				curr_dist = g->get_distance(gx, gy);
			}
			while (steps.size() <= step_count && !g->is_origin(gx, gy)) {
				// First push next action to list, then do it
				if (!g->is_barrier(gx, gy)) {
					prev_dist = curr_dist;
					curr_dist = g->get_distance(gx, gy);
					if (curr_dist <= prev_dist || curr_dist == 0) {
						px += step_length * cos(rad);
						py += step_length * sin(rad);
						rad += dr(e);
						gx = std::floor(px), gy = std::floor(py);
						steps.push_back({ Forward,step_length,px,py,rad });
						continue;
					}
				}
				int c = 0;
				double ox = px, oy = py, orad = rad;
				do {
					rad = orad - dr(e);
					px = ox - step_length * cos(rad);
					py = oy - step_length * sin(rad);
					gx = std::floor(px), gy = std::floor(py);
					++c;
				} while (c < retry_count && g->is_barrier(gx, gy));
				if (c == retry_count) {
					steps.push_back({ Failed,0,px,py,rad });
#ifdef _DEBUG
					std::cout << "Simulation Failed" << std::endl;
#endif // _DEBUG
					return;
				}
				steps.push_back({ Backward,step_length,px,py,rad });
				rad += rotate_radius + dr(e);
				steps.push_back({ Rotate,step_length,px,py,rad });
			}
			if (g->is_origin(gx, gy)) {
				steps.push_back({ Found,0,px,py,rad });
				return;
			}
		}
	};
} // namespace paramecium