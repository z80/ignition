/**************************************************************************/
/*  directory_create_dialog.cpp                                           */
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

#include "directory_create_dialog.h"

#include "core/io/dir_access.h"
#include "editor/editor_node.h"
#include "editor/editor_scale.h"
#include "scene/gui/box_container.h"
#include "scene/gui/label.h"
#include "scene/gui/line_edit.h"
#include "scene/gui/panel_container.h"

static String sanitize_input(const String &p_path) {
	String path = p_path.strip_edges();
	if (path.ends_with("/")) {
		path = path.left(path.length() - 1);
	}
	return path;
}

String DirectoryCreateDialog::_validate_path(const String &p_path) const {
	if (p_path.is_empty()) {
		return TTR("Folder name cannot be empty.");
	}

	for (const String &part : p_path.split("/")) {
		if (part.is_empty()) {
			return TTR("Folder name cannot be empty.");
		}
		if (p_path.contains("\\") || p_path.contains(":") || p_path.contains("*") ||
				p_path.contains("|") || p_path.contains(">") || p_path.ends_with(".") || p_path.ends_with(" ")) {
			return TTR("Folder name contains invalid characters.");
		}
	}

	Ref<DirAccess> da = DirAccess::create(DirAccess::ACCESS_RESOURCES);
	da->change_dir(base_dir);
	if (da->file_exists(p_path)) {
		return TTR("File with that name already exists.");
	}
	if (da->dir_exists(p_path)) {
		return TTR("Folder with that name already exists.");
	}

	return String();
}

void DirectoryCreateDialog::_on_dir_path_changed(const String &p_text) {
	const String path = sanitize_input(p_text);
	const String error = _validate_path(path);

	if (error.is_empty()) {
		status_label->add_theme_color_override("font_color", get_theme_color(SNAME("success_color"), SNAME("Editor")));

		if (path.contains("/")) {
			status_label->set_text(TTR("Using slashes in folder names will create subfolders recursively."));
		} else {
			status_label->set_text(TTR("Folder name is valid."));
		}
	} else {
		status_label->add_theme_color_override("font_color", get_theme_color(SNAME("error_color"), SNAME("Editor")));
		status_label->set_text(error);
	}

	get_ok_button()->set_disabled(!error.is_empty());
}

void DirectoryCreateDialog::ok_pressed() {
	const String path = sanitize_input(dir_path->get_text());

	// The OK button should be disabled if the path is invalid, but just in case.
	const String error = _validate_path(path);
	ERR_FAIL_COND_MSG(!error.is_empty(), error);

	Error err;
	Ref<DirAccess> da = DirAccess::create(DirAccess::ACCESS_RESOURCES);

	err = da->change_dir(base_dir);
	ERR_FAIL_COND_MSG(err != OK, "Cannot open directory '" + base_dir + "'.");

	print_verbose("Making folder " + path + " in " + base_dir);
	err = da->make_dir_recursive(path);

	if (err == OK) {
		emit_signal(SNAME("dir_created"));
	} else {
		EditorNode::get_singleton()->show_warning(TTR("Could not create folder."));
	}
	hide();
}

void DirectoryCreateDialog::_post_popup() {
	ConfirmationDialog::_post_popup();
	dir_path->grab_focus();
}

void DirectoryCreateDialog::config(const String &p_base_dir) {
	base_dir = p_base_dir;
	label->set_text(vformat(TTR("Create new folder in %s:"), base_dir));
	dir_path->set_text("new folder");
	dir_path->select_all();
	_on_dir_path_changed(dir_path->get_text());
}

void DirectoryCreateDialog::_bind_methods() {
	ADD_SIGNAL(MethodInfo("dir_created"));
}

void DirectoryCreateDialog::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_THEME_CHANGED: {
			status_panel->add_theme_style_override("panel", get_theme_stylebox(SNAME("panel"), SNAME("Tree")));
		} break;
	}
}

DirectoryCreateDialog::DirectoryCreateDialog() {
	set_title(TTR("Create Folder"));
	set_min_size(Size2i(480, 0) * EDSCALE);

	VBoxContainer *vb = memnew(VBoxContainer);
	add_child(vb);

	label = memnew(Label);
	label->set_text_overrun_behavior(TextServer::OVERRUN_TRIM_WORD_ELLIPSIS);
	vb->add_child(label);

	dir_path = memnew(LineEdit);
	dir_path->connect("text_changed", callable_mp(this, &DirectoryCreateDialog::_on_dir_path_changed));
	vb->add_child(dir_path);
	register_text_enter(dir_path);

	Control *spacing = memnew(Control);
	spacing->set_custom_minimum_size(Size2(0, 10 * EDSCALE));
	vb->add_child(spacing);

	status_panel = memnew(PanelContainer);
	status_panel->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	vb->add_child(status_panel);

	status_label = memnew(Label);
	status_label->set_v_size_flags(Control::SIZE_EXPAND_FILL);
	status_panel->add_child(status_label);
}
