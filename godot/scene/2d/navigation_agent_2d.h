/**************************************************************************/
/*  navigation_agent_2d.h                                                 */
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

#ifndef NAVIGATION_AGENT_2D_H
#define NAVIGATION_AGENT_2D_H

#include "scene/main/node.h"
#include "servers/navigation/navigation_path_query_parameters_2d.h"
#include "servers/navigation/navigation_path_query_result_2d.h"

class Node2D;

class NavigationAgent2D : public Node {
	GDCLASS(NavigationAgent2D, Node);

	Node2D *agent_parent = nullptr;

	RID agent;
	RID map_before_pause;
	RID map_override;

	bool avoidance_enabled = false;
	uint32_t navigation_layers = 1;
	BitField<NavigationPathQueryParameters2D::PathMetadataFlags> path_metadata_flags = NavigationPathQueryParameters2D::PathMetadataFlags::PATH_METADATA_INCLUDE_ALL;

	real_t path_desired_distance = 20.0;
	real_t target_desired_distance = 10.0;
	real_t radius = 10.0;
	real_t neighbor_distance = 500.0;
	int max_neighbors = 10;
	real_t time_horizon = 1.0;
	real_t max_speed = 100.0;
	real_t path_max_distance = 100.0;

	Vector2 target_position;
	bool target_position_submitted = false;
	Ref<NavigationPathQueryParameters2D> navigation_query;
	Ref<NavigationPathQueryResult2D> navigation_result;
	int navigation_path_index = 0;
	bool velocity_submitted = false;
	Vector2 prev_safe_velocity;
	/// The submitted target velocity
	Vector2 target_velocity;
	bool target_reached = false;
	bool navigation_finished = true;
	// No initialized on purpose
	uint32_t update_frame_id = 0;

	// Debug properties for exposed bindings
	bool debug_enabled = false;
	float debug_path_custom_point_size = 4.0;
	float debug_path_custom_line_width = 1.0;
	bool debug_use_custom = false;
	Color debug_path_custom_color = Color(1.0, 1.0, 1.0, 1.0);
#ifdef DEBUG_ENABLED
	// Debug properties internal only
	bool debug_path_dirty = true;
	RID debug_path_instance;

private:
	void _navigation_debug_changed();
	void _update_debug_path();
#endif // DEBUG_ENABLED

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	NavigationAgent2D();
	virtual ~NavigationAgent2D();

	RID get_rid() const {
		return agent;
	}

	void set_avoidance_enabled(bool p_enabled);
	bool get_avoidance_enabled() const;

	void set_agent_parent(Node *p_agent_parent);

	void set_navigation_layers(uint32_t p_navigation_layers);
	uint32_t get_navigation_layers() const;

	void set_navigation_layer_value(int p_layer_number, bool p_value);
	bool get_navigation_layer_value(int p_layer_number) const;

	void set_path_metadata_flags(BitField<NavigationPathQueryParameters2D::PathMetadataFlags> p_flags);
	BitField<NavigationPathQueryParameters2D::PathMetadataFlags> get_path_metadata_flags() const {
		return path_metadata_flags;
	}

	void set_navigation_map(RID p_navigation_map);
	RID get_navigation_map() const;

	void set_path_desired_distance(real_t p_dd);
	real_t get_path_desired_distance() const {
		return path_desired_distance;
	}

	void set_target_desired_distance(real_t p_dd);
	real_t get_target_desired_distance() const {
		return target_desired_distance;
	}

	void set_radius(real_t p_radius);
	real_t get_radius() const {
		return radius;
	}

	void set_neighbor_distance(real_t p_distance);
	real_t get_neighbor_distance() const {
		return neighbor_distance;
	}

	void set_max_neighbors(int p_count);
	int get_max_neighbors() const {
		return max_neighbors;
	}

	void set_time_horizon(real_t p_time);
	real_t get_time_horizon() const {
		return time_horizon;
	}

	void set_max_speed(real_t p_max_speed);
	real_t get_max_speed() const {
		return max_speed;
	}

	void set_path_max_distance(real_t p_pmd);
	real_t get_path_max_distance();

	void set_target_position(Vector2 p_position);
	Vector2 get_target_position() const;

	Vector2 get_next_path_position();

	Ref<NavigationPathQueryResult2D> get_current_navigation_result() const {
		return navigation_result;
	}
	const Vector<Vector2> &get_current_navigation_path() const {
		return navigation_result->get_path();
	}
	int get_current_navigation_path_index() const {
		return navigation_path_index;
	}

	real_t distance_to_target() const;
	bool is_target_reached() const;
	bool is_target_reachable();
	bool is_navigation_finished();
	Vector2 get_final_position();

	void set_velocity(Vector2 p_velocity);
	void _avoidance_done(Vector3 p_new_velocity);

	PackedStringArray get_configuration_warnings() const override;

	void set_debug_enabled(bool p_enabled);
	bool get_debug_enabled() const;

	void set_debug_use_custom(bool p_enabled);
	bool get_debug_use_custom() const;

	void set_debug_path_custom_color(Color p_color);
	Color get_debug_path_custom_color() const;

	void set_debug_path_custom_point_size(float p_point_size);
	float get_debug_path_custom_point_size() const;

	void set_debug_path_custom_line_width(float p_line_width);
	float get_debug_path_custom_line_width() const;

private:
	void update_navigation();
	void _request_repath();
	void _check_distance_to_target();
};

#endif // NAVIGATION_AGENT_2D_H
