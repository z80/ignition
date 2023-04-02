/**************************************************************************/
/*  slider.cpp                                                            */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "slider.h"

#include "core/os/keyboard.h"

Size2 Slider::get_minimum_size() const {
	Size2i ss = theme_cache.slider_style->get_minimum_size();
	Size2i rs = theme_cache.grabber_icon->get_size();

	if (orientation == HORIZONTAL) {
		return Size2i(ss.width, MAX(ss.height, rs.height));
	} else {
		return Size2i(MAX(ss.width, rs.width), ss.height);
	}
}

void Slider::gui_input(const Ref<InputEvent> &p_event) {
	ERR_FAIL_COND(p_event.is_null());

	if (!editable) {
		return;
	}

	Ref<InputEventMouseButton> mb = p_event;

	if (mb.is_valid()) {
		if (mb->get_button_index() == MouseButton::LEFT) {
			if (mb->is_pressed()) {
				Ref<Texture2D> grabber;
				if (mouse_inside || has_focus()) {
					grabber = theme_cache.grabber_hl_icon;
				} else {
					grabber = theme_cache.grabber_icon;
				}

				grab.pos = orientation == VERTICAL ? mb->get_position().y : mb->get_position().x;

				double grab_width = (double)grabber->get_size().width;
				double grab_height = (double)grabber->get_size().height;
				double max = orientation == VERTICAL ? get_size().height - grab_height : get_size().width - grab_width;
				if (orientation == VERTICAL) {
					set_as_ratio(1 - (((double)grab.pos - (grab_height / 2.0)) / max));
				} else {
					set_as_ratio(((double)grab.pos - (grab_width / 2.0)) / max);
				}
				grab.active = true;
				grab.uvalue = get_as_ratio();

				emit_signal(SNAME("drag_started"));
			} else {
				grab.active = false;

				const bool value_changed = !Math::is_equal_approx((double)grab.uvalue, get_as_ratio());
				emit_signal(SNAME("drag_ended"), value_changed);
			}
		} else if (scrollable) {
			if (mb->is_pressed() && mb->get_button_index() == MouseButton::WHEEL_UP) {
				grab_focus();
				set_value(get_value() + get_step());
			} else if (mb->is_pressed() && mb->get_button_index() == MouseButton::WHEEL_DOWN) {
				grab_focus();
				set_value(get_value() - get_step());
			}
		}
	}

	Ref<InputEventMouseMotion> mm = p_event;

	if (mm.is_valid()) {
		if (grab.active) {
			Size2i size = get_size();
			Ref<Texture2D> grabber = theme_cache.grabber_icon;
			double motion = (orientation == VERTICAL ? mm->get_position().y : mm->get_position().x) - grab.pos;
			if (orientation == VERTICAL) {
				motion = -motion;
			}
			double areasize = orientation == VERTICAL ? size.height - grabber->get_size().height : size.width - grabber->get_size().width;
			if (areasize <= 0) {
				return;
			}
			double umotion = motion / double(areasize);
			set_as_ratio(grab.uvalue + umotion);
		}
	}

	if (!mm.is_valid() && !mb.is_valid()) {
		if (p_event->is_action_pressed("ui_left", true)) {
			if (orientation != HORIZONTAL) {
				return;
			}
			set_value(get_value() - (custom_step >= 0 ? custom_step : get_step()));
			accept_event();
		} else if (p_event->is_action_pressed("ui_right", true)) {
			if (orientation != HORIZONTAL) {
				return;
			}
			set_value(get_value() + (custom_step >= 0 ? custom_step : get_step()));
			accept_event();
		} else if (p_event->is_action_pressed("ui_up", true)) {
			if (orientation != VERTICAL) {
				return;
			}

			set_value(get_value() + (custom_step >= 0 ? custom_step : get_step()));
			accept_event();
		} else if (p_event->is_action_pressed("ui_down", true)) {
			if (orientation != VERTICAL) {
				return;
			}
			set_value(get_value() - (custom_step >= 0 ? custom_step : get_step()));
			accept_event();
		} else if (p_event->is_action("ui_home", true) && p_event->is_pressed()) {
			set_value(get_min());
			accept_event();
		} else if (p_event->is_action("ui_end", true) && p_event->is_pressed()) {
			set_value(get_max());
			accept_event();
		}
	}
}

void Slider::_update_theme_item_cache() {
	Range::_update_theme_item_cache();

	theme_cache.slider_style = get_theme_stylebox(SNAME("slider"));
	theme_cache.grabber_area_style = get_theme_stylebox(SNAME("grabber_area"));
	theme_cache.grabber_area_hl_style = get_theme_stylebox(SNAME("grabber_area_highlight"));

	theme_cache.grabber_icon = get_theme_icon(SNAME("grabber"));
	theme_cache.grabber_hl_icon = get_theme_icon(SNAME("grabber_highlight"));
	theme_cache.grabber_disabled_icon = get_theme_icon(SNAME("grabber_disabled"));
	theme_cache.tick_icon = get_theme_icon(SNAME("tick"));
}

void Slider::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_THEME_CHANGED: {
			update_minimum_size();
			queue_redraw();
		} break;

		case NOTIFICATION_MOUSE_ENTER: {
			mouse_inside = true;
			queue_redraw();
		} break;

		case NOTIFICATION_MOUSE_EXIT: {
			mouse_inside = false;
			queue_redraw();
		} break;

		case NOTIFICATION_VISIBILITY_CHANGED:
		case NOTIFICATION_EXIT_TREE: {
			mouse_inside = false;
			grab.active = false;
		} break;

		case NOTIFICATION_DRAW: {
			RID ci = get_canvas_item();
			Size2i size = get_size();
			double ratio = Math::is_nan(get_as_ratio()) ? 0 : get_as_ratio();

			Ref<StyleBox> style = theme_cache.slider_style;
			Ref<Texture2D> tick = theme_cache.tick_icon;

			bool highlighted = mouse_inside || has_focus();
			Ref<Texture2D> grabber;
			if (editable) {
				if (highlighted) {
					grabber = theme_cache.grabber_hl_icon;
				} else {
					grabber = theme_cache.grabber_icon;
				}
			} else {
				grabber = theme_cache.grabber_disabled_icon;
			}

			Ref<StyleBox> grabber_area;
			if (highlighted) {
				grabber_area = theme_cache.grabber_area_hl_style;
			} else {
				grabber_area = theme_cache.grabber_area_style;
			}

			if (orientation == VERTICAL) {
				int widget_width = style->get_minimum_size().width;
				double areasize = size.height - grabber->get_size().height;
				style->draw(ci, Rect2i(Point2i(size.width / 2 - widget_width / 2, 0), Size2i(widget_width, size.height)));
				grabber_area->draw(ci, Rect2i(Point2i((size.width - widget_width) / 2, size.height - areasize * ratio - grabber->get_size().height / 2), Size2i(widget_width, areasize * ratio + grabber->get_size().height / 2)));

				if (ticks > 1) {
					int grabber_offset = (grabber->get_size().height / 2 - tick->get_height() / 2);
					for (int i = 0; i < ticks; i++) {
						if (!ticks_on_borders && (i == 0 || i + 1 == ticks)) {
							continue;
						}
						int ofs = (i * areasize / (ticks - 1)) + grabber_offset;
						tick->draw(ci, Point2i((size.width - widget_width) / 2, ofs));
					}
				}
				grabber->draw(ci, Point2i(size.width / 2 - grabber->get_size().width / 2 + get_theme_constant(SNAME("grabber_offset")), size.height - ratio * areasize - grabber->get_size().height));
			} else {
				int widget_height = style->get_minimum_size().height;
				double areasize = size.width - grabber->get_size().width;

				style->draw(ci, Rect2i(Point2i(0, (size.height - widget_height) / 2), Size2i(size.width, widget_height)));
				grabber_area->draw(ci, Rect2i(Point2i(0, (size.height - widget_height) / 2), Size2i(areasize * ratio + grabber->get_size().width / 2, widget_height)));

				if (ticks > 1) {
					int grabber_offset = (grabber->get_size().width / 2 - tick->get_width() / 2);
					for (int i = 0; i < ticks; i++) {
						if ((!ticks_on_borders) && ((i == 0) || ((i + 1) == ticks))) {
							continue;
						}
						int ofs = (i * areasize / (ticks - 1)) + grabber_offset;
						tick->draw(ci, Point2i(ofs, (size.height - widget_height) / 2));
					}
				}
				grabber->draw(ci, Point2i(ratio * areasize, size.height / 2 - grabber->get_size().height / 2 + get_theme_constant(SNAME("grabber_offset"))));
			}
		} break;
	}
}

void Slider::set_custom_step(double p_custom_step) {
	custom_step = p_custom_step;
}

double Slider::get_custom_step() const {
	return custom_step;
}

void Slider::set_ticks(int p_count) {
	if (ticks == p_count) {
		return;
	}

	ticks = p_count;
	queue_redraw();
}

int Slider::get_ticks() const {
	return ticks;
}

bool Slider::get_ticks_on_borders() const {
	return ticks_on_borders;
}

void Slider::set_ticks_on_borders(bool _tob) {
	if (ticks_on_borders == _tob) {
		return;
	}

	ticks_on_borders = _tob;
	queue_redraw();
}

void Slider::set_editable(bool p_editable) {
	if (editable == p_editable) {
		return;
	}

	editable = p_editable;
	queue_redraw();
}

bool Slider::is_editable() const {
	return editable;
}

void Slider::set_scrollable(bool p_scrollable) {
	scrollable = p_scrollable;
}

bool Slider::is_scrollable() const {
	return scrollable;
}

void Slider::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_ticks", "count"), &Slider::set_ticks);
	ClassDB::bind_method(D_METHOD("get_ticks"), &Slider::get_ticks);

	ClassDB::bind_method(D_METHOD("get_ticks_on_borders"), &Slider::get_ticks_on_borders);
	ClassDB::bind_method(D_METHOD("set_ticks_on_borders", "ticks_on_border"), &Slider::set_ticks_on_borders);

	ClassDB::bind_method(D_METHOD("set_editable", "editable"), &Slider::set_editable);
	ClassDB::bind_method(D_METHOD("is_editable"), &Slider::is_editable);
	ClassDB::bind_method(D_METHOD("set_scrollable", "scrollable"), &Slider::set_scrollable);
	ClassDB::bind_method(D_METHOD("is_scrollable"), &Slider::is_scrollable);

	ADD_SIGNAL(MethodInfo("drag_started"));
	ADD_SIGNAL(MethodInfo("drag_ended", PropertyInfo(Variant::BOOL, "value_changed")));

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "editable"), "set_editable", "is_editable");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "scrollable"), "set_scrollable", "is_scrollable");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "tick_count", PROPERTY_HINT_RANGE, "0,4096,1"), "set_ticks", "get_ticks");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "ticks_on_borders"), "set_ticks_on_borders", "get_ticks_on_borders");
}

Slider::Slider(Orientation p_orientation) {
	orientation = p_orientation;
	set_focus_mode(FOCUS_ALL);
}
