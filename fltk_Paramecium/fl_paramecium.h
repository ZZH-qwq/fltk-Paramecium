#pragma once

#include "drawings.h"
#include "paramecium.h"

namespace paramecium {

    class Fl_Paramecium : public Fl_Widget, public Paramecium {
    public:
        double px, py, rad, temp_x, temp_y, temp_rad;
        int steps_completed = 0, updates = 0;
        int curr_back = 0;

        int pixels_per_grid, updates_per_step = 10, back_cd = 6;
        double update_factor = 20;

        Fl_Box* op_bg;
        Fl_Output* score_op, * rate_op;
        Fl_Button* reset_pos_bt, * default_val_bt;
        Fl_Hor_Value_Slider* total_energy_ip, * step_len_ip, * rotate_rad_ip, * deviation_m_ip, * deviation_v_ip;

        Fl_Paramecium(int x_, int y_, int w_, int h_, int g_size) : Fl_Widget(x_, y_, w_, h_), pixels_per_grid(g_size) {
            temp_x = temp_y = px = py = 20;
            temp_rad = rad = 0;
            op_bg = nullptr;
            score_op = rate_op = nullptr;
            reset_pos_bt = default_val_bt = nullptr;
            total_energy_ip = step_len_ip = rotate_rad_ip = deviation_m_ip = deviation_v_ip = nullptr;
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
                simulate_steps({ Forward,0,px,py,init_r(e) });
                steps_completed = 0;
                resimulate_flag = false;
            } else if (!steps.empty()) {
                simulate_steps(steps.back());
            }
            fl_color(FL_BLUE);
            int c = 0;
            double sx = px * pixels_per_grid, sy = py * pixels_per_grid, sz = std::min(min_list_len, 2 * steps_completed);
            for (auto& s : steps) {
                fl_color(draw::path_linear_gradient(c / sz));
                fl_line_style(FL_SOLID, 3);
                fl_line(sx, sy, s.x * pixels_per_grid, s.y * pixels_per_grid);
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
                    if (!steps.empty() && steps.front().type == Backward) {
                        steps.pop_front();
                        ++steps_completed;
                        curr_back = back_cd;
                    } else if (curr_back > 0) {
                        curr_back--;
                    }
                    updates_per_step = update_factor * (1.2 - std::min((int)steps.size(), std::min(min_list_len, 2 * steps_completed)) / (double)min_list_len) * step_length + 1;
                    updates = 0;
                }
            }
            if (step_flag && steps.empty()) {
                reset_pos();
                resimulate_flag = true;
            }
            auto sim = simulate_score();
            score_sum += sim.score;
            simulation_count++;
            if (simulation_count % 25 == 0) {
                op_bg->copy_label((std::to_string(simulation_count) + " Samples Simulated").c_str());
                score_op->value((" " + std::to_string(score_sum / simulation_count)).c_str());
                rate_op->value((" " + std::to_string(100.0 * success_count / simulation_count) + " %").c_str());
            }
            if (sim.result == Found) {
                ++success_count;
            }
#ifdef _DEBUG
            //std::cout << (sim.result == Found ? "Found\t" : "Failed\t") << sim.score << "\t" << sim.energy_used << std::endl;
            //std::cout << score_sum / simulation_count << "\t" << (double)success_count / simulation_count << std::endl;
#endif // _DEBUG
            draw::draw_paramecium_indicator(st_x * pixels_per_grid, st_y * pixels_per_grid, st_rad, pixels_per_grid);
            draw::draw_paramecium(px * pixels_per_grid, py * pixels_per_grid, rad, pixels_per_grid, curr_back / (double)back_cd);
        }

        void reset_pos() { px = st_x, py = st_y, rad = init_r(e); }

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
                if (!Fl::event_is_click() || !(Fl::event_button() == FL_LEFT_MOUSE)) {
                    return 0;
                }
                if (has_temp) {
                    st_x = temp_x;
                    st_y = temp_y;
                    st_rad = rad;
                    reset_status();
                    reset_pos();
                    g->show_border = false;
                    has_temp = false;
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

    static void total_energy_cb(Fl_Widget* o, void* v) {
        Fl_Slider* s = (Fl_Slider*)o;
        Fl_Paramecium* p = (Fl_Paramecium*)v;
        p->total_energy = s->value();
        p->reset_status();
        p->reset_pos();
    }

    static void step_len_cb(Fl_Widget* o, void* v) {
        Fl_Slider* s = (Fl_Slider*)o;
        Fl_Paramecium* p = (Fl_Paramecium*)v;
        p->step_length = s->value();
        p->reset_status();
        p->reset_pos();
    }

    static void rotate_rad_cb(Fl_Widget* o, void* v) {
        Fl_Slider* s = (Fl_Slider*)o;
        Fl_Paramecium* p = (Fl_Paramecium*)v;
        p->rotate_radius = s->value();
        p->reset_status();
        p->reset_pos();
    }

    static void reset_pos_cb(Fl_Widget* o, void* v) {
        Fl_Paramecium* p = (Fl_Paramecium*)v;
        p->reset_pos();
        p->resimulate_flag = true;
    }

    static void default_val_cb(Fl_Widget* o, void* v) {
        Fl_Paramecium* p = (Fl_Paramecium*)v;
        p->total_energy_ip->value(500);
        p->total_energy = 500;
        p->step_len_ip->value(0.5);
        p->step_length = 0.5;
        p->rotate_rad_ip->value(0.5);
        p->rotate_radius = 0.5;
        p->deviation_m_ip->value(0);
        p->deviation_mean = 0;
        p->deviation_v_ip->value(0.01);
        p->deviation_variance = 0.1;
        p->dr = std::normal_distribution<double>(p->deviation_mean, p->deviation_variance);
        p->reset_status();
        p->reset_pos();
        p->resimulate_flag = true;
    }

    static void deviation_m_cb(Fl_Widget* o, void* v) {
        Fl_Slider* s = (Fl_Slider*)o;
        Fl_Paramecium* p = (Fl_Paramecium*)v;
        p->deviation_mean = s->value();
        p->dr = std::normal_distribution<double>(p->deviation_mean, p->deviation_variance);
        p->reset_status();
        p->reset_pos();
    }

    static void deviation_v_cb(Fl_Widget* o, void* v) {
        Fl_Slider* s = (Fl_Slider*)o;
        Fl_Paramecium* p = (Fl_Paramecium*)v;
        p->deviation_variance = s->value();
        p->dr = std::normal_distribution<double>(p->deviation_mean, p->deviation_variance);
        p->reset_status();
        p->reset_pos();
    }
} // namespace paramecium