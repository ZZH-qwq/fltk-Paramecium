#pragma once

/*
*  drawings.h
* 
*  Useful tools for drawing things, including color mapping, shapes, etc.
*  Needs to be improved as more features are added
*/

namespace draw {

	void draw_paramecium_vertex(double x, double y, double r, double size) {
		double dx1 = cos(r + 0.5) * size, dx2 = cos(r - 0.5) * size, dy1 = sin(r + 0.5) * size, dy2 = sin(r - 0.5) * size;
		fl_vertex(x + dx1, y + dy1);
		fl_vertex(x + dx2, y + dy2);
		fl_vertex(x - dx1, y - dy1);
		fl_vertex(x - dx2, y - dy2);
	}

	void draw_paramecium(double x, double y, double r, double size, double back = 0) {
		fl_color(fl_rgb_color(0x33 + 0x99 * back, 0xcc - 0x99 * back, 0x66));
		fl_begin_polygon();
		draw_paramecium_vertex(x, y, r, size);
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
		} else if (s > 0.75) {
			return fl_rgb_color(128, 510 * (1 - s) + 128, 255);
		} else {
			return fl_rgb_color(128, 255, 510 * (s - 0.5) + 128);
		}
	}

	Fl_Color rainbow_linear_gradient_base(double s) {
		if (s < 0.25) {
			return fl_rgb_color(255, 1020 * s, 0);
		} else if (s < 0.5) {
			return fl_rgb_color(1020 * (0.5 - s), 255, 0);
		} else if (s > 0.75) {
			return fl_rgb_color(0, 1020 * (1 - s), 255);
		} else {
			return fl_rgb_color(0, 255, 1020 * (s - 0.5));
		}
	}
} // namespace draw