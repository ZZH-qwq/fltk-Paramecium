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
		int step_count = 500;
		double step_length = 0.5, rotate_radius = 0.5, rotate_mean = 0, rotate_variance = 0.1;

		bool redraw_flag = true, enable_simulate = false;
		bool resimulate_flag = true;
		
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
			auto e = std::default_random_engine();
			auto dr = std::normal_distribution<double>(rotate_mean, rotate_variance);
			double px = s.x, py = s.y, rad = s.rad;
			int gx = std::floor(px), gy = std::floor(py);
			int curr_dist = g->get_distance(px, py), prev_dist = curr_dist;
			while (steps.size() <= step_count && !g->is_origin(gx, gy)) {
				// First push next action to list, then do it
				if (!g->is_barrier(gx, gy)) {
					prev_dist = curr_dist;
					curr_dist = g->get_distance(gx, gy);
					if (curr_dist <= prev_dist) {
						px += step_length * cos(rad);
						py += step_length * sin(rad);
						rad += dr(e);
						gx = std::floor(px), gy = std::floor(py);
						steps.push_back({ Forward,step_length,px,py,rad });
						continue;
					}
				}
				px -= step_length * cos(rad);
				py -= step_length * sin(rad);
				rad -= dr(e);
				gx = std::floor(px), gy = std::floor(py);
				if (g->is_barrier(gx, gy)) {
					steps.push_back({ Failed,0,px,py,rad });
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