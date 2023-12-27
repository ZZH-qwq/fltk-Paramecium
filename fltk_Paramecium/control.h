#pragma once

#include "fl_event_handler.h"

namespace control {

	class Fl_Control : public Fl_Tabs {
	public:
		Fl_Group* environment_control, * paramecium_control, * plot_control;
		int dx = 25, dy = 30;

		Fl_Control(int x_, int y_, int w_, int h_) :Fl_Tabs(x_, y_, w_, h_) {
			callback(tabs_cb);
			this->labelsize(20);
			int curr_y;
			environment_control = new Fl_Group(x_ + dx, y_ + dy, w_ - 2 * dx, h_ - dy - dx, "Environment");
			environment_control->user_data((void*)fl_intptr_t(1));
			curr_y = y_ + dy + 35;

			set_group_indicator(curr_y, 180, "Maze");
			curr_y += 10;
			m->m_size_ip = set_paramecium_slider(curr_y, 1, 5, 2, 16, "Maze Unit Size", nullptr, true);
			curr_y += 55;
			m->seed_ip = new Fl_Int_Input(x_ + dx, curr_y, w_ - 2 * dx, 30, "Random Seed (0 for default)");
			m->seed_ip->align(FL_ALIGN_BOTTOM);
			m->seed_ip->color(FL_LIGHT2);
			m->seed_ip->value("0");
			m->seed_ip->textsize(18);
			m->seed_ip->labelsize(16);
			curr_y += 65;
			m->gen_maze_bt = new Fl_Button(x_ + dx, curr_y, w_ - 2 * dx, 40, "Generate Maze");
			m->gen_maze_bt->color(FL_LIGHT2);
			m->gen_maze_bt->labelsize(18);
			m->gen_maze_bt->callback(generate_maze_cb);
			curr_y += 90;

			handler->switch_indicator = set_group_indicator(curr_y, 110, "Click on Grid to set Barrier");
			curr_y += 10;

			g->clear_barrier_bt = new Fl_Button(x_ + dx, curr_y, w_ - 2 * dx, 40, "Clear Barriers");
			g->clear_barrier_bt->color(FL_LIGHT2);
			g->clear_barrier_bt->labelsize(18);
			g->clear_barrier_bt->callback(clear_env_cb, g);
			curr_y += 50;

			handler->switch_bar_orig_bt = new Fl_Button(x_ + dx, curr_y, w_ - 2 * dx, 40, "Switch to Origin");
			handler->switch_bar_orig_bt->color(FL_LIGHT2);
			handler->switch_bar_orig_bt->labelsize(18);
			handler->switch_bar_orig_bt->callback(switch_bar_orig_cb);

			environment_control->end();

			paramecium_control = new Fl_Group(x_ + dx, y_ + dy, w_ - 2 * dx, h_ - dy - dx, "Paramecium");
			paramecium_control->user_data((void*)fl_intptr_t(2));
			curr_y = y_ + dy + 35;

			kiana->op_bg = set_group_indicator(curr_y, 145, "0 Samples Simulated");
			curr_y += 10;
			kiana->score_op = new Fl_Output(x_ + dx, curr_y, w_ - 2 * dx, 40, "Adaptation Score");
			kiana->score_op->align(FL_ALIGN_BOTTOM);
			kiana->score_op->value(" 0.000");
			kiana->score_op->textsize(20);
			kiana->score_op->labelsize(16);
			curr_y += 70;
			kiana->rate_op = new Fl_Output(x_ + dx, curr_y, w_ - 2 * dx, 40, "Arrival Rate");
			kiana->rate_op->align(FL_ALIGN_BOTTOM);
			kiana->rate_op->value(" 0.000%");
			kiana->rate_op->textsize(20);
			kiana->rate_op->labelsize(16);
			curr_y += 90;

			kiana->reset_pos_bt = new Fl_Button(x_ + dx, curr_y, w_ - 2 * dx, 40, "Reset Position");
			kiana->reset_pos_bt->color(FL_LIGHT2);
			kiana->reset_pos_bt->callback(paramecium::reset_pos_cb, (void*)kiana);
			kiana->reset_pos_bt->labelsize(18);
			kiana->reset_pos_bt->tooltip("Click in main area to set position.");
			curr_y += 50;
			kiana->default_val_bt = new Fl_Button(x_ + dx, curr_y, w_ - 2 * dx, 40, "Default Arguments");
			kiana->default_val_bt->color(FL_LIGHT2);
			kiana->default_val_bt->callback(paramecium::default_val_cb, (void*)kiana);
			kiana->default_val_bt->labelsize(18);
			curr_y += 80;

			set_group_indicator(curr_y, 185, "Basic");
			curr_y += 10;
			kiana->total_energy_ip = set_paramecium_slider(curr_y, 50, 500, 50, 2500, "Total Energy", paramecium::total_energy_cb);
			curr_y += 65;
			kiana->step_len_ip = set_paramecium_slider(curr_y, 0.01, 0.5, 0.01, 1, "Step Length", paramecium::step_len_cb);
			kiana->rotate_rad_ip = set_paramecium_slider(curr_y + 55, 0.01, 0.5, -M_PI, M_PI, "Rotate Radius", paramecium::rotate_rad_cb);
			curr_y += 140;
			set_group_indicator(curr_y, 120, "Deviation");
			curr_y += 10;
			kiana->deviation_m_ip = set_paramecium_slider(curr_y, 0.01, 0, -0.25, 0.25, "Deviation Mean", paramecium::deviation_m_cb);
			kiana->deviation_v_ip = set_paramecium_slider(curr_y + 55, 0.01, 0.1, 0.01, 0.5, "Deviation Various", paramecium::deviation_v_cb);
			curr_y += 140;

			paramecium_control->end();

			plot_control = new Fl_Group(x_ + dx, y_ + dy, w_ - 2 * dx, h_ - dy - dx, "Plotting");
			plot_control->user_data((void*)fl_intptr_t(3));
			curr_y = y_ + dy + 35;

			plt->samples_ip = set_plot_slider(curr_y, 50, 250, 50, 2500, "Samples per Unit", 0, true);
			curr_y += 60;
			plt->confirm_plot_bt = new Fl_Button(x_ + dx, curr_y, w_ - 2 * dx, 40, "Confirm Arguments");
			plt->confirm_plot_bt->color(FL_LIGHT2);
			plt->confirm_plot_bt->callback(paramecium::confirm_plot_cb, (void*)plt);
			plt->confirm_plot_bt->labelsize(18);
			curr_y += 75;
			set_group_indicator(curr_y, 165, "Basic");
			curr_y += 10;
			plt->total_energy_ip = set_plot_slider(curr_y, 50, 500, 50, 2500, "Total Energy", 0);
			curr_y += 55;
			plt->step_len_ip = set_plot_slider(curr_y, 0.01, 0.5, 0.01, 1, "Step Length", 1);
			plt->rotate_rad_ip = set_plot_slider(curr_y + 50, 0.01, 0.5, -M_PI, M_PI, "Rotate Radius", 2);
			curr_y += 130;
			set_group_indicator(curr_y, 110, "Deviation");
			curr_y += 10;
			plt->deviation_m_ip = set_plot_slider(curr_y, 0.01, 0, -0.25, 0.25, "Deviation Mean", 3);
			plt->deviation_v_ip = set_plot_slider(curr_y + 50, 0.01, 0.1, 0.01, 0.5, "Deviation Various", 4);
			curr_y += 120;

			plot_control->end();
			end();
		}

		static void init(int x, int y, int w, int h, int g_size) {
			g = new grid::Fl_Grid(x, y, w, h, g_size);
			m = new grid::Maze(w / g_size, h / g_size);
			bar = new grid::Fl_Barrier(x, y, w, h, g_size);
			orig = new grid::Fl_Origin(x, y, w, h, g_size);
			kiana = new paramecium::Fl_Paramecium(x, y, w, h, g_size);
			kiana->g = g;
			kiana->hide();
			plt = new paramecium::Fl_Plot(x, y, w, h, g_size * 2);
			plt->p = kiana;
			plt->hide();
			handler = new Fl_Event_Handler(x, y, w, h, g_size);
			control = new Fl_Control(w, y, 300, h);
			plt->sync();
			generate_maze_cb(nullptr, nullptr);
		}

		Fl_Box* set_group_indicator(int y_, int h_, const char* t) {
			Fl_Box* b = new Fl_Box(x() + dx - 10, y_, w() - 2 * dx + 20, h_, t);
			b->align(FL_ALIGN_TOP);
			b->labelsize(16);
			b->labelcolor(0x66666600);
			b->labeltype(FL_ENGRAVED_LABEL);
			b->box(FL_PLASTIC_DOWN_BOX);
			b->color(FL_LIGHT2);
			return b;
		}

		Fl_Hor_Value_Slider* set_paramecium_slider(int y_, double step, double init, double mi, double ma, const char* t, Fl_Callback* c, bool skip = false) {
			auto s = new Fl_Hor_Value_Slider(x() + dx, y_, w() - 2 * dx, 30, t);
			s->align(FL_ALIGN_BOTTOM);
			s->color(FL_LIGHT2);
			s->step(step);
			s->value(init);
			s->bounds(mi, ma);
			s->textsize(14);
			s->labelsize(16);
			if (!skip) {
				s->callback(c, (void*)kiana);
			}
			return s;
		}

		Fl_Hor_Value_Slider* set_plot_slider(int y_, double step, double init, double mi, double ma, const char* t, int argc, bool skip = false) {
			auto s = new Fl_Hor_Value_Slider(x() + dx, y_, w() - 2 * dx, 25, t);
			s->align(FL_ALIGN_BOTTOM);
			s->color(FL_LIGHT2);
			s->step(step);
			s->value(init);
			s->bounds(mi, ma);
			s->textsize(14);
			s->labelsize(16);
			if (!skip) {
				s->callback(plot_slider_cb, (void*)(fl_intptr_t)argc);
			}
			return s;
		}


		static void tabs_cb(Fl_Widget* w, void*) {
			Fl_Tabs* tabs = (Fl_Tabs*)w;
			// When tab changed, make sure it has same color as its group
			int target = fl_intptr_t(tabs->value()->user_data());
			switch (target) {
			case 1:
#ifdef _DEBUG
				std::cout << "Handleing environment" << std::endl;
#endif // _DEBUG
				handler->target = handler->prev_target;
				kiana->enable_simulate = false;
				kiana->hide();
				g->show();
				kiana->has_temp = false;
				g->stabled = false;
				if (handler->target == Fl_Event_Handler::Barrier) {
					bar->show();
				} else {
					orig->show();
				}
				plt->hide();
				break;
			case 2:
#ifdef _DEBUG
				std::cout << "Handleing paramecium" << std::endl;
#endif // _DEBUG
				handler->target = Fl_Event_Handler::Paramecium;
				kiana->enable_simulate = true;
				kiana->show();
				g->show();
				g->show_border = false;
				g->stabled = true;
				bar->hide();
				orig->hide();
				plt->hide();
				plt->sync_back(true);
				kiana->update_sliders();
				break;
			case 3:
#ifdef _DEBUG
				std::cout << "Handleing plot" << std::endl;
#endif // _DEBUG
				handler->target = Fl_Event_Handler::Plot;
				kiana->enable_simulate = false;
				kiana->hide();
				g->hide();
				bar->hide();
				orig->hide();
				plt->sync();
				plt->show();
				break;
			}
			handler->send_redraw();
		}
	}*control;
} // namespace control