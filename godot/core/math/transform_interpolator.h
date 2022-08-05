/*************************************************************************/
/*  transform_interpolator.h                                             */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2022 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2022 Godot Engine contributors (cf. AUTHORS.md).   */
/*                                                                       */
/* Permission is hereby granted, free of charge, to any person obtaining */
/* a copy of this software and associated documentation files (the       */
/* "Software"), to deal in the Software without restriction, including   */
/* without limitation the rights to use, copy, modify, merge, publish,   */
/* distribute, sublicense, and/or sell copies of the Software, and to    */
/* permit persons to whom the Software is furnished to do so, subject to */
/* the following conditions:                                             */
/*                                                                       */
/* The above copyright notice and this permission notice shall be        */
/* included in all copies or substantial portions of the Software.       */
/*                                                                       */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.*/
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY  */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,  */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE     */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                */
/*************************************************************************/

#ifndef TRANSFORM_INTERPOLATOR_H
#define TRANSFORM_INTERPOLATOR_H

#include "core/math/math_defs.h"
#include "core/math/quat.h"
#include "core/math/transform.h"
#include "core/math/vector3.h"

// Keep all the functions for fixed timestep interpolation together.
// There are two stages involved:
// Finding a method, for determining the interpolation method between two
// keyframes (which are physics ticks).
// And applying that pre-determined method.

// Pre-determining the method makes sense because it is expensive and often
// several frames may occur between each physics tick, which will make it cheaper
// than performing every frame.

class Transform;

class TransformInterpolator {
public:
	enum Method : unsigned int {
		INTERP_LERP,
		INTERP_SLERP,
		INTERP_SCALED_SLERP,
	};

private:
	static real_t _vec3_normalize(Vector3 &p_vec);
	static Vector3 _basis_orthonormalize(Basis &r_basis);
	static real_t vec3_sum(const Vector3 &p_pt) { return p_pt.x + p_pt.y + p_pt.z; }
	static Method _test_basis(Basis p_basis, bool r_needed_normalize, Quat &r_quat);
	static Basis _basis_slerp_unchecked(Basis p_from, Basis p_to, real_t p_fraction);
	static Quat _quat_slerp_unchecked(const Quat &p_from, const Quat &p_to, real_t p_fraction);
	static Quat _basis_to_quat_unchecked(const Basis &p_basis);
	static bool _basis_is_orthogonal(const Basis &p_basis, real_t p_epsilon = 0.01f);
	static bool _basis_is_orthogonal_any_scale(const Basis &p_basis);

	static void interpolate_basis_linear(const Basis &p_prev, const Basis &p_curr, Basis &r_result, real_t p_fraction);
	static void interpolate_basis_scaled_slerp(Basis p_prev, Basis p_curr, Basis &r_result, real_t p_fraction);

public:
	// Generic functions, use when you don't know what method should be used, e.g. from gdscript.
	// These will be slower.
	static void interpolate_transform(const Transform &p_prev, const Transform &p_curr, Transform &r_result, real_t p_fraction);
	static void interpolate_basis(const Basis &p_prev, const Basis &p_curr, Basis &r_result, real_t p_fraction);

	// Optimized function when you know ahead of time the method
	static void interpolate_transform_via_method(const Transform &p_prev, const Transform &p_curr, Transform &r_result, real_t p_fraction, Method p_method);
	static void interpolate_basis_via_method(const Basis &p_prev, const Basis &p_curr, Basis &r_result, real_t p_fraction, Method p_method);

	static real_t checksum_transform(const Transform &p_transform);
	static Method find_method(const Basis &p_a, const Basis &p_b);
};

#endif // TRANSFORM_INTERPOLATOR_H
