/**************************************************************************/
/*  gradient.h                                                            */
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

#ifndef GRADIENT_H
#define GRADIENT_H

#include "core/io/resource.h"

class Gradient : public Resource {
	GDCLASS(Gradient, Resource);
	OBJ_SAVE_TYPE(Gradient);

public:
	enum InterpolationMode {
		GRADIENT_INTERPOLATE_LINEAR,
		GRADIENT_INTERPOLATE_CONSTANT,
		GRADIENT_INTERPOLATE_CUBIC,
	};

	struct Point {
		float offset = 0.0;
		Color color;
		bool operator<(const Point &p_ponit) const {
			return offset < p_ponit.offset;
		}
	};

private:
	Vector<Point> points;
	bool is_sorted = true;
	InterpolationMode interpolation_mode = GRADIENT_INTERPOLATE_LINEAR;

	_FORCE_INLINE_ void _update_sorting() {
		if (!is_sorted) {
			points.sort();
			is_sorted = true;
		}
	}

protected:
	static void _bind_methods();

public:
	Gradient();
	virtual ~Gradient();

	void add_point(float p_offset, const Color &p_color);
	void remove_point(int p_index);
	void set_points(const Vector<Point> &p_points);
	Vector<Point> &get_points();
	void reverse();

	void set_offset(int pos, const float offset);
	float get_offset(int pos);

	void set_color(int pos, const Color &color);
	Color get_color(int pos);

	void set_offsets(const Vector<float> &p_offsets);
	Vector<float> get_offsets() const;

	void set_colors(const Vector<Color> &p_colors);
	Vector<Color> get_colors() const;

	void set_interpolation_mode(InterpolationMode p_interp_mode);
	InterpolationMode get_interpolation_mode();

	_FORCE_INLINE_ Color get_color_at_offset(float p_offset) {
		if (points.is_empty()) {
			return Color(0, 0, 0, 1);
		}

		_update_sorting();

		// Binary search.
		int low = 0;
		int high = points.size() - 1;
		int middle = 0;

#ifdef DEBUG_ENABLED
		if (low > high) {
			ERR_PRINT("low > high, this may be a bug");
		}
#endif

		while (low <= high) {
			middle = (low + high) / 2;
			const Point &point = points[middle];
			if (point.offset > p_offset) {
				high = middle - 1; //search low end of array
			} else if (point.offset < p_offset) {
				low = middle + 1; //search high end of array
			} else {
				return point.color;
			}
		}

		// Return sampled value.
		if (points[middle].offset > p_offset) {
			middle--;
		}
		int first = middle;
		int second = middle + 1;
		if (second >= points.size()) {
			return points[points.size() - 1].color;
		}
		if (first < 0) {
			return points[0].color;
		}
		const Point &pointFirst = points[first];
		const Point &pointSecond = points[second];

		switch (interpolation_mode) {
			case GRADIENT_INTERPOLATE_LINEAR: {
				return pointFirst.color.lerp(pointSecond.color, (p_offset - pointFirst.offset) / (pointSecond.offset - pointFirst.offset));
			} break;
			case GRADIENT_INTERPOLATE_CONSTANT: {
				return pointFirst.color;
			} break;
			case GRADIENT_INTERPOLATE_CUBIC: {
				int p0 = first - 1;
				int p3 = second + 1;
				if (p3 >= points.size()) {
					p3 = second;
				}
				if (p0 < 0) {
					p0 = first;
				}
				const Point &pointP0 = points[p0];
				const Point &pointP3 = points[p3];

				float x = (p_offset - pointFirst.offset) / (pointSecond.offset - pointFirst.offset);
				float r = Math::cubic_interpolate(pointFirst.color.r, pointSecond.color.r, pointP0.color.r, pointP3.color.r, x);
				float g = Math::cubic_interpolate(pointFirst.color.g, pointSecond.color.g, pointP0.color.g, pointP3.color.g, x);
				float b = Math::cubic_interpolate(pointFirst.color.b, pointSecond.color.b, pointP0.color.b, pointP3.color.b, x);
				float a = Math::cubic_interpolate(pointFirst.color.a, pointSecond.color.a, pointP0.color.a, pointP3.color.a, x);

				return Color(r, g, b, a);
			} break;
			default: {
				// Fallback to linear interpolation.
				return pointFirst.color.lerp(pointSecond.color, (p_offset - pointFirst.offset) / (pointSecond.offset - pointFirst.offset));
			}
		}
	}

	int get_point_count() const;
};

VARIANT_ENUM_CAST(Gradient::InterpolationMode);

#endif // GRADIENT_H
