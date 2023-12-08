#include <FL/Fl.H>
#include <FL/Enumerations.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_RGB_Image.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H> 

#include <vector>
#include <list>
#include <set>
#include <tuple>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <cassert>

#include "fl_event_handler.h"


void timeout_cb(void*) {
    if (!control::g->queue.empty()) {
        control::g->bfs_step();
    }
    control::g->step_flag = true;
    Fl::repeat_timeout(0.001, timeout_cb);
}

int main() {
    const int w = 800, h = 800, x = 0, y = 0, g_size = 20;

    control::win = new Fl_Double_Window(w, h, "Paramecium");
    control::g = new grid::Fl_Grid(x, y, w, h, g_size);
    control::bar = new grid::Fl_Barrier(x, y, w, h, g_size);
    control::handler = new control::Fl_Event_Handler(x, y, w, h, g_size);
    control::win->end();
    control::win->show();
    Fl::add_timeout(0.05, timeout_cb);
    while (true) {
        /*if (!g->queue.empty()) {
            g->step_flag = true;
        }*/
        control::g->redraw();
        control::bar->redraw();
        auto nWin = Fl::wait();
        if (nWin == 0) {
            break;
        }
    }
    return 0;
}