#pragma once

/*
*  drawings.h
* 
*  Useful tools for drawing things, including color mapping, shapes, etc.
*  Needs to be improved as more features are added
*/

namespace draw {

	Fl_Color rainbow_linear_gradient(double s) {
		if (s < 0.25) {
			return fl_rgb_color(255, 510 * s + 128, 128);
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