#include <FL/Fl.H>
#include <FL/Enumerations.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_RGB_Image.H>
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H> 

#include <vector>
#include <list>
#include <deque>
#include <set>
#include <tuple>
#include <iostream>
#include <cmath>
#include <random>
#include <algorithm>
#include <cassert>

#include "control.h"


static void timeout_cb(void*) {
    if (!control::g->queue.empty()) {
        control::g->step_flag = true;
    } else if (!control::kiana->has_temp) {
        control::g->redraw_flag = true;
        control::kiana->step_flag = true;
    }
    Fl::repeat_timeout(0.001, timeout_cb);
}

int main() {
    Fl::scheme("gtk+");
    Fl::background(233, 233, 233);
    Fl::visual(FL_DOUBLE | FL_RGB);
    const int w = 800, h = 800, x = 0, y = 0, g_size = 20;

    control::win = new Fl_Double_Window(w + 200, h, "Paramecium");
    control::win->color(FL_LIGHT3);
    control::g = new grid::Fl_Grid(x, y, w, h, g_size);
    control::bar = new grid::Fl_Barrier(x, y, w, h, g_size);
    control::kiana = new paramecium::Fl_Paramecium(x, y, w, h, g_size);
    control::kiana->g = control::g;
    control::handler = new control::Fl_Event_Handler(x, y, w, h, g_size);
    control::control = new control::Fl_Control(w, y, 200, h);
    control::win->end();
    control::win->show();
    Fl::add_timeout(0.05, timeout_cb);
    while (true) {
        /*if (!g->queue.empty()) {
            g->step_flag = true;
        }*/
        control::g->redraw();
        control::bar->redraw();
        control::kiana->redraw();
        auto nWin = Fl::wait();
        if (nWin == 0) {
            break;
        }
    }
    return 0;
}