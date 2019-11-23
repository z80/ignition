/*************************************************************************/
/*  script_create_dialog.h                                               */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2019 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2019 Godot Engine contributors (cf. AUTHORS.md)    */
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

#ifndef SCRIPT_CREATE_DIALOG_H
#define SCRIPT_CREATE_DIALOG_H

#include "editor/editor_file_dialog.h"
#include "editor/editor_settings.h"
#include "scene/gui/check_button.h"
#include "scene/gui/dialogs.h"
#include "scene/gui/grid_container.h"
#include "scene/gui/line_edit.h"
#include "scene/gui/option_button.h"
#include "scene/gui/panel_container.h"

class ScriptCreateDialog : public ConfirmationDialog {
	GDCLASS(ScriptCreateDialog, ConfirmationDialog);

	LineEdit *class_name;
	Label *error_label;
	Label *path_error_label;
	PanelContainer *status_panel;
	LineEdit *parent_name;
	Button *parent_browse_button;
	OptionButton *language_menu;
	OptionButton *template_menu;
	LineEdit *file_path;
	Button *path_button;
	EditorFileDialog *file_browse;
	CheckButton *internal;
	VBoxContainer *path_vb;
	AcceptDialog *alert;
	bool path_valid;
	bool create_new;
	bool is_browsing_parent;
	String initial_bp;
	bool is_new_script_created;
	bool is_path_valid;
	bool has_named_classes;
	bool supports_built_in;
	bool can_inherit_from_file;
	bool is_parent_name_valid;
	bool is_class_name_valid;
	bool is_built_in;
	bool built_in_enabled;
	int current_language;
	bool re_check_path;
	String script_template;
	Vector<String> template_list;

	bool _can_be_built_in();
	void _path_changed(const String &p_path = String());
	void _path_entered(const String &p_path = String());
	void _lang_changed(int l = 0);
	void _built_in_pressed();
	bool _validate(const String &p_string);
	void _class_name_changed(const String &p_name);
	void _parent_name_changed(const String &p_parent);
	void _template_changed(int p_template = 0);
	void _browse_path(bool browse_parent, bool p_save);
	void _file_selected(const String &p_file);
	virtual void ok_pressed();
	void _create_new();
	void _load_exist();
	void _msg_script_valid(bool valid, const String &p_msg = String());
	void _msg_path_valid(bool valid, const String &p_msg = String());
	void _update_dialog();

protected:
	void _notification(int p_what);
	static void _bind_methods();

public:
	void config(const String &p_base_name, const String &p_base_path, bool p_built_in_enabled = true);
	ScriptCreateDialog();
};

#endif // SCRIPT_CREATE_DIALOG_H
