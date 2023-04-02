/**************************************************************************/
/*  node_3d_editor_gizmos.cpp                                             */
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

#include "node_3d_editor_gizmos.h"

#include "core/config/project_settings.h"
#include "core/math/convex_hull.h"
#include "core/math/geometry_2d.h"
#include "core/math/geometry_3d.h"
#include "editor/editor_node.h"
#include "editor/editor_settings.h"
#include "editor/editor_undo_redo_manager.h"
#include "editor/plugins/node_3d_editor_plugin.h"
#include "scene/3d/audio_listener_3d.h"
#include "scene/3d/audio_stream_player_3d.h"
#include "scene/3d/camera_3d.h"
#include "scene/3d/collision_polygon_3d.h"
#include "scene/3d/collision_shape_3d.h"
#include "scene/3d/cpu_particles_3d.h"
#include "scene/3d/decal.h"
#include "scene/3d/fog_volume.h"
#include "scene/3d/gpu_particles_3d.h"
#include "scene/3d/gpu_particles_collision_3d.h"
#include "scene/3d/joint_3d.h"
#include "scene/3d/label_3d.h"
#include "scene/3d/light_3d.h"
#include "scene/3d/lightmap_gi.h"
#include "scene/3d/lightmap_probe.h"
#include "scene/3d/marker_3d.h"
#include "scene/3d/mesh_instance_3d.h"
#include "scene/3d/navigation_link_3d.h"
#include "scene/3d/navigation_region_3d.h"
#include "scene/3d/occluder_instance_3d.h"
#include "scene/3d/ray_cast_3d.h"
#include "scene/3d/reflection_probe.h"
#include "scene/3d/shape_cast_3d.h"
#include "scene/3d/soft_body_3d.h"
#include "scene/3d/spring_arm_3d.h"
#include "scene/3d/sprite_3d.h"
#include "scene/3d/vehicle_body_3d.h"
#include "scene/3d/visible_on_screen_notifier_3d.h"
#include "scene/3d/voxel_gi.h"
#include "scene/resources/box_shape_3d.h"
#include "scene/resources/capsule_shape_3d.h"
#include "scene/resources/concave_polygon_shape_3d.h"
#include "scene/resources/convex_polygon_shape_3d.h"
#include "scene/resources/cylinder_shape_3d.h"
#include "scene/resources/height_map_shape_3d.h"
#include "scene/resources/primitive_meshes.h"
#include "scene/resources/separation_ray_shape_3d.h"
#include "scene/resources/sphere_shape_3d.h"
#include "scene/resources/surface_tool.h"
#include "scene/resources/world_boundary_shape_3d.h"
#include "servers/navigation_server_3d.h"

#define HANDLE_HALF_SIZE 9.5

bool EditorNode3DGizmo::is_editable() const {
	ERR_FAIL_COND_V(!spatial_node, false);
	Node *edited_root = spatial_node->get_tree()->get_edited_scene_root();
	if (spatial_node == edited_root) {
		return true;
	}
	if (spatial_node->get_owner() == edited_root) {
		return true;
	}

	if (edited_root->is_editable_instance(spatial_node->get_owner())) {
		return true;
	}

	return false;
}

void EditorNode3DGizmo::clear() {
	ERR_FAIL_NULL(RenderingServer::get_singleton());
	for (int i = 0; i < instances.size(); i++) {
		if (instances[i].instance.is_valid()) {
			RS::get_singleton()->free(instances[i].instance);
		}
	}

	billboard_handle = false;
	collision_segments.clear();
	collision_mesh = Ref<TriangleMesh>();
	instances.clear();
	handles.clear();
	handle_ids.clear();
	secondary_handles.clear();
	secondary_handle_ids.clear();
}

void EditorNode3DGizmo::redraw() {
	if (!GDVIRTUAL_CALL(_redraw)) {
		ERR_FAIL_COND(!gizmo_plugin);
		gizmo_plugin->redraw(this);
	}

	if (Node3DEditor::get_singleton()->is_current_selected_gizmo(this)) {
		Node3DEditor::get_singleton()->update_transform_gizmo();
	}
}

String EditorNode3DGizmo::get_handle_name(int p_id, bool p_secondary) const {
	String ret;
	if (GDVIRTUAL_CALL(_get_handle_name, p_id, p_secondary, ret)) {
		return ret;
	}

	ERR_FAIL_COND_V(!gizmo_plugin, "");
	return gizmo_plugin->get_handle_name(this, p_id, p_secondary);
}

bool EditorNode3DGizmo::is_handle_highlighted(int p_id, bool p_secondary) const {
	bool success;
	if (GDVIRTUAL_CALL(_is_handle_highlighted, p_id, p_secondary, success)) {
		return success;
	}

	ERR_FAIL_COND_V(!gizmo_plugin, false);
	return gizmo_plugin->is_handle_highlighted(this, p_id, p_secondary);
}

Variant EditorNode3DGizmo::get_handle_value(int p_id, bool p_secondary) const {
	Variant value;
	if (GDVIRTUAL_CALL(_get_handle_value, p_id, p_secondary, value)) {
		return value;
	}

	ERR_FAIL_COND_V(!gizmo_plugin, Variant());
	return gizmo_plugin->get_handle_value(this, p_id, p_secondary);
}

void EditorNode3DGizmo::set_handle(int p_id, bool p_secondary, Camera3D *p_camera, const Point2 &p_point) {
	if (GDVIRTUAL_CALL(_set_handle, p_id, p_secondary, p_camera, p_point)) {
		return;
	}

	ERR_FAIL_COND(!gizmo_plugin);
	gizmo_plugin->set_handle(this, p_id, p_secondary, p_camera, p_point);
}

void EditorNode3DGizmo::commit_handle(int p_id, bool p_secondary, const Variant &p_restore, bool p_cancel) {
	if (GDVIRTUAL_CALL(_commit_handle, p_id, p_secondary, p_restore, p_cancel)) {
		return;
	}

	ERR_FAIL_COND(!gizmo_plugin);
	gizmo_plugin->commit_handle(this, p_id, p_secondary, p_restore, p_cancel);
}

int EditorNode3DGizmo::subgizmos_intersect_ray(Camera3D *p_camera, const Vector2 &p_point) const {
	int id;
	if (GDVIRTUAL_CALL(_subgizmos_intersect_ray, p_camera, p_point, id)) {
		return id;
	}

	ERR_FAIL_COND_V(!gizmo_plugin, -1);
	return gizmo_plugin->subgizmos_intersect_ray(this, p_camera, p_point);
}

Vector<int> EditorNode3DGizmo::subgizmos_intersect_frustum(const Camera3D *p_camera, const Vector<Plane> &p_frustum) const {
	TypedArray<Plane> frustum;
	frustum.resize(p_frustum.size());
	for (int i = 0; i < p_frustum.size(); i++) {
		frustum[i] = p_frustum[i];
	}
	Vector<int> ret;
	if (GDVIRTUAL_CALL(_subgizmos_intersect_frustum, p_camera, frustum, ret)) {
		return ret;
	}

	ERR_FAIL_COND_V(!gizmo_plugin, Vector<int>());
	return gizmo_plugin->subgizmos_intersect_frustum(this, p_camera, p_frustum);
}

Transform3D EditorNode3DGizmo::get_subgizmo_transform(int p_id) const {
	Transform3D ret;
	if (GDVIRTUAL_CALL(_get_subgizmo_transform, p_id, ret)) {
		return ret;
	}

	ERR_FAIL_COND_V(!gizmo_plugin, Transform3D());
	return gizmo_plugin->get_subgizmo_transform(this, p_id);
}

void EditorNode3DGizmo::set_subgizmo_transform(int p_id, Transform3D p_transform) {
	if (GDVIRTUAL_CALL(_set_subgizmo_transform, p_id, p_transform)) {
		return;
	}

	ERR_FAIL_COND(!gizmo_plugin);
	gizmo_plugin->set_subgizmo_transform(this, p_id, p_transform);
}

void EditorNode3DGizmo::commit_subgizmos(const Vector<int> &p_ids, const Vector<Transform3D> &p_restore, bool p_cancel) {
	TypedArray<Transform3D> restore;
	restore.resize(p_restore.size());
	for (int i = 0; i < p_restore.size(); i++) {
		restore[i] = p_restore[i];
	}

	if (GDVIRTUAL_CALL(_commit_subgizmos, p_ids, restore, p_cancel)) {
		return;
	}

	ERR_FAIL_COND(!gizmo_plugin);
	gizmo_plugin->commit_subgizmos(this, p_ids, p_restore, p_cancel);
}

void EditorNode3DGizmo::set_node_3d(Node3D *p_node) {
	ERR_FAIL_NULL(p_node);
	spatial_node = p_node;
}

void EditorNode3DGizmo::Instance::create_instance(Node3D *p_base, bool p_hidden) {
	instance = RS::get_singleton()->instance_create2(mesh->get_rid(), p_base->get_world_3d()->get_scenario());
	RS::get_singleton()->instance_attach_object_instance_id(instance, p_base->get_instance_id());
	if (skin_reference.is_valid()) {
		RS::get_singleton()->instance_attach_skeleton(instance, skin_reference->get_skeleton());
	}
	if (extra_margin) {
		RS::get_singleton()->instance_set_extra_visibility_margin(instance, 1);
	}
	RS::get_singleton()->instance_geometry_set_cast_shadows_setting(instance, RS::SHADOW_CASTING_SETTING_OFF);
	int layer = p_hidden ? 0 : 1 << Node3DEditorViewport::GIZMO_EDIT_LAYER;
	RS::get_singleton()->instance_set_layer_mask(instance, layer); //gizmos are 26
	RS::get_singleton()->instance_geometry_set_flag(instance, RS::INSTANCE_FLAG_IGNORE_OCCLUSION_CULLING, true);
	RS::get_singleton()->instance_geometry_set_flag(instance, RS::INSTANCE_FLAG_USE_BAKED_LIGHT, false);
}

void EditorNode3DGizmo::add_mesh(const Ref<Mesh> &p_mesh, const Ref<Material> &p_material, const Transform3D &p_xform, const Ref<SkinReference> &p_skin_reference) {
	ERR_FAIL_COND(!spatial_node);
	ERR_FAIL_COND_MSG(!p_mesh.is_valid(), "EditorNode3DGizmo.add_mesh() requires a valid Mesh resource.");

	Instance ins;

	ins.mesh = p_mesh;
	ins.skin_reference = p_skin_reference;
	ins.material = p_material;
	ins.xform = p_xform;
	if (valid) {
		ins.create_instance(spatial_node, hidden);
		RS::get_singleton()->instance_set_transform(ins.instance, spatial_node->get_global_transform() * ins.xform);
		if (ins.material.is_valid()) {
			RS::get_singleton()->instance_geometry_set_material_override(ins.instance, p_material->get_rid());
		}
	}

	instances.push_back(ins);
}

void EditorNode3DGizmo::add_lines(const Vector<Vector3> &p_lines, const Ref<Material> &p_material, bool p_billboard, const Color &p_modulate) {
	add_vertices(p_lines, p_material, Mesh::PRIMITIVE_LINES, p_billboard, p_modulate);
}

void EditorNode3DGizmo::add_vertices(const Vector<Vector3> &p_vertices, const Ref<Material> &p_material, Mesh::PrimitiveType p_primitive_type, bool p_billboard, const Color &p_modulate) {
	if (p_vertices.is_empty()) {
		return;
	}

	ERR_FAIL_COND(!spatial_node);
	Instance ins;

	Ref<ArrayMesh> mesh = memnew(ArrayMesh);
	Array a;
	a.resize(Mesh::ARRAY_MAX);

	a[Mesh::ARRAY_VERTEX] = p_vertices;

	Vector<Color> color;
	color.resize(p_vertices.size());
	{
		Color *w = color.ptrw();
		for (int i = 0; i < p_vertices.size(); i++) {
			if (is_selected()) {
				w[i] = Color(1, 1, 1, 0.8) * p_modulate;
			} else {
				w[i] = Color(1, 1, 1, 0.2) * p_modulate;
			}
		}
	}

	a[Mesh::ARRAY_COLOR] = color;

	mesh->add_surface_from_arrays(p_primitive_type, a);
	mesh->surface_set_material(0, p_material);

	if (p_billboard) {
		float md = 0;
		for (int i = 0; i < p_vertices.size(); i++) {
			md = MAX(0, p_vertices[i].length());
		}
		if (md) {
			mesh->set_custom_aabb(AABB(Vector3(-md, -md, -md), Vector3(md, md, md) * 2.0));
		}
	}

	ins.mesh = mesh;
	if (valid) {
		ins.create_instance(spatial_node, hidden);
		RS::get_singleton()->instance_set_transform(ins.instance, spatial_node->get_global_transform());
	}

	instances.push_back(ins);
}

void EditorNode3DGizmo::add_unscaled_billboard(const Ref<Material> &p_material, real_t p_scale, const Color &p_modulate) {
	ERR_FAIL_COND(!spatial_node);
	Instance ins;

	Vector<Vector3> vs = {
		Vector3(-p_scale, p_scale, 0),
		Vector3(p_scale, p_scale, 0),
		Vector3(p_scale, -p_scale, 0),
		Vector3(-p_scale, -p_scale, 0)
	};

	Vector<Vector2> uv = {
		Vector2(0, 0),
		Vector2(1, 0),
		Vector2(1, 1),
		Vector2(0, 1)
	};

	Vector<Color> colors = {
		p_modulate,
		p_modulate,
		p_modulate,
		p_modulate
	};

	Vector<int> indices = { 0, 1, 2, 0, 2, 3 };

	Ref<ArrayMesh> mesh = memnew(ArrayMesh);
	Array a;
	a.resize(Mesh::ARRAY_MAX);
	a[Mesh::ARRAY_VERTEX] = vs;
	a[Mesh::ARRAY_TEX_UV] = uv;
	a[Mesh::ARRAY_INDEX] = indices;
	a[Mesh::ARRAY_COLOR] = colors;
	mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, a);
	mesh->surface_set_material(0, p_material);

	float md = 0;
	for (int i = 0; i < vs.size(); i++) {
		md = MAX(0, vs[i].length());
	}
	if (md) {
		mesh->set_custom_aabb(AABB(Vector3(-md, -md, -md), Vector3(md, md, md) * 2.0));
	}

	selectable_icon_size = p_scale;
	mesh->set_custom_aabb(AABB(Vector3(-selectable_icon_size, -selectable_icon_size, -selectable_icon_size) * 100.0f, Vector3(selectable_icon_size, selectable_icon_size, selectable_icon_size) * 200.0f));

	ins.mesh = mesh;
	if (valid) {
		ins.create_instance(spatial_node, hidden);
		RS::get_singleton()->instance_set_transform(ins.instance, spatial_node->get_global_transform());
	}

	selectable_icon_size = p_scale;

	instances.push_back(ins);
}

void EditorNode3DGizmo::add_collision_triangles(const Ref<TriangleMesh> &p_tmesh) {
	collision_mesh = p_tmesh;
}

void EditorNode3DGizmo::add_collision_segments(const Vector<Vector3> &p_lines) {
	int from = collision_segments.size();
	collision_segments.resize(from + p_lines.size());
	for (int i = 0; i < p_lines.size(); i++) {
		collision_segments.write[from + i] = p_lines[i];
	}
}

void EditorNode3DGizmo::add_handles(const Vector<Vector3> &p_handles, const Ref<Material> &p_material, const Vector<int> &p_ids, bool p_billboard, bool p_secondary) {
	billboard_handle = p_billboard;

	if (!is_selected() || !is_editable()) {
		return;
	}

	ERR_FAIL_NULL(spatial_node);

	Vector<Vector3> &handle_list = p_secondary ? secondary_handles : handles;
	Vector<int> &id_list = p_secondary ? secondary_handle_ids : handle_ids;

	if (p_ids.is_empty()) {
		ERR_FAIL_COND_MSG(!id_list.is_empty(), "IDs must be provided for all handles, as handles with IDs already exist.");
	} else {
		ERR_FAIL_COND_MSG(p_handles.size() != p_ids.size(), "The number of IDs should be the same as the number of handles.");
	}

	bool is_current_hover_gizmo = Node3DEditor::get_singleton()->get_current_hover_gizmo() == this;
	bool current_hover_handle_secondary;
	int current_hover_handle = Node3DEditor::get_singleton()->get_current_hover_gizmo_handle(current_hover_handle_secondary);

	Instance ins;
	Ref<ArrayMesh> mesh = memnew(ArrayMesh);

	Array a;
	a.resize(RS::ARRAY_MAX);
	a[RS::ARRAY_VERTEX] = p_handles;
	Vector<Color> colors;
	{
		colors.resize(p_handles.size());
		Color *w = colors.ptrw();
		for (int i = 0; i < p_handles.size(); i++) {
			Color col(1, 1, 1, 1);
			if (is_handle_highlighted(i, p_secondary)) {
				col = Color(0, 0, 1, 0.9);
			}

			int id = p_ids.is_empty() ? i : p_ids[i];
			if (!is_current_hover_gizmo || current_hover_handle != id || p_secondary != current_hover_handle_secondary) {
				col.a = 0.8;
			}

			w[i] = col;
		}
	}
	a[RS::ARRAY_COLOR] = colors;
	mesh->add_surface_from_arrays(Mesh::PRIMITIVE_POINTS, a);
	mesh->surface_set_material(0, p_material);

	if (p_billboard) {
		float md = 0;
		for (int i = 0; i < p_handles.size(); i++) {
			md = MAX(0, p_handles[i].length());
		}
		if (md) {
			mesh->set_custom_aabb(AABB(Vector3(-md, -md, -md), Vector3(md, md, md) * 2.0));
		}
	}

	ins.mesh = mesh;
	ins.extra_margin = true;
	if (valid) {
		ins.create_instance(spatial_node, hidden);
		RS::get_singleton()->instance_set_transform(ins.instance, spatial_node->get_global_transform());
	}
	instances.push_back(ins);

	int current_size = handle_list.size();
	handle_list.resize(current_size + p_handles.size());
	for (int i = 0; i < p_handles.size(); i++) {
		handle_list.write[current_size + i] = p_handles[i];
	}

	if (!p_ids.is_empty()) {
		current_size = id_list.size();
		id_list.resize(current_size + p_ids.size());
		for (int i = 0; i < p_ids.size(); i++) {
			id_list.write[current_size + i] = p_ids[i];
		}
	}
}

void EditorNode3DGizmo::add_solid_box(const Ref<Material> &p_material, Vector3 p_size, Vector3 p_position, const Transform3D &p_xform) {
	ERR_FAIL_COND(!spatial_node);

	BoxMesh box_mesh;
	box_mesh.set_size(p_size);

	Array arrays = box_mesh.surface_get_arrays(0);
	PackedVector3Array vertex = arrays[RS::ARRAY_VERTEX];
	Vector3 *w = vertex.ptrw();

	for (int i = 0; i < vertex.size(); ++i) {
		w[i] += p_position;
	}

	arrays[RS::ARRAY_VERTEX] = vertex;

	Ref<ArrayMesh> m = memnew(ArrayMesh);
	m->add_surface_from_arrays(box_mesh.surface_get_primitive_type(0), arrays);
	add_mesh(m, p_material, p_xform);
}

bool EditorNode3DGizmo::intersect_frustum(const Camera3D *p_camera, const Vector<Plane> &p_frustum) {
	ERR_FAIL_COND_V(!spatial_node, false);
	ERR_FAIL_COND_V(!valid, false);

	if (hidden && !gizmo_plugin->is_selectable_when_hidden()) {
		return false;
	}

	if (selectable_icon_size > 0.0f) {
		Vector3 origin = spatial_node->get_global_transform().get_origin();

		const Plane *p = p_frustum.ptr();
		int fc = p_frustum.size();

		bool any_out = false;

		for (int j = 0; j < fc; j++) {
			if (p[j].is_point_over(origin)) {
				any_out = true;
				break;
			}
		}

		return !any_out;
	}

	if (collision_segments.size()) {
		const Plane *p = p_frustum.ptr();
		int fc = p_frustum.size();

		int vc = collision_segments.size();
		const Vector3 *vptr = collision_segments.ptr();
		Transform3D t = spatial_node->get_global_transform();

		bool any_out = false;
		for (int j = 0; j < fc; j++) {
			for (int i = 0; i < vc; i++) {
				Vector3 v = t.xform(vptr[i]);
				if (p[j].is_point_over(v)) {
					any_out = true;
					break;
				}
			}
			if (any_out) {
				break;
			}
		}

		if (!any_out) {
			return true;
		}
	}

	if (collision_mesh.is_valid()) {
		Transform3D t = spatial_node->get_global_transform();

		Vector3 mesh_scale = t.get_basis().get_scale();
		t.orthonormalize();

		Transform3D it = t.affine_inverse();

		Vector<Plane> transformed_frustum;
		int plane_count = p_frustum.size();
		transformed_frustum.resize(plane_count);

		for (int i = 0; i < plane_count; i++) {
			transformed_frustum.write[i] = it.xform(p_frustum[i]);
		}

		Vector<Vector3> convex_points = Geometry3D::compute_convex_mesh_points(transformed_frustum.ptr(), plane_count);
		if (collision_mesh->inside_convex_shape(transformed_frustum.ptr(), plane_count, convex_points.ptr(), convex_points.size(), mesh_scale)) {
			return true;
		}
	}

	return false;
}

void EditorNode3DGizmo::handles_intersect_ray(Camera3D *p_camera, const Vector2 &p_point, bool p_shift_pressed, int &r_id, bool &r_secondary) {
	r_id = -1;
	r_secondary = false;

	ERR_FAIL_COND(!spatial_node);
	ERR_FAIL_COND(!valid);

	if (hidden) {
		return;
	}

	Transform3D camera_xform = p_camera->get_global_transform();
	Transform3D t = spatial_node->get_global_transform();
	if (billboard_handle) {
		t.set_look_at(t.origin, t.origin - camera_xform.basis.get_column(2), camera_xform.basis.get_column(1));
	}

	float min_d = 1e20;

	for (int i = 0; i < secondary_handles.size(); i++) {
		Vector3 hpos = t.xform(secondary_handles[i]);
		Vector2 p = p_camera->unproject_position(hpos);

		if (p.distance_to(p_point) < HANDLE_HALF_SIZE) {
			real_t dp = p_camera->get_transform().origin.distance_to(hpos);
			if (dp < min_d) {
				min_d = dp;
				if (secondary_handle_ids.is_empty()) {
					r_id = i;
				} else {
					r_id = secondary_handle_ids[i];
				}
				r_secondary = true;
			}
		}
	}

	if (r_id != -1 && p_shift_pressed) {
		return;
	}

	min_d = 1e20;

	for (int i = 0; i < handles.size(); i++) {
		Vector3 hpos = t.xform(handles[i]);
		Vector2 p = p_camera->unproject_position(hpos);

		if (p.distance_to(p_point) < HANDLE_HALF_SIZE) {
			real_t dp = p_camera->get_transform().origin.distance_to(hpos);
			if (dp < min_d) {
				min_d = dp;
				if (handle_ids.is_empty()) {
					r_id = i;
				} else {
					r_id = handle_ids[i];
				}
				r_secondary = false;
			}
		}
	}
}

bool EditorNode3DGizmo::intersect_ray(Camera3D *p_camera, const Point2 &p_point, Vector3 &r_pos, Vector3 &r_normal) {
	ERR_FAIL_COND_V(!spatial_node, false);
	ERR_FAIL_COND_V(!valid, false);

	if (hidden && !gizmo_plugin->is_selectable_when_hidden()) {
		return false;
	}

	if (selectable_icon_size > 0.0f) {
		Transform3D t = spatial_node->get_global_transform();
		Vector3 camera_position = p_camera->get_camera_transform().origin;
		if (!camera_position.is_equal_approx(t.origin)) {
			t.set_look_at(t.origin, camera_position);
		}

		float scale = t.origin.distance_to(p_camera->get_camera_transform().origin);

		if (p_camera->get_projection() == Camera3D::PROJECTION_ORTHOGONAL) {
			float aspect = p_camera->get_viewport()->get_visible_rect().size.aspect();
			float size = p_camera->get_size();
			scale = size / aspect;
		}

		Point2 center = p_camera->unproject_position(t.origin);

		Transform3D orig_camera_transform = p_camera->get_camera_transform();

		if (!orig_camera_transform.origin.is_equal_approx(t.origin) &&
				ABS(orig_camera_transform.basis.get_column(Vector3::AXIS_Z).dot(Vector3(0, 1, 0))) < 0.99) {
			p_camera->look_at(t.origin);
		}

		Vector3 c0 = t.xform(Vector3(selectable_icon_size, selectable_icon_size, 0) * scale);
		Vector3 c1 = t.xform(Vector3(-selectable_icon_size, -selectable_icon_size, 0) * scale);

		Point2 p0 = p_camera->unproject_position(c0);
		Point2 p1 = p_camera->unproject_position(c1);

		p_camera->set_global_transform(orig_camera_transform);

		Rect2 rect(p0, (p1 - p0).abs());

		rect.set_position(center - rect.get_size() / 2.0);

		if (rect.has_point(p_point)) {
			r_pos = t.origin;
			r_normal = -p_camera->project_ray_normal(p_point);
			return true;
		}
	}

	if (collision_segments.size()) {
		Plane camp(-p_camera->get_transform().basis.get_column(2).normalized(), p_camera->get_transform().origin);

		int vc = collision_segments.size();
		const Vector3 *vptr = collision_segments.ptr();
		Transform3D t = spatial_node->get_global_transform();
		if (billboard_handle) {
			t.set_look_at(t.origin, t.origin - p_camera->get_transform().basis.get_column(2), p_camera->get_transform().basis.get_column(1));
		}

		Vector3 cp;
		float cpd = 1e20;

		for (int i = 0; i < vc / 2; i++) {
			Vector3 a = t.xform(vptr[i * 2 + 0]);
			Vector3 b = t.xform(vptr[i * 2 + 1]);
			Vector2 s[2];
			s[0] = p_camera->unproject_position(a);
			s[1] = p_camera->unproject_position(b);

			Vector2 p = Geometry2D::get_closest_point_to_segment(p_point, s);

			float pd = p.distance_to(p_point);

			if (pd < cpd) {
				float d = s[0].distance_to(s[1]);
				Vector3 tcp;
				if (d > 0) {
					float d2 = s[0].distance_to(p) / d;
					tcp = a + (b - a) * d2;

				} else {
					tcp = a;
				}

				if (camp.distance_to(tcp) < p_camera->get_near()) {
					continue;
				}
				cp = tcp;
				cpd = pd;
			}
		}

		if (cpd < 8) {
			r_pos = cp;
			r_normal = -p_camera->project_ray_normal(p_point);
			return true;
		}
	}

	if (collision_mesh.is_valid()) {
		Transform3D gt = spatial_node->get_global_transform();

		if (billboard_handle) {
			gt.set_look_at(gt.origin, gt.origin - p_camera->get_transform().basis.get_column(2), p_camera->get_transform().basis.get_column(1));
		}

		Transform3D ai = gt.affine_inverse();
		Vector3 ray_from = ai.xform(p_camera->project_ray_origin(p_point));
		Vector3 ray_dir = ai.basis.xform(p_camera->project_ray_normal(p_point)).normalized();
		Vector3 rpos, rnorm;

		if (collision_mesh->intersect_ray(ray_from, ray_dir, rpos, rnorm)) {
			r_pos = gt.xform(rpos);
			r_normal = gt.basis.xform(rnorm).normalized();
			return true;
		}
	}

	return false;
}

bool EditorNode3DGizmo::is_subgizmo_selected(int p_id) const {
	Node3DEditor *ed = Node3DEditor::get_singleton();
	ERR_FAIL_COND_V(!ed, false);
	return ed->is_current_selected_gizmo(this) && ed->is_subgizmo_selected(p_id);
}

Vector<int> EditorNode3DGizmo::get_subgizmo_selection() const {
	Vector<int> ret;

	Node3DEditor *ed = Node3DEditor::get_singleton();
	ERR_FAIL_COND_V(!ed, ret);

	if (ed->is_current_selected_gizmo(this)) {
		ret = ed->get_subgizmo_selection();
	}

	return ret;
}

void EditorNode3DGizmo::create() {
	ERR_FAIL_COND(!spatial_node);
	ERR_FAIL_COND(valid);
	valid = true;

	for (int i = 0; i < instances.size(); i++) {
		instances.write[i].create_instance(spatial_node, hidden);
	}

	transform();
}

void EditorNode3DGizmo::transform() {
	ERR_FAIL_COND(!spatial_node);
	ERR_FAIL_COND(!valid);
	for (int i = 0; i < instances.size(); i++) {
		RS::get_singleton()->instance_set_transform(instances[i].instance, spatial_node->get_global_transform() * instances[i].xform);
	}
}

void EditorNode3DGizmo::free() {
	ERR_FAIL_NULL(RenderingServer::get_singleton());
	ERR_FAIL_COND(!spatial_node);
	ERR_FAIL_COND(!valid);

	for (int i = 0; i < instances.size(); i++) {
		if (instances[i].instance.is_valid()) {
			RS::get_singleton()->free(instances[i].instance);
		}
		instances.write[i].instance = RID();
	}

	clear();

	valid = false;
}

void EditorNode3DGizmo::set_hidden(bool p_hidden) {
	hidden = p_hidden;
	int layer = hidden ? 0 : 1 << Node3DEditorViewport::GIZMO_EDIT_LAYER;
	for (int i = 0; i < instances.size(); ++i) {
		RS::get_singleton()->instance_set_layer_mask(instances[i].instance, layer);
	}
}

void EditorNode3DGizmo::set_plugin(EditorNode3DGizmoPlugin *p_plugin) {
	gizmo_plugin = p_plugin;
}

void EditorNode3DGizmo::_bind_methods() {
	ClassDB::bind_method(D_METHOD("add_lines", "lines", "material", "billboard", "modulate"), &EditorNode3DGizmo::add_lines, DEFVAL(false), DEFVAL(Color(1, 1, 1)));
	ClassDB::bind_method(D_METHOD("add_mesh", "mesh", "material", "transform", "skeleton"), &EditorNode3DGizmo::add_mesh, DEFVAL(Variant()), DEFVAL(Transform3D()), DEFVAL(Ref<SkinReference>()));
	ClassDB::bind_method(D_METHOD("add_collision_segments", "segments"), &EditorNode3DGizmo::add_collision_segments);
	ClassDB::bind_method(D_METHOD("add_collision_triangles", "triangles"), &EditorNode3DGizmo::add_collision_triangles);
	ClassDB::bind_method(D_METHOD("add_unscaled_billboard", "material", "default_scale", "modulate"), &EditorNode3DGizmo::add_unscaled_billboard, DEFVAL(1), DEFVAL(Color(1, 1, 1)));
	ClassDB::bind_method(D_METHOD("add_handles", "handles", "material", "ids", "billboard", "secondary"), &EditorNode3DGizmo::add_handles, DEFVAL(false), DEFVAL(false));
	ClassDB::bind_method(D_METHOD("set_node_3d", "node"), &EditorNode3DGizmo::_set_node_3d);
	ClassDB::bind_method(D_METHOD("get_node_3d"), &EditorNode3DGizmo::get_node_3d);
	ClassDB::bind_method(D_METHOD("get_plugin"), &EditorNode3DGizmo::get_plugin);
	ClassDB::bind_method(D_METHOD("clear"), &EditorNode3DGizmo::clear);
	ClassDB::bind_method(D_METHOD("set_hidden", "hidden"), &EditorNode3DGizmo::set_hidden);
	ClassDB::bind_method(D_METHOD("is_subgizmo_selected", "id"), &EditorNode3DGizmo::is_subgizmo_selected);
	ClassDB::bind_method(D_METHOD("get_subgizmo_selection"), &EditorNode3DGizmo::get_subgizmo_selection);

	GDVIRTUAL_BIND(_redraw);
	GDVIRTUAL_BIND(_get_handle_name, "id", "secondary");
	GDVIRTUAL_BIND(_is_handle_highlighted, "id", "secondary");

	GDVIRTUAL_BIND(_get_handle_value, "id", "secondary");
	GDVIRTUAL_BIND(_set_handle, "id", "secondary", "camera", "point");
	GDVIRTUAL_BIND(_commit_handle, "id", "secondary", "restore", "cancel");

	GDVIRTUAL_BIND(_subgizmos_intersect_ray, "camera", "point");
	GDVIRTUAL_BIND(_subgizmos_intersect_frustum, "camera", "frustum");
	GDVIRTUAL_BIND(_set_subgizmo_transform, "id", "transform");
	GDVIRTUAL_BIND(_get_subgizmo_transform, "id");
	GDVIRTUAL_BIND(_commit_subgizmos, "ids", "restores", "cancel");
}

EditorNode3DGizmo::EditorNode3DGizmo() {
	valid = false;
	billboard_handle = false;
	hidden = false;
	selected = false;
	spatial_node = nullptr;
	gizmo_plugin = nullptr;
	selectable_icon_size = -1.0f;
}

EditorNode3DGizmo::~EditorNode3DGizmo() {
	if (gizmo_plugin != nullptr) {
		gizmo_plugin->unregister_gizmo(this);
	}
	clear();
}

/////

void EditorNode3DGizmoPlugin::create_material(const String &p_name, const Color &p_color, bool p_billboard, bool p_on_top, bool p_use_vertex_color) {
	Color instantiated_color = EDITOR_GET("editors/3d_gizmos/gizmo_colors/instantiated");

	Vector<Ref<StandardMaterial3D>> mats;

	for (int i = 0; i < 4; i++) {
		bool selected = i % 2 == 1;
		bool instantiated = i < 2;

		Ref<StandardMaterial3D> material = Ref<StandardMaterial3D>(memnew(StandardMaterial3D));

		Color color = instantiated ? instantiated_color : p_color;

		if (!selected) {
			color.a *= 0.3;
		}

		material->set_albedo(color);
		material->set_shading_mode(StandardMaterial3D::SHADING_MODE_UNSHADED);
		material->set_transparency(StandardMaterial3D::TRANSPARENCY_ALPHA);
		material->set_render_priority(StandardMaterial3D::RENDER_PRIORITY_MIN + 1);
		material->set_cull_mode(StandardMaterial3D::CULL_DISABLED);

		if (p_use_vertex_color) {
			material->set_flag(StandardMaterial3D::FLAG_ALBEDO_FROM_VERTEX_COLOR, true);
			material->set_flag(StandardMaterial3D::FLAG_SRGB_VERTEX_COLOR, true);
		}

		if (p_billboard) {
			material->set_billboard_mode(StandardMaterial3D::BILLBOARD_ENABLED);
		}

		if (p_on_top && selected) {
			material->set_on_top_of_alpha();
		}

		mats.push_back(material);
	}

	materials[p_name] = mats;
}

void EditorNode3DGizmoPlugin::create_icon_material(const String &p_name, const Ref<Texture2D> &p_texture, bool p_on_top, const Color &p_albedo) {
	Color instantiated_color = EDITOR_GET("editors/3d_gizmos/gizmo_colors/instantiated");

	Vector<Ref<StandardMaterial3D>> icons;

	for (int i = 0; i < 4; i++) {
		bool selected = i % 2 == 1;
		bool instantiated = i < 2;

		Ref<StandardMaterial3D> icon = Ref<StandardMaterial3D>(memnew(StandardMaterial3D));

		Color color = instantiated ? instantiated_color : p_albedo;

		if (!selected) {
			color.a *= 0.85;
		}

		icon->set_albedo(color);

		icon->set_shading_mode(StandardMaterial3D::SHADING_MODE_UNSHADED);
		icon->set_flag(StandardMaterial3D::FLAG_ALBEDO_FROM_VERTEX_COLOR, true);
		icon->set_flag(StandardMaterial3D::FLAG_SRGB_VERTEX_COLOR, true);
		icon->set_cull_mode(StandardMaterial3D::CULL_DISABLED);
		icon->set_depth_draw_mode(StandardMaterial3D::DEPTH_DRAW_DISABLED);
		icon->set_transparency(StandardMaterial3D::TRANSPARENCY_ALPHA);
		icon->set_texture(StandardMaterial3D::TEXTURE_ALBEDO, p_texture);
		icon->set_flag(StandardMaterial3D::FLAG_FIXED_SIZE, true);
		icon->set_billboard_mode(StandardMaterial3D::BILLBOARD_ENABLED);
		icon->set_render_priority(StandardMaterial3D::RENDER_PRIORITY_MIN);

		if (p_on_top && selected) {
			icon->set_on_top_of_alpha();
		}

		icons.push_back(icon);
	}

	materials[p_name] = icons;
}

void EditorNode3DGizmoPlugin::create_handle_material(const String &p_name, bool p_billboard, const Ref<Texture2D> &p_icon) {
	Ref<StandardMaterial3D> handle_material = Ref<StandardMaterial3D>(memnew(StandardMaterial3D));

	handle_material->set_shading_mode(StandardMaterial3D::SHADING_MODE_UNSHADED);
	handle_material->set_flag(StandardMaterial3D::FLAG_USE_POINT_SIZE, true);
	Ref<Texture2D> handle_t = p_icon != nullptr ? p_icon : Node3DEditor::get_singleton()->get_theme_icon(SNAME("Editor3DHandle"), SNAME("EditorIcons"));
	handle_material->set_point_size(handle_t->get_width());
	handle_material->set_texture(StandardMaterial3D::TEXTURE_ALBEDO, handle_t);
	handle_material->set_albedo(Color(1, 1, 1));
	handle_material->set_flag(StandardMaterial3D::FLAG_ALBEDO_FROM_VERTEX_COLOR, true);
	handle_material->set_flag(StandardMaterial3D::FLAG_SRGB_VERTEX_COLOR, true);
	handle_material->set_on_top_of_alpha();
	if (p_billboard) {
		handle_material->set_billboard_mode(StandardMaterial3D::BILLBOARD_ENABLED);
		handle_material->set_on_top_of_alpha();
	}
	handle_material->set_transparency(StandardMaterial3D::TRANSPARENCY_ALPHA);

	materials[p_name] = Vector<Ref<StandardMaterial3D>>();
	materials[p_name].push_back(handle_material);
}

void EditorNode3DGizmoPlugin::add_material(const String &p_name, Ref<StandardMaterial3D> p_material) {
	materials[p_name] = Vector<Ref<StandardMaterial3D>>();
	materials[p_name].push_back(p_material);
}

Ref<StandardMaterial3D> EditorNode3DGizmoPlugin::get_material(const String &p_name, const Ref<EditorNode3DGizmo> &p_gizmo) {
	ERR_FAIL_COND_V(!materials.has(p_name), Ref<StandardMaterial3D>());
	ERR_FAIL_COND_V(materials[p_name].size() == 0, Ref<StandardMaterial3D>());

	if (p_gizmo.is_null() || materials[p_name].size() == 1) {
		return materials[p_name][0];
	}

	int index = (p_gizmo->is_selected() ? 1 : 0) + (p_gizmo->is_editable() ? 2 : 0);

	Ref<StandardMaterial3D> mat = materials[p_name][index];

	if (current_state == ON_TOP && p_gizmo->is_selected()) {
		mat->set_flag(StandardMaterial3D::FLAG_DISABLE_DEPTH_TEST, true);
	} else {
		mat->set_flag(StandardMaterial3D::FLAG_DISABLE_DEPTH_TEST, false);
	}

	return mat;
}

String EditorNode3DGizmoPlugin::get_gizmo_name() const {
	String ret;
	if (GDVIRTUAL_CALL(_get_gizmo_name, ret)) {
		return ret;
	}

	WARN_PRINT_ONCE("A 3D editor gizmo has no name defined (it will appear as \"Unnamed Gizmo\" in the \"View > Gizmos\" menu). To resolve this, override the `_get_gizmo_name()` function to return a String in the script that extends EditorNode3DGizmoPlugin.");
	return TTR("Unnamed Gizmo");
}

int EditorNode3DGizmoPlugin::get_priority() const {
	int ret;
	if (GDVIRTUAL_CALL(_get_priority, ret)) {
		return ret;
	}
	return 0;
}

Ref<EditorNode3DGizmo> EditorNode3DGizmoPlugin::get_gizmo(Node3D *p_spatial) {
	if (get_script_instance() && get_script_instance()->has_method("_get_gizmo")) {
		return get_script_instance()->call("_get_gizmo", p_spatial);
	}

	Ref<EditorNode3DGizmo> ref = create_gizmo(p_spatial);

	if (ref.is_null()) {
		return ref;
	}

	ref->set_plugin(this);
	ref->set_node_3d(p_spatial);
	ref->set_hidden(current_state == HIDDEN);

	current_gizmos.push_back(ref.ptr());
	return ref;
}

void EditorNode3DGizmoPlugin::_bind_methods() {
	ClassDB::bind_method(D_METHOD("create_material", "name", "color", "billboard", "on_top", "use_vertex_color"), &EditorNode3DGizmoPlugin::create_material, DEFVAL(false), DEFVAL(false), DEFVAL(false));
	ClassDB::bind_method(D_METHOD("create_icon_material", "name", "texture", "on_top", "color"), &EditorNode3DGizmoPlugin::create_icon_material, DEFVAL(false), DEFVAL(Color(1, 1, 1, 1)));
	ClassDB::bind_method(D_METHOD("create_handle_material", "name", "billboard", "texture"), &EditorNode3DGizmoPlugin::create_handle_material, DEFVAL(false), DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("add_material", "name", "material"), &EditorNode3DGizmoPlugin::add_material);

	ClassDB::bind_method(D_METHOD("get_material", "name", "gizmo"), &EditorNode3DGizmoPlugin::get_material, DEFVAL(Ref<EditorNode3DGizmo>()));

	GDVIRTUAL_BIND(_has_gizmo, "for_node_3d");
	GDVIRTUAL_BIND(_create_gizmo, "for_node_3d");

	GDVIRTUAL_BIND(_get_gizmo_name);
	GDVIRTUAL_BIND(_get_priority);
	GDVIRTUAL_BIND(_can_be_hidden);
	GDVIRTUAL_BIND(_is_selectable_when_hidden);

	GDVIRTUAL_BIND(_redraw, "gizmo");
	GDVIRTUAL_BIND(_get_handle_name, "gizmo", "handle_id", "secondary");
	GDVIRTUAL_BIND(_is_handle_highlighted, "gizmo", "handle_id", "secondary");
	GDVIRTUAL_BIND(_get_handle_value, "gizmo", "handle_id", "secondary");

	GDVIRTUAL_BIND(_set_handle, "gizmo", "handle_id", "secondary", "camera", "screen_pos");
	GDVIRTUAL_BIND(_commit_handle, "gizmo", "handle_id", "secondary", "restore", "cancel");

	GDVIRTUAL_BIND(_subgizmos_intersect_ray, "gizmo", "camera", "screen_pos");
	GDVIRTUAL_BIND(_subgizmos_intersect_frustum, "gizmo", "camera", "frustum_planes");
	GDVIRTUAL_BIND(_get_subgizmo_transform, "gizmo", "subgizmo_id");
	GDVIRTUAL_BIND(_set_subgizmo_transform, "gizmo", "subgizmo_id", "transform");
	GDVIRTUAL_BIND(_commit_subgizmos, "gizmo", "ids", "restores", "cancel");
}

bool EditorNode3DGizmoPlugin::has_gizmo(Node3D *p_spatial) {
	bool success = false;
	GDVIRTUAL_CALL(_has_gizmo, p_spatial, success);
	return success;
}

Ref<EditorNode3DGizmo> EditorNode3DGizmoPlugin::create_gizmo(Node3D *p_spatial) {
	Ref<EditorNode3DGizmo> ret;
	if (GDVIRTUAL_CALL(_create_gizmo, p_spatial, ret)) {
		return ret;
	}

	Ref<EditorNode3DGizmo> ref;
	if (has_gizmo(p_spatial)) {
		ref.instantiate();
	}
	return ref;
}

bool EditorNode3DGizmoPlugin::can_be_hidden() const {
	bool ret = true;
	GDVIRTUAL_CALL(_can_be_hidden, ret);
	return ret;
}

bool EditorNode3DGizmoPlugin::is_selectable_when_hidden() const {
	bool ret = false;
	GDVIRTUAL_CALL(_is_selectable_when_hidden, ret);
	return ret;
}

void EditorNode3DGizmoPlugin::redraw(EditorNode3DGizmo *p_gizmo) {
	GDVIRTUAL_CALL(_redraw, p_gizmo);
}

bool EditorNode3DGizmoPlugin::is_handle_highlighted(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary) const {
	bool ret = false;
	GDVIRTUAL_CALL(_is_handle_highlighted, Ref<EditorNode3DGizmo>(p_gizmo), p_id, p_secondary, ret);
	return ret;
}

String EditorNode3DGizmoPlugin::get_handle_name(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary) const {
	String ret;
	GDVIRTUAL_CALL(_get_handle_name, Ref<EditorNode3DGizmo>(p_gizmo), p_id, p_secondary, ret);
	return ret;
}

Variant EditorNode3DGizmoPlugin::get_handle_value(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary) const {
	Variant ret;
	GDVIRTUAL_CALL(_get_handle_value, Ref<EditorNode3DGizmo>(p_gizmo), p_id, p_secondary, ret);
	return ret;
}

void EditorNode3DGizmoPlugin::set_handle(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary, Camera3D *p_camera, const Point2 &p_point) {
	GDVIRTUAL_CALL(_set_handle, Ref<EditorNode3DGizmo>(p_gizmo), p_id, p_secondary, p_camera, p_point);
}

void EditorNode3DGizmoPlugin::commit_handle(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary, const Variant &p_restore, bool p_cancel) {
	GDVIRTUAL_CALL(_commit_handle, Ref<EditorNode3DGizmo>(p_gizmo), p_id, p_secondary, p_restore, p_cancel);
}

int EditorNode3DGizmoPlugin::subgizmos_intersect_ray(const EditorNode3DGizmo *p_gizmo, Camera3D *p_camera, const Vector2 &p_point) const {
	int ret = -1;
	GDVIRTUAL_CALL(_subgizmos_intersect_ray, Ref<EditorNode3DGizmo>(p_gizmo), p_camera, p_point, ret);
	return ret;
}

Vector<int> EditorNode3DGizmoPlugin::subgizmos_intersect_frustum(const EditorNode3DGizmo *p_gizmo, const Camera3D *p_camera, const Vector<Plane> &p_frustum) const {
	TypedArray<Plane> frustum;
	frustum.resize(p_frustum.size());
	for (int i = 0; i < p_frustum.size(); i++) {
		frustum[i] = p_frustum[i];
	}
	Vector<int> ret;
	GDVIRTUAL_CALL(_subgizmos_intersect_frustum, Ref<EditorNode3DGizmo>(p_gizmo), p_camera, frustum, ret);
	return ret;
}

Transform3D EditorNode3DGizmoPlugin::get_subgizmo_transform(const EditorNode3DGizmo *p_gizmo, int p_id) const {
	Transform3D ret;
	GDVIRTUAL_CALL(_get_subgizmo_transform, Ref<EditorNode3DGizmo>(p_gizmo), p_id, ret);
	return ret;
}

void EditorNode3DGizmoPlugin::set_subgizmo_transform(const EditorNode3DGizmo *p_gizmo, int p_id, Transform3D p_transform) {
	GDVIRTUAL_CALL(_set_subgizmo_transform, Ref<EditorNode3DGizmo>(p_gizmo), p_id, p_transform);
}

void EditorNode3DGizmoPlugin::commit_subgizmos(const EditorNode3DGizmo *p_gizmo, const Vector<int> &p_ids, const Vector<Transform3D> &p_restore, bool p_cancel) {
	TypedArray<Transform3D> restore;
	restore.resize(p_restore.size());
	for (int i = 0; i < p_restore.size(); i++) {
		restore[i] = p_restore[i];
	}

	GDVIRTUAL_CALL(_commit_subgizmos, Ref<EditorNode3DGizmo>(p_gizmo), p_ids, restore, p_cancel);
}

void EditorNode3DGizmoPlugin::set_state(int p_state) {
	current_state = p_state;
	for (int i = 0; i < current_gizmos.size(); ++i) {
		current_gizmos[i]->set_hidden(current_state == HIDDEN);
	}
}

int EditorNode3DGizmoPlugin::get_state() const {
	return current_state;
}

void EditorNode3DGizmoPlugin::unregister_gizmo(EditorNode3DGizmo *p_gizmo) {
	current_gizmos.erase(p_gizmo);
}

EditorNode3DGizmoPlugin::EditorNode3DGizmoPlugin() {
	current_state = VISIBLE;
}

EditorNode3DGizmoPlugin::~EditorNode3DGizmoPlugin() {
	for (int i = 0; i < current_gizmos.size(); ++i) {
		current_gizmos[i]->set_plugin(nullptr);
		current_gizmos[i]->get_node_3d()->remove_gizmo(current_gizmos[i]);
	}
	if (Node3DEditor::get_singleton()) {
		Node3DEditor::get_singleton()->update_all_gizmos();
	}
}

//// light gizmo

Light3DGizmoPlugin::Light3DGizmoPlugin() {
	// Enable vertex colors for the materials below as the gizmo color depends on the light color.
	create_material("lines_primary", Color(1, 1, 1), false, false, true);
	create_material("lines_secondary", Color(1, 1, 1, 0.35), false, false, true);
	create_material("lines_billboard", Color(1, 1, 1), true, false, true);

	create_icon_material("light_directional_icon", Node3DEditor::get_singleton()->get_theme_icon(SNAME("GizmoDirectionalLight"), SNAME("EditorIcons")));
	create_icon_material("light_omni_icon", Node3DEditor::get_singleton()->get_theme_icon(SNAME("GizmoLight"), SNAME("EditorIcons")));
	create_icon_material("light_spot_icon", Node3DEditor::get_singleton()->get_theme_icon(SNAME("GizmoSpotLight"), SNAME("EditorIcons")));

	create_handle_material("handles");
	create_handle_material("handles_billboard", true);
}

bool Light3DGizmoPlugin::has_gizmo(Node3D *p_spatial) {
	return Object::cast_to<Light3D>(p_spatial) != nullptr;
}

String Light3DGizmoPlugin::get_gizmo_name() const {
	return "Light3D";
}

int Light3DGizmoPlugin::get_priority() const {
	return -1;
}

String Light3DGizmoPlugin::get_handle_name(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary) const {
	if (p_id == 0) {
		return "Radius";
	} else {
		return "Aperture";
	}
}

Variant Light3DGizmoPlugin::get_handle_value(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary) const {
	Light3D *light = Object::cast_to<Light3D>(p_gizmo->get_node_3d());
	if (p_id == 0) {
		return light->get_param(Light3D::PARAM_RANGE);
	}
	if (p_id == 1) {
		return light->get_param(Light3D::PARAM_SPOT_ANGLE);
	}

	return Variant();
}

static float _find_closest_angle_to_half_pi_arc(const Vector3 &p_from, const Vector3 &p_to, float p_arc_radius, const Transform3D &p_arc_xform) {
	//bleh, discrete is simpler
	static const int arc_test_points = 64;
	float min_d = 1e20;
	Vector3 min_p;

	for (int i = 0; i < arc_test_points; i++) {
		float a = i * Math_PI * 0.5 / arc_test_points;
		float an = (i + 1) * Math_PI * 0.5 / arc_test_points;
		Vector3 p = Vector3(Math::cos(a), 0, -Math::sin(a)) * p_arc_radius;
		Vector3 n = Vector3(Math::cos(an), 0, -Math::sin(an)) * p_arc_radius;

		Vector3 ra, rb;
		Geometry3D::get_closest_points_between_segments(p, n, p_from, p_to, ra, rb);

		float d = ra.distance_to(rb);
		if (d < min_d) {
			min_d = d;
			min_p = ra;
		}
	}

	//min_p = p_arc_xform.affine_inverse().xform(min_p);
	float a = (Math_PI * 0.5) - Vector2(min_p.x, -min_p.z).angle();
	return Math::rad_to_deg(a);
}

void Light3DGizmoPlugin::set_handle(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary, Camera3D *p_camera, const Point2 &p_point) {
	Light3D *light = Object::cast_to<Light3D>(p_gizmo->get_node_3d());
	Transform3D gt = light->get_global_transform();
	Transform3D gi = gt.affine_inverse();

	Vector3 ray_from = p_camera->project_ray_origin(p_point);
	Vector3 ray_dir = p_camera->project_ray_normal(p_point);

	Vector3 s[2] = { gi.xform(ray_from), gi.xform(ray_from + ray_dir * 4096) };
	if (p_id == 0) {
		if (Object::cast_to<SpotLight3D>(light)) {
			Vector3 ra, rb;
			Geometry3D::get_closest_points_between_segments(Vector3(), Vector3(0, 0, -4096), s[0], s[1], ra, rb);

			float d = -ra.z;
			if (Node3DEditor::get_singleton()->is_snap_enabled()) {
				d = Math::snapped(d, Node3DEditor::get_singleton()->get_translate_snap());
			}

			if (d <= 0) { // Equal is here for negative zero.
				d = 0;
			}

			light->set_param(Light3D::PARAM_RANGE, d);
		} else if (Object::cast_to<OmniLight3D>(light)) {
			Plane cp = Plane(p_camera->get_transform().basis.get_column(2), gt.origin);

			Vector3 inters;
			if (cp.intersects_ray(ray_from, ray_dir, &inters)) {
				float r = inters.distance_to(gt.origin);
				if (Node3DEditor::get_singleton()->is_snap_enabled()) {
					r = Math::snapped(r, Node3DEditor::get_singleton()->get_translate_snap());
				}

				light->set_param(Light3D::PARAM_RANGE, r);
			}
		}

	} else if (p_id == 1) {
		float a = _find_closest_angle_to_half_pi_arc(s[0], s[1], light->get_param(Light3D::PARAM_RANGE), gt);
		light->set_param(Light3D::PARAM_SPOT_ANGLE, CLAMP(a, 0.01, 89.99));
	}
}

void Light3DGizmoPlugin::commit_handle(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary, const Variant &p_restore, bool p_cancel) {
	Light3D *light = Object::cast_to<Light3D>(p_gizmo->get_node_3d());
	if (p_cancel) {
		light->set_param(p_id == 0 ? Light3D::PARAM_RANGE : Light3D::PARAM_SPOT_ANGLE, p_restore);

	} else if (p_id == 0) {
		EditorUndoRedoManager *ur = EditorUndoRedoManager::get_singleton();
		ur->create_action(TTR("Change Light Radius"));
		ur->add_do_method(light, "set_param", Light3D::PARAM_RANGE, light->get_param(Light3D::PARAM_RANGE));
		ur->add_undo_method(light, "set_param", Light3D::PARAM_RANGE, p_restore);
		ur->commit_action();
	} else if (p_id == 1) {
		EditorUndoRedoManager *ur = EditorUndoRedoManager::get_singleton();
		ur->create_action(TTR("Change Light Radius"));
		ur->add_do_method(light, "set_param", Light3D::PARAM_SPOT_ANGLE, light->get_param(Light3D::PARAM_SPOT_ANGLE));
		ur->add_undo_method(light, "set_param", Light3D::PARAM_SPOT_ANGLE, p_restore);
		ur->commit_action();
	}
}

void Light3DGizmoPlugin::redraw(EditorNode3DGizmo *p_gizmo) {
	Light3D *light = Object::cast_to<Light3D>(p_gizmo->get_node_3d());

	Color color = light->get_color().srgb_to_linear() * light->get_correlated_color().srgb_to_linear();
	color = color.linear_to_srgb();
	// Make the gizmo color as bright as possible for better visibility
	color.set_hsv(color.get_h(), color.get_s(), 1);

	p_gizmo->clear();

	if (Object::cast_to<DirectionalLight3D>(light)) {
		Ref<Material> material = get_material("lines_primary", p_gizmo);
		Ref<Material> icon = get_material("light_directional_icon", p_gizmo);

		const int arrow_points = 7;
		const float arrow_length = 1.5;

		Vector3 arrow[arrow_points] = {
			Vector3(0, 0, -1),
			Vector3(0, 0.8, 0),
			Vector3(0, 0.3, 0),
			Vector3(0, 0.3, arrow_length),
			Vector3(0, -0.3, arrow_length),
			Vector3(0, -0.3, 0),
			Vector3(0, -0.8, 0)
		};

		int arrow_sides = 2;

		Vector<Vector3> lines;

		for (int i = 0; i < arrow_sides; i++) {
			for (int j = 0; j < arrow_points; j++) {
				Basis ma(Vector3(0, 0, 1), Math_PI * i / arrow_sides);

				Vector3 v1 = arrow[j] - Vector3(0, 0, arrow_length);
				Vector3 v2 = arrow[(j + 1) % arrow_points] - Vector3(0, 0, arrow_length);

				lines.push_back(ma.xform(v1));
				lines.push_back(ma.xform(v2));
			}
		}

		p_gizmo->add_lines(lines, material, false, color);
		p_gizmo->add_unscaled_billboard(icon, 0.05, color);
	}

	if (Object::cast_to<OmniLight3D>(light)) {
		// Use both a billboard circle and 3 non-billboard circles for a better sphere-like representation
		const Ref<Material> lines_material = get_material("lines_secondary", p_gizmo);
		const Ref<Material> lines_billboard_material = get_material("lines_billboard", p_gizmo);
		const Ref<Material> icon = get_material("light_omni_icon", p_gizmo);

		OmniLight3D *on = Object::cast_to<OmniLight3D>(light);
		const float r = on->get_param(Light3D::PARAM_RANGE);
		Vector<Vector3> points;
		Vector<Vector3> points_billboard;

		for (int i = 0; i < 120; i++) {
			// Create a circle
			const float ra = Math::deg_to_rad((float)(i * 3));
			const float rb = Math::deg_to_rad((float)((i + 1) * 3));
			const Point2 a = Vector2(Math::sin(ra), Math::cos(ra)) * r;
			const Point2 b = Vector2(Math::sin(rb), Math::cos(rb)) * r;

			// Draw axis-aligned circles
			points.push_back(Vector3(a.x, 0, a.y));
			points.push_back(Vector3(b.x, 0, b.y));
			points.push_back(Vector3(0, a.x, a.y));
			points.push_back(Vector3(0, b.x, b.y));
			points.push_back(Vector3(a.x, a.y, 0));
			points.push_back(Vector3(b.x, b.y, 0));

			// Draw a billboarded circle
			points_billboard.push_back(Vector3(a.x, a.y, 0));
			points_billboard.push_back(Vector3(b.x, b.y, 0));
		}

		p_gizmo->add_lines(points, lines_material, true, color);
		p_gizmo->add_lines(points_billboard, lines_billboard_material, true, color);
		p_gizmo->add_unscaled_billboard(icon, 0.05, color);

		Vector<Vector3> handles;
		handles.push_back(Vector3(r, 0, 0));
		p_gizmo->add_handles(handles, get_material("handles_billboard"), Vector<int>(), true);
	}

	if (Object::cast_to<SpotLight3D>(light)) {
		const Ref<Material> material_primary = get_material("lines_primary", p_gizmo);
		const Ref<Material> material_secondary = get_material("lines_secondary", p_gizmo);
		const Ref<Material> icon = get_material("light_spot_icon", p_gizmo);

		Vector<Vector3> points_primary;
		Vector<Vector3> points_secondary;
		SpotLight3D *sl = Object::cast_to<SpotLight3D>(light);

		float r = sl->get_param(Light3D::PARAM_RANGE);
		float w = r * Math::sin(Math::deg_to_rad(sl->get_param(Light3D::PARAM_SPOT_ANGLE)));
		float d = r * Math::cos(Math::deg_to_rad(sl->get_param(Light3D::PARAM_SPOT_ANGLE)));

		for (int i = 0; i < 120; i++) {
			// Draw a circle
			const float ra = Math::deg_to_rad((float)(i * 3));
			const float rb = Math::deg_to_rad((float)((i + 1) * 3));
			const Point2 a = Vector2(Math::sin(ra), Math::cos(ra)) * w;
			const Point2 b = Vector2(Math::sin(rb), Math::cos(rb)) * w;

			points_primary.push_back(Vector3(a.x, a.y, -d));
			points_primary.push_back(Vector3(b.x, b.y, -d));

			if (i % 15 == 0) {
				// Draw 8 lines from the cone origin to the sides of the circle
				points_secondary.push_back(Vector3(a.x, a.y, -d));
				points_secondary.push_back(Vector3());
			}
		}

		points_primary.push_back(Vector3(0, 0, -r));
		points_primary.push_back(Vector3());

		p_gizmo->add_lines(points_primary, material_primary, false, color);
		p_gizmo->add_lines(points_secondary, material_secondary, false, color);

		Vector<Vector3> handles = {
			Vector3(0, 0, -r),
			Vector3(w, 0, -d)
		};

		p_gizmo->add_handles(handles, get_material("handles"));
		p_gizmo->add_unscaled_billboard(icon, 0.05, color);
	}
}

//// player gizmo
AudioStreamPlayer3DGizmoPlugin::AudioStreamPlayer3DGizmoPlugin() {
	Color gizmo_color = EDITOR_DEF("editors/3d_gizmos/gizmo_colors/stream_player_3d", Color(0.4, 0.8, 1));

	create_icon_material("stream_player_3d_icon", Node3DEditor::get_singleton()->get_theme_icon(SNAME("Gizmo3DSamplePlayer"), SNAME("EditorIcons")));
	create_material("stream_player_3d_material_primary", gizmo_color);
	create_material("stream_player_3d_material_secondary", gizmo_color * Color(1, 1, 1, 0.35));
	// Enable vertex colors for the billboard material as the gizmo color depends on the
	// AudioStreamPlayer3D attenuation type and source (Unit Size or Max Distance).
	create_material("stream_player_3d_material_billboard", Color(1, 1, 1), true, false, true);
	create_handle_material("handles");
}

bool AudioStreamPlayer3DGizmoPlugin::has_gizmo(Node3D *p_spatial) {
	return Object::cast_to<AudioStreamPlayer3D>(p_spatial) != nullptr;
}

String AudioStreamPlayer3DGizmoPlugin::get_gizmo_name() const {
	return "AudioStreamPlayer3D";
}

int AudioStreamPlayer3DGizmoPlugin::get_priority() const {
	return -1;
}

String AudioStreamPlayer3DGizmoPlugin::get_handle_name(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary) const {
	return "Emission Radius";
}

Variant AudioStreamPlayer3DGizmoPlugin::get_handle_value(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary) const {
	AudioStreamPlayer3D *player = Object::cast_to<AudioStreamPlayer3D>(p_gizmo->get_node_3d());
	return player->get_emission_angle();
}

void AudioStreamPlayer3DGizmoPlugin::set_handle(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary, Camera3D *p_camera, const Point2 &p_point) {
	AudioStreamPlayer3D *player = Object::cast_to<AudioStreamPlayer3D>(p_gizmo->get_node_3d());

	Transform3D gt = player->get_global_transform();
	Transform3D gi = gt.affine_inverse();

	Vector3 ray_from = p_camera->project_ray_origin(p_point);
	Vector3 ray_dir = p_camera->project_ray_normal(p_point);
	Vector3 ray_to = ray_from + ray_dir * 4096;

	ray_from = gi.xform(ray_from);
	ray_to = gi.xform(ray_to);

	float closest_dist = 1e20;
	float closest_angle = 1e20;

	for (int i = 0; i < 180; i++) {
		float a = Math::deg_to_rad((float)i);
		float an = Math::deg_to_rad((float)(i + 1));

		Vector3 from(Math::sin(a), 0, -Math::cos(a));
		Vector3 to(Math::sin(an), 0, -Math::cos(an));

		Vector3 r1, r2;
		Geometry3D::get_closest_points_between_segments(from, to, ray_from, ray_to, r1, r2);
		float d = r1.distance_to(r2);
		if (d < closest_dist) {
			closest_dist = d;
			closest_angle = i;
		}
	}

	if (closest_angle < 91) {
		player->set_emission_angle(closest_angle);
	}
}

void AudioStreamPlayer3DGizmoPlugin::commit_handle(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary, const Variant &p_restore, bool p_cancel) {
	AudioStreamPlayer3D *player = Object::cast_to<AudioStreamPlayer3D>(p_gizmo->get_node_3d());

	if (p_cancel) {
		player->set_emission_angle(p_restore);

	} else {
		EditorUndoRedoManager *ur = EditorUndoRedoManager::get_singleton();
		ur->create_action(TTR("Change AudioStreamPlayer3D Emission Angle"));
		ur->add_do_method(player, "set_emission_angle", player->get_emission_angle());
		ur->add_undo_method(player, "set_emission_angle", p_restore);
		ur->commit_action();
	}
}

void AudioStreamPlayer3DGizmoPlugin::redraw(EditorNode3DGizmo *p_gizmo) {
	const AudioStreamPlayer3D *player = Object::cast_to<AudioStreamPlayer3D>(p_gizmo->get_node_3d());

	p_gizmo->clear();

	const Ref<Material> icon = get_material("stream_player_3d_icon", p_gizmo);

	if (player->get_attenuation_model() != AudioStreamPlayer3D::ATTENUATION_DISABLED || player->get_max_distance() > CMP_EPSILON) {
		// Draw a circle to represent sound volume attenuation.
		// Use only a billboard circle to represent radius.
		// This helps distinguish AudioStreamPlayer3D gizmos from OmniLight3D gizmos.
		const Ref<Material> lines_billboard_material = get_material("stream_player_3d_material_billboard", p_gizmo);

		// Soft distance cap varies depending on attenuation model, as some will fade out more aggressively than others.
		// Multipliers were empirically determined through testing.
		float soft_multiplier;
		switch (player->get_attenuation_model()) {
			case AudioStreamPlayer3D::ATTENUATION_INVERSE_DISTANCE:
				soft_multiplier = 12.0;
				break;
			case AudioStreamPlayer3D::ATTENUATION_INVERSE_SQUARE_DISTANCE:
				soft_multiplier = 4.0;
				break;
			case AudioStreamPlayer3D::ATTENUATION_LOGARITHMIC:
				soft_multiplier = 3.25;
				break;
			default:
				// Ensures Max Distance's radius visualization is not capped by Unit Size
				// (when the attenuation mode is Disabled).
				soft_multiplier = 10000.0;
				break;
		}

		// Draw the distance at which the sound can be reasonably heard.
		// This can be either a hard distance cap with the Max Distance property (if set above 0.0),
		// or a soft distance cap with the Unit Size property (sound never reaches true zero).
		// When Max Distance is 0.0, `r` represents the distance above which the
		// sound can't be heard in *most* (but not all) scenarios.
		float r;
		if (player->get_max_distance() > CMP_EPSILON) {
			r = MIN(player->get_unit_size() * soft_multiplier, player->get_max_distance());
		} else {
			r = player->get_unit_size() * soft_multiplier;
		}
		Vector<Vector3> points_billboard;

		for (int i = 0; i < 120; i++) {
			// Create a circle.
			const float ra = Math::deg_to_rad((float)(i * 3));
			const float rb = Math::deg_to_rad((float)((i + 1) * 3));
			const Point2 a = Vector2(Math::sin(ra), Math::cos(ra)) * r;
			const Point2 b = Vector2(Math::sin(rb), Math::cos(rb)) * r;

			// Draw a billboarded circle.
			points_billboard.push_back(Vector3(a.x, a.y, 0));
			points_billboard.push_back(Vector3(b.x, b.y, 0));
		}

		Color color;
		switch (player->get_attenuation_model()) {
			// Pick cold colors for all attenuation models (except Disabled),
			// so that soft caps can be easily distinguished from hard caps
			// (which use warm colors).
			case AudioStreamPlayer3D::ATTENUATION_INVERSE_DISTANCE:
				color = Color(0.4, 0.8, 1);
				break;
			case AudioStreamPlayer3D::ATTENUATION_INVERSE_SQUARE_DISTANCE:
				color = Color(0.4, 0.5, 1);
				break;
			case AudioStreamPlayer3D::ATTENUATION_LOGARITHMIC:
				color = Color(0.4, 0.2, 1);
				break;
			default:
				// Disabled attenuation mode.
				// This is never reached when Max Distance is 0, but the
				// hue-inverted form of this color will be used if Max Distance is greater than 0.
				color = Color(1, 1, 1);
				break;
		}

		if (player->get_max_distance() > CMP_EPSILON) {
			// Sound is hard-capped by max distance. The attenuation model still matters,
			// so invert the hue of the color that was chosen above.
			color.set_h(color.get_h() + 0.5);
		}

		p_gizmo->add_lines(points_billboard, lines_billboard_material, true, color);
	}

	if (player->is_emission_angle_enabled()) {
		const float pc = player->get_emission_angle();
		const float ofs = -Math::cos(Math::deg_to_rad(pc));
		const float radius = Math::sin(Math::deg_to_rad(pc));

		Vector<Vector3> points_primary;
		points_primary.resize(200);

		real_t step = Math_TAU / 100.0;
		for (int i = 0; i < 100; i++) {
			const float a = i * step;
			const float an = (i + 1) * step;

			const Vector3 from(Math::sin(a) * radius, Math::cos(a) * radius, ofs);
			const Vector3 to(Math::sin(an) * radius, Math::cos(an) * radius, ofs);

			points_primary.write[i * 2 + 0] = from;
			points_primary.write[i * 2 + 1] = to;
		}

		const Ref<Material> material_primary = get_material("stream_player_3d_material_primary", p_gizmo);
		p_gizmo->add_lines(points_primary, material_primary);

		Vector<Vector3> points_secondary;
		points_secondary.resize(16);

		for (int i = 0; i < 8; i++) {
			const float a = i * (Math_TAU / 8.0);
			const Vector3 from(Math::sin(a) * radius, Math::cos(a) * radius, ofs);

			points_secondary.write[i * 2 + 0] = from;
			points_secondary.write[i * 2 + 1] = Vector3();
		}

		const Ref<Material> material_secondary = get_material("stream_player_3d_material_secondary", p_gizmo);
		p_gizmo->add_lines(points_secondary, material_secondary);

		Vector<Vector3> handles;
		const float ha = Math::deg_to_rad(player->get_emission_angle());
		handles.push_back(Vector3(Math::sin(ha), 0, -Math::cos(ha)));
		p_gizmo->add_handles(handles, get_material("handles"));
	}

	p_gizmo->add_unscaled_billboard(icon, 0.05);
}

//////

AudioListener3DGizmoPlugin::AudioListener3DGizmoPlugin() {
	create_icon_material("audio_listener_3d_icon", Node3DEditor::get_singleton()->get_theme_icon(SNAME("GizmoAudioListener3D"), SNAME("EditorIcons")));
}

bool AudioListener3DGizmoPlugin::has_gizmo(Node3D *p_spatial) {
	return Object::cast_to<AudioListener3D>(p_spatial) != nullptr;
}

String AudioListener3DGizmoPlugin::get_gizmo_name() const {
	return "AudioListener3D";
}

int AudioListener3DGizmoPlugin::get_priority() const {
	return -1;
}

void AudioListener3DGizmoPlugin::redraw(EditorNode3DGizmo *p_gizmo) {
	const Ref<Material> icon = get_material("audio_listener_3d_icon", p_gizmo);
	p_gizmo->add_unscaled_billboard(icon, 0.05);
}

//////

Camera3DGizmoPlugin::Camera3DGizmoPlugin() {
	Color gizmo_color = EDITOR_DEF("editors/3d_gizmos/gizmo_colors/camera", Color(0.8, 0.4, 0.8));

	create_material("camera_material", gizmo_color);
	create_handle_material("handles");
}

Size2i Camera3DGizmoPlugin::_get_viewport_size(Camera3D *p_camera) {
	Viewport *viewport = p_camera->get_viewport();

	Window *window = Object::cast_to<Window>(viewport);
	if (window) {
		return window->get_size();
	}

	SubViewport *sub_viewport = Object::cast_to<SubViewport>(viewport);
	ERR_FAIL_NULL_V(sub_viewport, Size2i());

	if (sub_viewport == EditorNode::get_singleton()->get_scene_root()) {
		return Size2(GLOBAL_GET("display/window/size/viewport_width"), GLOBAL_GET("display/window/size/viewport_height"));
	}

	return sub_viewport->get_size();
}

bool Camera3DGizmoPlugin::has_gizmo(Node3D *p_spatial) {
	return Object::cast_to<Camera3D>(p_spatial) != nullptr;
}

String Camera3DGizmoPlugin::get_gizmo_name() const {
	return "Camera3D";
}

int Camera3DGizmoPlugin::get_priority() const {
	return -1;
}

String Camera3DGizmoPlugin::get_handle_name(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary) const {
	Camera3D *camera = Object::cast_to<Camera3D>(p_gizmo->get_node_3d());

	if (camera->get_projection() == Camera3D::PROJECTION_PERSPECTIVE) {
		return "FOV";
	} else {
		return "Size";
	}
}

Variant Camera3DGizmoPlugin::get_handle_value(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary) const {
	Camera3D *camera = Object::cast_to<Camera3D>(p_gizmo->get_node_3d());

	if (camera->get_projection() == Camera3D::PROJECTION_PERSPECTIVE) {
		return camera->get_fov();
	} else {
		return camera->get_size();
	}
}

void Camera3DGizmoPlugin::set_handle(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary, Camera3D *p_camera, const Point2 &p_point) {
	Camera3D *camera = Object::cast_to<Camera3D>(p_gizmo->get_node_3d());

	Transform3D gt = camera->get_global_transform();
	Transform3D gi = gt.affine_inverse();

	Vector3 ray_from = p_camera->project_ray_origin(p_point);
	Vector3 ray_dir = p_camera->project_ray_normal(p_point);

	Vector3 s[2] = { gi.xform(ray_from), gi.xform(ray_from + ray_dir * 4096) };

	if (camera->get_projection() == Camera3D::PROJECTION_PERSPECTIVE) {
		Transform3D gt2 = camera->get_global_transform();
		float a = _find_closest_angle_to_half_pi_arc(s[0], s[1], 1.0, gt2);
		camera->set("fov", CLAMP(a * 2.0, 1, 179));
	} else {
		Vector3 ra, rb;
		Geometry3D::get_closest_points_between_segments(Vector3(0, 0, -1), Vector3(4096, 0, -1), s[0], s[1], ra, rb);
		float d = ra.x * 2;
		if (Node3DEditor::get_singleton()->is_snap_enabled()) {
			d = Math::snapped(d, Node3DEditor::get_singleton()->get_translate_snap());
		}

		d = CLAMP(d, 0.1, 16384);

		camera->set("size", d);
	}
}

void Camera3DGizmoPlugin::commit_handle(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary, const Variant &p_restore, bool p_cancel) {
	Camera3D *camera = Object::cast_to<Camera3D>(p_gizmo->get_node_3d());

	if (camera->get_projection() == Camera3D::PROJECTION_PERSPECTIVE) {
		if (p_cancel) {
			camera->set("fov", p_restore);
		} else {
			EditorUndoRedoManager *ur = EditorUndoRedoManager::get_singleton();
			ur->create_action(TTR("Change Camera FOV"));
			ur->add_do_property(camera, "fov", camera->get_fov());
			ur->add_undo_property(camera, "fov", p_restore);
			ur->commit_action();
		}

	} else {
		if (p_cancel) {
			camera->set("size", p_restore);
		} else {
			EditorUndoRedoManager *ur = EditorUndoRedoManager::get_singleton();
			ur->create_action(TTR("Change Camera Size"));
			ur->add_do_property(camera, "size", camera->get_size());
			ur->add_undo_property(camera, "size", p_restore);
			ur->commit_action();
		}
	}
}

void Camera3DGizmoPlugin::redraw(EditorNode3DGizmo *p_gizmo) {
	Camera3D *camera = Object::cast_to<Camera3D>(p_gizmo->get_node_3d());

	p_gizmo->clear();

	Vector<Vector3> lines;
	Vector<Vector3> handles;

	Ref<Material> material = get_material("camera_material", p_gizmo);

	const Size2i viewport_size = _get_viewport_size(camera);
	const real_t viewport_aspect = viewport_size.x > 0 && viewport_size.y > 0 ? viewport_size.aspect() : 1.0;
	const Size2 size_factor = viewport_aspect > 1.0 ? Size2(1.0, 1.0 / viewport_aspect) : Size2(viewport_aspect, 1.0);

#define ADD_TRIANGLE(m_a, m_b, m_c) \
	{                               \
		lines.push_back(m_a);       \
		lines.push_back(m_b);       \
		lines.push_back(m_b);       \
		lines.push_back(m_c);       \
		lines.push_back(m_c);       \
		lines.push_back(m_a);       \
	}

#define ADD_QUAD(m_a, m_b, m_c, m_d) \
	{                                \
		lines.push_back(m_a);        \
		lines.push_back(m_b);        \
		lines.push_back(m_b);        \
		lines.push_back(m_c);        \
		lines.push_back(m_c);        \
		lines.push_back(m_d);        \
		lines.push_back(m_d);        \
		lines.push_back(m_a);        \
	}

	switch (camera->get_projection()) {
		case Camera3D::PROJECTION_PERSPECTIVE: {
			// The real FOV is halved for accurate representation
			float fov = camera->get_fov() / 2.0;

			const float hsize = Math::sin(Math::deg_to_rad(fov));
			const float depth = -Math::cos(Math::deg_to_rad(fov));
			Vector3 side = Vector3(hsize * size_factor.x, 0, depth);
			Vector3 nside = Vector3(-side.x, side.y, side.z);
			Vector3 up = Vector3(0, hsize * size_factor.y, 0);

			ADD_TRIANGLE(Vector3(), side + up, side - up);
			ADD_TRIANGLE(Vector3(), nside + up, nside - up);
			ADD_TRIANGLE(Vector3(), side + up, nside + up);
			ADD_TRIANGLE(Vector3(), side - up, nside - up);

			handles.push_back(side);
			side.x = MIN(side.x, hsize * 0.25);
			nside.x = -side.x;
			Vector3 tup(0, up.y + hsize / 2, side.z);
			ADD_TRIANGLE(tup, side + up, nside + up);
		} break;

		case Camera3D::PROJECTION_ORTHOGONAL: {
			float size = camera->get_size();

			float hsize = size * 0.5;
			Vector3 right(hsize * size_factor.x, 0, 0);
			Vector3 up(0, hsize * size_factor.y, 0);
			Vector3 back(0, 0, -1.0);
			Vector3 front(0, 0, 0);

			ADD_QUAD(-up - right, -up + right, up + right, up - right);
			ADD_QUAD(-up - right + back, -up + right + back, up + right + back, up - right + back);
			ADD_QUAD(up + right, up + right + back, up - right + back, up - right);
			ADD_QUAD(-up + right, -up + right + back, -up - right + back, -up - right);

			handles.push_back(right + back);

			right.x = MIN(right.x, hsize * 0.25);
			Vector3 tup(0, up.y + hsize / 2, back.z);
			ADD_TRIANGLE(tup, right + up + back, -right + up + back);

		} break;

		case Camera3D::PROJECTION_FRUSTUM: {
			float hsize = camera->get_size() / 2.0;

			Vector3 side = Vector3(hsize, 0, -camera->get_near()).normalized();
			side.x *= size_factor.x;
			Vector3 nside = Vector3(-side.x, side.y, side.z);
			Vector3 up = Vector3(0, hsize * size_factor.y, 0);
			Vector3 offset = Vector3(camera->get_frustum_offset().x, camera->get_frustum_offset().y, 0.0);

			ADD_TRIANGLE(Vector3(), side + up + offset, side - up + offset);
			ADD_TRIANGLE(Vector3(), nside + up + offset, nside - up + offset);
			ADD_TRIANGLE(Vector3(), side + up + offset, nside + up + offset);
			ADD_TRIANGLE(Vector3(), side - up + offset, nside - up + offset);

			side.x = MIN(side.x, hsize * 0.25);
			nside.x = -side.x;
			Vector3 tup(0, up.y + hsize / 2, side.z);
			ADD_TRIANGLE(tup + offset, side + up + offset, nside + up + offset);
		} break;
	}

#undef ADD_TRIANGLE
#undef ADD_QUAD

	p_gizmo->add_lines(lines, material);
	p_gizmo->add_collision_segments(lines);

	if (!handles.is_empty()) {
		p_gizmo->add_handles(handles, get_material("handles"));
	}
}

//////

MeshInstance3DGizmoPlugin::MeshInstance3DGizmoPlugin() {
}

bool MeshInstance3DGizmoPlugin::has_gizmo(Node3D *p_spatial) {
	return Object::cast_to<MeshInstance3D>(p_spatial) != nullptr && Object::cast_to<SoftBody3D>(p_spatial) == nullptr;
}

String MeshInstance3DGizmoPlugin::get_gizmo_name() const {
	return "MeshInstance3D";
}

int MeshInstance3DGizmoPlugin::get_priority() const {
	return -1;
}

bool MeshInstance3DGizmoPlugin::can_be_hidden() const {
	return false;
}

void MeshInstance3DGizmoPlugin::redraw(EditorNode3DGizmo *p_gizmo) {
	MeshInstance3D *mesh = Object::cast_to<MeshInstance3D>(p_gizmo->get_node_3d());

	p_gizmo->clear();

	Ref<Mesh> m = mesh->get_mesh();

	if (!m.is_valid()) {
		return; //none
	}

	Ref<TriangleMesh> tm = m->generate_triangle_mesh();
	if (tm.is_valid()) {
		p_gizmo->add_collision_triangles(tm);
	}
}

/////

OccluderInstance3DGizmoPlugin::OccluderInstance3DGizmoPlugin() {
	create_material("line_material", EDITOR_DEF("editors/3d_gizmos/gizmo_colors/occluder", Color(0.8, 0.5, 1)));
	create_handle_material("handles");
}

bool OccluderInstance3DGizmoPlugin::has_gizmo(Node3D *p_spatial) {
	return Object::cast_to<OccluderInstance3D>(p_spatial) != nullptr;
}

String OccluderInstance3DGizmoPlugin::get_gizmo_name() const {
	return "OccluderInstance3D";
}

int OccluderInstance3DGizmoPlugin::get_priority() const {
	return -1;
}

String OccluderInstance3DGizmoPlugin::get_handle_name(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary) const {
	const OccluderInstance3D *cs = Object::cast_to<OccluderInstance3D>(p_gizmo->get_node_3d());

	Ref<Occluder3D> o = cs->get_occluder();
	if (o.is_null()) {
		return "";
	}

	if (Object::cast_to<SphereOccluder3D>(*o)) {
		return "Radius";
	}

	if (Object::cast_to<BoxOccluder3D>(*o) || Object::cast_to<QuadOccluder3D>(*o)) {
		return "Size";
	}

	return "";
}

Variant OccluderInstance3DGizmoPlugin::get_handle_value(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary) const {
	OccluderInstance3D *oi = Object::cast_to<OccluderInstance3D>(p_gizmo->get_node_3d());

	Ref<Occluder3D> o = oi->get_occluder();
	if (o.is_null()) {
		return Variant();
	}

	if (Object::cast_to<SphereOccluder3D>(*o)) {
		Ref<SphereOccluder3D> so = o;
		return so->get_radius();
	}

	if (Object::cast_to<BoxOccluder3D>(*o)) {
		Ref<BoxOccluder3D> bo = o;
		return bo->get_size();
	}

	if (Object::cast_to<QuadOccluder3D>(*o)) {
		Ref<QuadOccluder3D> qo = o;
		return qo->get_size();
	}

	return Variant();
}

void OccluderInstance3DGizmoPlugin::set_handle(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary, Camera3D *p_camera, const Point2 &p_point) {
	OccluderInstance3D *oi = Object::cast_to<OccluderInstance3D>(p_gizmo->get_node_3d());

	Ref<Occluder3D> o = oi->get_occluder();
	if (o.is_null()) {
		return;
	}

	Transform3D gt = oi->get_global_transform();
	Transform3D gi = gt.affine_inverse();

	Vector3 ray_from = p_camera->project_ray_origin(p_point);
	Vector3 ray_dir = p_camera->project_ray_normal(p_point);

	Vector3 sg[2] = { gi.xform(ray_from), gi.xform(ray_from + ray_dir * 4096) };

	bool snap_enabled = Node3DEditor::get_singleton()->is_snap_enabled();
	float snap = Node3DEditor::get_singleton()->get_translate_snap();

	if (Object::cast_to<SphereOccluder3D>(*o)) {
		Ref<SphereOccluder3D> so = o;
		Vector3 ra, rb;
		Geometry3D::get_closest_points_between_segments(Vector3(), Vector3(4096, 0, 0), sg[0], sg[1], ra, rb);
		float d = ra.x;
		if (snap_enabled) {
			d = Math::snapped(d, snap);
		}

		if (d < 0.001) {
			d = 0.001;
		}

		so->set_radius(d);
	}

	if (Object::cast_to<BoxOccluder3D>(*o)) {
		Vector3 axis;
		axis[p_id] = 1.0;
		Ref<BoxOccluder3D> bo = o;
		Vector3 ra, rb;
		Geometry3D::get_closest_points_between_segments(Vector3(), axis * 4096, sg[0], sg[1], ra, rb);
		float d = ra[p_id] * 2;
		if (snap_enabled) {
			d = Math::snapped(d, snap);
		}

		if (d < 0.001) {
			d = 0.001;
		}

		Vector3 he = bo->get_size();
		he[p_id] = d;
		bo->set_size(he);
	}

	if (Object::cast_to<QuadOccluder3D>(*o)) {
		Ref<QuadOccluder3D> qo = o;
		Plane p = Plane(Vector3(0.0f, 0.0f, 1.0f), 0.0f);
		Vector3 intersection;
		if (!p.intersects_segment(sg[0], sg[1], &intersection)) {
			return;
		}

		if (p_id == 2) {
			Vector2 s = Vector2(intersection.x, intersection.y) * 2.0f;
			if (snap_enabled) {
				s = s.snapped(Vector2(snap, snap));
			}
			s = s.max(Vector2(0.001, 0.001));
			qo->set_size(s);
		} else {
			float d = intersection[p_id];
			if (snap_enabled) {
				d = Math::snapped(d, snap);
			}

			if (d < 0.001) {
				d = 0.001;
			}

			Vector2 he = qo->get_size();
			he[p_id] = d * 2.0f;
			qo->set_size(he);
		}
	}
}

void OccluderInstance3DGizmoPlugin::commit_handle(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary, const Variant &p_restore, bool p_cancel) {
	OccluderInstance3D *oi = Object::cast_to<OccluderInstance3D>(p_gizmo->get_node_3d());

	Ref<Occluder3D> o = oi->get_occluder();
	if (o.is_null()) {
		return;
	}

	if (Object::cast_to<SphereOccluder3D>(*o)) {
		Ref<SphereOccluder3D> so = o;
		if (p_cancel) {
			so->set_radius(p_restore);
			return;
		}

		EditorUndoRedoManager *ur = EditorUndoRedoManager::get_singleton();
		ur->create_action(TTR("Change Sphere Shape Radius"));
		ur->add_do_method(so.ptr(), "set_radius", so->get_radius());
		ur->add_undo_method(so.ptr(), "set_radius", p_restore);
		ur->commit_action();
	}

	if (Object::cast_to<BoxOccluder3D>(*o)) {
		Ref<BoxOccluder3D> bo = o;
		if (p_cancel) {
			bo->set_size(p_restore);
			return;
		}

		EditorUndoRedoManager *ur = EditorUndoRedoManager::get_singleton();
		ur->create_action(TTR("Change Box Shape Size"));
		ur->add_do_method(bo.ptr(), "set_size", bo->get_size());
		ur->add_undo_method(bo.ptr(), "set_size", p_restore);
		ur->commit_action();
	}

	if (Object::cast_to<QuadOccluder3D>(*o)) {
		Ref<QuadOccluder3D> qo = o;
		if (p_cancel) {
			qo->set_size(p_restore);
			return;
		}

		EditorUndoRedoManager *ur = EditorUndoRedoManager::get_singleton();
		ur->create_action(TTR("Change Box Shape Size"));
		ur->add_do_method(qo.ptr(), "set_size", qo->get_size());
		ur->add_undo_method(qo.ptr(), "set_size", p_restore);
		ur->commit_action();
	}
}

void OccluderInstance3DGizmoPlugin::redraw(EditorNode3DGizmo *p_gizmo) {
	OccluderInstance3D *occluder_instance = Object::cast_to<OccluderInstance3D>(p_gizmo->get_node_3d());

	p_gizmo->clear();

	Ref<Occluder3D> o = occluder_instance->get_occluder();

	if (!o.is_valid()) {
		return;
	}

	Vector<Vector3> lines = o->get_debug_lines();
	if (!lines.is_empty()) {
		Ref<Material> material = get_material("line_material", p_gizmo);
		p_gizmo->add_lines(lines, material);
		p_gizmo->add_collision_segments(lines);
	}

	Ref<Material> handles_material = get_material("handles");
	if (Object::cast_to<SphereOccluder3D>(*o)) {
		Ref<SphereOccluder3D> so = o;
		float r = so->get_radius();
		Vector<Vector3> handles = { Vector3(r, 0, 0) };
		p_gizmo->add_handles(handles, handles_material);
	}

	if (Object::cast_to<BoxOccluder3D>(*o)) {
		Ref<BoxOccluder3D> bo = o;

		Vector<Vector3> handles;
		for (int i = 0; i < 3; i++) {
			Vector3 ax;
			ax[i] = bo->get_size()[i] / 2;
			handles.push_back(ax);
		}

		p_gizmo->add_handles(handles, handles_material);
	}

	if (Object::cast_to<QuadOccluder3D>(*o)) {
		Ref<QuadOccluder3D> qo = o;
		Vector2 size = qo->get_size();
		Vector3 s = Vector3(size.x, size.y, 0.0f) / 2.0f;
		Vector<Vector3> handles = { Vector3(s.x, 0.0f, 0.0f), Vector3(0.0f, s.y, 0.0f), Vector3(s.x, s.y, 0.0f) };
		p_gizmo->add_handles(handles, handles_material);
	}
}

/////

Sprite3DGizmoPlugin::Sprite3DGizmoPlugin() {
}

bool Sprite3DGizmoPlugin::has_gizmo(Node3D *p_spatial) {
	return Object::cast_to<Sprite3D>(p_spatial) != nullptr;
}

String Sprite3DGizmoPlugin::get_gizmo_name() const {
	return "Sprite3D";
}

int Sprite3DGizmoPlugin::get_priority() const {
	return -1;
}

bool Sprite3DGizmoPlugin::can_be_hidden() const {
	return false;
}

void Sprite3DGizmoPlugin::redraw(EditorNode3DGizmo *p_gizmo) {
	Sprite3D *sprite = Object::cast_to<Sprite3D>(p_gizmo->get_node_3d());

	p_gizmo->clear();

	Ref<TriangleMesh> tm = sprite->generate_triangle_mesh();
	if (tm.is_valid()) {
		p_gizmo->add_collision_triangles(tm);
	}
}

///

Label3DGizmoPlugin::Label3DGizmoPlugin() {
}

bool Label3DGizmoPlugin::has_gizmo(Node3D *p_spatial) {
	return Object::cast_to<Label3D>(p_spatial) != nullptr;
}

String Label3DGizmoPlugin::get_gizmo_name() const {
	return "Label3D";
}

int Label3DGizmoPlugin::get_priority() const {
	return -1;
}

bool Label3DGizmoPlugin::can_be_hidden() const {
	return false;
}

void Label3DGizmoPlugin::redraw(EditorNode3DGizmo *p_gizmo) {
	Label3D *label = Object::cast_to<Label3D>(p_gizmo->get_node_3d());

	p_gizmo->clear();

	Ref<TriangleMesh> tm = label->generate_triangle_mesh();
	if (tm.is_valid()) {
		p_gizmo->add_collision_triangles(tm);
	}
}

///

Marker3DGizmoPlugin::Marker3DGizmoPlugin() {
	pos3d_mesh = Ref<ArrayMesh>(memnew(ArrayMesh));

	Vector<Vector3> cursor_points;
	Vector<Color> cursor_colors;
	const float cs = 1.0;
	// Add more points to create a "hard stop" in the color gradient.
	cursor_points.push_back(Vector3(+cs, 0, 0));
	cursor_points.push_back(Vector3());
	cursor_points.push_back(Vector3());
	cursor_points.push_back(Vector3(-cs, 0, 0));

	cursor_points.push_back(Vector3(0, +cs, 0));
	cursor_points.push_back(Vector3());
	cursor_points.push_back(Vector3());
	cursor_points.push_back(Vector3(0, -cs, 0));

	cursor_points.push_back(Vector3(0, 0, +cs));
	cursor_points.push_back(Vector3());
	cursor_points.push_back(Vector3());
	cursor_points.push_back(Vector3(0, 0, -cs));

	// Use the axis color which is brighter for the positive axis.
	// Use a darkened axis color for the negative axis.
	// This makes it possible to see in which direction the Marker3D node is rotated
	// (which can be important depending on how it's used).
	const Color color_x = EditorNode::get_singleton()->get_gui_base()->get_theme_color(SNAME("axis_x_color"), SNAME("Editor"));
	cursor_colors.push_back(color_x);
	cursor_colors.push_back(color_x);
	// FIXME: Use less strong darkening factor once GH-48573 is fixed.
	// The current darkening factor compensates for lines being too bright in the 3D editor.
	cursor_colors.push_back(color_x.lerp(Color(0, 0, 0), 0.75));
	cursor_colors.push_back(color_x.lerp(Color(0, 0, 0), 0.75));

	const Color color_y = EditorNode::get_singleton()->get_gui_base()->get_theme_color(SNAME("axis_y_color"), SNAME("Editor"));
	cursor_colors.push_back(color_y);
	cursor_colors.push_back(color_y);
	cursor_colors.push_back(color_y.lerp(Color(0, 0, 0), 0.75));
	cursor_colors.push_back(color_y.lerp(Color(0, 0, 0), 0.75));

	const Color color_z = EditorNode::get_singleton()->get_gui_base()->get_theme_color(SNAME("axis_z_color"), SNAME("Editor"));
	cursor_colors.push_back(color_z);
	cursor_colors.push_back(color_z);
	cursor_colors.push_back(color_z.lerp(Color(0, 0, 0), 0.75));
	cursor_colors.push_back(color_z.lerp(Color(0, 0, 0), 0.75));

	Ref<StandardMaterial3D> mat = memnew(StandardMaterial3D);
	mat->set_shading_mode(StandardMaterial3D::SHADING_MODE_UNSHADED);
	mat->set_flag(StandardMaterial3D::FLAG_ALBEDO_FROM_VERTEX_COLOR, true);
	mat->set_flag(StandardMaterial3D::FLAG_SRGB_VERTEX_COLOR, true);
	mat->set_transparency(StandardMaterial3D::TRANSPARENCY_ALPHA);

	Array d;
	d.resize(RS::ARRAY_MAX);
	d[Mesh::ARRAY_VERTEX] = cursor_points;
	d[Mesh::ARRAY_COLOR] = cursor_colors;
	pos3d_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_LINES, d);
	pos3d_mesh->surface_set_material(0, mat);
}

bool Marker3DGizmoPlugin::has_gizmo(Node3D *p_spatial) {
	return Object::cast_to<Marker3D>(p_spatial) != nullptr;
}

String Marker3DGizmoPlugin::get_gizmo_name() const {
	return "Marker3D";
}

int Marker3DGizmoPlugin::get_priority() const {
	return -1;
}

void Marker3DGizmoPlugin::redraw(EditorNode3DGizmo *p_gizmo) {
	const Marker3D *marker = Object::cast_to<Marker3D>(p_gizmo->get_node_3d());
	const real_t extents = marker->get_gizmo_extents();
	const Transform3D xform(Basis::from_scale(Vector3(extents, extents, extents)));

	p_gizmo->clear();
	p_gizmo->add_mesh(pos3d_mesh, Ref<Material>(), xform);

	const Vector<Vector3> points = {
		Vector3(-extents, 0, 0),
		Vector3(+extents, 0, 0),
		Vector3(0, -extents, 0),
		Vector3(0, +extents, 0),
		Vector3(0, 0, -extents),
		Vector3(0, 0, +extents),
	};
	p_gizmo->add_collision_segments(points);
}

////

PhysicalBone3DGizmoPlugin::PhysicalBone3DGizmoPlugin() {
	create_material("joint_material", EDITOR_GET("editors/3d_gizmos/gizmo_colors/joint"));
}

bool PhysicalBone3DGizmoPlugin::has_gizmo(Node3D *p_spatial) {
	return Object::cast_to<PhysicalBone3D>(p_spatial) != nullptr;
}

String PhysicalBone3DGizmoPlugin::get_gizmo_name() const {
	return "PhysicalBone3D";
}

int PhysicalBone3DGizmoPlugin::get_priority() const {
	return -1;
}

void PhysicalBone3DGizmoPlugin::redraw(EditorNode3DGizmo *p_gizmo) {
	p_gizmo->clear();

	PhysicalBone3D *physical_bone = Object::cast_to<PhysicalBone3D>(p_gizmo->get_node_3d());

	if (!physical_bone) {
		return;
	}

	Skeleton3D *sk(physical_bone->find_skeleton_parent());
	if (!sk) {
		return;
	}

	PhysicalBone3D *pb(sk->get_physical_bone(physical_bone->get_bone_id()));
	if (!pb) {
		return;
	}

	PhysicalBone3D *pbp(sk->get_physical_bone_parent(physical_bone->get_bone_id()));
	if (!pbp) {
		return;
	}

	Vector<Vector3> points;

	switch (physical_bone->get_joint_type()) {
		case PhysicalBone3D::JOINT_TYPE_PIN: {
			Joint3DGizmoPlugin::CreatePinJointGizmo(physical_bone->get_joint_offset(), points);
		} break;
		case PhysicalBone3D::JOINT_TYPE_CONE: {
			const PhysicalBone3D::ConeJointData *cjd(static_cast<const PhysicalBone3D::ConeJointData *>(physical_bone->get_joint_data()));
			Joint3DGizmoPlugin::CreateConeTwistJointGizmo(
					physical_bone->get_joint_offset(),
					physical_bone->get_global_transform() * physical_bone->get_joint_offset(),
					pb->get_global_transform(),
					pbp->get_global_transform(),
					cjd->swing_span,
					cjd->twist_span,
					&points,
					&points);
		} break;
		case PhysicalBone3D::JOINT_TYPE_HINGE: {
			const PhysicalBone3D::HingeJointData *hjd(static_cast<const PhysicalBone3D::HingeJointData *>(physical_bone->get_joint_data()));
			Joint3DGizmoPlugin::CreateHingeJointGizmo(
					physical_bone->get_joint_offset(),
					physical_bone->get_global_transform() * physical_bone->get_joint_offset(),
					pb->get_global_transform(),
					pbp->get_global_transform(),
					hjd->angular_limit_lower,
					hjd->angular_limit_upper,
					hjd->angular_limit_enabled,
					points,
					&points,
					&points);
		} break;
		case PhysicalBone3D::JOINT_TYPE_SLIDER: {
			const PhysicalBone3D::SliderJointData *sjd(static_cast<const PhysicalBone3D::SliderJointData *>(physical_bone->get_joint_data()));
			Joint3DGizmoPlugin::CreateSliderJointGizmo(
					physical_bone->get_joint_offset(),
					physical_bone->get_global_transform() * physical_bone->get_joint_offset(),
					pb->get_global_transform(),
					pbp->get_global_transform(),
					sjd->angular_limit_lower,
					sjd->angular_limit_upper,
					sjd->linear_limit_lower,
					sjd->linear_limit_upper,
					points,
					&points,
					&points);
		} break;
		case PhysicalBone3D::JOINT_TYPE_6DOF: {
			const PhysicalBone3D::SixDOFJointData *sdofjd(static_cast<const PhysicalBone3D::SixDOFJointData *>(physical_bone->get_joint_data()));
			Joint3DGizmoPlugin::CreateGeneric6DOFJointGizmo(
					physical_bone->get_joint_offset(),

					physical_bone->get_global_transform() * physical_bone->get_joint_offset(),
					pb->get_global_transform(),
					pbp->get_global_transform(),

					sdofjd->axis_data[0].angular_limit_lower,
					sdofjd->axis_data[0].angular_limit_upper,
					sdofjd->axis_data[0].linear_limit_lower,
					sdofjd->axis_data[0].linear_limit_upper,
					sdofjd->axis_data[0].angular_limit_enabled,
					sdofjd->axis_data[0].linear_limit_enabled,

					sdofjd->axis_data[1].angular_limit_lower,
					sdofjd->axis_data[1].angular_limit_upper,
					sdofjd->axis_data[1].linear_limit_lower,
					sdofjd->axis_data[1].linear_limit_upper,
					sdofjd->axis_data[1].angular_limit_enabled,
					sdofjd->axis_data[1].linear_limit_enabled,

					sdofjd->axis_data[2].angular_limit_lower,
					sdofjd->axis_data[2].angular_limit_upper,
					sdofjd->axis_data[2].linear_limit_lower,
					sdofjd->axis_data[2].linear_limit_upper,
					sdofjd->axis_data[2].angular_limit_enabled,
					sdofjd->axis_data[2].linear_limit_enabled,

					points,
					&points,
					&points);
		} break;
		default:
			return;
	}

	Ref<Material> material = get_material("joint_material", p_gizmo);

	p_gizmo->add_collision_segments(points);
	p_gizmo->add_lines(points, material);
}

/////

RayCast3DGizmoPlugin::RayCast3DGizmoPlugin() {
	const Color gizmo_color = EDITOR_GET("editors/3d_gizmos/gizmo_colors/shape");
	create_material("shape_material", gizmo_color);
	const float gizmo_value = gizmo_color.get_v();
	const Color gizmo_color_disabled = Color(gizmo_value, gizmo_value, gizmo_value, 0.65);
	create_material("shape_material_disabled", gizmo_color_disabled);
}

bool RayCast3DGizmoPlugin::has_gizmo(Node3D *p_spatial) {
	return Object::cast_to<RayCast3D>(p_spatial) != nullptr;
}

String RayCast3DGizmoPlugin::get_gizmo_name() const {
	return "RayCast3D";
}

int RayCast3DGizmoPlugin::get_priority() const {
	return -1;
}

void RayCast3DGizmoPlugin::redraw(EditorNode3DGizmo *p_gizmo) {
	RayCast3D *raycast = Object::cast_to<RayCast3D>(p_gizmo->get_node_3d());

	p_gizmo->clear();

	const Ref<StandardMaterial3D> material = raycast->is_enabled() ? raycast->get_debug_material() : get_material("shape_material_disabled");

	p_gizmo->add_lines(raycast->get_debug_line_vertices(), material);

	if (raycast->get_debug_shape_thickness() > 1) {
		p_gizmo->add_vertices(raycast->get_debug_shape_vertices(), material, Mesh::PRIMITIVE_TRIANGLE_STRIP);
	}

	p_gizmo->add_collision_segments(raycast->get_debug_line_vertices());
}

/////

ShapeCast3DGizmoPlugin::ShapeCast3DGizmoPlugin() {
	const Color gizmo_color = EDITOR_GET("editors/3d_gizmos/gizmo_colors/shape");
	create_material("shape_material", gizmo_color);
	const float gizmo_value = gizmo_color.get_v();
	const Color gizmo_color_disabled = Color(gizmo_value, gizmo_value, gizmo_value, 0.65);
	create_material("shape_material_disabled", gizmo_color_disabled);
}

bool ShapeCast3DGizmoPlugin::has_gizmo(Node3D *p_spatial) {
	return Object::cast_to<ShapeCast3D>(p_spatial) != nullptr;
}

String ShapeCast3DGizmoPlugin::get_gizmo_name() const {
	return "ShapeCast3D";
}

int ShapeCast3DGizmoPlugin::get_priority() const {
	return -1;
}

void ShapeCast3DGizmoPlugin::redraw(EditorNode3DGizmo *p_gizmo) {
	ShapeCast3D *shapecast = Object::cast_to<ShapeCast3D>(p_gizmo->get_node_3d());

	p_gizmo->clear();

	const Ref<StandardMaterial3D> material = shapecast->is_enabled() ? shapecast->get_debug_material() : get_material("shape_material_disabled");

	p_gizmo->add_lines(shapecast->get_debug_line_vertices(), material);

	if (shapecast->get_shape().is_valid()) {
		p_gizmo->add_lines(shapecast->get_debug_shape_vertices(), material);
	}

	p_gizmo->add_collision_segments(shapecast->get_debug_line_vertices());
}

/////

void SpringArm3DGizmoPlugin::redraw(EditorNode3DGizmo *p_gizmo) {
	SpringArm3D *spring_arm = Object::cast_to<SpringArm3D>(p_gizmo->get_node_3d());

	p_gizmo->clear();

	Vector<Vector3> lines = {
		Vector3(),
		Vector3(0, 0, 1.0) * spring_arm->get_length()
	};

	Ref<StandardMaterial3D> material = get_material("shape_material", p_gizmo);

	p_gizmo->add_lines(lines, material);
	p_gizmo->add_collision_segments(lines);
}

SpringArm3DGizmoPlugin::SpringArm3DGizmoPlugin() {
	Color gizmo_color = EDITOR_GET("editors/3d_gizmos/gizmo_colors/shape");
	create_material("shape_material", gizmo_color);
}

bool SpringArm3DGizmoPlugin::has_gizmo(Node3D *p_spatial) {
	return Object::cast_to<SpringArm3D>(p_spatial) != nullptr;
}

String SpringArm3DGizmoPlugin::get_gizmo_name() const {
	return "SpringArm3D";
}

int SpringArm3DGizmoPlugin::get_priority() const {
	return -1;
}

/////

VehicleWheel3DGizmoPlugin::VehicleWheel3DGizmoPlugin() {
	Color gizmo_color = EDITOR_GET("editors/3d_gizmos/gizmo_colors/shape");
	create_material("shape_material", gizmo_color);
}

bool VehicleWheel3DGizmoPlugin::has_gizmo(Node3D *p_spatial) {
	return Object::cast_to<VehicleWheel3D>(p_spatial) != nullptr;
}

String VehicleWheel3DGizmoPlugin::get_gizmo_name() const {
	return "VehicleWheel3D";
}

int VehicleWheel3DGizmoPlugin::get_priority() const {
	return -1;
}

void VehicleWheel3DGizmoPlugin::redraw(EditorNode3DGizmo *p_gizmo) {
	VehicleWheel3D *car_wheel = Object::cast_to<VehicleWheel3D>(p_gizmo->get_node_3d());

	p_gizmo->clear();

	Vector<Vector3> points;

	float r = car_wheel->get_radius();
	const int skip = 10;
	for (int i = 0; i <= 360; i += skip) {
		float ra = Math::deg_to_rad((float)i);
		float rb = Math::deg_to_rad((float)i + skip);
		Point2 a = Vector2(Math::sin(ra), Math::cos(ra)) * r;
		Point2 b = Vector2(Math::sin(rb), Math::cos(rb)) * r;

		points.push_back(Vector3(0, a.x, a.y));
		points.push_back(Vector3(0, b.x, b.y));

		const int springsec = 4;

		for (int j = 0; j < springsec; j++) {
			float t = car_wheel->get_suspension_rest_length() * 5;
			points.push_back(Vector3(a.x, i / 360.0 * t / springsec + j * (t / springsec), a.y) * 0.2);
			points.push_back(Vector3(b.x, (i + skip) / 360.0 * t / springsec + j * (t / springsec), b.y) * 0.2);
		}
	}

	//travel
	points.push_back(Vector3(0, 0, 0));
	points.push_back(Vector3(0, car_wheel->get_suspension_rest_length(), 0));

	//axis
	points.push_back(Vector3(r * 0.2, car_wheel->get_suspension_rest_length(), 0));
	points.push_back(Vector3(-r * 0.2, car_wheel->get_suspension_rest_length(), 0));
	//axis
	points.push_back(Vector3(r * 0.2, 0, 0));
	points.push_back(Vector3(-r * 0.2, 0, 0));

	//forward line
	points.push_back(Vector3(0, -r, 0));
	points.push_back(Vector3(0, -r, r * 2));
	points.push_back(Vector3(0, -r, r * 2));
	points.push_back(Vector3(r * 2 * 0.2, -r, r * 2 * 0.8));
	points.push_back(Vector3(0, -r, r * 2));
	points.push_back(Vector3(-r * 2 * 0.2, -r, r * 2 * 0.8));

	Ref<Material> material = get_material("shape_material", p_gizmo);

	p_gizmo->add_lines(points, material);
	p_gizmo->add_collision_segments(points);
}

///////////

SoftBody3DGizmoPlugin::SoftBody3DGizmoPlugin() {
	Color gizmo_color = EDITOR_GET("editors/3d_gizmos/gizmo_colors/shape");
	create_material("shape_material", gizmo_color);
	create_handle_material("handles");
}

bool SoftBody3DGizmoPlugin::has_gizmo(Node3D *p_spatial) {
	return Object::cast_to<SoftBody3D>(p_spatial) != nullptr;
}

String SoftBody3DGizmoPlugin::get_gizmo_name() const {
	return "SoftBody3D";
}

int SoftBody3DGizmoPlugin::get_priority() const {
	return -1;
}

bool SoftBody3DGizmoPlugin::is_selectable_when_hidden() const {
	return true;
}

void SoftBody3DGizmoPlugin::redraw(EditorNode3DGizmo *p_gizmo) {
	SoftBody3D *soft_body = Object::cast_to<SoftBody3D>(p_gizmo->get_node_3d());

	p_gizmo->clear();

	if (!soft_body || soft_body->get_mesh().is_null()) {
		return;
	}

	// find mesh

	Vector<Vector3> lines;

	soft_body->get_mesh()->generate_debug_mesh_lines(lines);

	if (!lines.size()) {
		return;
	}

	Ref<TriangleMesh> tm = soft_body->get_mesh()->generate_triangle_mesh();

	Vector<Vector3> points;
	for (int i = 0; i < soft_body->get_mesh()->get_surface_count(); i++) {
		Array arrays = soft_body->get_mesh()->surface_get_arrays(i);
		ERR_CONTINUE(arrays.is_empty());

		const Vector<Vector3> &vertices = arrays[Mesh::ARRAY_VERTEX];
		points.append_array(vertices);
	}

	Ref<Material> material = get_material("shape_material", p_gizmo);

	p_gizmo->add_lines(lines, material);
	p_gizmo->add_handles(points, get_material("handles"));
	p_gizmo->add_collision_triangles(tm);
}

String SoftBody3DGizmoPlugin::get_handle_name(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary) const {
	return "SoftBody3D pin point";
}

Variant SoftBody3DGizmoPlugin::get_handle_value(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary) const {
	SoftBody3D *soft_body = Object::cast_to<SoftBody3D>(p_gizmo->get_node_3d());
	return Variant(soft_body->is_point_pinned(p_id));
}

void SoftBody3DGizmoPlugin::commit_handle(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary, const Variant &p_restore, bool p_cancel) {
	SoftBody3D *soft_body = Object::cast_to<SoftBody3D>(p_gizmo->get_node_3d());
	soft_body->pin_point_toggle(p_id);
}

bool SoftBody3DGizmoPlugin::is_handle_highlighted(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary) const {
	SoftBody3D *soft_body = Object::cast_to<SoftBody3D>(p_gizmo->get_node_3d());
	return soft_body->is_point_pinned(p_id);
}

///////////

VisibleOnScreenNotifier3DGizmoPlugin::VisibleOnScreenNotifier3DGizmoPlugin() {
	Color gizmo_color = EDITOR_DEF("editors/3d_gizmos/gizmo_colors/visibility_notifier", Color(0.8, 0.5, 0.7));
	create_material("visibility_notifier_material", gizmo_color);
	gizmo_color.a = 0.1;
	create_material("visibility_notifier_solid_material", gizmo_color);
	create_handle_material("handles");
}

bool VisibleOnScreenNotifier3DGizmoPlugin::has_gizmo(Node3D *p_spatial) {
	return Object::cast_to<VisibleOnScreenNotifier3D>(p_spatial) != nullptr;
}

String VisibleOnScreenNotifier3DGizmoPlugin::get_gizmo_name() const {
	return "VisibleOnScreenNotifier3D";
}

int VisibleOnScreenNotifier3DGizmoPlugin::get_priority() const {
	return -1;
}

String VisibleOnScreenNotifier3DGizmoPlugin::get_handle_name(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary) const {
	switch (p_id) {
		case 0:
			return "Size X";
		case 1:
			return "Size Y";
		case 2:
			return "Size Z";
		case 3:
			return "Pos X";
		case 4:
			return "Pos Y";
		case 5:
			return "Pos Z";
	}

	return "";
}

Variant VisibleOnScreenNotifier3DGizmoPlugin::get_handle_value(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary) const {
	VisibleOnScreenNotifier3D *notifier = Object::cast_to<VisibleOnScreenNotifier3D>(p_gizmo->get_node_3d());
	return notifier->get_aabb();
}

void VisibleOnScreenNotifier3DGizmoPlugin::set_handle(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary, Camera3D *p_camera, const Point2 &p_point) {
	VisibleOnScreenNotifier3D *notifier = Object::cast_to<VisibleOnScreenNotifier3D>(p_gizmo->get_node_3d());

	Transform3D gt = notifier->get_global_transform();

	Transform3D gi = gt.affine_inverse();

	bool move = p_id >= 3;
	p_id = p_id % 3;

	AABB aabb = notifier->get_aabb();
	Vector3 ray_from = p_camera->project_ray_origin(p_point);
	Vector3 ray_dir = p_camera->project_ray_normal(p_point);

	Vector3 sg[2] = { gi.xform(ray_from), gi.xform(ray_from + ray_dir * 4096) };

	Vector3 ofs = aabb.get_center();

	Vector3 axis;
	axis[p_id] = 1.0;

	if (move) {
		Vector3 ra, rb;
		Geometry3D::get_closest_points_between_segments(ofs - axis * 4096, ofs + axis * 4096, sg[0], sg[1], ra, rb);

		float d = ra[p_id];
		if (Node3DEditor::get_singleton()->is_snap_enabled()) {
			d = Math::snapped(d, Node3DEditor::get_singleton()->get_translate_snap());
		}

		aabb.position[p_id] = d - 1.0 - aabb.size[p_id] * 0.5;
		notifier->set_aabb(aabb);

	} else {
		Vector3 ra, rb;
		Geometry3D::get_closest_points_between_segments(ofs, ofs + axis * 4096, sg[0], sg[1], ra, rb);

		float d = ra[p_id] - ofs[p_id];
		if (Node3DEditor::get_singleton()->is_snap_enabled()) {
			d = Math::snapped(d, Node3DEditor::get_singleton()->get_translate_snap());
		}

		if (d < 0.001) {
			d = 0.001;
		}
		//resize
		aabb.position[p_id] = (aabb.position[p_id] + aabb.size[p_id] * 0.5) - d;
		aabb.size[p_id] = d * 2;
		notifier->set_aabb(aabb);
	}
}

void VisibleOnScreenNotifier3DGizmoPlugin::commit_handle(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary, const Variant &p_restore, bool p_cancel) {
	VisibleOnScreenNotifier3D *notifier = Object::cast_to<VisibleOnScreenNotifier3D>(p_gizmo->get_node_3d());

	if (p_cancel) {
		notifier->set_aabb(p_restore);
		return;
	}

	EditorUndoRedoManager *ur = EditorUndoRedoManager::get_singleton();
	ur->create_action(TTR("Change Notifier AABB"));
	ur->add_do_method(notifier, "set_aabb", notifier->get_aabb());
	ur->add_undo_method(notifier, "set_aabb", p_restore);
	ur->commit_action();
}

void VisibleOnScreenNotifier3DGizmoPlugin::redraw(EditorNode3DGizmo *p_gizmo) {
	VisibleOnScreenNotifier3D *notifier = Object::cast_to<VisibleOnScreenNotifier3D>(p_gizmo->get_node_3d());

	p_gizmo->clear();

	Vector<Vector3> lines;
	AABB aabb = notifier->get_aabb();

	for (int i = 0; i < 12; i++) {
		Vector3 a, b;
		aabb.get_edge(i, a, b);
		lines.push_back(a);
		lines.push_back(b);
	}

	Vector<Vector3> handles;

	for (int i = 0; i < 3; i++) {
		Vector3 ax;
		ax[i] = aabb.position[i] + aabb.size[i];
		ax[(i + 1) % 3] = aabb.position[(i + 1) % 3] + aabb.size[(i + 1) % 3] * 0.5;
		ax[(i + 2) % 3] = aabb.position[(i + 2) % 3] + aabb.size[(i + 2) % 3] * 0.5;
		handles.push_back(ax);
	}

	Vector3 center = aabb.get_center();
	for (int i = 0; i < 3; i++) {
		Vector3 ax;
		ax[i] = 1.0;
		handles.push_back(center + ax);
		lines.push_back(center);
		lines.push_back(center + ax);
	}

	Ref<Material> material = get_material("visibility_notifier_material", p_gizmo);

	p_gizmo->add_lines(lines, material);
	p_gizmo->add_collision_segments(lines);

	if (p_gizmo->is_selected()) {
		Ref<Material> solid_material = get_material("visibility_notifier_solid_material", p_gizmo);
		p_gizmo->add_solid_box(solid_material, aabb.get_size(), aabb.get_center());
	}

	p_gizmo->add_handles(handles, get_material("handles"));
}

////

CPUParticles3DGizmoPlugin::CPUParticles3DGizmoPlugin() {
	create_icon_material("particles_icon", Node3DEditor::get_singleton()->get_theme_icon(SNAME("GizmoCPUParticles3D"), SNAME("EditorIcons")));
}

bool CPUParticles3DGizmoPlugin::has_gizmo(Node3D *p_spatial) {
	return Object::cast_to<CPUParticles3D>(p_spatial) != nullptr;
}

String CPUParticles3DGizmoPlugin::get_gizmo_name() const {
	return "CPUParticles3D";
}

int CPUParticles3DGizmoPlugin::get_priority() const {
	return -1;
}

bool CPUParticles3DGizmoPlugin::is_selectable_when_hidden() const {
	return true;
}

void CPUParticles3DGizmoPlugin::redraw(EditorNode3DGizmo *p_gizmo) {
	Ref<Material> icon = get_material("particles_icon", p_gizmo);
	p_gizmo->add_unscaled_billboard(icon, 0.05);
}

////

GPUParticles3DGizmoPlugin::GPUParticles3DGizmoPlugin() {
	Color gizmo_color = EDITOR_DEF("editors/3d_gizmos/gizmo_colors/particles", Color(0.8, 0.7, 0.4));
	create_material("particles_material", gizmo_color);
	gizmo_color.a = MAX((gizmo_color.a - 0.2) * 0.02, 0.0);
	create_material("particles_solid_material", gizmo_color);
	create_icon_material("particles_icon", Node3DEditor::get_singleton()->get_theme_icon(SNAME("GizmoGPUParticles3D"), SNAME("EditorIcons")));
	create_handle_material("handles");
}

bool GPUParticles3DGizmoPlugin::has_gizmo(Node3D *p_spatial) {
	return Object::cast_to<GPUParticles3D>(p_spatial) != nullptr;
}

String GPUParticles3DGizmoPlugin::get_gizmo_name() const {
	return "GPUParticles3D";
}

int GPUParticles3DGizmoPlugin::get_priority() const {
	return -1;
}

bool GPUParticles3DGizmoPlugin::is_selectable_when_hidden() const {
	return true;
}

String GPUParticles3DGizmoPlugin::get_handle_name(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary) const {
	switch (p_id) {
		case 0:
			return "Size X";
		case 1:
			return "Size Y";
		case 2:
			return "Size Z";
		case 3:
			return "Pos X";
		case 4:
			return "Pos Y";
		case 5:
			return "Pos Z";
	}

	return "";
}

Variant GPUParticles3DGizmoPlugin::get_handle_value(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary) const {
	GPUParticles3D *particles = Object::cast_to<GPUParticles3D>(p_gizmo->get_node_3d());
	return particles->get_visibility_aabb();
}

void GPUParticles3DGizmoPlugin::set_handle(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary, Camera3D *p_camera, const Point2 &p_point) {
	GPUParticles3D *particles = Object::cast_to<GPUParticles3D>(p_gizmo->get_node_3d());

	Transform3D gt = particles->get_global_transform();
	Transform3D gi = gt.affine_inverse();

	bool move = p_id >= 3;
	p_id = p_id % 3;

	AABB aabb = particles->get_visibility_aabb();
	Vector3 ray_from = p_camera->project_ray_origin(p_point);
	Vector3 ray_dir = p_camera->project_ray_normal(p_point);

	Vector3 sg[2] = { gi.xform(ray_from), gi.xform(ray_from + ray_dir * 4096) };

	Vector3 ofs = aabb.get_center();

	Vector3 axis;
	axis[p_id] = 1.0;

	if (move) {
		Vector3 ra, rb;
		Geometry3D::get_closest_points_between_segments(ofs - axis * 4096, ofs + axis * 4096, sg[0], sg[1], ra, rb);

		float d = ra[p_id];
		if (Node3DEditor::get_singleton()->is_snap_enabled()) {
			d = Math::snapped(d, Node3DEditor::get_singleton()->get_translate_snap());
		}

		aabb.position[p_id] = d - 1.0 - aabb.size[p_id] * 0.5;
		particles->set_visibility_aabb(aabb);

	} else {
		Vector3 ra, rb;
		Geometry3D::get_closest_points_between_segments(ofs, ofs + axis * 4096, sg[0], sg[1], ra, rb);

		float d = ra[p_id] - ofs[p_id];
		if (Node3DEditor::get_singleton()->is_snap_enabled()) {
			d = Math::snapped(d, Node3DEditor::get_singleton()->get_translate_snap());
		}

		if (d < 0.001) {
			d = 0.001;
		}
		//resize
		aabb.position[p_id] = (aabb.position[p_id] + aabb.size[p_id] * 0.5) - d;
		aabb.size[p_id] = d * 2;
		particles->set_visibility_aabb(aabb);
	}
}

void GPUParticles3DGizmoPlugin::commit_handle(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary, const Variant &p_restore, bool p_cancel) {
	GPUParticles3D *particles = Object::cast_to<GPUParticles3D>(p_gizmo->get_node_3d());

	if (p_cancel) {
		particles->set_visibility_aabb(p_restore);
		return;
	}

	EditorUndoRedoManager *ur = EditorUndoRedoManager::get_singleton();
	ur->create_action(TTR("Change Particles AABB"));
	ur->add_do_method(particles, "set_visibility_aabb", particles->get_visibility_aabb());
	ur->add_undo_method(particles, "set_visibility_aabb", p_restore);
	ur->commit_action();
}

void GPUParticles3DGizmoPlugin::redraw(EditorNode3DGizmo *p_gizmo) {
	GPUParticles3D *particles = Object::cast_to<GPUParticles3D>(p_gizmo->get_node_3d());

	p_gizmo->clear();

	Vector<Vector3> lines;
	AABB aabb = particles->get_visibility_aabb();

	for (int i = 0; i < 12; i++) {
		Vector3 a, b;
		aabb.get_edge(i, a, b);
		lines.push_back(a);
		lines.push_back(b);
	}

	Vector<Vector3> handles;

	for (int i = 0; i < 3; i++) {
		Vector3 ax;
		ax[i] = aabb.position[i] + aabb.size[i];
		ax[(i + 1) % 3] = aabb.position[(i + 1) % 3] + aabb.size[(i + 1) % 3] * 0.5;
		ax[(i + 2) % 3] = aabb.position[(i + 2) % 3] + aabb.size[(i + 2) % 3] * 0.5;
		handles.push_back(ax);
	}

	Vector3 center = aabb.get_center();
	for (int i = 0; i < 3; i++) {
		Vector3 ax;
		ax[i] = 1.0;
		handles.push_back(center + ax);
		lines.push_back(center);
		lines.push_back(center + ax);
	}

	Ref<Material> material = get_material("particles_material", p_gizmo);
	Ref<Material> icon = get_material("particles_icon", p_gizmo);

	p_gizmo->add_lines(lines, material);

	if (p_gizmo->is_selected()) {
		Ref<Material> solid_material = get_material("particles_solid_material", p_gizmo);
		p_gizmo->add_solid_box(solid_material, aabb.get_size(), aabb.get_center());
	}

	p_gizmo->add_handles(handles, get_material("handles"));
	p_gizmo->add_unscaled_billboard(icon, 0.05);
}

////

GPUParticlesCollision3DGizmoPlugin::GPUParticlesCollision3DGizmoPlugin() {
	Color gizmo_color_attractor = EDITOR_DEF("editors/3d_gizmos/gizmo_colors/particle_attractor", Color(1, 0.7, 0.5));
	create_material("shape_material_attractor", gizmo_color_attractor);
	gizmo_color_attractor.a = 0.15;
	create_material("shape_material_attractor_internal", gizmo_color_attractor);

	Color gizmo_color_collision = EDITOR_DEF("editors/3d_gizmos/gizmo_colors/particle_collision", Color(0.5, 0.7, 1));
	create_material("shape_material_collision", gizmo_color_collision);
	gizmo_color_collision.a = 0.15;
	create_material("shape_material_collision_internal", gizmo_color_collision);

	create_handle_material("handles");
}

bool GPUParticlesCollision3DGizmoPlugin::has_gizmo(Node3D *p_spatial) {
	return (Object::cast_to<GPUParticlesCollision3D>(p_spatial) != nullptr) || (Object::cast_to<GPUParticlesAttractor3D>(p_spatial) != nullptr);
}

String GPUParticlesCollision3DGizmoPlugin::get_gizmo_name() const {
	return "GPUParticlesCollision3D";
}

int GPUParticlesCollision3DGizmoPlugin::get_priority() const {
	return -1;
}

String GPUParticlesCollision3DGizmoPlugin::get_handle_name(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary) const {
	const Node3D *cs = p_gizmo->get_node_3d();

	if (Object::cast_to<GPUParticlesCollisionSphere3D>(cs) || Object::cast_to<GPUParticlesAttractorSphere3D>(cs)) {
		return "Radius";
	}

	if (Object::cast_to<GPUParticlesCollisionBox3D>(cs) || Object::cast_to<GPUParticlesAttractorBox3D>(cs) || Object::cast_to<GPUParticlesAttractorVectorField3D>(cs) || Object::cast_to<GPUParticlesCollisionSDF3D>(cs) || Object::cast_to<GPUParticlesCollisionHeightField3D>(cs)) {
		return "Size";
	}

	return "";
}

Variant GPUParticlesCollision3DGizmoPlugin::get_handle_value(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary) const {
	const Node3D *cs = p_gizmo->get_node_3d();

	if (Object::cast_to<GPUParticlesCollisionSphere3D>(cs) || Object::cast_to<GPUParticlesAttractorSphere3D>(cs)) {
		return p_gizmo->get_node_3d()->call("get_radius");
	}

	if (Object::cast_to<GPUParticlesCollisionBox3D>(cs) || Object::cast_to<GPUParticlesAttractorBox3D>(cs) || Object::cast_to<GPUParticlesAttractorVectorField3D>(cs) || Object::cast_to<GPUParticlesCollisionSDF3D>(cs) || Object::cast_to<GPUParticlesCollisionHeightField3D>(cs)) {
		return Vector3(p_gizmo->get_node_3d()->call("get_size"));
	}

	return Variant();
}

void GPUParticlesCollision3DGizmoPlugin::set_handle(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary, Camera3D *p_camera, const Point2 &p_point) {
	Node3D *sn = p_gizmo->get_node_3d();

	Transform3D gt = sn->get_global_transform();
	Transform3D gi = gt.affine_inverse();

	Vector3 ray_from = p_camera->project_ray_origin(p_point);
	Vector3 ray_dir = p_camera->project_ray_normal(p_point);

	Vector3 sg[2] = { gi.xform(ray_from), gi.xform(ray_from + ray_dir * 4096) };

	if (Object::cast_to<GPUParticlesCollisionSphere3D>(sn) || Object::cast_to<GPUParticlesAttractorSphere3D>(sn)) {
		Vector3 ra, rb;
		Geometry3D::get_closest_points_between_segments(Vector3(), Vector3(4096, 0, 0), sg[0], sg[1], ra, rb);
		float d = ra.x;
		if (Node3DEditor::get_singleton()->is_snap_enabled()) {
			d = Math::snapped(d, Node3DEditor::get_singleton()->get_translate_snap());
		}

		if (d < 0.001) {
			d = 0.001;
		}

		sn->call("set_radius", d);
	}

	if (Object::cast_to<GPUParticlesCollisionBox3D>(sn) || Object::cast_to<GPUParticlesAttractorBox3D>(sn) || Object::cast_to<GPUParticlesAttractorVectorField3D>(sn) || Object::cast_to<GPUParticlesCollisionSDF3D>(sn) || Object::cast_to<GPUParticlesCollisionHeightField3D>(sn)) {
		Vector3 axis;
		axis[p_id] = 1.0;
		Vector3 ra, rb;
		Geometry3D::get_closest_points_between_segments(Vector3(), axis * 4096, sg[0], sg[1], ra, rb);
		float d = ra[p_id] * 2;
		if (Node3DEditor::get_singleton()->is_snap_enabled()) {
			d = Math::snapped(d, Node3DEditor::get_singleton()->get_translate_snap());
		}

		if (d < 0.001) {
			d = 0.001;
		}

		Vector3 he = sn->call("get_size");
		he[p_id] = d;
		sn->call("set_size", he);
	}
}

void GPUParticlesCollision3DGizmoPlugin::commit_handle(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary, const Variant &p_restore, bool p_cancel) {
	Node3D *sn = p_gizmo->get_node_3d();

	if (Object::cast_to<GPUParticlesCollisionSphere3D>(sn) || Object::cast_to<GPUParticlesAttractorSphere3D>(sn)) {
		if (p_cancel) {
			sn->call("set_radius", p_restore);
			return;
		}

		EditorUndoRedoManager *ur = EditorUndoRedoManager::get_singleton();
		ur->create_action(TTR("Change Radius"));
		ur->add_do_method(sn, "set_radius", sn->call("get_radius"));
		ur->add_undo_method(sn, "set_radius", p_restore);
		ur->commit_action();
	}

	if (Object::cast_to<GPUParticlesCollisionBox3D>(sn) || Object::cast_to<GPUParticlesAttractorBox3D>(sn) || Object::cast_to<GPUParticlesAttractorVectorField3D>(sn) || Object::cast_to<GPUParticlesCollisionSDF3D>(sn) || Object::cast_to<GPUParticlesCollisionHeightField3D>(sn)) {
		if (p_cancel) {
			sn->call("set_size", p_restore);
			return;
		}

		EditorUndoRedoManager *ur = EditorUndoRedoManager::get_singleton();
		ur->create_action(TTR("Change Box Shape Size"));
		ur->add_do_method(sn, "set_size", sn->call("get_size"));
		ur->add_undo_method(sn, "set_size", p_restore);
		ur->commit_action();
	}
}

void GPUParticlesCollision3DGizmoPlugin::redraw(EditorNode3DGizmo *p_gizmo) {
	Node3D *cs = p_gizmo->get_node_3d();

	p_gizmo->clear();

	Ref<Material> material;
	Ref<Material> material_internal;
	if (Object::cast_to<GPUParticlesAttractor3D>(cs)) {
		material = get_material("shape_material_attractor", p_gizmo);
		material_internal = get_material("shape_material_attractor_internal", p_gizmo);
	} else {
		material = get_material("shape_material_collision", p_gizmo);
		material_internal = get_material("shape_material_collision_internal", p_gizmo);
	}

	const Ref<Material> handles_material = get_material("handles");

	if (Object::cast_to<GPUParticlesCollisionSphere3D>(cs) || Object::cast_to<GPUParticlesAttractorSphere3D>(cs)) {
		float r = cs->call("get_radius");

		Vector<Vector3> points;

		for (int i = 0; i <= 360; i++) {
			float ra = Math::deg_to_rad((float)i);
			float rb = Math::deg_to_rad((float)i + 1);
			Point2 a = Vector2(Math::sin(ra), Math::cos(ra)) * r;
			Point2 b = Vector2(Math::sin(rb), Math::cos(rb)) * r;

			points.push_back(Vector3(a.x, 0, a.y));
			points.push_back(Vector3(b.x, 0, b.y));
			points.push_back(Vector3(0, a.x, a.y));
			points.push_back(Vector3(0, b.x, b.y));
			points.push_back(Vector3(a.x, a.y, 0));
			points.push_back(Vector3(b.x, b.y, 0));
		}

		Vector<Vector3> collision_segments;

		for (int i = 0; i < 64; i++) {
			float ra = i * (Math_TAU / 64.0);
			float rb = (i + 1) * (Math_TAU / 64.0);
			Point2 a = Vector2(Math::sin(ra), Math::cos(ra)) * r;
			Point2 b = Vector2(Math::sin(rb), Math::cos(rb)) * r;

			collision_segments.push_back(Vector3(a.x, 0, a.y));
			collision_segments.push_back(Vector3(b.x, 0, b.y));
			collision_segments.push_back(Vector3(0, a.x, a.y));
			collision_segments.push_back(Vector3(0, b.x, b.y));
			collision_segments.push_back(Vector3(a.x, a.y, 0));
			collision_segments.push_back(Vector3(b.x, b.y, 0));
		}

		p_gizmo->add_lines(points, material);
		p_gizmo->add_collision_segments(collision_segments);
		Vector<Vector3> handles;
		handles.push_back(Vector3(r, 0, 0));
		p_gizmo->add_handles(handles, handles_material);
	}

	if (Object::cast_to<GPUParticlesCollisionBox3D>(cs) || Object::cast_to<GPUParticlesAttractorBox3D>(cs) || Object::cast_to<GPUParticlesAttractorVectorField3D>(cs) || Object::cast_to<GPUParticlesCollisionSDF3D>(cs) || Object::cast_to<GPUParticlesCollisionHeightField3D>(cs)) {
		Vector<Vector3> lines;
		AABB aabb;
		aabb.size = cs->call("get_size").operator Vector3();
		aabb.position = aabb.size / -2;

		for (int i = 0; i < 12; i++) {
			Vector3 a, b;
			aabb.get_edge(i, a, b);
			lines.push_back(a);
			lines.push_back(b);
		}

		Vector<Vector3> handles;

		for (int i = 0; i < 3; i++) {
			Vector3 ax;
			ax[i] = cs->call("get_size").operator Vector3()[i] / 2;
			handles.push_back(ax);
		}

		p_gizmo->add_lines(lines, material);
		p_gizmo->add_collision_segments(lines);
		p_gizmo->add_handles(handles, handles_material);

		GPUParticlesCollisionSDF3D *col_sdf = Object::cast_to<GPUParticlesCollisionSDF3D>(cs);
		if (col_sdf) {
			static const int subdivs[GPUParticlesCollisionSDF3D::RESOLUTION_MAX] = { 16, 32, 64, 128, 256, 512 };
			int subdiv = subdivs[col_sdf->get_resolution()];
			float cell_size = aabb.get_longest_axis_size() / subdiv;

			lines.clear();

			for (int i = 1; i < subdiv; i++) {
				for (int j = 0; j < 3; j++) {
					if (cell_size * i > aabb.size[j]) {
						continue;
					}

					int j_n1 = (j + 1) % 3;
					int j_n2 = (j + 2) % 3;

					for (int k = 0; k < 4; k++) {
						Vector3 from = aabb.position, to = aabb.position;
						from[j] += cell_size * i;
						to[j] += cell_size * i;

						if (k & 1) {
							to[j_n1] += aabb.size[j_n1];
						} else {
							to[j_n2] += aabb.size[j_n2];
						}

						if (k & 2) {
							from[j_n1] += aabb.size[j_n1];
							from[j_n2] += aabb.size[j_n2];
						}

						lines.push_back(from);
						lines.push_back(to);
					}
				}
			}

			p_gizmo->add_lines(lines, material_internal);
		}
	}
}

/////

////

ReflectionProbeGizmoPlugin::ReflectionProbeGizmoPlugin() {
	Color gizmo_color = EDITOR_DEF("editors/3d_gizmos/gizmo_colors/reflection_probe", Color(0.6, 1, 0.5));

	create_material("reflection_probe_material", gizmo_color);

	gizmo_color.a = 0.5;
	create_material("reflection_internal_material", gizmo_color);

	gizmo_color.a = 0.1;
	create_material("reflection_probe_solid_material", gizmo_color);

	create_icon_material("reflection_probe_icon", Node3DEditor::get_singleton()->get_theme_icon(SNAME("GizmoReflectionProbe"), SNAME("EditorIcons")));
	create_handle_material("handles");
}

bool ReflectionProbeGizmoPlugin::has_gizmo(Node3D *p_spatial) {
	return Object::cast_to<ReflectionProbe>(p_spatial) != nullptr;
}

String ReflectionProbeGizmoPlugin::get_gizmo_name() const {
	return "ReflectionProbe";
}

int ReflectionProbeGizmoPlugin::get_priority() const {
	return -1;
}

String ReflectionProbeGizmoPlugin::get_handle_name(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary) const {
	switch (p_id) {
		case 0:
			return "Size X";
		case 1:
			return "Size Y";
		case 2:
			return "Size Z";
		case 3:
			return "Origin X";
		case 4:
			return "Origin Y";
		case 5:
			return "Origin Z";
	}

	return "";
}

Variant ReflectionProbeGizmoPlugin::get_handle_value(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary) const {
	ReflectionProbe *probe = Object::cast_to<ReflectionProbe>(p_gizmo->get_node_3d());
	return AABB(probe->get_origin_offset(), probe->get_size());
}

void ReflectionProbeGizmoPlugin::set_handle(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary, Camera3D *p_camera, const Point2 &p_point) {
	ReflectionProbe *probe = Object::cast_to<ReflectionProbe>(p_gizmo->get_node_3d());
	Transform3D gt = probe->get_global_transform();

	Transform3D gi = gt.affine_inverse();

	if (p_id < 3) {
		Vector3 size = probe->get_size();

		Vector3 ray_from = p_camera->project_ray_origin(p_point);
		Vector3 ray_dir = p_camera->project_ray_normal(p_point);

		Vector3 sg[2] = { gi.xform(ray_from), gi.xform(ray_from + ray_dir * 16384) };

		Vector3 axis;
		axis[p_id] = 1.0;

		Vector3 ra, rb;
		Geometry3D::get_closest_points_between_segments(Vector3(), axis * 16384, sg[0], sg[1], ra, rb);
		float d = ra[p_id] * 2;
		if (Node3DEditor::get_singleton()->is_snap_enabled()) {
			d = Math::snapped(d, Node3DEditor::get_singleton()->get_translate_snap());
		}

		if (d < 0.001) {
			d = 0.001;
		}

		size[p_id] = d;
		probe->set_size(size);
	} else {
		p_id -= 3;

		Vector3 origin = probe->get_origin_offset();
		origin[p_id] = 0;

		Vector3 ray_from = p_camera->project_ray_origin(p_point);
		Vector3 ray_dir = p_camera->project_ray_normal(p_point);

		Vector3 sg[2] = { gi.xform(ray_from), gi.xform(ray_from + ray_dir * 16384) };

		Vector3 axis;
		axis[p_id] = 1.0;

		Vector3 ra, rb;
		Geometry3D::get_closest_points_between_segments(origin - axis * 16384, origin + axis * 16384, sg[0], sg[1], ra, rb);
		// Adjust the actual position to account for the gizmo handle position
		float d = ra[p_id] + 0.25;
		if (Node3DEditor::get_singleton()->is_snap_enabled()) {
			d = Math::snapped(d, Node3DEditor::get_singleton()->get_translate_snap());
		}

		origin[p_id] = d;
		probe->set_origin_offset(origin);
	}
}

void ReflectionProbeGizmoPlugin::commit_handle(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary, const Variant &p_restore, bool p_cancel) {
	ReflectionProbe *probe = Object::cast_to<ReflectionProbe>(p_gizmo->get_node_3d());

	AABB restore = p_restore;

	if (p_cancel) {
		probe->set_origin_offset(restore.position);
		probe->set_size(restore.size);
		return;
	}

	EditorUndoRedoManager *ur = EditorUndoRedoManager::get_singleton();
	ur->create_action(TTR("Change Probe Size"));
	ur->add_do_method(probe, "set_size", probe->get_size());
	ur->add_do_method(probe, "set_origin_offset", probe->get_origin_offset());
	ur->add_undo_method(probe, "set_size", restore.size);
	ur->add_undo_method(probe, "set_origin_offset", restore.position);
	ur->commit_action();
}

void ReflectionProbeGizmoPlugin::redraw(EditorNode3DGizmo *p_gizmo) {
	ReflectionProbe *probe = Object::cast_to<ReflectionProbe>(p_gizmo->get_node_3d());

	p_gizmo->clear();

	Vector<Vector3> lines;
	Vector<Vector3> internal_lines;
	Vector3 size = probe->get_size();

	AABB aabb;
	aabb.position = -size / 2;
	aabb.size = size;

	for (int i = 0; i < 12; i++) {
		Vector3 a, b;
		aabb.get_edge(i, a, b);
		lines.push_back(a);
		lines.push_back(b);
	}

	for (int i = 0; i < 8; i++) {
		Vector3 ep = aabb.get_endpoint(i);
		internal_lines.push_back(probe->get_origin_offset());
		internal_lines.push_back(ep);
	}

	Vector<Vector3> handles;

	for (int i = 0; i < 3; i++) {
		Vector3 ax;
		ax[i] = aabb.position[i] + aabb.size[i];
		handles.push_back(ax);
	}

	for (int i = 0; i < 3; i++) {
		Vector3 orig_handle = probe->get_origin_offset();
		orig_handle[i] -= 0.25;
		lines.push_back(orig_handle);
		handles.push_back(orig_handle);

		orig_handle[i] += 0.5;
		lines.push_back(orig_handle);
	}

	Ref<Material> material = get_material("reflection_probe_material", p_gizmo);
	Ref<Material> material_internal = get_material("reflection_internal_material", p_gizmo);
	Ref<Material> icon = get_material("reflection_probe_icon", p_gizmo);

	p_gizmo->add_lines(lines, material);
	p_gizmo->add_lines(internal_lines, material_internal);

	if (p_gizmo->is_selected()) {
		Ref<Material> solid_material = get_material("reflection_probe_solid_material", p_gizmo);
		p_gizmo->add_solid_box(solid_material, probe->get_size());
	}

	p_gizmo->add_unscaled_billboard(icon, 0.05);
	p_gizmo->add_handles(handles, get_material("handles"));
}

///////////////////////////////

////

DecalGizmoPlugin::DecalGizmoPlugin() {
	Color gizmo_color = EDITOR_DEF("editors/3d_gizmos/gizmo_colors/decal", Color(0.6, 0.5, 1.0));

	create_material("decal_material", gizmo_color);

	create_handle_material("handles");
}

bool DecalGizmoPlugin::has_gizmo(Node3D *p_spatial) {
	return Object::cast_to<Decal>(p_spatial) != nullptr;
}

String DecalGizmoPlugin::get_gizmo_name() const {
	return "Decal";
}

int DecalGizmoPlugin::get_priority() const {
	return -1;
}

String DecalGizmoPlugin::get_handle_name(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary) const {
	switch (p_id) {
		case 0:
			return "Size X";
		case 1:
			return "Size Y";
		case 2:
			return "Size Z";
	}

	return "";
}

Variant DecalGizmoPlugin::get_handle_value(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary) const {
	Decal *decal = Object::cast_to<Decal>(p_gizmo->get_node_3d());
	return decal->get_size();
}

void DecalGizmoPlugin::set_handle(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary, Camera3D *p_camera, const Point2 &p_point) {
	Decal *decal = Object::cast_to<Decal>(p_gizmo->get_node_3d());
	Transform3D gt = decal->get_global_transform();

	Transform3D gi = gt.affine_inverse();

	Vector3 size = decal->get_size();

	Vector3 ray_from = p_camera->project_ray_origin(p_point);
	Vector3 ray_dir = p_camera->project_ray_normal(p_point);

	Vector3 sg[2] = { gi.xform(ray_from), gi.xform(ray_from + ray_dir * 16384) };

	Vector3 axis;
	axis[p_id] = 1.0;

	Vector3 ra, rb;
	Geometry3D::get_closest_points_between_segments(Vector3(), axis * 16384, sg[0], sg[1], ra, rb);
	float d = ra[p_id] * 2;
	if (Node3DEditor::get_singleton()->is_snap_enabled()) {
		d = Math::snapped(d, Node3DEditor::get_singleton()->get_translate_snap());
	}

	if (d < 0.001) {
		d = 0.001;
	}

	size[p_id] = d;
	decal->set_size(size);
}

void DecalGizmoPlugin::commit_handle(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary, const Variant &p_restore, bool p_cancel) {
	Decal *decal = Object::cast_to<Decal>(p_gizmo->get_node_3d());

	Vector3 restore = p_restore;

	if (p_cancel) {
		decal->set_size(restore);
		return;
	}

	EditorUndoRedoManager *ur = EditorUndoRedoManager::get_singleton();
	ur->create_action(TTR("Change Decal Size"));
	ur->add_do_method(decal, "set_size", decal->get_size());
	ur->add_undo_method(decal, "set_size", restore);
	ur->commit_action();
}

void DecalGizmoPlugin::redraw(EditorNode3DGizmo *p_gizmo) {
	Decal *decal = Object::cast_to<Decal>(p_gizmo->get_node_3d());

	p_gizmo->clear();

	Vector<Vector3> lines;
	Vector3 size = decal->get_size();

	AABB aabb;
	aabb.position = -size / 2;
	aabb.size = size;

	for (int i = 0; i < 12; i++) {
		Vector3 a, b;
		aabb.get_edge(i, a, b);
		if (a.y == b.y) {
			lines.push_back(a);
			lines.push_back(b);
		} else {
			Vector3 ah = a.lerp(b, 0.2);
			lines.push_back(a);
			lines.push_back(ah);
			Vector3 bh = b.lerp(a, 0.2);
			lines.push_back(b);
			lines.push_back(bh);
		}
	}

	float half_size_y = size.y / 2;
	lines.push_back(Vector3(0, half_size_y, 0));
	lines.push_back(Vector3(0, half_size_y * 1.2, 0));

	Vector<Vector3> handles;

	for (int i = 0; i < 3; i++) {
		Vector3 ax;
		ax[i] = aabb.position[i] + aabb.size[i];
		handles.push_back(ax);
	}

	Ref<Material> material = get_material("decal_material", p_gizmo);

	p_gizmo->add_lines(lines, material);
	p_gizmo->add_handles(handles, get_material("handles"));
}

///////////////////////////////
VoxelGIGizmoPlugin::VoxelGIGizmoPlugin() {
	Color gizmo_color = EDITOR_DEF("editors/3d_gizmos/gizmo_colors/voxel_gi", Color(0.5, 1, 0.6));

	create_material("voxel_gi_material", gizmo_color);

	// This gizmo draws a lot of lines. Use a low opacity to make it not too intrusive.
	gizmo_color.a = 0.1;
	create_material("voxel_gi_internal_material", gizmo_color);

	gizmo_color.a = 0.05;
	create_material("voxel_gi_solid_material", gizmo_color);

	create_icon_material("voxel_gi_icon", Node3DEditor::get_singleton()->get_theme_icon(SNAME("GizmoVoxelGI"), SNAME("EditorIcons")));
	create_handle_material("handles");
}

bool VoxelGIGizmoPlugin::has_gizmo(Node3D *p_spatial) {
	return Object::cast_to<VoxelGI>(p_spatial) != nullptr;
}

String VoxelGIGizmoPlugin::get_gizmo_name() const {
	return "VoxelGI";
}

int VoxelGIGizmoPlugin::get_priority() const {
	return -1;
}

String VoxelGIGizmoPlugin::get_handle_name(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary) const {
	switch (p_id) {
		case 0:
			return "Size X";
		case 1:
			return "Size Y";
		case 2:
			return "Size Z";
	}

	return "";
}

Variant VoxelGIGizmoPlugin::get_handle_value(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary) const {
	VoxelGI *probe = Object::cast_to<VoxelGI>(p_gizmo->get_node_3d());
	return probe->get_size();
}

void VoxelGIGizmoPlugin::set_handle(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary, Camera3D *p_camera, const Point2 &p_point) {
	VoxelGI *probe = Object::cast_to<VoxelGI>(p_gizmo->get_node_3d());

	Transform3D gt = probe->get_global_transform();
	Transform3D gi = gt.affine_inverse();

	Vector3 size = probe->get_size();

	Vector3 ray_from = p_camera->project_ray_origin(p_point);
	Vector3 ray_dir = p_camera->project_ray_normal(p_point);

	Vector3 sg[2] = { gi.xform(ray_from), gi.xform(ray_from + ray_dir * 16384) };

	Vector3 axis;
	axis[p_id] = 1.0;

	Vector3 ra, rb;
	Geometry3D::get_closest_points_between_segments(Vector3(), axis * 16384, sg[0], sg[1], ra, rb);
	float d = ra[p_id] * 2;
	if (Node3DEditor::get_singleton()->is_snap_enabled()) {
		d = Math::snapped(d, Node3DEditor::get_singleton()->get_translate_snap());
	}

	if (d < 0.001) {
		d = 0.001;
	}

	size[p_id] = d;
	probe->set_size(size);
}

void VoxelGIGizmoPlugin::commit_handle(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary, const Variant &p_restore, bool p_cancel) {
	VoxelGI *probe = Object::cast_to<VoxelGI>(p_gizmo->get_node_3d());

	Vector3 restore = p_restore;

	if (p_cancel) {
		probe->set_size(restore);
		return;
	}

	EditorUndoRedoManager *ur = EditorUndoRedoManager::get_singleton();
	ur->create_action(TTR("Change Probe Size"));
	ur->add_do_method(probe, "set_size", probe->get_size());
	ur->add_undo_method(probe, "set_size", restore);
	ur->commit_action();
}

void VoxelGIGizmoPlugin::redraw(EditorNode3DGizmo *p_gizmo) {
	VoxelGI *probe = Object::cast_to<VoxelGI>(p_gizmo->get_node_3d());

	Ref<Material> material = get_material("voxel_gi_material", p_gizmo);
	Ref<Material> icon = get_material("voxel_gi_icon", p_gizmo);
	Ref<Material> material_internal = get_material("voxel_gi_internal_material", p_gizmo);

	p_gizmo->clear();

	Vector<Vector3> lines;
	Vector3 size = probe->get_size();

	static const int subdivs[VoxelGI::SUBDIV_MAX] = { 64, 128, 256, 512 };

	AABB aabb = AABB(-size / 2, size);
	int subdiv = subdivs[probe->get_subdiv()];
	float cell_size = aabb.get_longest_axis_size() / subdiv;

	for (int i = 0; i < 12; i++) {
		Vector3 a, b;
		aabb.get_edge(i, a, b);
		lines.push_back(a);
		lines.push_back(b);
	}

	p_gizmo->add_lines(lines, material);

	lines.clear();

	for (int i = 1; i < subdiv; i++) {
		for (int j = 0; j < 3; j++) {
			if (cell_size * i > aabb.size[j]) {
				continue;
			}

			int j_n1 = (j + 1) % 3;
			int j_n2 = (j + 2) % 3;

			for (int k = 0; k < 4; k++) {
				Vector3 from = aabb.position, to = aabb.position;
				from[j] += cell_size * i;
				to[j] += cell_size * i;

				if (k & 1) {
					to[j_n1] += aabb.size[j_n1];
				} else {
					to[j_n2] += aabb.size[j_n2];
				}

				if (k & 2) {
					from[j_n1] += aabb.size[j_n1];
					from[j_n2] += aabb.size[j_n2];
				}

				lines.push_back(from);
				lines.push_back(to);
			}
		}
	}

	p_gizmo->add_lines(lines, material_internal);

	Vector<Vector3> handles;

	for (int i = 0; i < 3; i++) {
		Vector3 ax;
		ax[i] = aabb.position[i] + aabb.size[i];
		handles.push_back(ax);
	}

	if (p_gizmo->is_selected()) {
		Ref<Material> solid_material = get_material("voxel_gi_solid_material", p_gizmo);
		p_gizmo->add_solid_box(solid_material, aabb.get_size());
	}

	p_gizmo->add_unscaled_billboard(icon, 0.05);
	p_gizmo->add_handles(handles, get_material("handles"));
}

////

LightmapGIGizmoPlugin::LightmapGIGizmoPlugin() {
	Color gizmo_color = EDITOR_DEF("editors/3d_gizmos/gizmo_colors/lightmap_lines", Color(0.5, 0.6, 1));

	gizmo_color.a = 0.1;
	create_material("lightmap_lines", gizmo_color);

	Ref<StandardMaterial3D> mat = memnew(StandardMaterial3D);
	mat->set_shading_mode(StandardMaterial3D::SHADING_MODE_UNSHADED);
	mat->set_cull_mode(StandardMaterial3D::CULL_DISABLED);
	mat->set_flag(StandardMaterial3D::FLAG_ALBEDO_FROM_VERTEX_COLOR, true);
	mat->set_flag(StandardMaterial3D::FLAG_SRGB_VERTEX_COLOR, false);

	add_material("lightmap_probe_material", mat);

	create_icon_material("baked_indirect_light_icon", Node3DEditor::get_singleton()->get_theme_icon(SNAME("GizmoLightmapGI"), SNAME("EditorIcons")));
}

bool LightmapGIGizmoPlugin::has_gizmo(Node3D *p_spatial) {
	return Object::cast_to<LightmapGI>(p_spatial) != nullptr;
}

String LightmapGIGizmoPlugin::get_gizmo_name() const {
	return "LightmapGI";
}

int LightmapGIGizmoPlugin::get_priority() const {
	return -1;
}

void LightmapGIGizmoPlugin::redraw(EditorNode3DGizmo *p_gizmo) {
	Ref<Material> icon = get_material("baked_indirect_light_icon", p_gizmo);
	LightmapGI *baker = Object::cast_to<LightmapGI>(p_gizmo->get_node_3d());
	Ref<LightmapGIData> data = baker->get_light_data();

	p_gizmo->add_unscaled_billboard(icon, 0.05);

	if (data.is_null()) {
		return;
	}

	Ref<Material> material_lines = get_material("lightmap_lines", p_gizmo);
	Ref<Material> material_probes = get_material("lightmap_probe_material", p_gizmo);

	p_gizmo->clear();

	Vector<Vector3> lines;
	HashSet<Vector2i> lines_found;

	Vector<Vector3> points = data->get_capture_points();
	if (points.size() == 0) {
		return;
	}
	Vector<Color> sh = data->get_capture_sh();
	if (sh.size() != points.size() * 9) {
		return;
	}

	Vector<int> tetrahedrons = data->get_capture_tetrahedra();

	for (int i = 0; i < tetrahedrons.size(); i += 4) {
		for (int j = 0; j < 4; j++) {
			for (int k = j + 1; k < 4; k++) {
				Vector2i pair;
				pair.x = tetrahedrons[i + j];
				pair.y = tetrahedrons[i + k];

				if (pair.y < pair.x) {
					SWAP(pair.x, pair.y);
				}
				if (lines_found.has(pair)) {
					continue;
				}
				lines_found.insert(pair);
				lines.push_back(points[pair.x]);
				lines.push_back(points[pair.y]);
			}
		}
	}

	p_gizmo->add_lines(lines, material_lines);

	int stack_count = 8;
	int sector_count = 16;

	float sector_step = (Math_PI * 2.0) / sector_count;
	float stack_step = Math_PI / stack_count;

	Vector<Vector3> vertices;
	Vector<Color> colors;
	Vector<int> indices;
	float radius = 0.3;

	for (int p = 0; p < points.size(); p++) {
		int vertex_base = vertices.size();
		Vector3 sh_col[9];
		for (int i = 0; i < 9; i++) {
			sh_col[i].x = sh[p * 9 + i].r;
			sh_col[i].y = sh[p * 9 + i].g;
			sh_col[i].z = sh[p * 9 + i].b;
		}

		for (int i = 0; i <= stack_count; ++i) {
			float stack_angle = Math_PI / 2 - i * stack_step; // starting from pi/2 to -pi/2
			float xy = radius * Math::cos(stack_angle); // r * cos(u)
			float z = radius * Math::sin(stack_angle); // r * sin(u)

			// add (sector_count+1) vertices per stack
			// the first and last vertices have same position and normal, but different tex coords
			for (int j = 0; j <= sector_count; ++j) {
				float sector_angle = j * sector_step; // starting from 0 to 2pi

				// vertex position (x, y, z)
				float x = xy * Math::cos(sector_angle); // r * cos(u) * cos(v)
				float y = xy * Math::sin(sector_angle); // r * cos(u) * sin(v)

				Vector3 n = Vector3(x, z, y);
				vertices.push_back(points[p] + n);
				n.normalize();

				const float c1 = 0.429043;
				const float c2 = 0.511664;
				const float c3 = 0.743125;
				const float c4 = 0.886227;
				const float c5 = 0.247708;
				Vector3 light = (c1 * sh_col[8] * (n.x * n.x - n.y * n.y) +
						c3 * sh_col[6] * n.z * n.z +
						c4 * sh_col[0] -
						c5 * sh_col[6] +
						2.0 * c1 * sh_col[4] * n.x * n.y +
						2.0 * c1 * sh_col[7] * n.x * n.z +
						2.0 * c1 * sh_col[5] * n.y * n.z +
						2.0 * c2 * sh_col[3] * n.x +
						2.0 * c2 * sh_col[1] * n.y +
						2.0 * c2 * sh_col[2] * n.z);

				colors.push_back(Color(light.x, light.y, light.z, 1));
			}
		}

		for (int i = 0; i < stack_count; ++i) {
			int k1 = i * (sector_count + 1); // beginning of current stack
			int k2 = k1 + sector_count + 1; // beginning of next stack

			for (int j = 0; j < sector_count; ++j, ++k1, ++k2) {
				// 2 triangles per sector excluding first and last stacks
				// k1 => k2 => k1+1
				if (i != 0) {
					indices.push_back(vertex_base + k1);
					indices.push_back(vertex_base + k2);
					indices.push_back(vertex_base + k1 + 1);
				}

				// k1+1 => k2 => k2+1
				if (i != (stack_count - 1)) {
					indices.push_back(vertex_base + k1 + 1);
					indices.push_back(vertex_base + k2);
					indices.push_back(vertex_base + k2 + 1);
				}
			}
		}
	}

	Array array;
	array.resize(RS::ARRAY_MAX);
	array[RS::ARRAY_VERTEX] = vertices;
	array[RS::ARRAY_INDEX] = indices;
	array[RS::ARRAY_COLOR] = colors;

	Ref<ArrayMesh> mesh;
	mesh.instantiate();
	mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, array, Array(), Dictionary(), 0); //no compression
	mesh->surface_set_material(0, material_probes);

	p_gizmo->add_mesh(mesh);
}

/////////

LightmapProbeGizmoPlugin::LightmapProbeGizmoPlugin() {
	Color gizmo_color = EDITOR_DEF("editors/3d_gizmos/gizmo_colors/lightprobe_lines", Color(0.5, 0.6, 1));

	gizmo_color.a = 0.3;
	create_material("lightprobe_lines", gizmo_color);
}

bool LightmapProbeGizmoPlugin::has_gizmo(Node3D *p_spatial) {
	return Object::cast_to<LightmapProbe>(p_spatial) != nullptr;
}

String LightmapProbeGizmoPlugin::get_gizmo_name() const {
	return "LightmapProbe";
}

int LightmapProbeGizmoPlugin::get_priority() const {
	return -1;
}

void LightmapProbeGizmoPlugin::redraw(EditorNode3DGizmo *p_gizmo) {
	Ref<Material> material_lines = get_material("lightprobe_lines", p_gizmo);

	p_gizmo->clear();

	Vector<Vector3> lines;

	int stack_count = 8;
	int sector_count = 16;

	float sector_step = (Math_PI * 2.0) / sector_count;
	float stack_step = Math_PI / stack_count;

	Vector<Vector3> vertices;
	float radius = 0.2;

	for (int i = 0; i <= stack_count; ++i) {
		float stack_angle = Math_PI / 2 - i * stack_step; // starting from pi/2 to -pi/2
		float xy = radius * Math::cos(stack_angle); // r * cos(u)
		float z = radius * Math::sin(stack_angle); // r * sin(u)

		// add (sector_count+1) vertices per stack
		// the first and last vertices have same position and normal, but different tex coords
		for (int j = 0; j <= sector_count; ++j) {
			float sector_angle = j * sector_step; // starting from 0 to 2pi

			// vertex position (x, y, z)
			float x = xy * Math::cos(sector_angle); // r * cos(u) * cos(v)
			float y = xy * Math::sin(sector_angle); // r * cos(u) * sin(v)

			Vector3 n = Vector3(x, z, y);
			vertices.push_back(n);
		}
	}

	for (int i = 0; i < stack_count; ++i) {
		int k1 = i * (sector_count + 1); // beginning of current stack
		int k2 = k1 + sector_count + 1; // beginning of next stack

		for (int j = 0; j < sector_count; ++j, ++k1, ++k2) {
			// 2 triangles per sector excluding first and last stacks
			// k1 => k2 => k1+1
			if (i != 0) {
				lines.push_back(vertices[k1]);
				lines.push_back(vertices[k2]);
				lines.push_back(vertices[k1]);
				lines.push_back(vertices[k1 + 1]);
			}

			if (i != (stack_count - 1)) {
				lines.push_back(vertices[k1 + 1]);
				lines.push_back(vertices[k2]);
				lines.push_back(vertices[k2]);
				lines.push_back(vertices[k2 + 1]);
			}
		}
	}

	p_gizmo->add_lines(lines, material_lines);
}

////

CollisionObject3DGizmoPlugin::CollisionObject3DGizmoPlugin() {
	const Color gizmo_color = EDITOR_GET("editors/3d_gizmos/gizmo_colors/shape");
	create_material("shape_material", gizmo_color);
	const float gizmo_value = gizmo_color.get_v();
	const Color gizmo_color_disabled = Color(gizmo_value, gizmo_value, gizmo_value, 0.65);
	create_material("shape_material_disabled", gizmo_color_disabled);
}

bool CollisionObject3DGizmoPlugin::has_gizmo(Node3D *p_spatial) {
	return Object::cast_to<CollisionObject3D>(p_spatial) != nullptr;
}

String CollisionObject3DGizmoPlugin::get_gizmo_name() const {
	return "CollisionObject3D";
}

int CollisionObject3DGizmoPlugin::get_priority() const {
	return -2;
}

void CollisionObject3DGizmoPlugin::redraw(EditorNode3DGizmo *p_gizmo) {
	CollisionObject3D *co = Object::cast_to<CollisionObject3D>(p_gizmo->get_node_3d());

	p_gizmo->clear();

	List<uint32_t> owner_ids;
	co->get_shape_owners(&owner_ids);
	for (uint32_t &owner_id : owner_ids) {
		Transform3D xform = co->shape_owner_get_transform(owner_id);
		Object *owner = co->shape_owner_get_owner(owner_id);
		// Exclude CollisionShape3D and CollisionPolygon3D as they have their gizmo.
		if (!Object::cast_to<CollisionShape3D>(owner) && !Object::cast_to<CollisionPolygon3D>(owner)) {
			Ref<Material> material = get_material(!co->is_shape_owner_disabled(owner_id) ? "shape_material" : "shape_material_disabled", p_gizmo);
			for (int shape_id = 0; shape_id < co->shape_owner_get_shape_count(owner_id); shape_id++) {
				Ref<Shape3D> s = co->shape_owner_get_shape(owner_id, shape_id);
				if (s.is_null()) {
					continue;
				}
				SurfaceTool st;
				st.append_from(s->get_debug_mesh(), 0, xform);

				p_gizmo->add_mesh(st.commit(), material);
				p_gizmo->add_collision_segments(s->get_debug_mesh_lines());
			}
		}
	}
}

////

CollisionShape3DGizmoPlugin::CollisionShape3DGizmoPlugin() {
	const Color gizmo_color = EDITOR_GET("editors/3d_gizmos/gizmo_colors/shape");
	create_material("shape_material", gizmo_color);
	const float gizmo_value = gizmo_color.get_v();
	const Color gizmo_color_disabled = Color(gizmo_value, gizmo_value, gizmo_value, 0.65);
	create_material("shape_material_disabled", gizmo_color_disabled);
	create_handle_material("handles");
}

bool CollisionShape3DGizmoPlugin::has_gizmo(Node3D *p_spatial) {
	return Object::cast_to<CollisionShape3D>(p_spatial) != nullptr;
}

String CollisionShape3DGizmoPlugin::get_gizmo_name() const {
	return "CollisionShape3D";
}

int CollisionShape3DGizmoPlugin::get_priority() const {
	return -1;
}

String CollisionShape3DGizmoPlugin::get_handle_name(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary) const {
	const CollisionShape3D *cs = Object::cast_to<CollisionShape3D>(p_gizmo->get_node_3d());

	Ref<Shape3D> s = cs->get_shape();
	if (s.is_null()) {
		return "";
	}

	if (Object::cast_to<SphereShape3D>(*s)) {
		return "Radius";
	}

	if (Object::cast_to<BoxShape3D>(*s)) {
		return "Size";
	}

	if (Object::cast_to<CapsuleShape3D>(*s)) {
		return p_id == 0 ? "Radius" : "Height";
	}

	if (Object::cast_to<CylinderShape3D>(*s)) {
		return p_id == 0 ? "Radius" : "Height";
	}

	if (Object::cast_to<SeparationRayShape3D>(*s)) {
		return "Length";
	}

	return "";
}

Variant CollisionShape3DGizmoPlugin::get_handle_value(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary) const {
	CollisionShape3D *cs = Object::cast_to<CollisionShape3D>(p_gizmo->get_node_3d());

	Ref<Shape3D> s = cs->get_shape();
	if (s.is_null()) {
		return Variant();
	}

	if (Object::cast_to<SphereShape3D>(*s)) {
		Ref<SphereShape3D> ss = s;
		return ss->get_radius();
	}

	if (Object::cast_to<BoxShape3D>(*s)) {
		Ref<BoxShape3D> bs = s;
		return bs->get_size();
	}

	if (Object::cast_to<CapsuleShape3D>(*s)) {
		Ref<CapsuleShape3D> cs2 = s;
		return Vector2(cs2->get_radius(), cs2->get_height());
	}

	if (Object::cast_to<CylinderShape3D>(*s)) {
		Ref<CylinderShape3D> cs2 = s;
		return p_id == 0 ? cs2->get_radius() : cs2->get_height();
	}

	if (Object::cast_to<SeparationRayShape3D>(*s)) {
		Ref<SeparationRayShape3D> cs2 = s;
		return cs2->get_length();
	}

	return Variant();
}

void CollisionShape3DGizmoPlugin::set_handle(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary, Camera3D *p_camera, const Point2 &p_point) {
	CollisionShape3D *cs = Object::cast_to<CollisionShape3D>(p_gizmo->get_node_3d());

	Ref<Shape3D> s = cs->get_shape();
	if (s.is_null()) {
		return;
	}

	Transform3D gt = cs->get_global_transform();
	Transform3D gi = gt.affine_inverse();

	Vector3 ray_from = p_camera->project_ray_origin(p_point);
	Vector3 ray_dir = p_camera->project_ray_normal(p_point);

	Vector3 sg[2] = { gi.xform(ray_from), gi.xform(ray_from + ray_dir * 4096) };

	if (Object::cast_to<SphereShape3D>(*s)) {
		Ref<SphereShape3D> ss = s;
		Vector3 ra, rb;
		Geometry3D::get_closest_points_between_segments(Vector3(), Vector3(4096, 0, 0), sg[0], sg[1], ra, rb);
		float d = ra.x;
		if (Node3DEditor::get_singleton()->is_snap_enabled()) {
			d = Math::snapped(d, Node3DEditor::get_singleton()->get_translate_snap());
		}

		if (d < 0.001) {
			d = 0.001;
		}

		ss->set_radius(d);
	}

	if (Object::cast_to<SeparationRayShape3D>(*s)) {
		Ref<SeparationRayShape3D> rs = s;
		Vector3 ra, rb;
		Geometry3D::get_closest_points_between_segments(Vector3(), Vector3(0, 0, 4096), sg[0], sg[1], ra, rb);
		float d = ra.z;
		if (Node3DEditor::get_singleton()->is_snap_enabled()) {
			d = Math::snapped(d, Node3DEditor::get_singleton()->get_translate_snap());
		}

		if (d < 0.001) {
			d = 0.001;
		}

		rs->set_length(d);
	}

	if (Object::cast_to<BoxShape3D>(*s)) {
		Vector3 axis;
		axis[p_id] = 1.0;
		Ref<BoxShape3D> bs = s;
		Vector3 ra, rb;
		Geometry3D::get_closest_points_between_segments(Vector3(), axis * 4096, sg[0], sg[1], ra, rb);
		float d = ra[p_id] * 2;
		if (Node3DEditor::get_singleton()->is_snap_enabled()) {
			d = Math::snapped(d, Node3DEditor::get_singleton()->get_translate_snap());
		}

		if (d < 0.001) {
			d = 0.001;
		}

		Vector3 he = bs->get_size();
		he[p_id] = d;
		bs->set_size(he);
	}

	if (Object::cast_to<CapsuleShape3D>(*s)) {
		Vector3 axis;
		axis[p_id == 0 ? 0 : 1] = 1.0;
		Ref<CapsuleShape3D> cs2 = s;
		Vector3 ra, rb;
		Geometry3D::get_closest_points_between_segments(Vector3(), axis * 4096, sg[0], sg[1], ra, rb);
		float d = axis.dot(ra);

		if (Node3DEditor::get_singleton()->is_snap_enabled()) {
			d = Math::snapped(d, Node3DEditor::get_singleton()->get_translate_snap());
		}

		if (d < 0.001) {
			d = 0.001;
		}

		if (p_id == 0) {
			cs2->set_radius(d);
		} else if (p_id == 1) {
			cs2->set_height(d * 2.0);
		}
	}

	if (Object::cast_to<CylinderShape3D>(*s)) {
		Vector3 axis;
		axis[p_id == 0 ? 0 : 1] = 1.0;
		Ref<CylinderShape3D> cs2 = s;
		Vector3 ra, rb;
		Geometry3D::get_closest_points_between_segments(Vector3(), axis * 4096, sg[0], sg[1], ra, rb);
		float d = axis.dot(ra);
		if (Node3DEditor::get_singleton()->is_snap_enabled()) {
			d = Math::snapped(d, Node3DEditor::get_singleton()->get_translate_snap());
		}

		if (d < 0.001) {
			d = 0.001;
		}

		if (p_id == 0) {
			cs2->set_radius(d);
		} else if (p_id == 1) {
			cs2->set_height(d * 2.0);
		}
	}
}

void CollisionShape3DGizmoPlugin::commit_handle(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary, const Variant &p_restore, bool p_cancel) {
	CollisionShape3D *cs = Object::cast_to<CollisionShape3D>(p_gizmo->get_node_3d());

	Ref<Shape3D> s = cs->get_shape();
	if (s.is_null()) {
		return;
	}

	if (Object::cast_to<SphereShape3D>(*s)) {
		Ref<SphereShape3D> ss = s;
		if (p_cancel) {
			ss->set_radius(p_restore);
			return;
		}

		EditorUndoRedoManager *ur = EditorUndoRedoManager::get_singleton();
		ur->create_action(TTR("Change Sphere Shape Radius"));
		ur->add_do_method(ss.ptr(), "set_radius", ss->get_radius());
		ur->add_undo_method(ss.ptr(), "set_radius", p_restore);
		ur->commit_action();
	}

	if (Object::cast_to<BoxShape3D>(*s)) {
		Ref<BoxShape3D> ss = s;
		if (p_cancel) {
			ss->set_size(p_restore);
			return;
		}

		EditorUndoRedoManager *ur = EditorUndoRedoManager::get_singleton();
		ur->create_action(TTR("Change Box Shape Size"));
		ur->add_do_method(ss.ptr(), "set_size", ss->get_size());
		ur->add_undo_method(ss.ptr(), "set_size", p_restore);
		ur->commit_action();
	}

	if (Object::cast_to<CapsuleShape3D>(*s)) {
		Ref<CapsuleShape3D> ss = s;
		Vector2 values = p_restore;

		if (p_cancel) {
			ss->set_radius(values[0]);
			ss->set_height(values[1]);
			return;
		}

		EditorUndoRedoManager *ur = EditorUndoRedoManager::get_singleton();
		if (p_id == 0) {
			ur->create_action(TTR("Change Capsule Shape Radius"));
			ur->add_do_method(ss.ptr(), "set_radius", ss->get_radius());
		} else {
			ur->create_action(TTR("Change Capsule Shape Height"));
			ur->add_do_method(ss.ptr(), "set_height", ss->get_height());
		}
		ur->add_undo_method(ss.ptr(), "set_radius", values[0]);
		ur->add_undo_method(ss.ptr(), "set_height", values[1]);

		ur->commit_action();
	}

	if (Object::cast_to<CylinderShape3D>(*s)) {
		Ref<CylinderShape3D> ss = s;
		if (p_cancel) {
			if (p_id == 0) {
				ss->set_radius(p_restore);
			} else {
				ss->set_height(p_restore);
			}
			return;
		}

		EditorUndoRedoManager *ur = EditorUndoRedoManager::get_singleton();
		if (p_id == 0) {
			ur->create_action(TTR("Change Cylinder Shape Radius"));
			ur->add_do_method(ss.ptr(), "set_radius", ss->get_radius());
			ur->add_undo_method(ss.ptr(), "set_radius", p_restore);
		} else {
			ur->create_action(
					///

					////////
					TTR("Change Cylinder Shape Height"));
			ur->add_do_method(ss.ptr(), "set_height", ss->get_height());
			ur->add_undo_method(ss.ptr(), "set_height", p_restore);
		}

		ur->commit_action();
	}

	if (Object::cast_to<SeparationRayShape3D>(*s)) {
		Ref<SeparationRayShape3D> ss = s;
		if (p_cancel) {
			ss->set_length(p_restore);
			return;
		}

		EditorUndoRedoManager *ur = EditorUndoRedoManager::get_singleton();
		ur->create_action(TTR("Change Separation Ray Shape Length"));
		ur->add_do_method(ss.ptr(), "set_length", ss->get_length());
		ur->add_undo_method(ss.ptr(), "set_length", p_restore);
		ur->commit_action();
	}
}

void CollisionShape3DGizmoPlugin::redraw(EditorNode3DGizmo *p_gizmo) {
	CollisionShape3D *cs = Object::cast_to<CollisionShape3D>(p_gizmo->get_node_3d());

	p_gizmo->clear();

	Ref<Shape3D> s = cs->get_shape();
	if (s.is_null()) {
		return;
	}

	const Ref<Material> material =
			get_material(!cs->is_disabled() ? "shape_material" : "shape_material_disabled", p_gizmo);
	Ref<Material> handles_material = get_material("handles");

	if (Object::cast_to<SphereShape3D>(*s)) {
		Ref<SphereShape3D> sp = s;
		float r = sp->get_radius();

		Vector<Vector3> points;

		for (int i = 0; i <= 360; i++) {
			float ra = Math::deg_to_rad((float)i);
			float rb = Math::deg_to_rad((float)i + 1);
			Point2 a = Vector2(Math::sin(ra), Math::cos(ra)) * r;
			Point2 b = Vector2(Math::sin(rb), Math::cos(rb)) * r;

			points.push_back(Vector3(a.x, 0, a.y));
			points.push_back(Vector3(b.x, 0, b.y));
			points.push_back(Vector3(0, a.x, a.y));
			points.push_back(Vector3(0, b.x, b.y));
			points.push_back(Vector3(a.x, a.y, 0));
			points.push_back(Vector3(b.x, b.y, 0));
		}

		Vector<Vector3> collision_segments;

		for (int i = 0; i < 64; i++) {
			float ra = i * (Math_TAU / 64.0);
			float rb = (i + 1) * (Math_TAU / 64.0);
			Point2 a = Vector2(Math::sin(ra), Math::cos(ra)) * r;
			Point2 b = Vector2(Math::sin(rb), Math::cos(rb)) * r;

			collision_segments.push_back(Vector3(a.x, 0, a.y));
			collision_segments.push_back(Vector3(b.x, 0, b.y));
			collision_segments.push_back(Vector3(0, a.x, a.y));
			collision_segments.push_back(Vector3(0, b.x, b.y));
			collision_segments.push_back(Vector3(a.x, a.y, 0));
			collision_segments.push_back(Vector3(b.x, b.y, 0));
		}

		p_gizmo->add_lines(points, material);
		p_gizmo->add_collision_segments(collision_segments);
		Vector<Vector3> handles;
		handles.push_back(Vector3(r, 0, 0));
		p_gizmo->add_handles(handles, handles_material);
	}

	if (Object::cast_to<BoxShape3D>(*s)) {
		Ref<BoxShape3D> bs = s;
		Vector<Vector3> lines;
		AABB aabb;
		aabb.position = -bs->get_size() / 2;
		aabb.size = bs->get_size();

		for (int i = 0; i < 12; i++) {
			Vector3 a, b;
			aabb.get_edge(i, a, b);
			lines.push_back(a);
			lines.push_back(b);
		}

		Vector<Vector3> handles;

		for (int i = 0; i < 3; i++) {
			Vector3 ax;
			ax[i] = bs->get_size()[i] / 2;
			handles.push_back(ax);
		}

		p_gizmo->add_lines(lines, material);
		p_gizmo->add_collision_segments(lines);
		p_gizmo->add_handles(handles, handles_material);
	}

	if (Object::cast_to<CapsuleShape3D>(*s)) {
		Ref<CapsuleShape3D> cs2 = s;
		float radius = cs2->get_radius();
		float height = cs2->get_height();

		Vector<Vector3> points;

		Vector3 d(0, height * 0.5 - radius, 0);
		for (int i = 0; i < 360; i++) {
			float ra = Math::deg_to_rad((float)i);
			float rb = Math::deg_to_rad((float)i + 1);
			Point2 a = Vector2(Math::sin(ra), Math::cos(ra)) * radius;
			Point2 b = Vector2(Math::sin(rb), Math::cos(rb)) * radius;

			points.push_back(Vector3(a.x, 0, a.y) + d);
			points.push_back(Vector3(b.x, 0, b.y) + d);

			points.push_back(Vector3(a.x, 0, a.y) - d);
			points.push_back(Vector3(b.x, 0, b.y) - d);

			if (i % 90 == 0) {
				points.push_back(Vector3(a.x, 0, a.y) + d);
				points.push_back(Vector3(a.x, 0, a.y) - d);
			}

			Vector3 dud = i < 180 ? d : -d;

			points.push_back(Vector3(0, a.x, a.y) + dud);
			points.push_back(Vector3(0, b.x, b.y) + dud);
			points.push_back(Vector3(a.y, a.x, 0) + dud);
			points.push_back(Vector3(b.y, b.x, 0) + dud);
		}

		p_gizmo->add_lines(points, material);

		Vector<Vector3> collision_segments;

		for (int i = 0; i < 64; i++) {
			float ra = i * (Math_TAU / 64.0);
			float rb = (i + 1) * (Math_TAU / 64.0);
			Point2 a = Vector2(Math::sin(ra), Math::cos(ra)) * radius;
			Point2 b = Vector2(Math::sin(rb), Math::cos(rb)) * radius;

			collision_segments.push_back(Vector3(a.x, 0, a.y) + d);
			collision_segments.push_back(Vector3(b.x, 0, b.y) + d);

			collision_segments.push_back(Vector3(a.x, 0, a.y) - d);
			collision_segments.push_back(Vector3(b.x, 0, b.y) - d);

			if (i % 16 == 0) {
				collision_segments.push_back(Vector3(a.x, 0, a.y) + d);
				collision_segments.push_back(Vector3(a.x, 0, a.y) - d);
			}

			Vector3 dud = i < 32 ? d : -d;

			collision_segments.push_back(Vector3(0, a.x, a.y) + dud);
			collision_segments.push_back(Vector3(0, b.x, b.y) + dud);
			collision_segments.push_back(Vector3(a.y, a.x, 0) + dud);
			collision_segments.push_back(Vector3(b.y, b.x, 0) + dud);
		}

		p_gizmo->add_collision_segments(collision_segments);

		Vector<Vector3> handles = {
			Vector3(cs2->get_radius(), 0, 0),
			Vector3(0, cs2->get_height() * 0.5, 0)
		};
		p_gizmo->add_handles(handles, handles_material);
	}

	if (Object::cast_to<CylinderShape3D>(*s)) {
		Ref<CylinderShape3D> cs2 = s;
		float radius = cs2->get_radius();
		float height = cs2->get_height();

		Vector<Vector3> points;

		Vector3 d(0, height * 0.5, 0);
		for (int i = 0; i < 360; i++) {
			float ra = Math::deg_to_rad((float)i);
			float rb = Math::deg_to_rad((float)i + 1);
			Point2 a = Vector2(Math::sin(ra), Math::cos(ra)) * radius;
			Point2 b = Vector2(Math::sin(rb), Math::cos(rb)) * radius;

			points.push_back(Vector3(a.x, 0, a.y) + d);
			points.push_back(Vector3(b.x, 0, b.y) + d);

			points.push_back(Vector3(a.x, 0, a.y) - d);
			points.push_back(Vector3(b.x, 0, b.y) - d);

			if (i % 90 == 0) {
				points.push_back(Vector3(a.x, 0, a.y) + d);
				points.push_back(Vector3(a.x, 0, a.y) - d);
			}
		}

		p_gizmo->add_lines(points, material);

		Vector<Vector3> collision_segments;

		for (int i = 0; i < 64; i++) {
			float ra = i * (Math_TAU / 64.0);
			float rb = (i + 1) * (Math_TAU / 64.0);
			Point2 a = Vector2(Math::sin(ra), Math::cos(ra)) * radius;
			Point2 b = Vector2(Math::sin(rb), Math::cos(rb)) * radius;

			collision_segments.push_back(Vector3(a.x, 0, a.y) + d);
			collision_segments.push_back(Vector3(b.x, 0, b.y) + d);

			collision_segments.push_back(Vector3(a.x, 0, a.y) - d);
			collision_segments.push_back(Vector3(b.x, 0, b.y) - d);

			if (i % 16 == 0) {
				collision_segments.push_back(Vector3(a.x, 0, a.y) + d);
				collision_segments.push_back(Vector3(a.x, 0, a.y) - d);
			}
		}

		p_gizmo->add_collision_segments(collision_segments);

		Vector<Vector3> handles = {
			Vector3(cs2->get_radius(), 0, 0),
			Vector3(0, cs2->get_height() * 0.5, 0)
		};
		p_gizmo->add_handles(handles, handles_material);
	}

	if (Object::cast_to<WorldBoundaryShape3D>(*s)) {
		Ref<WorldBoundaryShape3D> wbs = s;
		const Plane &p = wbs->get_plane();

		Vector3 n1 = p.get_any_perpendicular_normal();
		Vector3 n2 = p.normal.cross(n1).normalized();

		Vector3 pface[4] = {
			p.normal * p.d + n1 * 10.0 + n2 * 10.0,
			p.normal * p.d + n1 * 10.0 + n2 * -10.0,
			p.normal * p.d + n1 * -10.0 + n2 * -10.0,
			p.normal * p.d + n1 * -10.0 + n2 * 10.0,
		};

		Vector<Vector3> points = {
			pface[0],
			pface[1],
			pface[1],
			pface[2],
			pface[2],
			pface[3],
			pface[3],
			pface[0],
			p.normal * p.d,
			p.normal * p.d + p.normal * 3
		};

		p_gizmo->add_lines(points, material);
		p_gizmo->add_collision_segments(points);
	}

	if (Object::cast_to<ConvexPolygonShape3D>(*s)) {
		Vector<Vector3> points = Object::cast_to<ConvexPolygonShape3D>(*s)->get_points();

		if (points.size() > 3) {
			Vector<Vector3> varr = Variant(points);
			Geometry3D::MeshData md;
			Error err = ConvexHullComputer::convex_hull(varr, md);
			if (err == OK) {
				Vector<Vector3> points2;
				points2.resize(md.edges.size() * 2);
				for (uint32_t i = 0; i < md.edges.size(); i++) {
					points2.write[i * 2 + 0] = md.vertices[md.edges[i].vertex_a];
					points2.write[i * 2 + 1] = md.vertices[md.edges[i].vertex_b];
				}

				p_gizmo->add_lines(points2, material);
				p_gizmo->add_collision_segments(points2);
			}
		}
	}

	if (Object::cast_to<ConcavePolygonShape3D>(*s)) {
		Ref<ConcavePolygonShape3D> cs2 = s;
		Ref<ArrayMesh> mesh = cs2->get_debug_mesh();
		p_gizmo->add_mesh(mesh, material);
		p_gizmo->add_collision_segments(cs2->get_debug_mesh_lines());
	}

	if (Object::cast_to<SeparationRayShape3D>(*s)) {
		Ref<SeparationRayShape3D> rs = s;

		Vector<Vector3> points = {
			Vector3(),
			Vector3(0, 0, rs->get_length())
		};
		p_gizmo->add_lines(points, material);
		p_gizmo->add_collision_segments(points);
		Vector<Vector3> handles;
		handles.push_back(Vector3(0, 0, rs->get_length()));
		p_gizmo->add_handles(handles, handles_material);
	}

	if (Object::cast_to<HeightMapShape3D>(*s)) {
		Ref<HeightMapShape3D> hms = s;

		Ref<ArrayMesh> mesh = hms->get_debug_mesh();
		p_gizmo->add_mesh(mesh, material);
	}
}

/////

CollisionPolygon3DGizmoPlugin::CollisionPolygon3DGizmoPlugin() {
	const Color gizmo_color = EDITOR_GET("editors/3d_gizmos/gizmo_colors/shape");
	create_material("shape_material", gizmo_color);
	const float gizmo_value = gizmo_color.get_v();
	const Color gizmo_color_disabled = Color(gizmo_value, gizmo_value, gizmo_value, 0.65);
	create_material("shape_material_disabled", gizmo_color_disabled);
}

bool CollisionPolygon3DGizmoPlugin::has_gizmo(Node3D *p_spatial) {
	return Object::cast_to<CollisionPolygon3D>(p_spatial) != nullptr;
}

String CollisionPolygon3DGizmoPlugin::get_gizmo_name() const {
	return "CollisionPolygon3D";
}

int CollisionPolygon3DGizmoPlugin::get_priority() const {
	return -1;
}

void CollisionPolygon3DGizmoPlugin::redraw(EditorNode3DGizmo *p_gizmo) {
	CollisionPolygon3D *polygon = Object::cast_to<CollisionPolygon3D>(p_gizmo->get_node_3d());

	p_gizmo->clear();

	Vector<Vector2> points = polygon->get_polygon();
	float depth = polygon->get_depth() * 0.5;

	Vector<Vector3> lines;
	for (int i = 0; i < points.size(); i++) {
		int n = (i + 1) % points.size();
		lines.push_back(Vector3(points[i].x, points[i].y, depth));
		lines.push_back(Vector3(points[n].x, points[n].y, depth));
		lines.push_back(Vector3(points[i].x, points[i].y, -depth));
		lines.push_back(Vector3(points[n].x, points[n].y, -depth));
		lines.push_back(Vector3(points[i].x, points[i].y, depth));
		lines.push_back(Vector3(points[i].x, points[i].y, -depth));
	}

	const Ref<Material> material =
			get_material(!polygon->is_disabled() ? "shape_material" : "shape_material_disabled", p_gizmo);

	p_gizmo->add_lines(lines, material);
	p_gizmo->add_collision_segments(lines);
}

////

NavigationRegion3DGizmoPlugin::NavigationRegion3DGizmoPlugin() {
	create_material("face_material", NavigationServer3D::get_singleton()->get_debug_navigation_geometry_face_color(), false, false, true);
	create_material("face_material_disabled", NavigationServer3D::get_singleton()->get_debug_navigation_geometry_face_disabled_color(), false, false, true);
	create_material("edge_material", NavigationServer3D::get_singleton()->get_debug_navigation_geometry_edge_color());
	create_material("edge_material_disabled", NavigationServer3D::get_singleton()->get_debug_navigation_geometry_edge_disabled_color());

	Color baking_aabb_material_color = Color(0.8, 0.5, 0.7);
	baking_aabb_material_color.a = 0.1;
	create_material("baking_aabb_material", baking_aabb_material_color);
}

bool NavigationRegion3DGizmoPlugin::has_gizmo(Node3D *p_spatial) {
	return Object::cast_to<NavigationRegion3D>(p_spatial) != nullptr;
}

String NavigationRegion3DGizmoPlugin::get_gizmo_name() const {
	return "NavigationRegion3D";
}

int NavigationRegion3DGizmoPlugin::get_priority() const {
	return -1;
}

void NavigationRegion3DGizmoPlugin::redraw(EditorNode3DGizmo *p_gizmo) {
	NavigationRegion3D *navigationregion = Object::cast_to<NavigationRegion3D>(p_gizmo->get_node_3d());

	p_gizmo->clear();
	Ref<NavigationMesh> navigationmesh = navigationregion->get_navigation_mesh();
	if (navigationmesh.is_null()) {
		return;
	}

	AABB baking_aabb = navigationmesh->get_filter_baking_aabb();
	if (baking_aabb.has_volume()) {
		Vector3 baking_aabb_offset = navigationmesh->get_filter_baking_aabb_offset();

		if (p_gizmo->is_selected()) {
			Ref<Material> material = get_material("baking_aabb_material", p_gizmo);
			p_gizmo->add_solid_box(material, baking_aabb.get_size(), baking_aabb.get_center() + baking_aabb_offset);
		}
	}

	Vector<Vector3> vertices = navigationmesh->get_vertices();
	const Vector3 *vr = vertices.ptr();
	List<Face3> faces;
	for (int i = 0; i < navigationmesh->get_polygon_count(); i++) {
		Vector<int> p = navigationmesh->get_polygon(i);

		for (int j = 2; j < p.size(); j++) {
			Face3 f;
			f.vertex[0] = vr[p[0]];
			f.vertex[1] = vr[p[j - 1]];
			f.vertex[2] = vr[p[j]];

			faces.push_back(f);
		}
	}

	if (faces.is_empty()) {
		return;
	}

	HashMap<_EdgeKey, bool, _EdgeKey> edge_map;
	Vector<Vector3> tmeshfaces;
	tmeshfaces.resize(faces.size() * 3);

	{
		Vector3 *tw = tmeshfaces.ptrw();
		int tidx = 0;

		for (const Face3 &f : faces) {
			for (int j = 0; j < 3; j++) {
				tw[tidx++] = f.vertex[j];
				_EdgeKey ek;
				ek.from = f.vertex[j].snapped(Vector3(CMP_EPSILON, CMP_EPSILON, CMP_EPSILON));
				ek.to = f.vertex[(j + 1) % 3].snapped(Vector3(CMP_EPSILON, CMP_EPSILON, CMP_EPSILON));
				if (ek.from < ek.to) {
					SWAP(ek.from, ek.to);
				}

				HashMap<_EdgeKey, bool, _EdgeKey>::Iterator F = edge_map.find(ek);

				if (F) {
					F->value = false;

				} else {
					edge_map[ek] = true;
				}
			}
		}
	}
	Vector<Vector3> lines;

	for (const KeyValue<_EdgeKey, bool> &E : edge_map) {
		if (E.value) {
			lines.push_back(E.key.from);
			lines.push_back(E.key.to);
		}
	}

	Ref<TriangleMesh> tmesh = memnew(TriangleMesh);
	tmesh->create(tmeshfaces);

	p_gizmo->add_collision_triangles(tmesh);
	p_gizmo->add_collision_segments(lines);

	Ref<ArrayMesh> debug_mesh = Ref<ArrayMesh>(memnew(ArrayMesh));
	int polygon_count = navigationmesh->get_polygon_count();

	// build geometry face surface
	Vector<Vector3> face_vertex_array;
	face_vertex_array.resize(polygon_count * 3);

	for (int i = 0; i < polygon_count; i++) {
		Vector<int> polygon = navigationmesh->get_polygon(i);

		face_vertex_array.push_back(vertices[polygon[0]]);
		face_vertex_array.push_back(vertices[polygon[1]]);
		face_vertex_array.push_back(vertices[polygon[2]]);
	}

	Array face_mesh_array;
	face_mesh_array.resize(Mesh::ARRAY_MAX);
	face_mesh_array[Mesh::ARRAY_VERTEX] = face_vertex_array;

	// if enabled add vertex colors to colorize each face individually
	RandomPCG rand;
	bool enabled_geometry_face_random_color = NavigationServer3D::get_singleton()->get_debug_navigation_enable_geometry_face_random_color();
	if (enabled_geometry_face_random_color) {
		Color debug_navigation_geometry_face_color = NavigationServer3D::get_singleton()->get_debug_navigation_geometry_face_color();
		Color polygon_color = debug_navigation_geometry_face_color;

		Vector<Color> face_color_array;
		face_color_array.resize(polygon_count * 3);

		for (int i = 0; i < polygon_count; i++) {
			// Generate the polygon color, slightly randomly modified from the settings one.
			polygon_color.set_hsv(debug_navigation_geometry_face_color.get_h() + rand.random(-1.0, 1.0) * 0.1, debug_navigation_geometry_face_color.get_s(), debug_navigation_geometry_face_color.get_v() + rand.random(-1.0, 1.0) * 0.2);
			polygon_color.a = debug_navigation_geometry_face_color.a;

			Vector<int> polygon = navigationmesh->get_polygon(i);

			face_color_array.push_back(polygon_color);
			face_color_array.push_back(polygon_color);
			face_color_array.push_back(polygon_color);
		}
		face_mesh_array[Mesh::ARRAY_COLOR] = face_color_array;
	}

	debug_mesh->add_surface_from_arrays(Mesh::PRIMITIVE_TRIANGLES, face_mesh_array);
	p_gizmo->add_mesh(debug_mesh, navigationregion->is_enabled() ? get_material("face_material", p_gizmo) : get_material("face_material_disabled", p_gizmo));

	// if enabled build geometry edge line surface
	bool enabled_edge_lines = NavigationServer3D::get_singleton()->get_debug_navigation_enable_edge_lines();
	if (enabled_edge_lines) {
		Vector<Vector3> line_vertex_array;
		line_vertex_array.resize(polygon_count * 6);

		for (int i = 0; i < polygon_count; i++) {
			Vector<int> polygon = navigationmesh->get_polygon(i);

			line_vertex_array.push_back(vertices[polygon[0]]);
			line_vertex_array.push_back(vertices[polygon[1]]);
			line_vertex_array.push_back(vertices[polygon[1]]);
			line_vertex_array.push_back(vertices[polygon[2]]);
			line_vertex_array.push_back(vertices[polygon[2]]);
			line_vertex_array.push_back(vertices[polygon[0]]);
		}

		p_gizmo->add_lines(line_vertex_array, navigationregion->is_enabled() ? get_material("edge_material", p_gizmo) : get_material("edge_material_disabled", p_gizmo));
	}
}

////

NavigationLink3DGizmoPlugin::NavigationLink3DGizmoPlugin() {
	create_material("navigation_link_material", NavigationServer3D::get_singleton()->get_debug_navigation_link_connection_color());
	create_material("navigation_link_material_disabled", NavigationServer3D::get_singleton()->get_debug_navigation_link_connection_disabled_color());
	create_handle_material("handles");
}

bool NavigationLink3DGizmoPlugin::has_gizmo(Node3D *p_spatial) {
	return Object::cast_to<NavigationLink3D>(p_spatial) != nullptr;
}

String NavigationLink3DGizmoPlugin::get_gizmo_name() const {
	return "NavigationLink3D";
}

int NavigationLink3DGizmoPlugin::get_priority() const {
	return -1;
}

void NavigationLink3DGizmoPlugin::redraw(EditorNode3DGizmo *p_gizmo) {
	NavigationLink3D *link = Object::cast_to<NavigationLink3D>(p_gizmo->get_node_3d());

	RID nav_map = link->get_world_3d()->get_navigation_map();
	real_t search_radius = NavigationServer3D::get_singleton()->map_get_link_connection_radius(nav_map);
	Vector3 up_vector = NavigationServer3D::get_singleton()->map_get_up(nav_map);
	Vector3::Axis up_axis = up_vector.max_axis_index();

	Vector3 start_position = link->get_start_position();
	Vector3 end_position = link->get_end_position();

	Ref<Material> link_material = get_material("navigation_link_material", p_gizmo);
	Ref<Material> link_material_disabled = get_material("navigation_link_material_disabled", p_gizmo);
	Ref<Material> handles_material = get_material("handles");

	p_gizmo->clear();

	// Draw line between the points.
	Vector<Vector3> lines;
	lines.append(start_position);
	lines.append(end_position);

	// Draw start position search radius
	for (int i = 0; i < 30; i++) {
		// Create a circle
		const float ra = Math::deg_to_rad((float)(i * 12));
		const float rb = Math::deg_to_rad((float)((i + 1) * 12));
		const Point2 a = Vector2(Math::sin(ra), Math::cos(ra)) * search_radius;
		const Point2 b = Vector2(Math::sin(rb), Math::cos(rb)) * search_radius;

		// Draw axis-aligned circle
		switch (up_axis) {
			case Vector3::AXIS_X:
				lines.append(start_position + Vector3(0, a.x, a.y));
				lines.append(start_position + Vector3(0, b.x, b.y));
				break;
			case Vector3::AXIS_Y:
				lines.append(start_position + Vector3(a.x, 0, a.y));
				lines.append(start_position + Vector3(b.x, 0, b.y));
				break;
			case Vector3::AXIS_Z:
				lines.append(start_position + Vector3(a.x, a.y, 0));
				lines.append(start_position + Vector3(b.x, b.y, 0));
				break;
		}
	}

	// Draw end position search radius
	for (int i = 0; i < 30; i++) {
		// Create a circle
		const float ra = Math::deg_to_rad((float)(i * 12));
		const float rb = Math::deg_to_rad((float)((i + 1) * 12));
		const Point2 a = Vector2(Math::sin(ra), Math::cos(ra)) * search_radius;
		const Point2 b = Vector2(Math::sin(rb), Math::cos(rb)) * search_radius;

		// Draw axis-aligned circle
		switch (up_axis) {
			case Vector3::AXIS_X:
				lines.append(end_position + Vector3(0, a.x, a.y));
				lines.append(end_position + Vector3(0, b.x, b.y));
				break;
			case Vector3::AXIS_Y:
				lines.append(end_position + Vector3(a.x, 0, a.y));
				lines.append(end_position + Vector3(b.x, 0, b.y));
				break;
			case Vector3::AXIS_Z:
				lines.append(end_position + Vector3(a.x, a.y, 0));
				lines.append(end_position + Vector3(b.x, b.y, 0));
				break;
		}
	}

	p_gizmo->add_lines(lines, link->is_enabled() ? link_material : link_material_disabled);
	p_gizmo->add_collision_segments(lines);

	Vector<Vector3> handles;
	handles.append(start_position);
	handles.append(end_position);
	p_gizmo->add_handles(handles, handles_material);
}

String NavigationLink3DGizmoPlugin::get_handle_name(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary) const {
	return p_id == 0 ? TTR("Start Location") : TTR("End Location");
}

Variant NavigationLink3DGizmoPlugin::get_handle_value(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary) const {
	NavigationLink3D *link = Object::cast_to<NavigationLink3D>(p_gizmo->get_node_3d());
	return p_id == 0 ? link->get_start_position() : link->get_end_position();
}

void NavigationLink3DGizmoPlugin::set_handle(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary, Camera3D *p_camera, const Point2 &p_point) {
	NavigationLink3D *link = Object::cast_to<NavigationLink3D>(p_gizmo->get_node_3d());

	Transform3D gt = link->get_global_transform();
	Transform3D gi = gt.affine_inverse();

	Transform3D ct = p_camera->get_global_transform();
	Vector3 cam_dir = ct.basis.get_column(Vector3::AXIS_Z);

	Vector3 ray_from = p_camera->project_ray_origin(p_point);
	Vector3 ray_dir = p_camera->project_ray_normal(p_point);

	Vector3 position = p_id == 0 ? link->get_start_position() : link->get_end_position();
	Plane move_plane = Plane(cam_dir, gt.xform(position));

	Vector3 intersection;
	if (!move_plane.intersects_ray(ray_from, ray_dir, &intersection)) {
		return;
	}

	if (Node3DEditor::get_singleton()->is_snap_enabled()) {
		double snap = Node3DEditor::get_singleton()->get_translate_snap();
		intersection.snap(Vector3(snap, snap, snap));
	}

	position = gi.xform(intersection);
	if (p_id == 0) {
		link->set_start_position(position);
	} else if (p_id == 1) {
		link->set_end_position(position);
	}
}

void NavigationLink3DGizmoPlugin::commit_handle(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary, const Variant &p_restore, bool p_cancel) {
	NavigationLink3D *link = Object::cast_to<NavigationLink3D>(p_gizmo->get_node_3d());

	if (p_cancel) {
		if (p_id == 0) {
			link->set_start_position(p_restore);
		} else {
			link->set_end_position(p_restore);
		}
		return;
	}

	EditorUndoRedoManager *ur = EditorUndoRedoManager::get_singleton();
	if (p_id == 0) {
		ur->create_action(TTR("Change Start Position"));
		ur->add_do_method(link, "set_start_position", link->get_start_position());
		ur->add_undo_method(link, "set_start_position", p_restore);
	} else {
		ur->create_action(TTR("Change End Position"));
		ur->add_do_method(link, "set_end_position", link->get_end_position());
		ur->add_undo_method(link, "set_end_position", p_restore);
	}

	ur->commit_action();
}

//////

#define BODY_A_RADIUS 0.25
#define BODY_B_RADIUS 0.27

Basis JointGizmosDrawer::look_body(const Transform3D &p_joint_transform, const Transform3D &p_body_transform) {
	const Vector3 &p_eye(p_joint_transform.origin);
	const Vector3 &p_target(p_body_transform.origin);

	Vector3 v_x, v_y, v_z;

	// Look the body with X
	v_x = p_target - p_eye;
	v_x.normalize();

	v_z = v_x.cross(Vector3(0, 1, 0));
	v_z.normalize();

	v_y = v_z.cross(v_x);
	v_y.normalize();

	Basis base;
	base.set_columns(v_x, v_y, v_z);

	// Absorb current joint transform
	base = p_joint_transform.basis.inverse() * base;

	return base;
}

Basis JointGizmosDrawer::look_body_toward(Vector3::Axis p_axis, const Transform3D &joint_transform, const Transform3D &body_transform) {
	switch (p_axis) {
		case Vector3::AXIS_X:
			return look_body_toward_x(joint_transform, body_transform);
		case Vector3::AXIS_Y:
			return look_body_toward_y(joint_transform, body_transform);
		case Vector3::AXIS_Z:
			return look_body_toward_z(joint_transform, body_transform);
		default:
			return Basis();
	}
}

Basis JointGizmosDrawer::look_body_toward_x(const Transform3D &p_joint_transform, const Transform3D &p_body_transform) {
	const Vector3 &p_eye(p_joint_transform.origin);
	const Vector3 &p_target(p_body_transform.origin);

	const Vector3 p_front(p_joint_transform.basis.get_column(0));

	Vector3 v_x, v_y, v_z;

	// Look the body with X
	v_x = p_target - p_eye;
	v_x.normalize();

	v_y = p_front.cross(v_x);
	v_y.normalize();

	v_z = v_y.cross(p_front);
	v_z.normalize();

	// Clamp X to FRONT axis
	v_x = p_front;
	v_x.normalize();

	Basis base;
	base.set_columns(v_x, v_y, v_z);

	// Absorb current joint transform
	base = p_joint_transform.basis.inverse() * base;

	return base;
}

Basis JointGizmosDrawer::look_body_toward_y(const Transform3D &p_joint_transform, const Transform3D &p_body_transform) {
	const Vector3 &p_eye(p_joint_transform.origin);
	const Vector3 &p_target(p_body_transform.origin);

	const Vector3 p_up(p_joint_transform.basis.get_column(1));

	Vector3 v_x, v_y, v_z;

	// Look the body with X
	v_x = p_target - p_eye;
	v_x.normalize();

	v_z = v_x.cross(p_up);
	v_z.normalize();

	v_x = p_up.cross(v_z);
	v_x.normalize();

	// Clamp Y to UP axis
	v_y = p_up;
	v_y.normalize();

	Basis base;
	base.set_columns(v_x, v_y, v_z);

	// Absorb current joint transform
	base = p_joint_transform.basis.inverse() * base;

	return base;
}

Basis JointGizmosDrawer::look_body_toward_z(const Transform3D &p_joint_transform, const Transform3D &p_body_transform) {
	const Vector3 &p_eye(p_joint_transform.origin);
	const Vector3 &p_target(p_body_transform.origin);

	const Vector3 p_lateral(p_joint_transform.basis.get_column(2));

	Vector3 v_x, v_y, v_z;

	// Look the body with X
	v_x = p_target - p_eye;
	v_x.normalize();

	v_z = p_lateral;
	v_z.normalize();

	v_y = v_z.cross(v_x);
	v_y.normalize();

	// Clamp X to Z axis
	v_x = v_y.cross(v_z);
	v_x.normalize();

	Basis base;
	base.set_columns(v_x, v_y, v_z);

	// Absorb current joint transform
	base = p_joint_transform.basis.inverse() * base;

	return base;
}

void JointGizmosDrawer::draw_circle(Vector3::Axis p_axis, real_t p_radius, const Transform3D &p_offset, const Basis &p_base, real_t p_limit_lower, real_t p_limit_upper, Vector<Vector3> &r_points, bool p_inverse) {
	if (p_limit_lower == p_limit_upper) {
		r_points.push_back(p_offset.translated_local(Vector3()).origin);
		r_points.push_back(p_offset.translated_local(p_base.xform(Vector3(0.5, 0, 0))).origin);

	} else {
		if (p_limit_lower > p_limit_upper) {
			p_limit_lower = -Math_PI;
			p_limit_upper = Math_PI;
		}

		const int points = 32;

		for (int i = 0; i < points; i++) {
			real_t s = p_limit_lower + i * (p_limit_upper - p_limit_lower) / points;
			real_t n = p_limit_lower + (i + 1) * (p_limit_upper - p_limit_lower) / points;

			Vector3 from;
			Vector3 to;
			switch (p_axis) {
				case Vector3::AXIS_X:
					if (p_inverse) {
						from = p_base.xform(Vector3(0, Math::sin(s), Math::cos(s))) * p_radius;
						to = p_base.xform(Vector3(0, Math::sin(n), Math::cos(n))) * p_radius;
					} else {
						from = p_base.xform(Vector3(0, -Math::sin(s), Math::cos(s))) * p_radius;
						to = p_base.xform(Vector3(0, -Math::sin(n), Math::cos(n))) * p_radius;
					}
					break;
				case Vector3::AXIS_Y:
					if (p_inverse) {
						from = p_base.xform(Vector3(Math::cos(s), 0, -Math::sin(s))) * p_radius;
						to = p_base.xform(Vector3(Math::cos(n), 0, -Math::sin(n))) * p_radius;
					} else {
						from = p_base.xform(Vector3(Math::cos(s), 0, Math::sin(s))) * p_radius;
						to = p_base.xform(Vector3(Math::cos(n), 0, Math::sin(n))) * p_radius;
					}
					break;
				case Vector3::AXIS_Z:
					from = p_base.xform(Vector3(Math::cos(s), Math::sin(s), 0)) * p_radius;
					to = p_base.xform(Vector3(Math::cos(n), Math::sin(n), 0)) * p_radius;
					break;
			}

			if (i == points - 1) {
				r_points.push_back(p_offset.translated_local(to).origin);
				r_points.push_back(p_offset.translated_local(Vector3()).origin);
			}
			if (i == 0) {
				r_points.push_back(p_offset.translated_local(from).origin);
				r_points.push_back(p_offset.translated_local(Vector3()).origin);
			}

			r_points.push_back(p_offset.translated_local(from).origin);
			r_points.push_back(p_offset.translated_local(to).origin);
		}

		r_points.push_back(p_offset.translated_local(Vector3(0, p_radius * 1.5, 0)).origin);
		r_points.push_back(p_offset.translated_local(Vector3()).origin);
	}
}

void JointGizmosDrawer::draw_cone(const Transform3D &p_offset, const Basis &p_base, real_t p_swing, real_t p_twist, Vector<Vector3> &r_points) {
	float r = 1.0;
	float w = r * Math::sin(p_swing);
	float d = r * Math::cos(p_swing);

	//swing
	for (int i = 0; i < 360; i += 10) {
		float ra = Math::deg_to_rad((float)i);
		float rb = Math::deg_to_rad((float)i + 10);
		Point2 a = Vector2(Math::sin(ra), Math::cos(ra)) * w;
		Point2 b = Vector2(Math::sin(rb), Math::cos(rb)) * w;

		r_points.push_back(p_offset.translated_local(p_base.xform(Vector3(d, a.x, a.y))).origin);
		r_points.push_back(p_offset.translated_local(p_base.xform(Vector3(d, b.x, b.y))).origin);

		if (i % 90 == 0) {
			r_points.push_back(p_offset.translated_local(p_base.xform(Vector3(d, a.x, a.y))).origin);
			r_points.push_back(p_offset.translated_local(p_base.xform(Vector3())).origin);
		}
	}

	r_points.push_back(p_offset.translated_local(p_base.xform(Vector3())).origin);
	r_points.push_back(p_offset.translated_local(p_base.xform(Vector3(1, 0, 0))).origin);

	/// Twist
	float ts = Math::rad_to_deg(p_twist);
	ts = MIN(ts, 720);

	for (int i = 0; i < int(ts); i += 5) {
		float ra = Math::deg_to_rad((float)i);
		float rb = Math::deg_to_rad((float)i + 5);
		float c = i / 720.0;
		float cn = (i + 5) / 720.0;
		Point2 a = Vector2(Math::sin(ra), Math::cos(ra)) * w * c;
		Point2 b = Vector2(Math::sin(rb), Math::cos(rb)) * w * cn;

		r_points.push_back(p_offset.translated_local(p_base.xform(Vector3(c, a.x, a.y))).origin);
		r_points.push_back(p_offset.translated_local(p_base.xform(Vector3(cn, b.x, b.y))).origin);
	}
}

////

Joint3DGizmoPlugin::Joint3DGizmoPlugin() {
	create_material("joint_material", EDITOR_GET("editors/3d_gizmos/gizmo_colors/joint"));
	create_material("joint_body_a_material", EDITOR_DEF("editors/3d_gizmos/gizmo_colors/joint_body_a", Color(0.6, 0.8, 1)));
	create_material("joint_body_b_material", EDITOR_DEF("editors/3d_gizmos/gizmo_colors/joint_body_b", Color(0.6, 0.9, 1)));

	update_timer = memnew(Timer);
	update_timer->set_name("JointGizmoUpdateTimer");
	update_timer->set_wait_time(1.0 / 120.0);
	update_timer->connect("timeout", callable_mp(this, &Joint3DGizmoPlugin::incremental_update_gizmos));
	update_timer->set_autostart(true);
	EditorNode::get_singleton()->call_deferred(SNAME("add_child"), update_timer);
}

void Joint3DGizmoPlugin::incremental_update_gizmos() {
	if (!current_gizmos.is_empty()) {
		update_idx++;
		update_idx = update_idx % current_gizmos.size();
		redraw(current_gizmos[update_idx]);
	}
}

bool Joint3DGizmoPlugin::has_gizmo(Node3D *p_spatial) {
	return Object::cast_to<Joint3D>(p_spatial) != nullptr;
}

String Joint3DGizmoPlugin::get_gizmo_name() const {
	return "Joint3D";
}

int Joint3DGizmoPlugin::get_priority() const {
	return -1;
}

void Joint3DGizmoPlugin::redraw(EditorNode3DGizmo *p_gizmo) {
	Joint3D *joint = Object::cast_to<Joint3D>(p_gizmo->get_node_3d());

	p_gizmo->clear();

	Node3D *node_body_a = nullptr;
	if (!joint->get_node_a().is_empty()) {
		node_body_a = Object::cast_to<Node3D>(joint->get_node(joint->get_node_a()));
	}

	Node3D *node_body_b = nullptr;
	if (!joint->get_node_b().is_empty()) {
		node_body_b = Object::cast_to<Node3D>(joint->get_node(joint->get_node_b()));
	}

	if (!node_body_a && !node_body_b) {
		return;
	}

	Ref<Material> common_material = get_material("joint_material", p_gizmo);
	Ref<Material> body_a_material = get_material("joint_body_a_material", p_gizmo);
	Ref<Material> body_b_material = get_material("joint_body_b_material", p_gizmo);

	Vector<Vector3> points;
	Vector<Vector3> body_a_points;
	Vector<Vector3> body_b_points;

	if (Object::cast_to<PinJoint3D>(joint)) {
		CreatePinJointGizmo(Transform3D(), points);
		p_gizmo->add_collision_segments(points);
		p_gizmo->add_lines(points, common_material);
	}

	HingeJoint3D *hinge = Object::cast_to<HingeJoint3D>(joint);
	if (hinge) {
		CreateHingeJointGizmo(
				Transform3D(),
				hinge->get_global_transform(),
				node_body_a ? node_body_a->get_global_transform() : Transform3D(),
				node_body_b ? node_body_b->get_global_transform() : Transform3D(),
				hinge->get_param(HingeJoint3D::PARAM_LIMIT_LOWER),
				hinge->get_param(HingeJoint3D::PARAM_LIMIT_UPPER),
				hinge->get_flag(HingeJoint3D::FLAG_USE_LIMIT),
				points,
				node_body_a ? &body_a_points : nullptr,
				node_body_b ? &body_b_points : nullptr);

		p_gizmo->add_collision_segments(points);
		p_gizmo->add_collision_segments(body_a_points);
		p_gizmo->add_collision_segments(body_b_points);

		p_gizmo->add_lines(points, common_material);
		p_gizmo->add_lines(body_a_points, body_a_material);
		p_gizmo->add_lines(body_b_points, body_b_material);
	}

	SliderJoint3D *slider = Object::cast_to<SliderJoint3D>(joint);
	if (slider) {
		CreateSliderJointGizmo(
				Transform3D(),
				slider->get_global_transform(),
				node_body_a ? node_body_a->get_global_transform() : Transform3D(),
				node_body_b ? node_body_b->get_global_transform() : Transform3D(),
				slider->get_param(SliderJoint3D::PARAM_ANGULAR_LIMIT_LOWER),
				slider->get_param(SliderJoint3D::PARAM_ANGULAR_LIMIT_UPPER),
				slider->get_param(SliderJoint3D::PARAM_LINEAR_LIMIT_LOWER),
				slider->get_param(SliderJoint3D::PARAM_LINEAR_LIMIT_UPPER),
				points,
				node_body_a ? &body_a_points : nullptr,
				node_body_b ? &body_b_points : nullptr);

		p_gizmo->add_collision_segments(points);
		p_gizmo->add_collision_segments(body_a_points);
		p_gizmo->add_collision_segments(body_b_points);

		p_gizmo->add_lines(points, common_material);
		p_gizmo->add_lines(body_a_points, body_a_material);
		p_gizmo->add_lines(body_b_points, body_b_material);
	}

	ConeTwistJoint3D *cone = Object::cast_to<ConeTwistJoint3D>(joint);
	if (cone) {
		CreateConeTwistJointGizmo(
				Transform3D(),
				cone->get_global_transform(),
				node_body_a ? node_body_a->get_global_transform() : Transform3D(),
				node_body_b ? node_body_b->get_global_transform() : Transform3D(),
				cone->get_param(ConeTwistJoint3D::PARAM_SWING_SPAN),
				cone->get_param(ConeTwistJoint3D::PARAM_TWIST_SPAN),
				node_body_a ? &body_a_points : nullptr,
				node_body_b ? &body_b_points : nullptr);

		p_gizmo->add_collision_segments(body_a_points);
		p_gizmo->add_collision_segments(body_b_points);

		p_gizmo->add_lines(body_a_points, body_a_material);
		p_gizmo->add_lines(body_b_points, body_b_material);
	}

	Generic6DOFJoint3D *gen = Object::cast_to<Generic6DOFJoint3D>(joint);
	if (gen) {
		CreateGeneric6DOFJointGizmo(
				Transform3D(),
				gen->get_global_transform(),
				node_body_a ? node_body_a->get_global_transform() : Transform3D(),
				node_body_b ? node_body_b->get_global_transform() : Transform3D(),

				gen->get_param_x(Generic6DOFJoint3D::PARAM_ANGULAR_LOWER_LIMIT),
				gen->get_param_x(Generic6DOFJoint3D::PARAM_ANGULAR_UPPER_LIMIT),
				gen->get_param_x(Generic6DOFJoint3D::PARAM_LINEAR_LOWER_LIMIT),
				gen->get_param_x(Generic6DOFJoint3D::PARAM_LINEAR_UPPER_LIMIT),
				gen->get_flag_x(Generic6DOFJoint3D::FLAG_ENABLE_ANGULAR_LIMIT),
				gen->get_flag_x(Generic6DOFJoint3D::FLAG_ENABLE_LINEAR_LIMIT),

				gen->get_param_y(Generic6DOFJoint3D::PARAM_ANGULAR_LOWER_LIMIT),
				gen->get_param_y(Generic6DOFJoint3D::PARAM_ANGULAR_UPPER_LIMIT),
				gen->get_param_y(Generic6DOFJoint3D::PARAM_LINEAR_LOWER_LIMIT),
				gen->get_param_y(Generic6DOFJoint3D::PARAM_LINEAR_UPPER_LIMIT),
				gen->get_flag_y(Generic6DOFJoint3D::FLAG_ENABLE_ANGULAR_LIMIT),
				gen->get_flag_y(Generic6DOFJoint3D::FLAG_ENABLE_LINEAR_LIMIT),

				gen->get_param_z(Generic6DOFJoint3D::PARAM_ANGULAR_LOWER_LIMIT),
				gen->get_param_z(Generic6DOFJoint3D::PARAM_ANGULAR_UPPER_LIMIT),
				gen->get_param_z(Generic6DOFJoint3D::PARAM_LINEAR_LOWER_LIMIT),
				gen->get_param_z(Generic6DOFJoint3D::PARAM_LINEAR_UPPER_LIMIT),
				gen->get_flag_z(Generic6DOFJoint3D::FLAG_ENABLE_ANGULAR_LIMIT),
				gen->get_flag_z(Generic6DOFJoint3D::FLAG_ENABLE_LINEAR_LIMIT),

				points,
				node_body_a ? &body_a_points : nullptr,
				node_body_a ? &body_b_points : nullptr);

		p_gizmo->add_collision_segments(points);
		p_gizmo->add_collision_segments(body_a_points);
		p_gizmo->add_collision_segments(body_b_points);

		p_gizmo->add_lines(points, common_material);
		p_gizmo->add_lines(body_a_points, body_a_material);
		p_gizmo->add_lines(body_b_points, body_b_material);
	}
}

void Joint3DGizmoPlugin::CreatePinJointGizmo(const Transform3D &p_offset, Vector<Vector3> &r_cursor_points) {
	float cs = 0.25;

	r_cursor_points.push_back(p_offset.translated_local(Vector3(+cs, 0, 0)).origin);
	r_cursor_points.push_back(p_offset.translated_local(Vector3(-cs, 0, 0)).origin);
	r_cursor_points.push_back(p_offset.translated_local(Vector3(0, +cs, 0)).origin);
	r_cursor_points.push_back(p_offset.translated_local(Vector3(0, -cs, 0)).origin);
	r_cursor_points.push_back(p_offset.translated_local(Vector3(0, 0, +cs)).origin);
	r_cursor_points.push_back(p_offset.translated_local(Vector3(0, 0, -cs)).origin);
}

void Joint3DGizmoPlugin::CreateHingeJointGizmo(const Transform3D &p_offset, const Transform3D &p_trs_joint, const Transform3D &p_trs_body_a, const Transform3D &p_trs_body_b, real_t p_limit_lower, real_t p_limit_upper, bool p_use_limit, Vector<Vector3> &r_common_points, Vector<Vector3> *r_body_a_points, Vector<Vector3> *r_body_b_points) {
	r_common_points.push_back(p_offset.translated_local(Vector3(0, 0, 0.5)).origin);
	r_common_points.push_back(p_offset.translated_local(Vector3(0, 0, -0.5)).origin);

	if (!p_use_limit) {
		p_limit_upper = -1;
		p_limit_lower = 0;
	}

	if (r_body_a_points) {
		JointGizmosDrawer::draw_circle(Vector3::AXIS_Z,
				BODY_A_RADIUS,
				p_offset,
				JointGizmosDrawer::look_body_toward_z(p_trs_joint, p_trs_body_a),
				p_limit_lower,
				p_limit_upper,
				*r_body_a_points);
	}

	if (r_body_b_points) {
		JointGizmosDrawer::draw_circle(Vector3::AXIS_Z,
				BODY_B_RADIUS,
				p_offset,
				JointGizmosDrawer::look_body_toward_z(p_trs_joint, p_trs_body_b),
				p_limit_lower,
				p_limit_upper,
				*r_body_b_points);
	}
}

void Joint3DGizmoPlugin::CreateSliderJointGizmo(const Transform3D &p_offset, const Transform3D &p_trs_joint, const Transform3D &p_trs_body_a, const Transform3D &p_trs_body_b, real_t p_angular_limit_lower, real_t p_angular_limit_upper, real_t p_linear_limit_lower, real_t p_linear_limit_upper, Vector<Vector3> &r_points, Vector<Vector3> *r_body_a_points, Vector<Vector3> *r_body_b_points) {
	p_linear_limit_lower = -p_linear_limit_lower;
	p_linear_limit_upper = -p_linear_limit_upper;

	float cs = 0.25;
	r_points.push_back(p_offset.translated_local(Vector3(0, 0, 0.5)).origin);
	r_points.push_back(p_offset.translated_local(Vector3(0, 0, -0.5)).origin);

	if (p_linear_limit_lower >= p_linear_limit_upper) {
		r_points.push_back(p_offset.translated_local(Vector3(p_linear_limit_upper, 0, 0)).origin);
		r_points.push_back(p_offset.translated_local(Vector3(p_linear_limit_lower, 0, 0)).origin);

		r_points.push_back(p_offset.translated_local(Vector3(p_linear_limit_upper, -cs, -cs)).origin);
		r_points.push_back(p_offset.translated_local(Vector3(p_linear_limit_upper, -cs, cs)).origin);
		r_points.push_back(p_offset.translated_local(Vector3(p_linear_limit_upper, -cs, cs)).origin);
		r_points.push_back(p_offset.translated_local(Vector3(p_linear_limit_upper, cs, cs)).origin);
		r_points.push_back(p_offset.translated_local(Vector3(p_linear_limit_upper, cs, cs)).origin);
		r_points.push_back(p_offset.translated_local(Vector3(p_linear_limit_upper, cs, -cs)).origin);
		r_points.push_back(p_offset.translated_local(Vector3(p_linear_limit_upper, cs, -cs)).origin);
		r_points.push_back(p_offset.translated_local(Vector3(p_linear_limit_upper, -cs, -cs)).origin);

		r_points.push_back(p_offset.translated_local(Vector3(p_linear_limit_lower, -cs, -cs)).origin);
		r_points.push_back(p_offset.translated_local(Vector3(p_linear_limit_lower, -cs, cs)).origin);
		r_points.push_back(p_offset.translated_local(Vector3(p_linear_limit_lower, -cs, cs)).origin);
		r_points.push_back(p_offset.translated_local(Vector3(p_linear_limit_lower, cs, cs)).origin);
		r_points.push_back(p_offset.translated_local(Vector3(p_linear_limit_lower, cs, cs)).origin);
		r_points.push_back(p_offset.translated_local(Vector3(p_linear_limit_lower, cs, -cs)).origin);
		r_points.push_back(p_offset.translated_local(Vector3(p_linear_limit_lower, cs, -cs)).origin);
		r_points.push_back(p_offset.translated_local(Vector3(p_linear_limit_lower, -cs, -cs)).origin);

	} else {
		r_points.push_back(p_offset.translated_local(Vector3(+cs * 2, 0, 0)).origin);
		r_points.push_back(p_offset.translated_local(Vector3(-cs * 2, 0, 0)).origin);
	}

	if (r_body_a_points) {
		JointGizmosDrawer::draw_circle(
				Vector3::AXIS_X,
				BODY_A_RADIUS,
				p_offset,
				JointGizmosDrawer::look_body_toward(Vector3::AXIS_X, p_trs_joint, p_trs_body_a),
				p_angular_limit_lower,
				p_angular_limit_upper,
				*r_body_a_points);
	}

	if (r_body_b_points) {
		JointGizmosDrawer::draw_circle(
				Vector3::AXIS_X,
				BODY_B_RADIUS,
				p_offset,
				JointGizmosDrawer::look_body_toward(Vector3::AXIS_X, p_trs_joint, p_trs_body_b),
				p_angular_limit_lower,
				p_angular_limit_upper,
				*r_body_b_points,
				true);
	}
}

void Joint3DGizmoPlugin::CreateConeTwistJointGizmo(const Transform3D &p_offset, const Transform3D &p_trs_joint, const Transform3D &p_trs_body_a, const Transform3D &p_trs_body_b, real_t p_swing, real_t p_twist, Vector<Vector3> *r_body_a_points, Vector<Vector3> *r_body_b_points) {
	if (r_body_a_points) {
		JointGizmosDrawer::draw_cone(
				p_offset,
				JointGizmosDrawer::look_body(p_trs_joint, p_trs_body_a),
				p_swing,
				p_twist,
				*r_body_a_points);
	}

	if (r_body_b_points) {
		JointGizmosDrawer::draw_cone(
				p_offset,
				JointGizmosDrawer::look_body(p_trs_joint, p_trs_body_b),
				p_swing,
				p_twist,
				*r_body_b_points);
	}
}

void Joint3DGizmoPlugin::CreateGeneric6DOFJointGizmo(
		const Transform3D &p_offset,
		const Transform3D &p_trs_joint,
		const Transform3D &p_trs_body_a,
		const Transform3D &p_trs_body_b,
		real_t p_angular_limit_lower_x,
		real_t p_angular_limit_upper_x,
		real_t p_linear_limit_lower_x,
		real_t p_linear_limit_upper_x,
		bool p_enable_angular_limit_x,
		bool p_enable_linear_limit_x,
		real_t p_angular_limit_lower_y,
		real_t p_angular_limit_upper_y,
		real_t p_linear_limit_lower_y,
		real_t p_linear_limit_upper_y,
		bool p_enable_angular_limit_y,
		bool p_enable_linear_limit_y,
		real_t p_angular_limit_lower_z,
		real_t p_angular_limit_upper_z,
		real_t p_linear_limit_lower_z,
		real_t p_linear_limit_upper_z,
		bool p_enable_angular_limit_z,
		bool p_enable_linear_limit_z,
		Vector<Vector3> &r_points,
		Vector<Vector3> *r_body_a_points,
		Vector<Vector3> *r_body_b_points) {
	float cs = 0.25;

	for (int ax = 0; ax < 3; ax++) {
		float ll = 0;
		float ul = 0;
		float lll = 0;
		float lul = 0;

		int a1 = 0;
		int a2 = 0;
		int a3 = 0;
		bool enable_ang = false;
		bool enable_lin = false;

		switch (ax) {
			case 0:
				ll = p_angular_limit_lower_x;
				ul = p_angular_limit_upper_x;
				lll = -p_linear_limit_lower_x;
				lul = -p_linear_limit_upper_x;
				enable_ang = p_enable_angular_limit_x;
				enable_lin = p_enable_linear_limit_x;
				a1 = 0;
				a2 = 1;
				a3 = 2;
				break;
			case 1:
				ll = p_angular_limit_lower_y;
				ul = p_angular_limit_upper_y;
				lll = -p_linear_limit_lower_y;
				lul = -p_linear_limit_upper_y;
				enable_ang = p_enable_angular_limit_y;
				enable_lin = p_enable_linear_limit_y;
				a1 = 1;
				a2 = 2;
				a3 = 0;
				break;
			case 2:
				ll = p_angular_limit_lower_z;
				ul = p_angular_limit_upper_z;
				lll = -p_linear_limit_lower_z;
				lul = -p_linear_limit_upper_z;
				enable_ang = p_enable_angular_limit_z;
				enable_lin = p_enable_linear_limit_z;
				a1 = 2;
				a2 = 0;
				a3 = 1;
				break;
		}

#define ADD_VTX(x, y, z)                                         \
	{                                                            \
		Vector3 v;                                               \
		v[a1] = (x);                                             \
		v[a2] = (y);                                             \
		v[a3] = (z);                                             \
		r_points.push_back(p_offset.translated_local(v).origin); \
	}

		if (enable_lin && lll >= lul) {
			ADD_VTX(lul, 0, 0);
			ADD_VTX(lll, 0, 0);

			ADD_VTX(lul, -cs, -cs);
			ADD_VTX(lul, -cs, cs);
			ADD_VTX(lul, -cs, cs);
			ADD_VTX(lul, cs, cs);
			ADD_VTX(lul, cs, cs);
			ADD_VTX(lul, cs, -cs);
			ADD_VTX(lul, cs, -cs);
			ADD_VTX(lul, -cs, -cs);

			ADD_VTX(lll, -cs, -cs);
			ADD_VTX(lll, -cs, cs);
			ADD_VTX(lll, -cs, cs);
			ADD_VTX(lll, cs, cs);
			ADD_VTX(lll, cs, cs);
			ADD_VTX(lll, cs, -cs);
			ADD_VTX(lll, cs, -cs);
			ADD_VTX(lll, -cs, -cs);

		} else {
			ADD_VTX(+cs * 2, 0, 0);
			ADD_VTX(-cs * 2, 0, 0);
		}

		if (!enable_ang) {
			ll = 0;
			ul = -1;
		}

		if (r_body_a_points) {
			JointGizmosDrawer::draw_circle(
					static_cast<Vector3::Axis>(ax),
					BODY_A_RADIUS,
					p_offset,
					JointGizmosDrawer::look_body_toward(static_cast<Vector3::Axis>(ax), p_trs_joint, p_trs_body_a),
					ll,
					ul,
					*r_body_a_points,
					true);
		}

		if (r_body_b_points) {
			JointGizmosDrawer::draw_circle(
					static_cast<Vector3::Axis>(ax),
					BODY_B_RADIUS,
					p_offset,
					JointGizmosDrawer::look_body_toward(static_cast<Vector3::Axis>(ax), p_trs_joint, p_trs_body_b),
					ll,
					ul,
					*r_body_b_points);
		}
	}

#undef ADD_VTX
}

////

FogVolumeGizmoPlugin::FogVolumeGizmoPlugin() {
	Color gizmo_color = EDITOR_DEF("editors/3d_gizmos/gizmo_colors/fog_volume", Color(0.5, 0.7, 1));
	create_material("shape_material", gizmo_color);
	gizmo_color.a = 0.15;
	create_material("shape_material_internal", gizmo_color);

	create_handle_material("handles");
}

bool FogVolumeGizmoPlugin::has_gizmo(Node3D *p_spatial) {
	return (Object::cast_to<FogVolume>(p_spatial) != nullptr);
}

String FogVolumeGizmoPlugin::get_gizmo_name() const {
	return "FogVolume";
}

int FogVolumeGizmoPlugin::get_priority() const {
	return -1;
}

String FogVolumeGizmoPlugin::get_handle_name(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary) const {
	return "Size";
}

Variant FogVolumeGizmoPlugin::get_handle_value(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary) const {
	return Vector3(p_gizmo->get_node_3d()->call("get_size"));
}

void FogVolumeGizmoPlugin::set_handle(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary, Camera3D *p_camera, const Point2 &p_point) {
	Node3D *sn = p_gizmo->get_node_3d();

	Transform3D gt = sn->get_global_transform();
	Transform3D gi = gt.affine_inverse();

	Vector3 ray_from = p_camera->project_ray_origin(p_point);
	Vector3 ray_dir = p_camera->project_ray_normal(p_point);

	Vector3 sg[2] = { gi.xform(ray_from), gi.xform(ray_from + ray_dir * 4096) };

	Vector3 axis;
	axis[p_id] = 1.0;
	Vector3 ra, rb;
	Geometry3D::get_closest_points_between_segments(Vector3(), axis * 4096, sg[0], sg[1], ra, rb);
	float d = ra[p_id] * 2;
	if (Node3DEditor::get_singleton()->is_snap_enabled()) {
		d = Math::snapped(d, Node3DEditor::get_singleton()->get_translate_snap());
	}

	if (d < 0.001) {
		d = 0.001;
	}

	Vector3 he = sn->call("get_size");
	he[p_id] = d;
	sn->call("set_size", he);
}

void FogVolumeGizmoPlugin::commit_handle(const EditorNode3DGizmo *p_gizmo, int p_id, bool p_secondary, const Variant &p_restore, bool p_cancel) {
	Node3D *sn = p_gizmo->get_node_3d();

	if (p_cancel) {
		sn->call("set_size", p_restore);
		return;
	}

	EditorUndoRedoManager *ur = EditorUndoRedoManager::get_singleton();
	ur->create_action(TTR("Change Fog Volume Size"));
	ur->add_do_method(sn, "set_size", sn->call("get_size"));
	ur->add_undo_method(sn, "set_size", p_restore);
	ur->commit_action();
}

void FogVolumeGizmoPlugin::redraw(EditorNode3DGizmo *p_gizmo) {
	Node3D *cs = p_gizmo->get_node_3d();

	p_gizmo->clear();

	if (RS::FogVolumeShape(int(p_gizmo->get_node_3d()->call("get_shape"))) != RS::FOG_VOLUME_SHAPE_WORLD) {
		const Ref<Material> material =
				get_material("shape_material", p_gizmo);
		const Ref<Material> material_internal =
				get_material("shape_material_internal", p_gizmo);

		Ref<Material> handles_material = get_material("handles");

		Vector<Vector3> lines;
		AABB aabb;
		aabb.size = cs->call("get_size").operator Vector3();
		aabb.position = aabb.size / -2;

		for (int i = 0; i < 12; i++) {
			Vector3 a, b;
			aabb.get_edge(i, a, b);
			lines.push_back(a);
			lines.push_back(b);
		}

		Vector<Vector3> handles;

		for (int i = 0; i < 3; i++) {
			Vector3 ax;
			ax[i] = cs->call("get_size").operator Vector3()[i] / 2;
			handles.push_back(ax);
		}

		p_gizmo->add_lines(lines, material);
		p_gizmo->add_collision_segments(lines);
		p_gizmo->add_handles(handles, handles_material);
	}
}

/////
