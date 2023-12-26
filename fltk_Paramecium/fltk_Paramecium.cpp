#include <FL/Fl.H>
#include <FL/Enumerations.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Tabs.H>
#include <FL/Fl_RGB_Image.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Float_Input.H>
#include <FL/Fl_Hor_Value_Slider.H>
#include <FL/fl_draw.H> 

#include <vector>
#include <list>
#include <deque>
#include <set>
#include <tuple>
#include <string>
#include <iostream>
#include <sstream>
#include <cmath>
#include <random>
#include <algorithm>
#include <cassert>
#include <climits>

#include "control.h"

 
static void timeout_cb(void*) {
    if (!control::g->queue.empty()) {
        control::g->step_flag = true;
    } else if (!control::kiana->has_temp && control::kiana->enable_simulate) {
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

    control::win = new Fl_Double_Window(w + 300, h, "Paramecium");
    control::win->color(FL_LIGHT3);
    control::g = new grid::Fl_Grid(x, y, w, h, g_size);
    control::bar = new grid::Fl_Barrier(x, y, w, h, g_size);
    control::orig = new grid::Fl_Origin(x, y, w, h, g_size);
    control::kiana = new paramecium::Fl_Paramecium(x, y, w, h, g_size);
    control::kiana->g = control::g;
    control::plt = new paramecium::Fl_Plot(x, y, w, h, g_size * 2);
    control::plt->p = control::kiana;
    control::plt->hide();
    control::handler = new control::Fl_Event_Handler(x, y, w, h, g_size);
    control::control = new control::Fl_Control(w, y, 300, h);
    control::plt->sync();
    control::win->end();
    control::win->show();
    Fl::add_timeout(0.05, timeout_cb);
    while (true) {
        /*if (!g->queue.empty()) {
            g->step_flag = true;
        }*/
        control::g->redraw();
        control::bar->redraw();
        control::orig->redraw();
        control::kiana->redraw();
        control::plt->redraw();
        control::control->redraw();
        auto nWin = Fl::wait();
        if (nWin == 0) {
            break;
        }
    }
    return 0;
}