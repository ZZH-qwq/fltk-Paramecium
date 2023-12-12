#pragma once

#include "paramecium.h"

namespace paramecium {

	class Fl_Paramecium : public Fl_Widget, public Paramecium {
	public:
		double px, py, rad;
		int pixels_per_grid;

		Fl_Paramecium(int x_, int y_, int w_, int h_, int g_size) : Fl_Widget(x_, y_, w_, h_), pixels_per_grid(g_size) {
			px = py = 20;
			rad = 0;
			
		}

		void draw() {
			if (!enable_simulate) {
				return;
			}
			if (resimulate_flag) {
				steps.clear();
				simulate({ Forward,0,20,20,0 });
				resimulate_flag = false;
			}
			fl_color(FL_BLUE);
			fl_line_style(FL_SOLID, 3);
			fl_begin_line();
			for (auto& s : steps) {
				px = s.x, py = s.y, rad = s.rad;
				fl_vertex(px * pixels_per_grid, py * pixels_per_grid);
			}
			fl_end_line();
		}
	};
} // namespace paramecium