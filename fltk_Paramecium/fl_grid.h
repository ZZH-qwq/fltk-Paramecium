#pragma once

/*
*  fl_grid.h
* 
*  Displays information about the grid, with color representing the distance to the origin
*/

#include "grid.h"

namespace grid {

	class Fl_Grid : public Grid, public Fl_Box {
	public:
		int pixels_per_grid;
		Fl_Offscreen oscr_grid;
		const size_t step_count;
		bool show_distance = true;
		Fl_Image* bg_white_transparent;
		Fl_RGB_Image* grid_image = nullptr;

		Fl_Button* clear_barrier_bt = nullptr;
		draw::Fl_Gradient* gradient_ind = nullptr;

		Fl_Grid(int x_, int y_, int w_, int h_, int g_size) : Fl_Box(x_, y_, w_, h_), Grid(w_ / g_size, h_ / g_size),
			pixels_per_grid(g_size), step_count(std::max(static_cast<int>(grid_w * grid_h / 20), 1)) {
			oscr_grid = fl_create_offscreen(w(), h());
			uchar data[4]{ 255,255,255,40 };
			Fl_RGB_Image img(data, 1, 1, 4);
			bg_white_transparent = img.copy(w_, h_);
		}

		void draw_border() {
			fl_color(FL_GRAY);
			fl_line_style(FL_DASH, 1);
			for (size_t i = 0; i < grid_w; i++) {
				fl_line(i * pixels_per_grid, 0, i * pixels_per_grid, grid_h * pixels_per_grid);
			}
			for (size_t j = 0; j < grid_h; j++) {
				fl_line(0, j * pixels_per_grid, grid_w * pixels_per_grid, j * pixels_per_grid);
			}
		}

		void draw() override {
			//bfs_step();
			if (step_flag && !queue.empty()) {
				for (size_t i = 0; i <= step_count && !queue.empty(); i++) {
					bfs_step();
				}
				if (queue.empty()) {
					flush_flag = true;
				}
				step_flag = false;
				gradient_ind->red = (double)d_min / 2;
				gradient_ind->blue = (double)d_max / 2;
			}
			if ((redraw_flag || flush_flag) && !stabled) {
				draw_grid();
				//print_dist();
				//draw_flow(20, 20);
				redraw_flag = false;
			} else if (redraw_flag && stabled && grid_image) {
				grid_image->draw(x(), y());
			} else {
				update_grid();
			}
			if (show_border) {
				draw_border();
			}
		}

		void draw_grid() {
			updated.clear();
			bool finished = queue.empty();
			if (finished || flush_flag) {
				fl_begin_offscreen(oscr_grid);
				fl_rectf(0, 0, w(), h(), FL_WHITE);
				for (size_t i = 0; i < grid_w; i++) {
					for (size_t j = 0; j < grid_h; j++) {
						draw_pos(i, j, true);
					}
				}
				auto data = fl_read_image(nullptr, 0, 0, w(), h());
				if (grid_image) {
					delete grid_image;
				}
				grid_image = new Fl_RGB_Image(data, w(), h());
				grid_image->draw(0, 0);
				fl_end_offscreen();
				fl_copy_offscreen(x(), y(), w(), h(), oscr_grid, 0, 0);
				flush_flag = false;
			} else {
				bg_white_transparent->draw(x(), y());
				for (size_t i = 0; i < grid_w; i++) {
					for (size_t j = 0; j < grid_h; j++) {
						draw_pos(i, j);
					}
				}
			}
			//fl_copy_offscreen(x(), y(), w(), h(), oscr_grid, 0, 0);
		}

		void draw_flow(int px, int py) {
			if (!status[px][py]) {
				return;
			}
			fl_color(FL_RED);
			fl_line_style(FL_SOLID, 3);
			fl_begin_line();
			int f = 1;
			while (f > 0) {
				fl_vertex(x() + px * pixels_per_grid + pixels_per_grid / 2.0, y() + py * pixels_per_grid + pixels_per_grid / 2.0);
				f = father[px][py];
				px = px + FATHER[f][0], py = py + FATHER[f][1];
			}
			fl_end_line();
		}

		void update_grid() {
			for (auto& p : updated) {
				draw_pos(p.first, p.second);
			}
			updated.clear();
		}

		void draw_pos(int cx, int cy, bool in_offscreen = false) {
			if (barrier[cx][cy] != 0) {
				fl_color(FL_DARK3);
			} else if (status[cx][cy] == 1 && show_distance) {
				double l = 1 - exp(-(distance[cx][cy] - d_min) / double(d_max) * 1.5);
				//std::cout << l << std::endl;
				fl_color(draw::rainbow_linear_gradient(l));
			} else {
				return;
			}
			if (in_offscreen) {
				fl_rectf(cx * pixels_per_grid, cy * pixels_per_grid, pixels_per_grid, pixels_per_grid);
			} else {
				fl_rectf(x() + cx * pixels_per_grid, y() + cy * pixels_per_grid, pixels_per_grid, pixels_per_grid);
			}
		}
	};
} // namespace grid