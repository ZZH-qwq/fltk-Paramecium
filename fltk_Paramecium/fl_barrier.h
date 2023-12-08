#pragma once

/*
*  fl_barrier.h
* 
*  Displaying the drawing board and handling events
*/

#include "barrier.h"

namespace grid {

	class Fl_Barrier : public Fl_Widget, public Barrier {
	public:
		int pixels_per_grid;
		bool redraw_flag = false;

		void draw_temp_line() {
			line_intersection();
			for (auto& i : segs) {
#ifdef _DEBUG
				//std::cout << "line point: (" << i.first << " " << i.second << ")" << std::endl;
#endif
				draw_pos(i.first, i.second, 0xAAAAAA00);
			}
			draw_pos(st_x, st_y, 0xFF88FF00);
			draw_pos(ed_x, ed_y, 0x88AAFF00);
			fl_color(FL_BLACK);
			fl_line_style(FL_SOLID, 5);
			fl_line(st_x * pixels_per_grid, st_y * pixels_per_grid, ed_x * pixels_per_grid, ed_y * pixels_per_grid);
		}

		void draw() override {
			if (!redraw_flag) {
				return;
			}
			//fl_rectf(x(), y(), w(), h(), FL_WHITE);
			if (has_temp) {
				draw_temp_line();
			}
			redraw_flag = false;
		}

		Fl_Barrier(int x_, int y_, int w_, int h_, int g_size) : Fl_Widget(x_, y_, w_, h_), Barrier(w_ / g_size, h_ / g_size), pixels_per_grid(g_size) {}

		void draw_pos(int cx, int cy, Fl_Color c, bool in_offscreen = false) {
			if (in_offscreen) {
				fl_rectf(cx * pixels_per_grid, cy * pixels_per_grid, pixels_per_grid, pixels_per_grid, c);
			} else {
				fl_rectf(x() + cx * pixels_per_grid, y() + cy * pixels_per_grid, pixels_per_grid, pixels_per_grid, c);
			}
		}

        int handle_add_line(int event, Fl_Grid* g, double grid_x, double grid_y) {
            switch (event) {
            case FL_ENTER: {
                return 1;
            }
            case FL_MOVE: {
                if (has_temp) {
                    ed_x = grid_x;
                    ed_y = grid_y;
                }
                return 1;
            }
            case FL_LEAVE: {
                if (has_temp) {
                    ed_x = st_x;
                    ed_y = st_y;
                }
                return 1;
            }
            case FL_PUSH: {
#ifdef _DEBUG
                std::cout << "Crusor x = " << grid_x << ", y = " << grid_y << std::endl;
#endif
                return 1;
            }
            case FL_RELEASE: {
                if (!Fl::event_is_click()) {
                    return 0;
                }
                switch (Fl::event_button()) {
                case FL_LEFT_MOUSE: {
                    if (has_temp) {
                        add_barrier_for(g);
                        g->show_border = false;
                    } else {
                        has_temp = true;
                        ed_x = st_x = grid_x;
                        ed_y = st_y = grid_y;
                        g->show_border = true;
                    }
                    break;
                }
                case FL_RIGHT_MOUSE: {
                    if (has_temp) {
                        return 0;
                    }
                    remove_barrier_for(g, grid_x, grid_y);
                    break;
                }
                default:
                    break;
                }
                return 1;
            }
            }
            return Fl_Widget::handle(event);
        }
	};
} // namespace grid