/**************************************************************************/
/*  navigation_server_3d.cpp                                              */
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

#include "navigation_server_3d.h"
#include "core/config/project_settings.h"

NavigationServer3D *NavigationServer3D::singleton = nullptr;

void NavigationServer3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_maps"), &NavigationServer3D::get_maps);

	ClassDB::bind_method(D_METHOD("map_create"), &NavigationServer3D::map_create);
	ClassDB::bind_method(D_METHOD("map_set_active", "map", "active"), &NavigationServer3D::map_set_active);
	ClassDB::bind_method(D_METHOD("map_is_active", "map"), &NavigationServer3D::map_is_active);
	ClassDB::bind_method(D_METHOD("map_set_up", "map", "up"), &NavigationServer3D::map_set_up);
	ClassDB::bind_method(D_METHOD("map_get_up", "map"), &NavigationServer3D::map_get_up);
	ClassDB::bind_method(D_METHOD("map_set_cell_size", "map", "cell_size"), &NavigationServer3D::map_set_cell_size);
	ClassDB::bind_method(D_METHOD("map_get_cell_size", "map"), &NavigationServer3D::map_get_cell_size);
	ClassDB::bind_method(D_METHOD("map_set_edge_connection_margin", "map", "margin"), &NavigationServer3D::map_set_edge_connection_margin);
	ClassDB::bind_method(D_METHOD("map_get_edge_connection_margin", "map"), &NavigationServer3D::map_get_edge_connection_margin);
	ClassDB::bind_method(D_METHOD("map_set_link_connection_radius", "map", "radius"), &NavigationServer3D::map_set_link_connection_radius);
	ClassDB::bind_method(D_METHOD("map_get_link_connection_radius", "map"), &NavigationServer3D::map_get_link_connection_radius);
	ClassDB::bind_method(D_METHOD("map_get_path", "map", "origin", "destination", "optimize", "navigation_layers"), &NavigationServer3D::map_get_path, DEFVAL(1));
	ClassDB::bind_method(D_METHOD("map_get_closest_point_to_segment", "map", "start", "end", "use_collision"), &NavigationServer3D::map_get_closest_point_to_segment, DEFVAL(false));
	ClassDB::bind_method(D_METHOD("map_get_closest_point", "map", "to_point"), &NavigationServer3D::map_get_closest_point);
	ClassDB::bind_method(D_METHOD("map_get_closest_point_normal", "map", "to_point"), &NavigationServer3D::map_get_closest_point_normal);
	ClassDB::bind_method(D_METHOD("map_get_closest_point_owner", "map", "to_point"), &NavigationServer3D::map_get_closest_point_owner);

	ClassDB::bind_method(D_METHOD("map_get_links", "map"), &NavigationServer3D::map_get_links);
	ClassDB::bind_method(D_METHOD("map_get_regions", "map"), &NavigationServer3D::map_get_regions);
	ClassDB::bind_method(D_METHOD("map_get_agents", "map"), &NavigationServer3D::map_get_agents);

	ClassDB::bind_method(D_METHOD("map_force_update", "map"), &NavigationServer3D::map_force_update);

	ClassDB::bind_method(D_METHOD("query_path", "parameters", "result"), &NavigationServer3D::query_path);

	ClassDB::bind_method(D_METHOD("region_create"), &NavigationServer3D::region_create);
	ClassDB::bind_method(D_METHOD("region_set_enter_cost", "region", "enter_cost"), &NavigationServer3D::region_set_enter_cost);
	ClassDB::bind_method(D_METHOD("region_get_enter_cost", "region"), &NavigationServer3D::region_get_enter_cost);
	ClassDB::bind_method(D_METHOD("region_set_travel_cost", "region", "travel_cost"), &NavigationServer3D::region_set_travel_cost);
	ClassDB::bind_method(D_METHOD("region_get_travel_cost", "region"), &NavigationServer3D::region_get_travel_cost);
	ClassDB::bind_method(D_METHOD("region_set_owner_id", "region", "owner_id"), &NavigationServer3D::region_set_owner_id);
	ClassDB::bind_method(D_METHOD("region_get_owner_id", "region"), &NavigationServer3D::region_get_owner_id);
	ClassDB::bind_method(D_METHOD("region_owns_point", "region", "point"), &NavigationServer3D::region_owns_point);
	ClassDB::bind_method(D_METHOD("region_set_map", "region", "map"), &NavigationServer3D::region_set_map);
	ClassDB::bind_method(D_METHOD("region_get_map", "region"), &NavigationServer3D::region_get_map);
	ClassDB::bind_method(D_METHOD("region_set_navigation_layers", "region", "navigation_layers"), &NavigationServer3D::region_set_navigation_layers);
	ClassDB::bind_method(D_METHOD("region_get_navigation_layers", "region"), &NavigationServer3D::region_get_navigation_layers);
	ClassDB::bind_method(D_METHOD("region_set_transform", "region", "transform"), &NavigationServer3D::region_set_transform);
	ClassDB::bind_method(D_METHOD("region_set_navigation_mesh", "region", "navigation_mesh"), &NavigationServer3D::region_set_navigation_mesh);
	ClassDB::bind_method(D_METHOD("region_bake_navigation_mesh", "navigation_mesh", "root_node"), &NavigationServer3D::region_bake_navigation_mesh);
	ClassDB::bind_method(D_METHOD("region_get_connections_count", "region"), &NavigationServer3D::region_get_connections_count);
	ClassDB::bind_method(D_METHOD("region_get_connection_pathway_start", "region", "connection"), &NavigationServer3D::region_get_connection_pathway_start);
	ClassDB::bind_method(D_METHOD("region_get_connection_pathway_end", "region", "connection"), &NavigationServer3D::region_get_connection_pathway_end);

	ClassDB::bind_method(D_METHOD("link_create"), &NavigationServer3D::link_create);
	ClassDB::bind_method(D_METHOD("link_set_map", "link", "map"), &NavigationServer3D::link_set_map);
	ClassDB::bind_method(D_METHOD("link_get_map", "link"), &NavigationServer3D::link_get_map);
	ClassDB::bind_method(D_METHOD("link_set_bidirectional", "link", "bidirectional"), &NavigationServer3D::link_set_bidirectional);
	ClassDB::bind_method(D_METHOD("link_is_bidirectional", "link"), &NavigationServer3D::link_is_bidirectional);
	ClassDB::bind_method(D_METHOD("link_set_navigation_layers", "link", "navigation_layers"), &NavigationServer3D::link_set_navigation_layers);
	ClassDB::bind_method(D_METHOD("link_get_navigation_layers", "link"), &NavigationServer3D::link_get_navigation_layers);
	ClassDB::bind_method(D_METHOD("link_set_start_position", "link", "position"), &NavigationServer3D::link_set_start_position);
	ClassDB::bind_method(D_METHOD("link_get_start_position", "link"), &NavigationServer3D::link_get_start_position);
	ClassDB::bind_method(D_METHOD("link_set_end_position", "link", "position"), &NavigationServer3D::link_set_end_position);
	ClassDB::bind_method(D_METHOD("link_get_end_position", "link"), &NavigationServer3D::link_get_end_position);
	ClassDB::bind_method(D_METHOD("link_set_enter_cost", "link", "enter_cost"), &NavigationServer3D::link_set_enter_cost);
	ClassDB::bind_method(D_METHOD("link_get_enter_cost", "link"), &NavigationServer3D::link_get_enter_cost);
	ClassDB::bind_method(D_METHOD("link_set_travel_cost", "link", "travel_cost"), &NavigationServer3D::link_set_travel_cost);
	ClassDB::bind_method(D_METHOD("link_get_travel_cost", "link"), &NavigationServer3D::link_get_travel_cost);
	ClassDB::bind_method(D_METHOD("link_set_owner_id", "link", "owner_id"), &NavigationServer3D::link_set_owner_id);
	ClassDB::bind_method(D_METHOD("link_get_owner_id", "link"), &NavigationServer3D::link_get_owner_id);

	ClassDB::bind_method(D_METHOD("agent_create"), &NavigationServer3D::agent_create);
	ClassDB::bind_method(D_METHOD("agent_set_map", "agent", "map"), &NavigationServer3D::agent_set_map);
	ClassDB::bind_method(D_METHOD("agent_get_map", "agent"), &NavigationServer3D::agent_get_map);
	ClassDB::bind_method(D_METHOD("agent_set_neighbor_distance", "agent", "distance"), &NavigationServer3D::agent_set_neighbor_distance);
	ClassDB::bind_method(D_METHOD("agent_set_max_neighbors", "agent", "count"), &NavigationServer3D::agent_set_max_neighbors);
	ClassDB::bind_method(D_METHOD("agent_set_time_horizon", "agent", "time"), &NavigationServer3D::agent_set_time_horizon);
	ClassDB::bind_method(D_METHOD("agent_set_radius", "agent", "radius"), &NavigationServer3D::agent_set_radius);
	ClassDB::bind_method(D_METHOD("agent_set_max_speed", "agent", "max_speed"), &NavigationServer3D::agent_set_max_speed);
	ClassDB::bind_method(D_METHOD("agent_set_velocity", "agent", "velocity"), &NavigationServer3D::agent_set_velocity);
	ClassDB::bind_method(D_METHOD("agent_set_target_velocity", "agent", "target_velocity"), &NavigationServer3D::agent_set_target_velocity);
	ClassDB::bind_method(D_METHOD("agent_set_position", "agent", "position"), &NavigationServer3D::agent_set_position);
	ClassDB::bind_method(D_METHOD("agent_is_map_changed", "agent"), &NavigationServer3D::agent_is_map_changed);
	ClassDB::bind_method(D_METHOD("agent_set_callback", "agent", "callback"), &NavigationServer3D::agent_set_callback);

	ClassDB::bind_method(D_METHOD("free_rid", "rid"), &NavigationServer3D::free);

	ClassDB::bind_method(D_METHOD("set_active", "active"), &NavigationServer3D::set_active);

	ClassDB::bind_method(D_METHOD("set_debug_enabled", "enabled"), &NavigationServer3D::set_debug_enabled);
	ClassDB::bind_method(D_METHOD("get_debug_enabled"), &NavigationServer3D::get_debug_enabled);

	ADD_SIGNAL(MethodInfo("map_changed", PropertyInfo(Variant::RID, "map")));

	ADD_SIGNAL(MethodInfo("navigation_debug_changed"));

	ClassDB::bind_method(D_METHOD("get_process_info", "process_info"), &NavigationServer3D::get_process_info);

	BIND_ENUM_CONSTANT(INFO_ACTIVE_MAPS);
	BIND_ENUM_CONSTANT(INFO_REGION_COUNT);
	BIND_ENUM_CONSTANT(INFO_AGENT_COUNT);
	BIND_ENUM_CONSTANT(INFO_LINK_COUNT);
	BIND_ENUM_CONSTANT(INFO_POLYGON_COUNT);
	BIND_ENUM_CONSTANT(INFO_EDGE_COUNT);
	BIND_ENUM_CONSTANT(INFO_EDGE_MERGE_COUNT);
	BIND_ENUM_CONSTANT(INFO_EDGE_CONNECTION_COUNT);
	BIND_ENUM_CONSTANT(INFO_EDGE_FREE_COUNT);
}

NavigationServer3D *NavigationServer3D::get_singleton() {
	return singleton;
}

NavigationServer3D::NavigationServer3D() {
	ERR_FAIL_COND(singleton != nullptr);
	singleton = this;

	GLOBAL_DEF("navigation/2d/default_cell_size", 1);
	GLOBAL_DEF("navigation/2d/default_edge_connection_margin", 1);
	GLOBAL_DEF("navigation/2d/default_link_connection_radius", 4);

	GLOBAL_DEF("navigation/3d/default_cell_size", 0.25);
	GLOBAL_DEF("navigation/3d/default_edge_connection_margin", 0.25);
	GLOBAL_DEF("navigation/3d/default_link_connection_radius", 1.0);

#ifdef DEBUG_ENABLED
	debug_navigation_edge_connection_color = GLOBAL_DEF("debug/shapes/navigation/edge_connection_color", Color(1.0, 0.0, 1.0, 1.0));
	debug_navigation_geometry_edge_color = GLOBAL_DEF("debug/shapes/navigation/geometry_edge_color", Color(0.5, 1.0, 1.0, 1.0));
	debug_navigation_geometry_face_color = GLOBAL_DEF("debug/shapes/navigation/geometry_face_color", Color(0.5, 1.0, 1.0, 0.4));
	debug_navigation_geometry_edge_disabled_color = GLOBAL_DEF("debug/shapes/navigation/geometry_edge_disabled_color", Color(0.5, 0.5, 0.5, 1.0));
	debug_navigation_geometry_face_disabled_color = GLOBAL_DEF("debug/shapes/navigation/geometry_face_disabled_color", Color(0.5, 0.5, 0.5, 0.4));
	debug_navigation_link_connection_color = GLOBAL_DEF("debug/shapes/navigation/link_connection_color", Color(1.0, 0.5, 1.0, 1.0));
	debug_navigation_link_connection_disabled_color = GLOBAL_DEF("debug/shapes/navigation/link_connection_disabled_color", Color(0.5, 0.5, 0.5, 1.0));
	debug_navigation_agent_path_color = GLOBAL_DEF("debug/shapes/navigation/agent_path_color", Color(1.0, 0.0, 0.0, 1.0));

	debug_navigation_enable_edge_connections = GLOBAL_DEF("debug/shapes/navigation/enable_edge_connections", true);
	debug_navigation_enable_edge_connections_xray = GLOBAL_DEF("debug/shapes/navigation/enable_edge_connections_xray", true);
	debug_navigation_enable_edge_lines = GLOBAL_DEF("debug/shapes/navigation/enable_edge_lines", true);
	debug_navigation_enable_edge_lines_xray = GLOBAL_DEF("debug/shapes/navigation/enable_edge_lines_xray", true);
	debug_navigation_enable_geometry_face_random_color = GLOBAL_DEF("debug/shapes/navigation/enable_geometry_face_random_color", true);
	debug_navigation_enable_link_connections = GLOBAL_DEF("debug/shapes/navigation/enable_link_connections", true);
	debug_navigation_enable_link_connections_xray = GLOBAL_DEF("debug/shapes/navigation/enable_link_connections_xray", true);

	debug_navigation_enable_agent_paths = GLOBAL_DEF("debug/shapes/navigation/enable_agent_paths", true);
	debug_navigation_enable_agent_paths_xray = GLOBAL_DEF("debug/shapes/navigation/enable_agent_paths_xray", true);

	debug_navigation_agent_path_point_size = GLOBAL_DEF("debug/shapes/navigation/agent_path_point_size", 4.0);

	if (Engine::get_singleton()->is_editor_hint()) {
		// enable NavigationServer3D when in Editor or else navigation mesh edge connections are invisible
		// on runtime tests SceneTree has "Visible Navigation" set and main iteration takes care of this
		set_debug_enabled(true);
	}
#endif // DEBUG_ENABLED
}

NavigationServer3D::~NavigationServer3D() {
	singleton = nullptr;
}

void NavigationServer3D::set_debug_enabled(bool p_enabled) {
#ifdef DEBUG_ENABLED
	if (debug_enabled != p_enabled) {
		debug_dirty = true;
	}

	debug_enabled = p_enabled;

	if (debug_dirty) {
		call_deferred("_emit_navigation_debug_changed_signal");
	}
#endif // DEBUG_ENABLED
}

bool NavigationServer3D::get_debug_enabled() const {
	return debug_enabled;
}

#ifdef DEBUG_ENABLED
void NavigationServer3D::_emit_navigation_debug_changed_signal() {
	if (debug_dirty) {
		debug_dirty = false;
		emit_signal(SNAME("navigation_debug_changed"));
	}
}
#endif // DEBUG_ENABLED

#ifdef DEBUG_ENABLED
Ref<StandardMaterial3D> NavigationServer3D::get_debug_navigation_geometry_face_material() {
	if (debug_navigation_geometry_face_material.is_valid()) {
		return debug_navigation_geometry_face_material;
	}

	bool enabled_geometry_face_random_color = get_debug_navigation_enable_geometry_face_random_color();

	Ref<StandardMaterial3D> face_material = Ref<StandardMaterial3D>(memnew(StandardMaterial3D));
	face_material->set_shading_mode(StandardMaterial3D::SHADING_MODE_UNSHADED);
	face_material->set_transparency(StandardMaterial3D::TRANSPARENCY_ALPHA);
	face_material->set_albedo(get_debug_navigation_geometry_face_color());
	if (enabled_geometry_face_random_color) {
		face_material->set_flag(StandardMaterial3D::FLAG_SRGB_VERTEX_COLOR, true);
		face_material->set_flag(StandardMaterial3D::FLAG_ALBEDO_FROM_VERTEX_COLOR, true);
	}

	debug_navigation_geometry_face_material = face_material;

	return debug_navigation_geometry_face_material;
}

Ref<StandardMaterial3D> NavigationServer3D::get_debug_navigation_geometry_edge_material() {
	if (debug_navigation_geometry_edge_material.is_valid()) {
		return debug_navigation_geometry_edge_material;
	}

	bool enabled_edge_lines_xray = get_debug_navigation_enable_edge_lines_xray();

	Ref<StandardMaterial3D> line_material = Ref<StandardMaterial3D>(memnew(StandardMaterial3D));
	line_material->set_shading_mode(StandardMaterial3D::SHADING_MODE_UNSHADED);
	line_material->set_albedo(get_debug_navigation_geometry_edge_color());
	if (enabled_edge_lines_xray) {
		line_material->set_flag(StandardMaterial3D::FLAG_DISABLE_DEPTH_TEST, true);
	}

	debug_navigation_geometry_edge_material = line_material;

	return debug_navigation_geometry_edge_material;
}

Ref<StandardMaterial3D> NavigationServer3D::get_debug_navigation_geometry_face_disabled_material() {
	if (debug_navigation_geometry_face_disabled_material.is_valid()) {
		return debug_navigation_geometry_face_disabled_material;
	}

	Ref<StandardMaterial3D> face_disabled_material = Ref<StandardMaterial3D>(memnew(StandardMaterial3D));
	face_disabled_material->set_shading_mode(StandardMaterial3D::SHADING_MODE_UNSHADED);
	face_disabled_material->set_transparency(StandardMaterial3D::TRANSPARENCY_ALPHA);
	face_disabled_material->set_albedo(get_debug_navigation_geometry_face_disabled_color());

	debug_navigation_geometry_face_disabled_material = face_disabled_material;

	return debug_navigation_geometry_face_disabled_material;
}

Ref<StandardMaterial3D> NavigationServer3D::get_debug_navigation_geometry_edge_disabled_material() {
	if (debug_navigation_geometry_edge_disabled_material.is_valid()) {
		return debug_navigation_geometry_edge_disabled_material;
	}

	bool enabled_edge_lines_xray = get_debug_navigation_enable_edge_lines_xray();

	Ref<StandardMaterial3D> line_disabled_material = Ref<StandardMaterial3D>(memnew(StandardMaterial3D));
	line_disabled_material->set_shading_mode(StandardMaterial3D::SHADING_MODE_UNSHADED);
	line_disabled_material->set_albedo(get_debug_navigation_geometry_edge_disabled_color());
	if (enabled_edge_lines_xray) {
		line_disabled_material->set_flag(StandardMaterial3D::FLAG_DISABLE_DEPTH_TEST, true);
	}

	debug_navigation_geometry_edge_disabled_material = line_disabled_material;

	return debug_navigation_geometry_edge_disabled_material;
}

Ref<StandardMaterial3D> NavigationServer3D::get_debug_navigation_edge_connections_material() {
	if (debug_navigation_edge_connections_material.is_valid()) {
		return debug_navigation_edge_connections_material;
	}

	bool enabled_edge_connections_xray = get_debug_navigation_enable_edge_connections_xray();

	Ref<StandardMaterial3D> edge_connections_material = Ref<StandardMaterial3D>(memnew(StandardMaterial3D));
	edge_connections_material->set_shading_mode(StandardMaterial3D::SHADING_MODE_UNSHADED);
	edge_connections_material->set_albedo(get_debug_navigation_edge_connection_color());
	if (enabled_edge_connections_xray) {
		edge_connections_material->set_flag(StandardMaterial3D::FLAG_DISABLE_DEPTH_TEST, true);
	}
	edge_connections_material->set_render_priority(StandardMaterial3D::RENDER_PRIORITY_MAX - 2);

	debug_navigation_edge_connections_material = edge_connections_material;

	return debug_navigation_edge_connections_material;
}

Ref<StandardMaterial3D> NavigationServer3D::get_debug_navigation_link_connections_material() {
	if (debug_navigation_link_connections_material.is_valid()) {
		return debug_navigation_link_connections_material;
	}

	Ref<StandardMaterial3D> material = Ref<StandardMaterial3D>(memnew(StandardMaterial3D));
	material->set_shading_mode(StandardMaterial3D::SHADING_MODE_UNSHADED);
	material->set_albedo(debug_navigation_link_connection_color);
	if (debug_navigation_enable_link_connections_xray) {
		material->set_flag(StandardMaterial3D::FLAG_DISABLE_DEPTH_TEST, true);
	}
	material->set_render_priority(StandardMaterial3D::RENDER_PRIORITY_MAX - 2);

	debug_navigation_link_connections_material = material;
	return debug_navigation_link_connections_material;
}

Ref<StandardMaterial3D> NavigationServer3D::get_debug_navigation_link_connections_disabled_material() {
	if (debug_navigation_link_connections_disabled_material.is_valid()) {
		return debug_navigation_link_connections_disabled_material;
	}

	Ref<StandardMaterial3D> material = Ref<StandardMaterial3D>(memnew(StandardMaterial3D));
	material->set_shading_mode(StandardMaterial3D::SHADING_MODE_UNSHADED);
	material->set_albedo(debug_navigation_link_connection_disabled_color);
	if (debug_navigation_enable_link_connections_xray) {
		material->set_flag(StandardMaterial3D::FLAG_DISABLE_DEPTH_TEST, true);
	}
	material->set_render_priority(StandardMaterial3D::RENDER_PRIORITY_MAX - 2);

	debug_navigation_link_connections_disabled_material = material;
	return debug_navigation_link_connections_disabled_material;
}

Ref<StandardMaterial3D> NavigationServer3D::get_debug_navigation_agent_path_line_material() {
	if (debug_navigation_agent_path_line_material.is_valid()) {
		return debug_navigation_agent_path_line_material;
	}

	Ref<StandardMaterial3D> material = Ref<StandardMaterial3D>(memnew(StandardMaterial3D));
	material->set_shading_mode(StandardMaterial3D::SHADING_MODE_UNSHADED);
	material->set_albedo(debug_navigation_agent_path_color);
	if (debug_navigation_enable_agent_paths_xray) {
		material->set_flag(StandardMaterial3D::FLAG_DISABLE_DEPTH_TEST, true);
	}
	material->set_render_priority(StandardMaterial3D::RENDER_PRIORITY_MAX - 2);

	debug_navigation_agent_path_line_material = material;
	return debug_navigation_agent_path_line_material;
}

Ref<StandardMaterial3D> NavigationServer3D::get_debug_navigation_agent_path_point_material() {
	if (debug_navigation_agent_path_point_material.is_valid()) {
		return debug_navigation_agent_path_point_material;
	}

	Ref<StandardMaterial3D> material = Ref<StandardMaterial3D>(memnew(StandardMaterial3D));
	material->set_shading_mode(StandardMaterial3D::SHADING_MODE_UNSHADED);
	material->set_albedo(debug_navigation_agent_path_color);
	material->set_flag(StandardMaterial3D::FLAG_USE_POINT_SIZE, true);
	material->set_point_size(debug_navigation_agent_path_point_size);
	if (debug_navigation_enable_agent_paths_xray) {
		material->set_flag(StandardMaterial3D::FLAG_DISABLE_DEPTH_TEST, true);
	}
	material->set_render_priority(StandardMaterial3D::RENDER_PRIORITY_MAX - 2);

	debug_navigation_agent_path_point_material = material;
	return debug_navigation_agent_path_point_material;
}

void NavigationServer3D::set_debug_navigation_edge_connection_color(const Color &p_color) {
	debug_navigation_edge_connection_color = p_color;
	if (debug_navigation_edge_connections_material.is_valid()) {
		debug_navigation_edge_connections_material->set_albedo(debug_navigation_edge_connection_color);
	}
}

Color NavigationServer3D::get_debug_navigation_edge_connection_color() const {
	return debug_navigation_edge_connection_color;
}

void NavigationServer3D::set_debug_navigation_geometry_edge_color(const Color &p_color) {
	debug_navigation_geometry_edge_color = p_color;
	if (debug_navigation_geometry_edge_material.is_valid()) {
		debug_navigation_geometry_edge_material->set_albedo(debug_navigation_geometry_edge_color);
	}
}

Color NavigationServer3D::get_debug_navigation_geometry_edge_color() const {
	return debug_navigation_geometry_edge_color;
}

void NavigationServer3D::set_debug_navigation_geometry_face_color(const Color &p_color) {
	debug_navigation_geometry_face_color = p_color;
	if (debug_navigation_geometry_face_material.is_valid()) {
		debug_navigation_geometry_face_material->set_albedo(debug_navigation_geometry_face_color);
	}
}

Color NavigationServer3D::get_debug_navigation_geometry_face_color() const {
	return debug_navigation_geometry_face_color;
}

void NavigationServer3D::set_debug_navigation_geometry_edge_disabled_color(const Color &p_color) {
	debug_navigation_geometry_edge_disabled_color = p_color;
	if (debug_navigation_geometry_edge_disabled_material.is_valid()) {
		debug_navigation_geometry_edge_disabled_material->set_albedo(debug_navigation_geometry_edge_disabled_color);
	}
}

Color NavigationServer3D::get_debug_navigation_geometry_edge_disabled_color() const {
	return debug_navigation_geometry_edge_disabled_color;
}

void NavigationServer3D::set_debug_navigation_geometry_face_disabled_color(const Color &p_color) {
	debug_navigation_geometry_face_disabled_color = p_color;
	if (debug_navigation_geometry_face_disabled_material.is_valid()) {
		debug_navigation_geometry_face_disabled_material->set_albedo(debug_navigation_geometry_face_disabled_color);
	}
}

Color NavigationServer3D::get_debug_navigation_geometry_face_disabled_color() const {
	return debug_navigation_geometry_face_disabled_color;
}

void NavigationServer3D::set_debug_navigation_link_connection_color(const Color &p_color) {
	debug_navigation_link_connection_color = p_color;
	if (debug_navigation_link_connections_material.is_valid()) {
		debug_navigation_link_connections_material->set_albedo(debug_navigation_link_connection_color);
	}
}

Color NavigationServer3D::get_debug_navigation_link_connection_color() const {
	return debug_navigation_link_connection_color;
}

void NavigationServer3D::set_debug_navigation_link_connection_disabled_color(const Color &p_color) {
	debug_navigation_link_connection_disabled_color = p_color;
	if (debug_navigation_link_connections_disabled_material.is_valid()) {
		debug_navigation_link_connections_disabled_material->set_albedo(debug_navigation_link_connection_disabled_color);
	}
}

Color NavigationServer3D::get_debug_navigation_link_connection_disabled_color() const {
	return debug_navigation_link_connection_disabled_color;
}

void NavigationServer3D::set_debug_navigation_agent_path_point_size(float p_point_size) {
	debug_navigation_agent_path_point_size = MAX(0.1, p_point_size);
	if (debug_navigation_agent_path_point_material.is_valid()) {
		debug_navigation_agent_path_point_material->set_point_size(debug_navigation_agent_path_point_size);
	}
}

float NavigationServer3D::get_debug_navigation_agent_path_point_size() const {
	return debug_navigation_agent_path_point_size;
}

void NavigationServer3D::set_debug_navigation_agent_path_color(const Color &p_color) {
	debug_navigation_agent_path_color = p_color;
	if (debug_navigation_agent_path_line_material.is_valid()) {
		debug_navigation_agent_path_line_material->set_albedo(debug_navigation_agent_path_color);
	}
	if (debug_navigation_agent_path_point_material.is_valid()) {
		debug_navigation_agent_path_point_material->set_albedo(debug_navigation_agent_path_color);
	}
}

Color NavigationServer3D::get_debug_navigation_agent_path_color() const {
	return debug_navigation_agent_path_color;
}

void NavigationServer3D::set_debug_navigation_enable_edge_connections(const bool p_value) {
	debug_navigation_enable_edge_connections = p_value;
	debug_dirty = true;
	call_deferred("_emit_navigation_debug_changed_signal");
}

bool NavigationServer3D::get_debug_navigation_enable_edge_connections() const {
	return debug_navigation_enable_edge_connections;
}

void NavigationServer3D::set_debug_navigation_enable_edge_connections_xray(const bool p_value) {
	debug_navigation_enable_edge_connections_xray = p_value;
	if (debug_navigation_edge_connections_material.is_valid()) {
		debug_navigation_edge_connections_material->set_flag(StandardMaterial3D::FLAG_DISABLE_DEPTH_TEST, debug_navigation_enable_edge_connections_xray);
	}
}

bool NavigationServer3D::get_debug_navigation_enable_edge_connections_xray() const {
	return debug_navigation_enable_edge_connections_xray;
}

void NavigationServer3D::set_debug_navigation_enable_edge_lines(const bool p_value) {
	debug_navigation_enable_edge_lines = p_value;
	debug_dirty = true;
	call_deferred("_emit_navigation_debug_changed_signal");
}

bool NavigationServer3D::get_debug_navigation_enable_edge_lines() const {
	return debug_navigation_enable_edge_lines;
}

void NavigationServer3D::set_debug_navigation_enable_edge_lines_xray(const bool p_value) {
	debug_navigation_enable_edge_lines_xray = p_value;
	if (debug_navigation_geometry_edge_material.is_valid()) {
		debug_navigation_geometry_edge_material->set_flag(StandardMaterial3D::FLAG_DISABLE_DEPTH_TEST, debug_navigation_enable_edge_lines_xray);
	}
}

bool NavigationServer3D::get_debug_navigation_enable_edge_lines_xray() const {
	return debug_navigation_enable_edge_lines_xray;
}

void NavigationServer3D::set_debug_navigation_enable_geometry_face_random_color(const bool p_value) {
	debug_navigation_enable_geometry_face_random_color = p_value;
	debug_dirty = true;
	call_deferred("_emit_navigation_debug_changed_signal");
}

bool NavigationServer3D::get_debug_navigation_enable_geometry_face_random_color() const {
	return debug_navigation_enable_geometry_face_random_color;
}

void NavigationServer3D::set_debug_navigation_enable_link_connections(const bool p_value) {
	debug_navigation_enable_link_connections = p_value;
	debug_dirty = true;
	call_deferred("_emit_navigation_debug_changed_signal");
}

bool NavigationServer3D::get_debug_navigation_enable_link_connections() const {
	return debug_navigation_enable_link_connections;
}

void NavigationServer3D::set_debug_navigation_enable_link_connections_xray(const bool p_value) {
	debug_navigation_enable_link_connections_xray = p_value;
	if (debug_navigation_link_connections_material.is_valid()) {
		debug_navigation_link_connections_material->set_flag(StandardMaterial3D::FLAG_DISABLE_DEPTH_TEST, debug_navigation_enable_link_connections_xray);
	}
}

bool NavigationServer3D::get_debug_navigation_enable_link_connections_xray() const {
	return debug_navigation_enable_link_connections_xray;
}

void NavigationServer3D::set_debug_navigation_enable_agent_paths(const bool p_value) {
	if (debug_navigation_enable_agent_paths != p_value) {
		debug_dirty = true;
	}

	debug_navigation_enable_agent_paths = p_value;

	if (debug_dirty) {
		call_deferred("_emit_navigation_debug_changed_signal");
	}
}

bool NavigationServer3D::get_debug_navigation_enable_agent_paths() const {
	return debug_navigation_enable_agent_paths;
}

void NavigationServer3D::set_debug_navigation_enable_agent_paths_xray(const bool p_value) {
	debug_navigation_enable_agent_paths_xray = p_value;
	if (debug_navigation_agent_path_line_material.is_valid()) {
		debug_navigation_agent_path_line_material->set_flag(StandardMaterial3D::FLAG_DISABLE_DEPTH_TEST, debug_navigation_enable_agent_paths_xray);
	}
	if (debug_navigation_agent_path_point_material.is_valid()) {
		debug_navigation_agent_path_point_material->set_flag(StandardMaterial3D::FLAG_DISABLE_DEPTH_TEST, debug_navigation_enable_agent_paths_xray);
	}
}

bool NavigationServer3D::get_debug_navigation_enable_agent_paths_xray() const {
	return debug_navigation_enable_agent_paths_xray;
}

#endif // DEBUG_ENABLED

void NavigationServer3D::query_path(const Ref<NavigationPathQueryParameters3D> &p_query_parameters, Ref<NavigationPathQueryResult3D> p_query_result) const {
	ERR_FAIL_COND(!p_query_parameters.is_valid());
	ERR_FAIL_COND(!p_query_result.is_valid());

	const NavigationUtilities::PathQueryResult _query_result = _query_path(p_query_parameters->get_parameters());

	p_query_result->set_path(_query_result.path);
	p_query_result->set_path_types(_query_result.path_types);
	p_query_result->set_path_rids(_query_result.path_rids);
	p_query_result->set_path_owner_ids(_query_result.path_owner_ids);
}

///////////////////////////////////////////////////////

NavigationServer3DCallback NavigationServer3DManager::create_callback = nullptr;

void NavigationServer3DManager::set_default_server(NavigationServer3DCallback p_callback) {
	create_callback = p_callback;
}

NavigationServer3D *NavigationServer3DManager::new_default_server() {
	if (create_callback == nullptr) {
		return nullptr;
	}

	return create_callback();
}
