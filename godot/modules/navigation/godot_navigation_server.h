/**************************************************************************/
/*  godot_navigation_server.h                                             */
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

#ifndef GODOT_NAVIGATION_SERVER_H
#define GODOT_NAVIGATION_SERVER_H

#include "core/templates/local_vector.h"
#include "core/templates/rid.h"
#include "core/templates/rid_owner.h"
#include "servers/navigation_server_3d.h"

#include "nav_agent.h"
#include "nav_link.h"
#include "nav_map.h"
#include "nav_region.h"

/// The commands are functions executed during the `sync` phase.

#define MERGE_INTERNAL(A, B) A##B
#define MERGE(A, B) MERGE_INTERNAL(A, B)

#define COMMAND_1(F_NAME, T_0, D_0)        \
	virtual void F_NAME(T_0 D_0) override; \
	void MERGE(_cmd_, F_NAME)(T_0 D_0)

#define COMMAND_2(F_NAME, T_0, D_0, T_1, D_1)       \
	virtual void F_NAME(T_0 D_0, T_1 D_1) override; \
	void MERGE(_cmd_, F_NAME)(T_0 D_0, T_1 D_1)

class GodotNavigationServer;

struct SetCommand {
	virtual ~SetCommand() {}
	virtual void exec(GodotNavigationServer *server) = 0;
};

class GodotNavigationServer : public NavigationServer3D {
	Mutex commands_mutex;
	/// Mutex used to make any operation threadsafe.
	Mutex operations_mutex;

	LocalVector<SetCommand *> commands;

	mutable RID_Owner<NavLink> link_owner;
	mutable RID_Owner<NavMap> map_owner;
	mutable RID_Owner<NavRegion> region_owner;
	mutable RID_Owner<NavAgent> agent_owner;

	bool active = true;
	LocalVector<NavMap *> active_maps;
	LocalVector<uint32_t> active_maps_update_id;

	// Performance Monitor
	int pm_region_count = 0;
	int pm_agent_count = 0;
	int pm_link_count = 0;
	int pm_polygon_count = 0;
	int pm_edge_count = 0;
	int pm_edge_merge_count = 0;
	int pm_edge_connection_count = 0;
	int pm_edge_free_count = 0;

public:
	GodotNavigationServer();
	virtual ~GodotNavigationServer();

	void add_command(SetCommand *command);

	virtual TypedArray<RID> get_maps() const override;

	virtual RID map_create() override;
	COMMAND_2(map_set_active, RID, p_map, bool, p_active);
	virtual bool map_is_active(RID p_map) const override;

	COMMAND_2(map_set_up, RID, p_map, Vector3, p_up);
	virtual Vector3 map_get_up(RID p_map) const override;

	COMMAND_2(map_set_cell_size, RID, p_map, real_t, p_cell_size);
	virtual real_t map_get_cell_size(RID p_map) const override;

	COMMAND_2(map_set_edge_connection_margin, RID, p_map, real_t, p_connection_margin);
	virtual real_t map_get_edge_connection_margin(RID p_map) const override;

	COMMAND_2(map_set_link_connection_radius, RID, p_map, real_t, p_connection_radius);
	virtual real_t map_get_link_connection_radius(RID p_map) const override;

	virtual Vector<Vector3> map_get_path(RID p_map, Vector3 p_origin, Vector3 p_destination, bool p_optimize, uint32_t p_navigation_layers = 1) const override;

	virtual Vector3 map_get_closest_point_to_segment(RID p_map, const Vector3 &p_from, const Vector3 &p_to, const bool p_use_collision = false) const override;
	virtual Vector3 map_get_closest_point(RID p_map, const Vector3 &p_point) const override;
	virtual Vector3 map_get_closest_point_normal(RID p_map, const Vector3 &p_point) const override;
	virtual RID map_get_closest_point_owner(RID p_map, const Vector3 &p_point) const override;

	virtual TypedArray<RID> map_get_links(RID p_map) const override;
	virtual TypedArray<RID> map_get_regions(RID p_map) const override;
	virtual TypedArray<RID> map_get_agents(RID p_map) const override;

	virtual void map_force_update(RID p_map) override;

	virtual RID region_create() override;

	COMMAND_2(region_set_enter_cost, RID, p_region, real_t, p_enter_cost);
	virtual real_t region_get_enter_cost(RID p_region) const override;
	COMMAND_2(region_set_travel_cost, RID, p_region, real_t, p_travel_cost);
	virtual real_t region_get_travel_cost(RID p_region) const override;

	COMMAND_2(region_set_owner_id, RID, p_region, ObjectID, p_owner_id);
	virtual ObjectID region_get_owner_id(RID p_region) const override;

	virtual bool region_owns_point(RID p_region, const Vector3 &p_point) const override;

	COMMAND_2(region_set_map, RID, p_region, RID, p_map);
	virtual RID region_get_map(RID p_region) const override;
	COMMAND_2(region_set_navigation_layers, RID, p_region, uint32_t, p_navigation_layers);
	virtual uint32_t region_get_navigation_layers(RID p_region) const override;
	COMMAND_2(region_set_transform, RID, p_region, Transform3D, p_transform);
	COMMAND_2(region_set_navigation_mesh, RID, p_region, Ref<NavigationMesh>, p_navigation_mesh);
	virtual void region_bake_navigation_mesh(Ref<NavigationMesh> p_navigation_mesh, Node *p_root_node) override;
	virtual int region_get_connections_count(RID p_region) const override;
	virtual Vector3 region_get_connection_pathway_start(RID p_region, int p_connection_id) const override;
	virtual Vector3 region_get_connection_pathway_end(RID p_region, int p_connection_id) const override;

	virtual RID link_create() override;
	COMMAND_2(link_set_map, RID, p_link, RID, p_map);
	virtual RID link_get_map(RID p_link) const override;
	COMMAND_2(link_set_bidirectional, RID, p_link, bool, p_bidirectional);
	virtual bool link_is_bidirectional(RID p_link) const override;
	COMMAND_2(link_set_navigation_layers, RID, p_link, uint32_t, p_navigation_layers);
	virtual uint32_t link_get_navigation_layers(RID p_link) const override;
	COMMAND_2(link_set_start_position, RID, p_link, Vector3, p_position);
	virtual Vector3 link_get_start_position(RID p_link) const override;
	COMMAND_2(link_set_end_position, RID, p_link, Vector3, p_position);
	virtual Vector3 link_get_end_position(RID p_link) const override;
	COMMAND_2(link_set_enter_cost, RID, p_link, real_t, p_enter_cost);
	virtual real_t link_get_enter_cost(RID p_link) const override;
	COMMAND_2(link_set_travel_cost, RID, p_link, real_t, p_travel_cost);
	virtual real_t link_get_travel_cost(RID p_link) const override;
	COMMAND_2(link_set_owner_id, RID, p_link, ObjectID, p_owner_id);
	virtual ObjectID link_get_owner_id(RID p_link) const override;

	virtual RID agent_create() override;
	COMMAND_2(agent_set_map, RID, p_agent, RID, p_map);
	virtual RID agent_get_map(RID p_agent) const override;
	COMMAND_2(agent_set_neighbor_distance, RID, p_agent, real_t, p_distance);
	COMMAND_2(agent_set_max_neighbors, RID, p_agent, int, p_count);
	COMMAND_2(agent_set_time_horizon, RID, p_agent, real_t, p_time);
	COMMAND_2(agent_set_radius, RID, p_agent, real_t, p_radius);
	COMMAND_2(agent_set_max_speed, RID, p_agent, real_t, p_max_speed);
	COMMAND_2(agent_set_velocity, RID, p_agent, Vector3, p_velocity);
	COMMAND_2(agent_set_target_velocity, RID, p_agent, Vector3, p_velocity);
	COMMAND_2(agent_set_position, RID, p_agent, Vector3, p_position);
	COMMAND_2(agent_set_ignore_y, RID, p_agent, bool, p_ignore);
	virtual bool agent_is_map_changed(RID p_agent) const override;
	COMMAND_2(agent_set_callback, RID, p_agent, Callable, p_callback);

	COMMAND_1(free, RID, p_object);

	virtual void set_active(bool p_active) override;

	void flush_queries();
	virtual void process(real_t p_delta_time) override;

	virtual NavigationUtilities::PathQueryResult _query_path(const NavigationUtilities::PathQueryParameters &p_parameters) const override;

	int get_process_info(ProcessInfo p_info) const override;
};

#undef COMMAND_1
#undef COMMAND_2

#endif // GODOT_NAVIGATION_SERVER_H
