#pragma once

/*
 *  fl_plot.h
 *
 *  Drawing 2D plot of adaptation score and other simulation results
 */

#include "paramecium.h"

namespace paramecium {

	class Fl_Plot : public Fl_Widget {
	public:
		Fl_Paramecium* p;
		int pixels_per_grid;
		size_t grid_w, grid_h;
		Fl_Offscreen oscr_plot;
		Fl_RGB_Image* plot_image = nullptr;

		int samples = 500;
		double sum_max, sum_min;
		bool redraw_flag = true, renew_flag = false, finished = false, sync_back_flag = true;

		struct Plot_Unit {
			double val1, val2;
			double score_sum;
			int total, arrived;
		};
		std::vector<std::vector<Plot_Unit>> data;
		int curr_x, curr_y;

		// TODO: only change values of p when start drawing
		Fl_Button* confirm_plot_bt;
		Fl_Hor_Value_Slider* samples_ip, * total_energy_ip, * step_len_ip, * rotate_rad_ip, * deviation_m_ip, * deviation_v_ip;

		// 0-erg, 1-slen, 2-rotr, 3-devm, 4-devv
		double args[5] = { 500,0.5,0.5,0,0.1 };
		double o_erg = 500, o_slen = 0.5, o_rotr = 0.5, o_devm = 0, o_devv = 0.1;

		Fl_Plot(int x_, int y_, int w_, int h_, int g_size) : Fl_Widget(x_, y_, w_, h_),
			grid_w(w_ / g_size), grid_h(h_ / g_size), pixels_per_grid(g_size) {
			clear_status();
			val1 = Step_len, val2 = Rot_rad;
			val1_min = 0.1, val1_max = 1, val2_min = -2.5, val2_max = 2.8;
		}

		void simulate_grid() {
			double score_sum = 0;
			int arrived = 0;
			auto [val1, val2] = set_values();
			for (int i = 0; i < samples; i++) {
				auto sim = p->simulate_score();
				score_sum += sim.score;
				if (sim.result == Paramecium::Found) {
					arrived++;
				}
			}
			data[curr_x][curr_y] = { val1,val2,score_sum,samples,arrived };
			sum_min = std::min(sum_min, score_sum);
			sum_max = std::max(sum_max, score_sum);
#ifdef _DEBUG
			std::cout << "Simulating x = " << curr_x << ", y = " << curr_y << ", result = " << data[curr_x][curr_y].score_sum << std::endl;
#endif
		}

		void draw() {
			if (!finished) {
				simulate_grid();
				if (curr_x == grid_w - 1) {
					if (curr_y == grid_h - 1) {
						finished = true;
					} else {
						curr_x = 0, curr_y++;
					}
				} else {
					curr_x++;
				}
			}
			for (int i = 0; i < grid_w; i++) {
				for (int j = 0; j < grid_h; j++) {
					draw_pos(i, j);
				}
			}
		}

		enum Value_Name { Step_len, Rot_rad, Dev_m, Dev_v } val1, val2;
		double val1_min, val1_max, val2_min, val2_max;

		std::pair<double, double> set_values() {
			return{ set_val(val1,val1_min,val1_max,curr_x,grid_w),set_val(val2,val2_min,val2_max,curr_y,grid_h) };
		}

		double set_val(Value_Name v, double mi, double ma, int step, int tot) {
			double val = mi + (ma - mi) * step / tot;
			switch (v) {
			case paramecium::Fl_Plot::Step_len:
				p->step_length = val;
				break;
			case paramecium::Fl_Plot::Rot_rad:
				p->rotate_radius = val;
				break;
			case paramecium::Fl_Plot::Dev_m:
				p->deviation_mean = val;
				break;
			case paramecium::Fl_Plot::Dev_v:
				p->deviation_variance = val;
				break;
			default:
				break;
			}
			return val;
		}

		void draw_pos(int cx, int cy, bool in_offscreen = false) {
			if (data[cx][cy].total == 0) {
				fl_color(FL_WHITE);
			} else {
				double l = (sum_max - data[cx][cy].score_sum) / std::max(sum_max - sum_min, 1.0);
				fl_color(draw::rainbow_linear_gradient(l));
			}
			if (in_offscreen) {
				fl_rectf(cx * pixels_per_grid, cy * pixels_per_grid, pixels_per_grid, pixels_per_grid);
			} else {
				fl_rectf(x() + cx * pixels_per_grid, y() + cy * pixels_per_grid, pixels_per_grid, pixels_per_grid);
			}
		}

		void clear_status() {
			data = std::vector<std::vector<Plot_Unit>>(grid_w, std::vector<Plot_Unit>(grid_h));
			finished = false, renew_flag = false, sync_back_flag = true;
			curr_x = curr_y = 0;
			sum_min = DBL_MAX, sum_max = 0;
		}

		std::string get_name(Value_Name v) {
			switch (v) {
			case paramecium::Fl_Plot::Step_len:
				return "Step Length";
				break;
			case paramecium::Fl_Plot::Rot_rad:
				return "Rotate Radius";
				break;
			case paramecium::Fl_Plot::Dev_m:
				return "Deviation Mean";
				break;
			case paramecium::Fl_Plot::Dev_v:
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

		void update_sliders() {
			total_energy_ip->value(o_erg);
			step_len_ip->value(o_slen);
			rotate_rad_ip->value(o_rotr);
			deviation_m_ip->value(o_devm);
			deviation_v_ip->value(o_devv);
		}
		void update_args() {
			samples = samples_ip->value();
			args[0] = o_erg = total_energy_ip->value();
			args[1] = o_slen = step_len_ip->value();
			args[2] = o_rotr = rotate_rad_ip->value();
			args[3] = o_devm = deviation_m_ip->value();
			args[4] = o_devv = deviation_v_ip->value();
		}
	};

	static void confirm_plot_cb(Fl_Widget* o, void* v) {
		Fl_Plot* p = (Fl_Plot*)v;
		p->update_args();
		p->clear_status();
		p->sync_back();
	}
} // namespace paramecium