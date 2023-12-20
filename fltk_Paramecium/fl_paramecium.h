#pragma once

#include "drawings.h"
#include "paramecium.h"

namespace paramecium {

	class Fl_Paramecium : public Fl_Widget, public Paramecium {
	public:
		double px, py, rad, temp_x, temp_y, temp_rad;
        int steps_completed = 0, updates = 0;

        int pixels_per_grid, updates_per_step = 10;
        double update_factor = 40;

		Fl_Paramecium(int x_, int y_, int w_, int h_, int g_size) : Fl_Widget(x_, y_, w_, h_), pixels_per_grid(g_size) {
            temp_x = temp_y = px = py = 20;
			rad = 0;
		}

		void draw() {
			if (!redraw_flag) {
				return;
			}
            if (has_temp) {
                draw::draw_paramecium(temp_x * pixels_per_grid, temp_y * pixels_per_grid, rad, pixels_per_grid);
                return;
            }
			if (!enable_simulate) {
                draw::draw_paramecium(px * pixels_per_grid, py * pixels_per_grid, rad, pixels_per_grid);
				return;
			}
			if (resimulate_flag) {
				steps.clear();
				simulate({ Forward,0,px,py,init_r(e)});
                steps_completed = 0;
				resimulate_flag = false;
			} else if (!steps.empty()) {
				simulate(steps.back());
			}
			fl_color(FL_BLUE);
            int c = 0;
            double sx = px * pixels_per_grid, sy = py * pixels_per_grid, sz = std::min(min_steps, 2 * steps_completed);
			for (auto& s : steps) {
                fl_color(draw::path_linear_gradient(c / sz));
                fl_line_style(FL_SOLID, 3);
				fl_line(sx,sy, s.x * pixels_per_grid, s.y * pixels_per_grid);
                sx = s.x * pixels_per_grid, sy = s.y * pixels_per_grid;
                if (++c > sz) {
                    break;
                }
			}
            if (step_flag && !steps.empty()) {
                px = (steps.front().x * updates + temp_x * (updates_per_step - updates)) / updates_per_step;
                py = (steps.front().y * updates + temp_y * (updates_per_step - updates)) / updates_per_step;
                rad = (steps.front().rad * updates + temp_rad * (updates_per_step - updates)) / updates_per_step;
                if (++updates > updates_per_step) {
                    temp_x = steps.front().x, temp_y = steps.front().y, temp_rad = steps.front().rad;
                    steps.pop_front();
                    ++steps_completed;
                    updates_per_step = update_factor * (exp(-std::min((int)steps.size(), std::min(min_steps, 2 * steps_completed)) / 50.0) + 0.1);
                    updates = 0;
                }
            }
            draw::draw_paramecium(px * pixels_per_grid, py * pixels_per_grid, rad, pixels_per_grid);
		}

        int handle_place_paramecium(int event, grid::Fl_Grid* g, double grid_x, double grid_y) {
            switch (event) {
            case FL_ENTER: {
                return 1;
            }
            case FL_MOVE: {
                if (has_temp) {
                    temp_x = grid_x;
                    temp_y = grid_y;
                    return 1;
                }
                return 0;
            }
            case FL_LEAVE: {
                if (has_temp) {
                    temp_x = px;
                    temp_y = py;
                    return 1;
                }
                return 0;
            }
            case FL_PUSH: {
#ifdef _DEBUG
                std::cout << "Crusor x = " << grid_x << ", y = " << grid_y << ", dist = " << g->distance[grid_x][grid_y] << std::endl;
#endif
                return 1;
            }
            case FL_RELEASE: {
                if (!Fl::event_is_click()) {
                    return 0;
                }
                if (has_temp) {
                    px = temp_x;
                    py = temp_y;
                    g->show_border = false;
                    has_temp = false;
                    resimulate_flag = true;
                } else {
                    has_temp = true;
                    temp_x = grid_x;
                    temp_y = grid_y;
                    g->show_border = true;
                }
                return 1;
            }
            }
            return Fl_Widget::handle(event);
        }
	};
} // namespace paramecium