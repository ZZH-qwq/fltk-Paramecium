#pragma once

/*
 *  plot.h
 *
 *  Simulating and process results for selected values
 */

#include "paramecium.h"

namespace paramecium {

	class Plot {
	public:
		Fl_Paramecium* p;
		int pixels_per_grid;
		size_t grid_w, grid_h;

		int samples = 250, grid_simulated = 0, step_max = 500;
		double sum_max, sum_min;
		bool redraw_flag = true, renew_flag = false, finished = false, sync_back_flag = true;

		struct Plot_Unit {
			double val1, val2;
			double score_sum;
			int total, arrived;
		};
		std::vector<std::vector<Plot_Unit>> data;
		int curr_x, curr_y;

		// 0-erg, 1-slen, 2-rotr, 3-devm, 4-devv
		double args[5] = { 500,0.5,0.5,0,0.1 };
		double o_erg = 500, o_slen = 0.5, o_rotr = 0.5, o_devm = 0, o_devv = 0.1;

		enum Value_Name { Step_len, Rot_rad, Dev_m, Dev_v } val1, val2;
		double val1_min, val1_max, val2_min, val2_max;

		Plot(int w_, int h_, int g_size) : grid_w(w_ / g_size), grid_h(h_ / g_size), pixels_per_grid(g_size) {
			clear_status();
			val1 = Step_len, val2 = Rot_rad;
			val1_min = 0.1, val1_max = 1, val2_min = -2.5, val2_max = 2.8;
		}

		bool simulate_grid() {
			double score_sum = 0, s = data[curr_x][curr_y].score_sum;
			int arrived = 0, a = data[curr_x][curr_y].arrived;
			auto [val1, val2] = set_values();
			int step_end = std::min(samples, grid_simulated + step_max);
			while (++grid_simulated < step_end) {
				auto sim = p->simulate_score();
				score_sum += sim.score;
				if (sim.result == Paramecium::Found) {
					arrived++;
				}
			}
			data[curr_x][curr_y] = { val1,val2,s + score_sum,grid_simulated,a + arrived };
			sum_max = std::max(sum_max, s + score_sum);
			if (grid_simulated >= samples) {
				sum_min = std::min(sum_min, s + score_sum);
				grid_simulated = 0;
#ifdef _DEBUG
				std::cout << "Simulating x = " << curr_x << ", y = " << curr_y << ", result = " << data[curr_x][curr_y].score_sum << std::endl;
#endif
				return true;
			}
			return false;
		}

		std::pair<double, double> set_values() {
			return{ set_val(val1,val1_min,val1_max,curr_x,grid_w),set_val(val2,val2_min,val2_max,curr_y,grid_h) };
		}

		double set_val(Value_Name v, double mi, double ma, int step, int tot) {
			double val = mi + (ma - mi) * step / tot;
			switch (v) {
			case paramecium::Plot::Step_len:
				p->step_length = val;
				break;
			case paramecium::Plot::Rot_rad:
				p->rotate_radius = val;
				break;
			case paramecium::Plot::Dev_m:
				p->deviation_mean = val;
				break;
			case paramecium::Plot::Dev_v:
				p->deviation_variance = val;
				break;
			default:
				break;
			}
			return val;
		}

		void clear_status() {
			data = std::vector<std::vector<Plot_Unit>>(grid_w, std::vector<Plot_Unit>(grid_h));
			finished = false, renew_flag = false, sync_back_flag = true;
			curr_x = curr_y = 0;
			sum_min = DBL_MAX, sum_max = 0;
		}

		static std::string get_val_name(Value_Name v) {
			switch (v) {
			case paramecium::Plot::Step_len:
				return "Step Length";
				break;
			case paramecium::Plot::Rot_rad:
				return "Rotate Radius";
				break;
			case paramecium::Plot::Dev_m:
				return "Deviation Mean";
				break;
			case paramecium::Plot::Dev_v:
				return "Deviation Various";
				break;
			default:
				return "";
				break;
			}
		}
		
		void sync() {
			args[0] = o_erg = p->total_energy;
			args[1] = o_slen = p->step_length;
			args[2] = o_rotr = p->rotate_radius;
			args[3] = o_devm = p->deviation_mean;
			args[4] = o_devv = p->deviation_variance;
			update_sliders();
			clear_status();
		}
		void sync_back(bool force = false) {
			if (!sync_back_flag && !force) {
				return;
			}
			sync_back_flag = false;
			p->total_energy = args[0];
			p->step_length = args[1];
			p->rotate_radius = args[2];
			p->deviation_mean = args[3];
			p->deviation_variance = args[4];
			p->resimulate_flag = true;
			p->reset_pos();
			p->update_sliders();
		}

		virtual void update_sliders() = 0;
	};
} // namespace paramecium