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
		grid::Grid* g = nullptr;
		int list_len = 500, retry_count = 1000, min_list_len = list_len - 50;
		double step_length = 0.5, rotate_radius = 0.5, deviation_mean = 0, deviation_variance = 0.1;

		bool redraw_flag = true, enable_simulate = false, step_flag = false, has_temp = false;
		bool resimulate_flag = true;

		double st_x, st_y, st_rad;

		std::default_random_engine e;
		std::uniform_real_distribution<double> init_r;
		std::normal_distribution<double> dr;

		Paramecium() : init_r(0, 2 * M_PI), dr(deviation_mean, deviation_variance) {
			st_x = st_y = 20, st_rad = 0;
		}

		enum Step_type { Forward, Backward, Rotate, Failed, Found };
		struct Step_Info {
			Step_type type;
			double amount;
			double x, y, rad;
		};
		std::list<Step_Info> steps;

		void simulate_steps(Step_Info s) {
			if (!g->queue.empty()) {
				steps.clear();
				return;
			}
			if (s.type == Found || s.type == Failed || steps.size() > min_list_len) {
				return;
			}
			double px = s.x, py = s.y, rad = s.rad;
			int gx = std::floor(px), gy = std::floor(py);
			int curr_dist = 0, prev_dist = curr_dist;
			if (!g->is_barrier(gx, gy)) {
				curr_dist = g->get_distance(gx, gy);
			}
			while (steps.size() <= list_len && !g->is_origin(gx, gy)) {
				// First push next action to list, then do it
				if (!g->is_barrier(gx, gy)) {
					prev_dist = curr_dist;
					curr_dist = g->get_distance(gx, gy);
					if (curr_dist <= prev_dist || curr_dist == 0) {
						step_forward(px, rad, py, gx, gy);
						steps.push_back({ Forward,step_length,px,py,rad });
						continue;
					}
				}
				int c = 0;
				double ox = px, oy = py, orad = rad;
				do {
					step_try_backward(rad, orad, px, ox, py, oy, gx, gy);
					++c;
				} while (c < retry_count && g->is_barrier(gx, gy));
				if (c == retry_count) {
					steps.push_back({ Failed,0,px,py,rad });
#ifdef _DEBUG
					// Paramecium may "stuck in wall" which cause failure
					std::cout << "Simulation Failed" << std::endl;
#endif // _DEBUG
					return;
				}
				steps.push_back({ Backward,step_length,px,py,rad });
				step_rotate(rad);
				steps.push_back({ Rotate,step_length,px,py,rad });
			}
			if (g->is_origin(gx, gy)) {
				steps.push_back({ Found,0,px,py,rad });
				return;
			}
		}

		struct Simulation_Result {
			Step_type result;
			double score, energy_used, ed_x, ed_y;
		};

		double total_energy = 500, origin_bonus = 0.5, base_usage = 1;
		double score_sum = 0;
		int simulation_count = 0, success_count = 0, simulation_time = 500;

		Simulation_Result simulate_score() {
			double energy_used = 0, score = 0;
			double px = st_x, py = st_y, rad = init_r(e);
			int gx = std::floor(px), gy = std::floor(py);
			int curr_dist = 0, prev_dist = curr_dist;
			if (!g->is_barrier(gx, gy)) {
				curr_dist = g->get_distance(gx, gy);
			}
			while (energy_used < total_energy && !g->is_origin(gx, gy)) {
				// First push next action to list, then do it
				if (!g->is_barrier(gx, gy)) {
					prev_dist = curr_dist;
					curr_dist = g->get_distance(gx, gy);
					if (curr_dist <= prev_dist || curr_dist == 0) {
						step_forward(px, rad, py, gx, gy);
						energy_used += step_length + base_usage / 10;
						score += std::max(step_length * exp(-curr_dist) * 10 - base_usage / curr_dist, 0.0);
						continue;
					}
				}
				int c = 0;
				double ox = px, oy = py, orad = rad;
				do {
					step_try_backward(rad, orad, px, ox, py, oy, gx, gy);
					++c;
				} while (c < retry_count && g->is_barrier(gx, gy));
				if (c == retry_count) {
					return { Failed,score,energy_used,px,py };
				}
				energy_used += (c + 1) / 2.0 * step_length;
				step_rotate(rad);
				energy_used += rotate_radius / M_PI;
			}
			if (g->is_origin(gx, gy)) {
				score += origin_bonus * (total_energy - energy_used);
				return { Found,score,energy_used,px,py };
			}
			return { Failed,score,energy_used,px,py };
		}
		
		void reset_status() {
			resimulate_flag = true, has_temp = false;
			score_sum = 0;
			simulation_count = success_count = 0;
		}

		private:
		void step_forward(double& px, double& rad, double& py, int& gx, int& gy) {
			px += step_length * cos(rad);
			py += step_length * sin(rad);
			rad += dr(e);
			gx = std::floor(px), gy = std::floor(py);
		}
		void step_try_backward(double& rad, double orad, double& px, double ox, double& py, double oy, int& gx, int& gy) {
			rad = orad - dr(e);
			px = ox - step_length * cos(rad);
			py = oy - step_length * sin(rad);
			gx = std::floor(px), gy = std::floor(py);
		}
		void step_rotate(double& rad) {
			rad += rotate_radius + dr(e);
		}
	};
} // namespace paramecium