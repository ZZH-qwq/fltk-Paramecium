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

		std::uniform_real_distribution<double> init_r;
		std::normal_distribution<double> dr;

		Paramecium() : init_r(0, 2 * std::numbers::pi), dr(deviation_mean, deviation_variance) {
			st_x = st_y = 22, st_rad = 0;
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
			double px = st_x, py = st_y, rad = init_r(grid::rand_e);
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
				energy_used += abs(rotate_radius) / std::numbers::pi;
			}
			if (g->is_origin(gx, gy)) {
				score += origin_bonus * (total_energy - energy_used);
				return { Found,score,energy_used,px,py };
			}
			return { Failed,score,energy_used,px,py };
		}

		int simu_count = 250, curr_count = 0, step_count = 50;
		double t_st = 1000, curr_t = 1000, t_min = 10, rate = 0.99;
		double new_l = 0.5, new_r = 0.5, new_m = 0, new_v = 0.1, new_score_sum = 0;
		double best_l, best_r, best_m, best_v, best_score_sum = 0;
		double curr_score_sum = 0;
		bool anneal_finished = true;
		bool simulate_anneal() {
			double l = step_length, r = rotate_radius, m = deviation_mean, v = deviation_variance;
			step_length = new_l, rotate_radius = new_r, deviation_mean = new_m, deviation_variance = new_v;
			std::uniform_real_distribution<double> rd(0, 1);
			dr = std::normal_distribution<double>(deviation_mean, deviation_variance);
			int step_end = std::min(curr_count + step_count, simu_count);
			while (++curr_count < step_end) {
				auto sim = simulate_score();
				new_score_sum += sim.score;
			}
			if (curr_count >= simu_count) {
				if (new_score_sum > best_score_sum) {
					best_score_sum = new_score_sum;
					best_l = new_l, best_r = new_r, best_m = new_m, best_v = new_v;
				}
				double accept = (new_score_sum - curr_score_sum) / curr_t / total_energy / simu_count * 1e4;
				curr_count = 0;
#ifdef _DEBUG
				std::cout << "curr_t = " << curr_t << ", curr_score = " << curr_score_sum << ", new_score = " << new_score_sum << std::endl;
#endif // _DEBUG
				if (new_score_sum > curr_score_sum || new_score_sum > simu_count / 10.0 && exp(accept) > rd(grid::rand_e)) {
#ifdef _DEBUG
					if (new_score_sum <= curr_score_sum) {
						std::cout << accept << std::endl;
					}
#endif // _DEBUG
					curr_score_sum = new_score_sum;
					reset_status();
					dr = std::normal_distribution<double>(deviation_mean, deviation_variance);
					gen_args(new_l, new_r, new_m, new_v);
					update_sliders();
					reset_pos();
					new_score_sum = 0;
					curr_t *= rate;
					update_anneal();
					return curr_t < t_min;
				}
				new_score_sum = 0;
				gen_args(l, r, m, v);
				curr_t *= rate;
				update_anneal();
			}
			step_length = l, rotate_radius = r, deviation_mean = m, deviation_variance = v;
			dr = std::normal_distribution<double>(deviation_mean, deviation_variance);
			return curr_t < t_min;
		}

		void gen_args(double l, double r, double m, double v) {
			std::normal_distribution<double> rd(0, 0.5);
			new_l = std::min(std::max(l + rd(grid::rand_e) * curr_t / 2000, 0.01), 1.0);
			new_r = std::min(std::max(r + rd(grid::rand_e) * curr_t / 500, -std::numbers::pi), std::numbers::pi);
			new_m = std::min(std::max(m + rd(grid::rand_e) * curr_t / 4000, -0.25), 0.25);
			new_v = std::min(std::max(v + rd(grid::rand_e) * curr_t / 1000, 0.01), 0.5);
		}

		void set_as_best() {
			step_length = best_l, rotate_radius = best_r, deviation_mean = best_m, deviation_variance = best_v;
			dr = std::normal_distribution<double>(deviation_mean, deviation_variance);
			update_sliders();
		}
		
		void reset_status() {
			resimulate_flag = true, has_temp = false;
			score_sum = 0;
			simulation_count = success_count = 0;
		}

		virtual void update_sliders() = 0;
		virtual void update_anneal() = 0;
		virtual void reset_pos() = 0;

		private:
		void step_forward(double& px, double& rad, double& py, int& gx, int& gy) {
			px += step_length * cos(rad);
			py += step_length * sin(rad);
			rad += dr(grid::rand_e);
			gx = std::floor(px), gy = std::floor(py);
		}
		void step_try_backward(double& rad, double orad, double& px, double ox, double& py, double oy, int& gx, int& gy) {
			rad = orad - dr(grid::rand_e);
			px = ox - step_length * cos(rad);
			py = oy - step_length * sin(rad);
			gx = std::floor(px), gy = std::floor(py);
		}
		void step_rotate(double& rad) {
			rad += rotate_radius + dr(grid::rand_e);
		}
	};
} // namespace paramecium