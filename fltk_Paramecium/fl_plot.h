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
		Fl_Image* bg_white_transparent, * bg_white_indicator;
		Fl_RGB_Image* plot_image = nullptr;

		int samples = 250;
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
			oscr_plot = fl_create_offscreen(w(), h());
			uchar data[4]{ 255,255,255,80 };
			Fl_RGB_Image img(data, 1, 1, 4);
			bg_white_transparent = img.copy(w_, h_);
			data[3] = 200;
			Fl_RGB_Image img2(data, 1, 1, 4);
			bg_white_indicator = img2.copy(7 * g_size, 6 * g_size);

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
						curr_x = curr_y = -1;
						gen_image();
					} else {
						curr_x = 0, curr_y++;
					}
				} else {
					curr_x++;
				}
			}
			if (!finished) {
				draw_grid();
			} else if (redraw_flag) {
				// Drectly use pre-generated image
				fl_begin_offscreen(oscr_plot);
				plot_image->draw(0, 0);
				display_detail();
				fl_end_offscreen();
				fl_copy_offscreen(x(), y(), w(), h(), oscr_plot, 0, 0);
				redraw_flag = false;
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

		void display_detail() {
			if (curr_x < 0 || curr_y < 0) {
				return;
			}
			auto& d = data[curr_x][curr_y];
			double score = d.score_sum / d.total, rate = (double)d.arrived / d.total;
#ifdef _DEBUG
			std::cout << "Pointing at (" << curr_x << ", " << curr_y << "), score = " << score << ", rate = " << rate << std::endl;
#endif // _DEBUG
			auto grid_color = draw::rainbow_linear_gradient_base((sum_max - d.score_sum) / std::max(sum_max - sum_min, 1.0));
			fl_rectf(curr_x * pixels_per_grid, curr_y * pixels_per_grid, pixels_per_grid, pixels_per_grid, grid_color);
			bool align = curr_x <= 6;
			double gx = (align ? curr_x + 1.6 : curr_x - 6.6), px = (align ? curr_x + 1 : curr_x - 7), gy = (curr_y > grid_h - 6 ? curr_y - 4.5 : curr_y + 0.5);
			auto tag_color = fl_darker(grid_color);
			bg_white_transparent->draw(0, 0);
			bg_white_indicator->draw(px * pixels_per_grid, (gy - 0.5) * pixels_per_grid);
			display_tag_num(get_val_name(val1), d.val1, gx, gy, 3 * pixels_per_grid, pixels_per_grid, tag_color, align);
			display_tag_num(get_val_name(val2), d.val2, gx + 3, gy, 3 * pixels_per_grid, pixels_per_grid, tag_color, align);
			display_tag_num("Adaptation Score", score, gx, gy + 1.2, 6 * pixels_per_grid, 1.8 * pixels_per_grid, grid_color, align);
			display_tag_num("Arrival Rate", rate, gx, gy + 3.2, 6 * pixels_per_grid, 1.8 * pixels_per_grid, grid_color, align);
		}
		void display_tag_num(std::string tag, double num, double gx, double gy, int w, int h, Fl_Color tag_color, bool align) {
			fl_push_clip(gx * pixels_per_grid, gy * pixels_per_grid, w, h);
			fl_color(tag_color);
			fl_font(FL_BOLD, h * 0.4);
			fl_draw(tag.c_str(), gx * pixels_per_grid, gy * pixels_per_grid, w, h * 0.4, (align ? FL_ALIGN_LEFT : FL_ALIGN_RIGHT));
			fl_color(FL_BLACK);
			fl_font(FL_BOLD, h * 0.6);
			auto n = std::to_string(num);
			if (n.size() >= 10) {
				n = n.substr(0, 10);
			}
			fl_draw(n.c_str(), gx * pixels_per_grid, gy * pixels_per_grid + h * 0.4, w, h * 0.6, (align ? FL_ALIGN_LEFT : FL_ALIGN_RIGHT));
			fl_pop_clip();
		}

		void gen_image() {
			fl_begin_offscreen(oscr_plot);
			fl_rectf(0, 0, w(), h(), FL_WHITE);
			draw_grid(true);
			auto data = fl_read_image(nullptr, 0, 0, w(), h());
			if (plot_image) {
				delete plot_image;
			}
			plot_image = new Fl_RGB_Image(data, w(), h());
			fl_end_offscreen();
			plot_image->draw(x(), y());
		}

		void draw_grid(bool in_offscreen = false) {
			for (int i = 0; i < grid_w; i++) {
				for (int j = 0; j < grid_h; j++) {
					draw_pos(i, j, in_offscreen);
				}
			}
		}

		void draw_pos(int cx, int cy, bool in_offscreen = false, bool use_base = false) {
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

		static std::string get_val_name(Value_Name v) {
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

		int handle_show_detail(int event, double grid_x, double grid_y) {
			if (!finished) {
				return Fl_Widget::handle(event);
			}
			switch (event) {
			case FL_ENTER: {
				return 1;
			}
			case FL_MOVE: {
				if (std::floor(grid_x / 2) == curr_x && std::floor(grid_y / 2) == curr_y) {
					return 0;
				}
				curr_x = std::floor(grid_x / 2);
				curr_y = std::floor(grid_y / 2);
				return 1;
			}
			case FL_LEAVE: {
				curr_x = -1;
				curr_y = -1;
				return 1;
			}
			default:
				break;
			}
			return Fl_Widget::handle(event);
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