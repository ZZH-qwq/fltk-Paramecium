#pragma once

/*
*  fl_event_handler.h
* 
*  Assign the corresponding event according to the current operation target
*/

#include "fl_grid.h"
#include "fl_barrier.h"

namespace control {

    Fl_Double_Window* win;
    grid::Fl_Barrier* bar;
    grid::Fl_Grid* g;

    class Fl_Event_Handler : public Fl_Box {
    public:
        size_t grid_w, grid_h;
        int pixels_per_grid;

        enum Event_Target { None, Barrier } target = Barrier;

        Fl_Event_Handler(int x_, int y_, int w_, int h_, int g_size) : Fl_Box(x_, y_, w_, h_),
            grid_w(w_ / g_size), grid_h(h_ / g_size), pixels_per_grid(g_size) {
        }

        int handle(int event) override {
            switch (target) {
            case Barrier:
                if (bar->handle_add_line(event, g, grid_x(), grid_y()) == 1) {
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
            case 1: {
                bar->redraw_flag = true;
                g->redraw_flag = true;
                break;
            }
            default:
                break;
            }
        }

        double grid_x() { return (Fl::event_x() - x()) / (double)pixels_per_grid; }
        double grid_y() { return (Fl::event_y() - y()) / (double)pixels_per_grid; }
    }*handler;
} // namespace control