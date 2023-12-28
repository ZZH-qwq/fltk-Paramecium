#pragma once

/*
*  drawings.h
* 
*  Useful tools for drawing things, including color mapping, shapes, etc.
*  Needs to be improved as more features are added
*/

namespace draw {

	double rads[] = { std::numbers::pi/2,std::numbers::pi,23 * std::numbers::pi / 12,0,std::numbers::pi / 6,std::numbers::pi / 3,3 * std::numbers::pi / 4,5 * std::numbers::pi / 6,std::numbers::pi ,13 * std::numbers::pi / 12,5 * std::numbers::pi / 4,4 * std::numbers::pi / 3,7 * std::numbers::pi / 4,7 * std::numbers::pi / 6,13 * std::numbers::pi / 12 };
	double lens[] = { 0,0,0.9,1,0.8,0.5,0.5,0.7,1,0.8,0.5,0.3,0.5,0.2,0.3,0.2,0.1,0.4,0.6,0.5 };

	void draw_paramecium_vertex(double x, double y, double r, double size) {
		double dx1 = cos(r) * 0.2, dy1 = sin(r) * 0.2;
		for (int i = 2; i < 13; i++) {
			double dx = (cos(r - rads[i]) * lens[i] - dx1) * size, dy = (sin(r - rads[i]) * lens[i] - dy1) * size;
			fl_vertex(x + dx, y + dy);
		}
	}

	void draw_paramecium(double x, double y, double r, double size, double back = 0) {
		fl_color(fl_rgb_color(0x99 + 0x33 * back, 0xff - 0x66 * back, 0x99));
		fl_begin_polygon();
		draw_paramecium_vertex(x, y, r, size);
		fl_end_polygon();
		fl_color(fl_rgb_color(0x33 + 0x99 * back, 0xcc - 0x99 * back, 0x66));
		fl_line_style(FL_SOLID, 3);
		fl_begin_loop();
		draw_paramecium_vertex(x, y, r, size);
		fl_end_loop();
		fl_color(fl_rgb_color(0x66 + 0x66 * back, 0xcc - 0x66 * back, 0x66));
		double dx1 = cos(r) * 0.2, dy1 = sin(r) * 0.2;
		fl_begin_polygon();
		for (int i = 11; i < 15; i++) {
			double dx = (cos(r - rads[i]) * lens[i] - dx1) * size, dy = (sin(r - rads[i]) * lens[i] - dy1) * size;
			fl_vertex(x + dx, y + dy);
		}
		fl_end_polygon();
		fl_color(fl_rgb_color(0x99 + 0x66 * back, 0xff - 0x99 * back, 0x66));
		fl_begin_polygon();
		for (int i = 0; i < 5; i++) {
			double dx = (cos(r - rads[i]) * lens[i + 15] - dx1) * size, dy = (sin(r - rads[i]) * lens[i + 15] - dy1) * size;
			fl_vertex(x + dx, y + dy);
		}
		fl_end_polygon();
	}

	void draw_paramecium_indicator(double x, double y, double r, double size) {
		fl_color(0x00993300);
		fl_line_style(FL_DOT, 2);
		fl_begin_loop();
		draw_paramecium_vertex(x, y, r, size);
		fl_end_loop();
	}

	void draw_endpoint_indicator(double x, double y, double size, double lev = 1) {
		fl_color(fl_rgb_color(0xff, 0x99 + 0x66 * lev, 0x99 + 0x66 * lev));
		fl_line_style(FL_SOLID, 3);
		fl_line(x - size, y - size, x + size, y + size);
		fl_line(x - size, y + size, x + size, y - size);
	}

	Fl_Color path_linear_gradient(double s) {
		return fl_rgb_color(255 * s, 255 * s, 255);
	}

	Fl_Color rainbow_linear_gradient(double s) {
		if (s < 0.125) {
			return fl_rgb_color(255, 510 * s + 128, 128);
		} else if (s < 0.25) {
			return fl_rgb_color(255 - 128 * (s - 0.125), 510 * s + 128 - 128 * (s - 0.125), 128);
		} else if (s < 0.375) {
			return fl_rgb_color(510 * (0.5 - s) + 128 - 128 * (0.375 - s), 255 - 128 * (0.375 - s), 128);
		} else if (s < 0.5) {
			return fl_rgb_color(510 * (0.5 - s) + 128, 255, 128);
		} else if (s < 0.75) {
			return fl_rgb_color(128, 255, 510 * (s - 0.5) + 128);
		} else {
			return fl_rgb_color(128, 510 * (1 - s) + 128, 255);
		}
	}

	Fl_Color rainbow_linear_gradient_base(double s) {
		if (s < 0.2) {
			s = s / 2;
		} else if (s < 0.8) {
			s = s / 6 * 8 - 1.0 / 6;
		} else {
			s = s / 2 + 0.5;
		}
		if (s < 0.125) {
			return fl_rgb_color(255, 1020 * s, 0);
		} else if (s < 0.25) {
			return fl_rgb_color(255 - 256 * (s - 0.125), 1020 * s - 256 * (s - 0.125), 256 * (s - 0.125));
		} else if (s < 0.375) {
			return fl_rgb_color(1020 * (0.5 - s) - 256 * (0.375 - s), 255 - 256 * (0.375 - s), 256 * (s - 0.125));
		} else if (s < 0.5) {
			return fl_rgb_color(1020 * (0.5 - s), 255, 0);
		} else if (s < 0.75) {
			return fl_rgb_color(0, 255, 1020 * (s - 0.5));
		} else {
			return fl_rgb_color(0, 1020 * (1 - s), 255);
		}
	}

	class Fl_Gradient : public Fl_Widget {
	public:
		Fl_Image* gradient_image = nullptr;

		int dx = 0, text_w = 30, img_w, img_h;
		double red = 0, blue = 0;

		Fl_Gradient(int x_, int y_, int w_, int h_, int g_h) :Fl_Widget(x_, y_, w_, h_) {
			img_w = w_ - 2 * dx, img_h = g_h;
			uchar* data = new uchar[img_w * 3];
			for (int i = 0; i < img_w; i++) {
				Fl_Color c = rainbow_linear_gradient((double)i / img_w);
				data[3 * i] = c >> 24;
				data[3 * i + 1] = c >> 16;
				data[3 * i + 2] = c >> 8;
			}
			Fl_RGB_Image img(data, img_w, 1, 3);
			gradient_image = img.copy(img_w, img_h);
			delete[] data;
		}

		void draw() override {
			fl_rectf(x(), y(), w(), h(), FL_BACKGROUND_COLOR);
			gradient_image->draw(x() + dx, y());
			fl_font(0, h() - img_h);
			fl_color(0x66666600);
			fl_draw(std::to_string(red).c_str(), x() + dx, y() + img_h, text_w, h() - img_h, FL_ALIGN_LEFT);
			fl_draw(std::to_string(blue).c_str(), x() + w() - text_w - dx, y() + img_h, text_w, h() - img_h, FL_ALIGN_RIGHT);
		}
	};

	class Fl_Axis : public Fl_Widget {
	public:
		double mins[2] = { 0.05, -2 }, maxs[2] = { 0.95,3 };
		int text_h, text_w = 30;
		const char* tags[2] = { "Step Length","Rotate Radius" };

		Fl_Axis(int x_, int y_, int w_, int h_, int t_h) :Fl_Widget(x_, y_, w_, h_), text_h(t_h) {}

		void draw() {
			fl_rectf(x(), y(), w(), h(), FL_LIGHT3);
			fl_color(0x66666600);
			fl_line_style(FL_SOLID, 3);
			fl_line(x() + 10, y() + 30, x() + 10, y() + h() - 10);
			fl_line(x() + 30, y() + 10, x() + w() - 10, y() + 10);
			fl_line_style(FL_SOLID, 2);
			fl_line(x() + 5, y() + h() - 15, x() + 10, y() + h() - 10, x() + 15, y() + h() - 15);
			fl_line(x() + w() - 15, y() + 5, x() + w() - 10, y() + 10, x() + w() - 15, y() + 15);
			fl_font(0, text_h);
			fl_draw(std::to_string(mins[0]).substr(0, 7).c_str(), x() + 30, y() + 15, text_w, text_h, FL_ALIGN_LEFT);
			fl_draw(std::to_string(maxs[0]).substr(0, 7).c_str(), x() + w() - text_w - 10, y() + 15, text_w, text_h, FL_ALIGN_RIGHT);
			fl_draw(std::to_string(mins[1]).substr(0, 7).c_str(), x() + 15, y() + 38, text_w, text_h, FL_ALIGN_LEFT);
			fl_draw(std::to_string(maxs[1]).substr(0, 7).c_str(), x() + 15, y() + h() - text_h - 18, text_w, text_h, FL_ALIGN_LEFT);
			fl_draw(90, tags[1], x() + 30, y() + (h() + fl_width(tags[1])) / 2 + 10);
			fl_draw(tags[0], x() + (w() - fl_width(tags[0])) / 2 + 10, y() + 30);
		}
	};
} // namespace draw