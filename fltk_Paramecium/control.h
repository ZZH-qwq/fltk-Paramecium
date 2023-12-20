#pragma once

#include "fl_event_handler.h"

namespace control {

	class Fl_Control : public Fl_Tabs {
	public:
		Fl_Group* Barrier, * Paramecium;
		Fl_Control(int x_, int y_, int w_, int h_) :Fl_Tabs(x_, y_, w_, h_) {
			int dx = 20, dy = 30;
			callback(tabs_cb);
			Barrier = new Fl_Group(x_ + dx, y_ + dy, w_ - 2 * dx, h_ - dy - dx, "Barrier");
			Barrier->user_data((void*)fl_intptr_t(1));
			Barrier->end();
			Paramecium = new Fl_Group(x_ + dx, y_ + dy, w_ - 2 * dx, h_ - dy - dx, "Paramecium");
			Paramecium->user_data((void*)fl_intptr_t(2));
			Paramecium->end();
			end();
		}

		static void tabs_cb(Fl_Widget* w, void*) {
			Fl_Tabs* tabs = (Fl_Tabs*)w;
			// When tab changed, make sure it has same color as its group
			int target = fl_intptr_t(tabs->value()->user_data());
			switch (target) {
			case 1:
#ifdef _DEBUG
				std::cout << "Handleing barrier" << std::endl;
#endif // _DEBUG
				handler->target = Fl_Event_Handler::Barrier;
				kiana->enable_simulate = false;
				kiana->has_temp = false;
				g->stabled = false;
				bar->show();
				break;
			case 2:
#ifdef _DEBUG
				std::cout << "Handleing paramecium" << std::endl;
#endif // _DEBUG
				handler->target = Fl_Event_Handler::Paramecium;
				kiana->enable_simulate = true;
				g->stabled = true;
				bar->hide();
				break;
			}
			handler->send_redraw();
		}
	}*control;
} // namespace control