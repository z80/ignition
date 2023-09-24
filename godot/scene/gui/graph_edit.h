/**************************************************************************/
/*  graph_edit.h                                                          */
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

#ifndef GRAPH_EDIT_H
#define GRAPH_EDIT_H

#include "scene/gui/box_container.h"
#include "scene/gui/button.h"
#include "scene/gui/graph_node.h"
#include "scene/gui/label.h"
#include "scene/gui/scroll_bar.h"
#include "scene/gui/spin_box.h"

class GraphEdit;
class ViewPanner;

class GraphEditFilter : public Control {
	GDCLASS(GraphEditFilter, Control);

	friend class GraphEdit;
	friend class GraphEditMinimap;
	GraphEdit *ge = nullptr;
	virtual bool has_point(const Point2 &p_point) const override;

public:
	GraphEditFilter(GraphEdit *p_edit);
};

class GraphEditMinimap : public Control {
	GDCLASS(GraphEditMinimap, Control);

	friend class GraphEdit;
	friend class GraphEditFilter;
	GraphEdit *ge = nullptr;

protected:
public:
	GraphEditMinimap(GraphEdit *p_edit);

	virtual CursorShape get_cursor_shape(const Point2 &p_pos = Point2i()) const override;

	void update_minimap();
	Rect2 get_camera_rect();

private:
	Vector2 minimap_padding;
	Vector2 minimap_offset;
	Vector2 graph_proportions;
	Vector2 graph_padding;
	Vector2 camera_position;
	Vector2 camera_size;

	bool is_pressing;
	bool is_resizing;

	Vector2 _get_render_size();
	Vector2 _get_graph_offset();
	Vector2 _get_graph_size();

	Vector2 _convert_from_graph_position(const Vector2 &p_position);
	Vector2 _convert_to_graph_position(const Vector2 &p_position);

	virtual void gui_input(const Ref<InputEvent> &p_ev) override;

	void _adjust_graph_scroll(const Vector2 &p_offset);
};

class GraphEdit : public Control {
	GDCLASS(GraphEdit, Control);

public:
	struct Connection {
		StringName from;
		StringName to;
		int from_port = 0;
		int to_port = 0;
		float activity = 0.0;
	};

	// Should be in sync with ControlScheme in ViewPanner.
	enum PanningScheme {
		SCROLL_ZOOMS,
		SCROLL_PANS,
	};

private:
	Label *zoom_label = nullptr;
	Button *zoom_minus = nullptr;
	Button *zoom_reset = nullptr;
	Button *zoom_plus = nullptr;

	Button *snap_button = nullptr;
	SpinBox *snap_amount = nullptr;

	Button *minimap_button = nullptr;

	Button *layout_button = nullptr;

	HScrollBar *h_scroll = nullptr;
	VScrollBar *v_scroll = nullptr;

	float port_hotzone_inner_extent = 0.0;
	float port_hotzone_outer_extent = 0.0;

	Ref<ViewPanner> panner;
	bool warped_panning = true;
	void _pan_callback(Vector2 p_scroll_vec, Ref<InputEvent> p_event);
	void _zoom_callback(float p_zoom_factor, Vector2 p_origin, Ref<InputEvent> p_event);

	bool arrange_nodes_button_hidden = false;

	bool connecting = false;
	StringName connecting_from;
	bool connecting_out = false;
	int connecting_index = 0;
	int connecting_type = 0;
	Color connecting_color;
	bool connecting_target = false;
	Vector2 connecting_to;
	StringName connecting_target_to;
	int connecting_target_index = 0;
	bool just_disconnected = false;
	bool connecting_valid = false;
	Vector2 click_pos;

	PanningScheme panning_scheme = SCROLL_ZOOMS;
	bool dragging = false;
	bool just_selected = false;
	bool moving_selection = false;
	Vector2 drag_accum;

	float zoom = 1.0;
	float zoom_step = 1.2;
	// Proper values set in constructor.
	float zoom_min = 0.0;
	float zoom_max = 0.0;

	void _zoom_minus();
	void _zoom_reset();
	void _zoom_plus();
	void _update_zoom_label();

	bool box_selecting = false;
	bool box_selection_mode_additive = false;
	Point2 box_selecting_from;
	Point2 box_selecting_to;
	Rect2 box_selecting_rect;
	List<GraphNode *> previous_selected;

	bool setting_scroll_ofs = false;
	bool right_disconnects = false;
	bool updating = false;
	bool awaiting_scroll_offset_update = false;
	List<Connection> connections;

	float lines_thickness = 2.0f;
	float lines_curvature = 0.5f;
	bool lines_antialiased = true;

	PackedVector2Array get_connection_line(const Vector2 &p_from, const Vector2 &p_to);
	void _draw_connection_line(CanvasItem *p_where, const Vector2 &p_from, const Vector2 &p_to, const Color &p_color, const Color &p_to_color, float p_width, float p_zoom);

	void _graph_node_selected(Node *p_gn);
	void _graph_node_deselected(Node *p_gn);
	void _graph_node_raised(Node *p_gn);
	void _graph_node_moved(Node *p_gn);
	void _graph_node_slot_updated(int p_index, Node *p_gn);

	void _update_scroll();
	void _scroll_moved(double);
	virtual void gui_input(const Ref<InputEvent> &p_ev) override;

	Control *connections_layer = nullptr;
	GraphEditFilter *top_layer = nullptr;
	GraphEditMinimap *minimap = nullptr;
	void _top_layer_input(const Ref<InputEvent> &p_ev);

	bool is_in_input_hotzone(GraphNode *p_node, int p_port, const Vector2 &p_mouse_pos, const Vector2i &p_port_size);
	bool is_in_output_hotzone(GraphNode *p_node, int p_port, const Vector2 &p_mouse_pos, const Vector2i &p_port_size);
	bool is_in_port_hotzone(const Vector2 &pos, const Vector2 &p_mouse_pos, const Vector2i &p_port_size, bool p_left);

	void _top_layer_draw();
	void _connections_layer_draw();
	void _minimap_draw();
	void _update_scroll_offset();

	TypedArray<Dictionary> _get_connection_list() const;

	bool lines_on_bg = false;

	struct ConnType {
		union {
			struct {
				uint32_t type_a;
				uint32_t type_b;
			};
			uint64_t key = 0;
		};

		static uint32_t hash(const ConnType &p_conn) {
			return hash_one_uint64(p_conn.key);
		}
		bool operator==(const ConnType &p_type) const {
			return key == p_type.key;
		}

		ConnType(uint32_t a = 0, uint32_t b = 0) {
			type_a = a;
			type_b = b;
		}
	};

	HashSet<ConnType, ConnType> valid_connection_types;
	HashSet<int> valid_left_disconnect_types;
	HashSet<int> valid_right_disconnect_types;

	HashMap<StringName, Vector<GraphNode *>> comment_enclosed_nodes;
	void _update_comment_enclosed_nodes_list(GraphNode *p_node, HashMap<StringName, Vector<GraphNode *>> &p_comment_enclosed_nodes);
	void _set_drag_comment_enclosed_nodes(GraphNode *p_node, HashMap<StringName, Vector<GraphNode *>> &p_comment_enclosed_nodes, bool p_drag);
	void _set_position_of_comment_enclosed_nodes(GraphNode *p_node, HashMap<StringName, Vector<GraphNode *>> &p_comment_enclosed_nodes, Vector2 p_pos);

	HBoxContainer *zoom_hb = nullptr;

	friend class GraphEditFilter;
	bool _filter_input(const Point2 &p_point);
	void _snap_toggled();
	void _snap_value_changed(double);

	friend class GraphEditMinimap;
	void _minimap_toggled();

	bool _check_clickable_control(Control *p_control, const Vector2 &r_mouse_pos, const Vector2 &p_offset);

	bool arranging_graph = false;

	enum SET_OPERATIONS {
		IS_EQUAL,
		IS_SUBSET,
		DIFFERENCE,
		UNION,
	};

	int _set_operations(SET_OPERATIONS p_operation, HashSet<StringName> &r_u, const HashSet<StringName> &r_v);
	HashMap<int, Vector<StringName>> _layering(const HashSet<StringName> &r_selected_nodes, const HashMap<StringName, HashSet<StringName>> &r_upper_neighbours);
	Vector<StringName> _split(const Vector<StringName> &r_layer, const HashMap<StringName, Dictionary> &r_crossings);
	void _horizontal_alignment(Dictionary &r_root, Dictionary &r_align, const HashMap<int, Vector<StringName>> &r_layers, const HashMap<StringName, HashSet<StringName>> &r_upper_neighbours, const HashSet<StringName> &r_selected_nodes);
	void _crossing_minimisation(HashMap<int, Vector<StringName>> &r_layers, const HashMap<StringName, HashSet<StringName>> &r_upper_neighbours);
	void _calculate_inner_shifts(Dictionary &r_inner_shifts, const Dictionary &r_root, const Dictionary &r_node_names, const Dictionary &r_align, const HashSet<StringName> &r_block_heads, const HashMap<StringName, Pair<int, int>> &r_port_info);
	float _calculate_threshold(StringName p_v, StringName p_w, const Dictionary &r_node_names, const HashMap<int, Vector<StringName>> &r_layers, const Dictionary &r_root, const Dictionary &r_align, const Dictionary &r_inner_shift, real_t p_current_threshold, const HashMap<StringName, Vector2> &r_node_positions);
	void _place_block(StringName p_v, float p_delta, const HashMap<int, Vector<StringName>> &r_layers, const Dictionary &r_root, const Dictionary &r_align, const Dictionary &r_node_name, const Dictionary &r_inner_shift, Dictionary &r_sink, Dictionary &r_shift, HashMap<StringName, Vector2> &r_node_positions);

protected:
	static void _bind_methods();
	virtual void add_child_notify(Node *p_child) override;
	virtual void remove_child_notify(Node *p_child) override;
	void _notification(int p_what);

	GDVIRTUAL2RC(Vector<Vector2>, _get_connection_line, Vector2, Vector2)
	GDVIRTUAL3R(bool, _is_in_input_hotzone, Object *, int, Vector2)
	GDVIRTUAL3R(bool, _is_in_output_hotzone, Object *, int, Vector2)
	GDVIRTUAL4R(bool, _is_node_hover_valid, StringName, int, StringName, int);

public:
	virtual CursorShape get_cursor_shape(const Point2 &p_pos = Point2i()) const override;

	PackedStringArray get_configuration_warnings() const override;

	Error connect_node(const StringName &p_from, int p_from_port, const StringName &p_to, int p_to_port);
	bool is_node_connected(const StringName &p_from, int p_from_port, const StringName &p_to, int p_to_port);
	void disconnect_node(const StringName &p_from, int p_from_port, const StringName &p_to, int p_to_port);
	void clear_connections();
	void force_connection_drag_end();
	virtual bool is_node_hover_valid(const StringName &p_from, int p_from_port, const StringName &p_to, int p_to_port);

	void set_connection_activity(const StringName &p_from, int p_from_port, const StringName &p_to, int p_to_port, float p_activity);

	void add_valid_connection_type(int p_type, int p_with_type);
	void remove_valid_connection_type(int p_type, int p_with_type);
	bool is_valid_connection_type(int p_type, int p_with_type) const;

	void set_panning_scheme(PanningScheme p_scheme);
	PanningScheme get_panning_scheme() const;

	void set_zoom(float p_zoom);
	void set_zoom_custom(float p_zoom, const Vector2 &p_center);
	float get_zoom() const;

	void set_zoom_min(float p_zoom_min);
	float get_zoom_min() const;

	void set_zoom_max(float p_zoom_max);
	float get_zoom_max() const;

	void set_zoom_step(float p_zoom_step);
	float get_zoom_step() const;

	void set_show_zoom_label(bool p_enable);
	bool is_showing_zoom_label() const;

	void set_minimap_size(Vector2 p_size);
	Vector2 get_minimap_size() const;
	void set_minimap_opacity(float p_opacity);
	float get_minimap_opacity() const;

	void set_minimap_enabled(bool p_enable);
	bool is_minimap_enabled() const;

	void set_arrange_nodes_button_hidden(bool p_enable);
	bool is_arrange_nodes_button_hidden() const;

	GraphEditFilter *get_top_layer() const { return top_layer; }
	GraphEditMinimap *get_minimap() const { return minimap; }
	void get_connection_list(List<Connection> *r_connections) const;

	void set_right_disconnects(bool p_enable);
	bool is_right_disconnects_enabled() const;

	void add_valid_right_disconnect_type(int p_type);
	void remove_valid_right_disconnect_type(int p_type);

	void add_valid_left_disconnect_type(int p_type);
	void remove_valid_left_disconnect_type(int p_type);

	void set_scroll_ofs(const Vector2 &p_ofs);
	Vector2 get_scroll_ofs() const;

	void set_selected(Node *p_child);

	void set_use_snap(bool p_enable);
	bool is_using_snap() const;

	int get_snap() const;
	void set_snap(int p_snap);

	void set_connection_lines_curvature(float p_curvature);
	float get_connection_lines_curvature() const;

	void set_connection_lines_thickness(float p_thickness);
	float get_connection_lines_thickness() const;

	void set_connection_lines_antialiased(bool p_antialiased);
	bool is_connection_lines_antialiased() const;

	HBoxContainer *get_zoom_hbox();
	Ref<ViewPanner> get_panner();
	void set_warped_panning(bool p_warped);

	void arrange_nodes();

	GraphEdit();
};

VARIANT_ENUM_CAST(GraphEdit::PanningScheme);

#endif // GRAPH_EDIT_H
