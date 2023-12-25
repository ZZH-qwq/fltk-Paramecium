#pragma once

#include "fl_event_handler.h"

namespace control {

	class Fl_Control : public Fl_Tabs {
	public:
		Fl_Group* Barrier, * Paramecium;
		int dx = 20, dy = 30;

		Fl_Control(int x_, int y_, int w_, int h_) :Fl_Tabs(x_, y_, w_, h_) {
			callback(tabs_cb);
			Barrier = new Fl_Group(x_ + dx, y_ + dy, w_ - 2 * dx, h_ - dy - dx, "Barrier");
			Barrier->user_data((void*)fl_intptr_t(1));
			Barrier->end();
			Paramecium = new Fl_Group(x_ + dx, y_ + dy, w_ - 2 * dx, h_ - dy - dx, "Paramecium");
			Paramecium->user_data((void*)fl_intptr_t(2));
			kiana->score_op = new Fl_Output(x_ + dx, y_ + dy + 40, w_ - 2 * dx, 40, "Adaptation Score");
			kiana->score_op->align(FL_ALIGN_TOP);
			kiana->score_op->value("0.000");
			kiana->score_op->textsize(20);
			kiana->rate_op = new Fl_Output(x_ + dx, y_ + dy + 100, w_ - 2 * dx, 40, "Arrival Rate");
			kiana->rate_op->align(FL_ALIGN_TOP);
			kiana->rate_op->value("0.000%");
			kiana->rate_op->textsize(20);
			set_paramecium_slider(kiana->step_len_ip, 180, 0.01, 0.5, 0.01, 1, "Step Length", paramecium::step_len_cb);
			set_paramecium_slider(kiana->rotate_rad_ip, 230, 0.01, 0.5, -M_PI, M_PI, "Rotate Radius", paramecium::rotate_rad_cb);
			Paramecium->end();
			end();
		}

		void set_paramecium_slider(Fl_Hor_Value_Slider*& s, int y_, double step, double init, double mi, double ma, const char* t, Fl_Callback* c) {
			s = new Fl_Hor_Value_Slider(x() + dx, y() + dy + y_, w() - 2 * dx, 30, t);
			s->align(FL_ALIGN_TOP);
			s->color(FL_LIGHT2);
			s->step(step);
			s->value(init);
			s->bounds(mi, ma);
			s->textsize(14);
			s->callback(c, (void*)kiana);
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