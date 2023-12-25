#pragma once

/*
 *  fl_origin.h
 *
 *  Setting origin(s) and handling events
 */

#include "grid.h"

namespace grid {

	class Fl_Origin : public Fl_Widget {
	public:
		int pixels_per_grid;
		int temp_x = -1, temp_y = -1;
		bool enabled = false, redraw_flag = true;

        Fl_Origin(int x_, int y_, int w_, int h_, int g_size) : Fl_Widget(x_, y_, w_, h_), pixels_per_grid(g_size) {}

        void draw() override {
            if (!enabled || !redraw_flag) {
                return;
            }
            draw_pos(temp_x, temp_y, 0xFF888800);
        }

		void draw_pos(int cx, int cy, Fl_Color c, bool in_offscreen = false) {
			if (in_offscreen) {
				fl_rectf(cx * pixels_per_grid, cy * pixels_per_grid, pixels_per_grid, pixels_per_grid, c);
			} else {
				fl_rectf(x() + cx * pixels_per_grid, y() + cy * pixels_per_grid, pixels_per_grid, pixels_per_grid, c);
			}
		}

        int handle_add_origin(int event, grid::Fl_Grid* g, paramecium::Paramecium* p, double grid_x, double grid_y) {
            switch (event) {
            case FL_ENTER: {
                return 1;
            }
            case FL_MOVE: {
                temp_x = std::floor(grid_x);
                temp_y = std::floor(grid_y);
                g->show_border = true;
                return 1;
            }
            case FL_LEAVE: {
                temp_x = -1;
                temp_y = -1;
                g->show_border = false;
                return 1;
            }
            case FL_PUSH: {
#ifdef _DEBUG
                std::cout << "Crusor x = " << grid_x << ", y = " << grid_y << ", dist = " << g->distance[grid_x][grid_y] << std::endl;
#endif
                return 1;
            }
            case FL_RELEASE: {
                if (!Fl::event_is_click() || !(Fl::event_button() == FL_LEFT_MOUSE)) {
                    return 0;
                }
                g->orig.push_back({ std::floor(grid_x) ,std::floor(grid_y) });
                g->clear_status();
                p->reset_status();
                g->show_border = false;
                return 1;
            }
            }
            return Fl_Widget::handle(event);
        }
	};
}