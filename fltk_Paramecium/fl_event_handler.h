#pragma once

/*
*  fl_event_handler.h
* 
*  Assign the corresponding event according to the current operation target
*/

#include "fl_grid.h"
#include "fl_barrier.h"
#include "fl_origin.h"
#include "fl_paramecium.h"
#include "fl_plot.h"
#include "maze.h"

namespace control {

    Fl_Double_Window* win;
    grid::Fl_Barrier* bar;
    grid::Fl_Origin* orig;
    paramecium::Fl_Paramecium* kiana;
    paramecium::Fl_Plot* plt;
    grid::Fl_Grid* g;
    grid::Maze* m;

    class Fl_Event_Handler : public Fl_Box {
    public:
        size_t grid_w, grid_h;
        int pixels_per_grid;

        enum Event_Target { None, Barrier, Origin, Paramecium, Plot } target, prev_target;

        Fl_Box* switch_indicator;
        Fl_Button* switch_bar_orig_bt;

        Fl_Event_Handler(int x_, int y_, int w_, int h_, int g_size) : Fl_Box(x_, y_, w_, h_),
            grid_w(w_ / g_size), grid_h(h_ / g_size), pixels_per_grid(g_size) {
            switch_indicator = nullptr;
            switch_bar_orig_bt = nullptr;
            target = prev_target = Barrier;
        }

        int handle(int event) override {
            switch (target) {
            case Barrier:
                if (bar->handle_add_line(event, g, kiana, grid_x(), grid_y()) == 1) {
                    send_redraw();
                    return 1;
                }
                break;
            case Origin:
                if (orig->handle_add_origin(event, g, kiana, grid_x(), grid_y()) == 1) {
                    send_redraw();
                    return 1;
                }
                break;
            case Paramecium:
                if (kiana->handle_place_paramecium(event, g, grid_x(), grid_y()) == 1) {
                    send_redraw();
                    return 1;
                }
                break;
            case Plot:
                if (plt->handle_show_detail(event, grid_x(), grid_y()) == 1) {
                    send_redraw();
                    return 1;
                }
                break;
            default:
                break;
            }
            return Fl_Widget::handle(event);
        }

        void send_redraw() {
            switch (target) {
            case Barrier: {
                bar->redraw_flag = true;
                g->redraw_flag = true;
                kiana->resimulate_flag = true;
                kiana->redraw_flag = true;
                break;
            }
            case Origin: {
                kiana->resimulate_flag = true;
                kiana->redraw_flag = true;
                g->redraw_flag = true;
                orig->redraw_flag = true;
                break;
            }
            case Paramecium: {
                kiana->redraw_flag = true;
                g->redraw_flag = true;
                break;
            }
            case Plot: {
                plt->redraw_flag = true;
                break;
            }
            default: {
                kiana->redraw_flag = true;
                g->redraw_flag = true;
                break;
            }
            }
        }

        double grid_x() const { return (Fl::event_x() - x()) / (double)pixels_per_grid; }
        double grid_y() const { return (Fl::event_y() - y()) / (double)pixels_per_grid; }
    }*handler;

    static void generate_maze_cb(Fl_Widget* o, void* v) {
        m->generate(m->m_size_ip->value(), std::atoi(m->seed_ip->value()));
        g->clear_barrier();
        g->add_barrier(m->get_barriers());
        g->redraw_flag = true;
        g->set_orig(m->get_orig());
        kiana->reset_status();
    }

	static void clear_env_cb(Fl_Widget* o, void* v) {
		grid::Fl_Grid* g = (grid::Fl_Grid*)v;
        if (handler->target==Fl_Event_Handler::Barrier) {
            g->clear_barrier();
        } else {
            g->clear_orig();
        }
        g->redraw_flag = true;
        kiana->reset_status();
        g->clear_status();
	}

    static void switch_bar_orig_cb(Fl_Widget* o, void* v) {
        Fl_Button* s = (Fl_Button*)o;
        if (handler->target == Fl_Event_Handler::Barrier) {
            handler->prev_target = handler->target = Fl_Event_Handler::Origin;
            g->clear_barrier_bt->label("Clear Origins");
            orig->enabled = true;
            orig->show();
            bar->hide();
            handler->switch_indicator->label("Click on Grid to set Origin");
            s->label("Switch to Barrier");
        } else {
            handler->prev_target = handler->target = Fl_Event_Handler::Barrier;
            g->clear_barrier_bt->label("Clear Barriers");
            g->show_border = false;
            orig->enabled = false;
            orig->hide();
            bar->show();
            handler->switch_indicator->label("Click on Grid to set Barrier");
            s->label("Switch to Origin");
        }
        g->redraw_flag = true;
        g->clear_status();
    }

    static void plot_slider_cb(Fl_Widget* o, void* v) {
        Fl_Slider* s = (Fl_Slider*)o;
        plt->args[fl_intptr_t(v)] = s->value();
        plt->sync_back_flag = true;
        plt->update_axis();
    }
} // namespace control