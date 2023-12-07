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
		//Fl_Offscreen oscr_grid;
		const size_t step_count;
		bool show_distance = true;

		Fl_Grid(int x_, int y_, int w_, int h_, int g_size) : Fl_Box(x_, y_, w_, h_), Grid(w_ / g_size, h_ / g_size),
			pixels_per_grid(g_size), step_count(std::max(static_cast<int>(grid_w* grid_h / 25), 1)) {
			//oscr_grid = fl_create_offscreen(w(), h());

			// for test only
			status[0][0] = 1;
			nodes[0][0] = { 10,0,0 };
			queue.push_back({ 0,0 });
			orig.push_back({ 0,0 });
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
				step_flag = false;
			}
			if (redraw_flag) {
				draw_grid();
				//print_flow();
				redraw_flag = false;
			} else {
				update_grid();
			}
			if (show_border) {
				draw_border();
			}
		}

		void draw_grid() {
			updated.clear();
			//fl_begin_offscreen(oscr_grid);
			fl_rectf(0, 0, w(), h(), FL_WHITE);
			for (size_t i = 0; i < grid_w; i++) {
				for (size_t j = 0; j < grid_h; j++) {
					draw_pos(i, j);
				}
			}
			//fl_end_offscreen();
			//fl_copy_offscreen(x(), y(), w(), h(), oscr_grid, 0, 0);
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
			} else if (status[cx][cy] && show_distance) {
				double l = 1 - exp(-(nodes[cx][cy].distance - d_min) / double(d_max) * 1.5);
				//std::cout << l << std::endl;
				fl_color(draw::linear_gradient(l));
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