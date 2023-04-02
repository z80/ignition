/**************************************************************************/
/*  path_3d.cpp                                                           */
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

#include "path_3d.h"

Path3D::Path3D() {
	SceneTree *st = SceneTree::get_singleton();
	if (st && st->is_debugging_paths_hint()) {
		debug_instance = RS::get_singleton()->instance_create();
		set_notify_transform(true);
		_update_debug_mesh();
	}
}

Path3D::~Path3D() {
	ERR_FAIL_NULL(RenderingServer::get_singleton());
	if (debug_instance.is_valid()) {
		RS::get_singleton()->free(debug_instance);
	}
	if (debug_mesh.is_valid()) {
		RS::get_singleton()->free(debug_mesh->get_rid());
	}
}

void Path3D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			SceneTree *st = SceneTree::get_singleton();
			if (st && st->is_debugging_paths_hint()) {
				_update_debug_mesh();
			}
		} break;

		case NOTIFICATION_EXIT_TREE: {
			SceneTree *st = SceneTree::get_singleton();
			if (st && st->is_debugging_paths_hint()) {
				RS::get_singleton()->instance_set_visible(debug_instance, false);
			}
		} break;

		case NOTIFICATION_TRANSFORM_CHANGED: {
			if (is_inside_tree() && debug_instance.is_valid()) {
				RS::get_singleton()->instance_set_transform(debug_instance, get_global_transform());
			}
		} break;
	}
}

void Path3D::_update_debug_mesh() {
	SceneTree *st = SceneTree::get_singleton();
	if (!(st && st->is_debugging_paths_hint())) {
		return;
	}

	if (!debug_mesh.is_valid()) {
		debug_mesh = Ref<ArrayMesh>(memnew(ArrayMesh));
	}

	if (!(curve.is_valid())) {
		RS::get_singleton()->instance_set_visible(debug_instance, false);
		return;
	}
	if (curve->get_point_count() < 2) {
		RS::get_singleton()->instance_set_visible(debug_instance, false);
		return;
	}

	Vector<Vector3> vertex_array;

	for (int i = 1; i < curve->get_point_count(); i++) {
		Vector3 line_end = curve->get_point_position(i);
		Vector3 line_start = curve->get_point_position(i - 1);
		vertex_array.push_back(line_start);
		vertex_array.push_back(line_end);
	}

	Array mesh_array;
	mesh_array.resize(Mesh::ARRAY_MAX);
	mesh_array[Mesh::ARRAY_VERTEX] = vertex_array;

	debug_mesh->clear_surfaces();
	debug_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_LINES, mesh_array);

	RS::get_singleton()->instance_set_base(debug_instance, debug_mesh->get_rid());
	RS::get_singleton()->mesh_surface_set_material(debug_mesh->get_rid(), 0, st->get_debug_paths_material()->get_rid());
	if (is_inside_tree()) {
		RS::get_singleton()->instance_set_scenario(debug_instance, get_world_3d()->get_scenario());
		RS::get_singleton()->instance_set_transform(debug_instance, get_global_transform());
		RS::get_singleton()->instance_set_visible(debug_instance, is_visible_in_tree());
	}
}

void Path3D::_curve_changed() {
	if (is_inside_tree() && Engine::get_singleton()->is_editor_hint()) {
		update_gizmos();
	}
	if (is_inside_tree()) {
		emit_signal(SNAME("curve_changed"));
	}

	// update the configuration warnings of all children of type PathFollow
	// previously used for PathFollowOriented (now enforced orientation is done in PathFollow)
	if (is_inside_tree()) {
		for (int i = 0; i < get_child_count(); i++) {
			PathFollow3D *child = Object::cast_to<PathFollow3D>(get_child(i));
			if (child) {
				child->update_configuration_warnings();
			}
		}
	}
	SceneTree *st = SceneTree::get_singleton();
	if (st && st->is_debugging_paths_hint()) {
		_update_debug_mesh();
	}
}

void Path3D::set_curve(const Ref<Curve3D> &p_curve) {
	if (curve.is_valid()) {
		curve->disconnect("changed", callable_mp(this, &Path3D::_curve_changed));
	}

	curve = p_curve;

	if (curve.is_valid()) {
		curve->connect("changed", callable_mp(this, &Path3D::_curve_changed));
	}
	_curve_changed();
}

Ref<Curve3D> Path3D::get_curve() const {
	return curve;
}

void Path3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_curve", "curve"), &Path3D::set_curve);
	ClassDB::bind_method(D_METHOD("get_curve"), &Path3D::get_curve);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "curve", PROPERTY_HINT_RESOURCE_TYPE, "Curve3D", PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_EDITOR_INSTANTIATE_OBJECT), "set_curve", "get_curve");

	ADD_SIGNAL(MethodInfo("curve_changed"));
}

//////////////

void PathFollow3D::_update_transform(bool p_update_xyz_rot) {
	if (!path) {
		return;
	}

	Ref<Curve3D> c = path->get_curve();
	if (!c.is_valid()) {
		return;
	}

	real_t bl = c->get_baked_length();
	if (bl == 0.0) {
		return;
	}

	Transform3D t;

	if (rotation_mode == ROTATION_NONE) {
		Vector3 pos = c->sample_baked(progress, cubic);
		t.origin = pos;
	} else {
		t = c->sample_baked_with_rotation(progress, cubic, false);
		Vector3 forward = t.basis.get_column(2); // Retain tangent for applying tilt
		t = PathFollow3D::correct_posture(t, rotation_mode);

		// Apply tilt *after* correct_posture
		if (tilt_enabled) {
			const real_t tilt = c->sample_baked_tilt(progress);

			const Basis twist(forward, tilt);
			t.basis = twist * t.basis;
		}
	}

	Vector3 scale = get_transform().basis.get_scale();

	t.translate_local(Vector3(h_offset, v_offset, 0));
	t.basis.scale_local(scale);

	set_transform(t);
}

void PathFollow3D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			Node *parent = get_parent();
			if (parent) {
				path = Object::cast_to<Path3D>(parent);
				if (path) {
					_update_transform(false);
				}
			}
		} break;

		case NOTIFICATION_EXIT_TREE: {
			path = nullptr;
		} break;
	}
}

void PathFollow3D::set_cubic_interpolation(bool p_enable) {
	cubic = p_enable;
}

bool PathFollow3D::get_cubic_interpolation() const {
	return cubic;
}

void PathFollow3D::_validate_property(PropertyInfo &p_property) const {
	if (p_property.name == "offset") {
		real_t max = 10000;
		if (path && path->get_curve().is_valid()) {
			max = path->get_curve()->get_baked_length();
		}

		p_property.hint_string = "0," + rtos(max) + ",0.01,or_less,or_greater";
	}
}

PackedStringArray PathFollow3D::get_configuration_warnings() const {
	PackedStringArray warnings = Node::get_configuration_warnings();

	if (is_visible_in_tree() && is_inside_tree()) {
		if (!Object::cast_to<Path3D>(get_parent())) {
			warnings.push_back(RTR("PathFollow3D only works when set as a child of a Path3D node."));
		} else {
			Path3D *p = Object::cast_to<Path3D>(get_parent());
			if (p->get_curve().is_valid() && !p->get_curve()->is_up_vector_enabled() && rotation_mode == ROTATION_ORIENTED) {
				warnings.push_back(RTR("PathFollow3D's ROTATION_ORIENTED requires \"Up Vector\" to be enabled in its parent Path3D's Curve resource."));
			}
		}
	}

	return warnings;
}

Transform3D PathFollow3D::correct_posture(Transform3D p_transform, PathFollow3D::RotationMode p_rotation_mode) {
	Transform3D t = p_transform;

	// Modify frame according to rotation mode.
	if (p_rotation_mode == PathFollow3D::ROTATION_NONE) {
		// Clear rotation.
		t.basis = Basis();
	} else if (p_rotation_mode == PathFollow3D::ROTATION_ORIENTED) {
		// Y-axis always straight up.
		Vector3 up(0.0, 1.0, 0.0);
		Vector3 forward = t.basis.get_column(2);

		t.basis = Basis::looking_at(-forward, up);
	} else {
		// Lock some euler axes.
		Vector3 euler = t.basis.get_euler_normalized(EulerOrder::YXZ);
		if (p_rotation_mode == PathFollow3D::ROTATION_Y) {
			// Only Y-axis allowed.
			euler[0] = 0;
			euler[2] = 0;
		} else if (p_rotation_mode == PathFollow3D::ROTATION_XY) {
			// XY allowed.
			euler[2] = 0;
		}

		Basis locked = Basis::from_euler(euler, EulerOrder::YXZ);
		t.basis = locked;
	}

	return t;
}

void PathFollow3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_progress", "progress"), &PathFollow3D::set_progress);
	ClassDB::bind_method(D_METHOD("get_progress"), &PathFollow3D::get_progress);

	ClassDB::bind_method(D_METHOD("set_h_offset", "h_offset"), &PathFollow3D::set_h_offset);
	ClassDB::bind_method(D_METHOD("get_h_offset"), &PathFollow3D::get_h_offset);

	ClassDB::bind_method(D_METHOD("set_v_offset", "v_offset"), &PathFollow3D::set_v_offset);
	ClassDB::bind_method(D_METHOD("get_v_offset"), &PathFollow3D::get_v_offset);

	ClassDB::bind_method(D_METHOD("set_progress_ratio", "ratio"), &PathFollow3D::set_progress_ratio);
	ClassDB::bind_method(D_METHOD("get_progress_ratio"), &PathFollow3D::get_progress_ratio);

	ClassDB::bind_method(D_METHOD("set_rotation_mode", "rotation_mode"), &PathFollow3D::set_rotation_mode);
	ClassDB::bind_method(D_METHOD("get_rotation_mode"), &PathFollow3D::get_rotation_mode);

	ClassDB::bind_method(D_METHOD("set_cubic_interpolation", "enable"), &PathFollow3D::set_cubic_interpolation);
	ClassDB::bind_method(D_METHOD("get_cubic_interpolation"), &PathFollow3D::get_cubic_interpolation);

	ClassDB::bind_method(D_METHOD("set_loop", "loop"), &PathFollow3D::set_loop);
	ClassDB::bind_method(D_METHOD("has_loop"), &PathFollow3D::has_loop);

	ClassDB::bind_method(D_METHOD("set_tilt_enabled", "enabled"), &PathFollow3D::set_tilt_enabled);
	ClassDB::bind_method(D_METHOD("is_tilt_enabled"), &PathFollow3D::is_tilt_enabled);

	ClassDB::bind_static_method("PathFollow3D", D_METHOD("correct_posture", "transform", "rotation_mode"), &PathFollow3D::correct_posture);

	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "progress", PROPERTY_HINT_RANGE, "0,10000,0.01,or_less,or_greater,suffix:m"), "set_progress", "get_progress");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "progress_ratio", PROPERTY_HINT_RANGE, "0,1,0.0001,or_less,or_greater", PROPERTY_USAGE_EDITOR), "set_progress_ratio", "get_progress_ratio");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "h_offset", PROPERTY_HINT_NONE, "suffix:m"), "set_h_offset", "get_h_offset");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "v_offset", PROPERTY_HINT_NONE, "suffix:m"), "set_v_offset", "get_v_offset");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "rotation_mode", PROPERTY_HINT_ENUM, "None,Y,XY,XYZ,Oriented"), "set_rotation_mode", "get_rotation_mode");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "cubic_interp"), "set_cubic_interpolation", "get_cubic_interpolation");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "loop"), "set_loop", "has_loop");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "tilt_enabled"), "set_tilt_enabled", "is_tilt_enabled");

	BIND_ENUM_CONSTANT(ROTATION_NONE);
	BIND_ENUM_CONSTANT(ROTATION_Y);
	BIND_ENUM_CONSTANT(ROTATION_XY);
	BIND_ENUM_CONSTANT(ROTATION_XYZ);
	BIND_ENUM_CONSTANT(ROTATION_ORIENTED);
}

void PathFollow3D::set_progress(real_t p_progress) {
	ERR_FAIL_COND(!isfinite(p_progress));
	progress = p_progress;

	if (path) {
		if (path->get_curve().is_valid()) {
			real_t path_length = path->get_curve()->get_baked_length();

			if (loop && path_length) {
				progress = Math::fposmod(progress, path_length);
				if (!Math::is_zero_approx(p_progress) && Math::is_zero_approx(progress)) {
					progress = path_length;
				}
			}
		}

		_update_transform();
	}
}

void PathFollow3D::set_h_offset(real_t p_h_offset) {
	h_offset = p_h_offset;
	if (path) {
		_update_transform();
	}
}

real_t PathFollow3D::get_h_offset() const {
	return h_offset;
}

void PathFollow3D::set_v_offset(real_t p_v_offset) {
	v_offset = p_v_offset;
	if (path) {
		_update_transform();
	}
}

real_t PathFollow3D::get_v_offset() const {
	return v_offset;
}

real_t PathFollow3D::get_progress() const {
	return progress;
}

void PathFollow3D::set_progress_ratio(real_t p_ratio) {
	if (path && path->get_curve().is_valid() && path->get_curve()->get_baked_length()) {
		set_progress(p_ratio * path->get_curve()->get_baked_length());
	}
}

real_t PathFollow3D::get_progress_ratio() const {
	if (path && path->get_curve().is_valid() && path->get_curve()->get_baked_length()) {
		return get_progress() / path->get_curve()->get_baked_length();
	} else {
		return 0;
	}
}

void PathFollow3D::set_rotation_mode(RotationMode p_rotation_mode) {
	rotation_mode = p_rotation_mode;

	update_configuration_warnings();
	_update_transform();
}

PathFollow3D::RotationMode PathFollow3D::get_rotation_mode() const {
	return rotation_mode;
}

void PathFollow3D::set_loop(bool p_loop) {
	loop = p_loop;
}

bool PathFollow3D::has_loop() const {
	return loop;
}

void PathFollow3D::set_tilt_enabled(bool p_enable) {
	tilt_enabled = p_enable;
}

bool PathFollow3D::is_tilt_enabled() const {
	return tilt_enabled;
}
