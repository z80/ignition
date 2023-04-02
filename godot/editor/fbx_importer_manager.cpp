/**************************************************************************/
/*  fbx_importer_manager.cpp                                              */
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

#include "fbx_importer_manager.h"

#include "editor/editor_scale.h"
#include "editor/editor_settings.h"
#include "scene/gui/link_button.h"

void FBXImporterManager::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_READY: {
			connect("confirmed", callable_mp(this, &FBXImporterManager::_path_confirmed));
		} break;
	}
}

void FBXImporterManager::show_dialog(bool p_exclusive) {
	String fbx2gltf_path = EDITOR_GET("filesystem/import/fbx/fbx2gltf_path");
	fbx_path->set_text(fbx2gltf_path);
	_validate_path(fbx2gltf_path);

	set_flag(Window::FLAG_BORDERLESS, p_exclusive); // Avoid closing accidentally .
	set_close_on_escape(!p_exclusive);

	popup_centered();
}

void FBXImporterManager::_validate_path(const String &p_path) {
	String error;
	bool success = false;

	if (p_path == "") {
		error = TTR("Path to FBX2glTF executable is empty.");
	} else if (!FileAccess::exists(p_path)) {
		error = TTR("Path to FBX2glTF executable is invalid.");
	} else {
		List<String> args;
		args.push_back("--version");
		int exitcode;
		Error err = OS::get_singleton()->execute(p_path, args, nullptr, &exitcode);

		if (err == OK && exitcode == 0) {
			success = true;
		} else {
			error = TTR("Error executing this file (wrong version or architecture).");
		}
	}

	if (success) {
		path_status->set_text(TTR("FBX2glTF executable is valid."));
		path_status->add_theme_color_override("font_color", path_status->get_theme_color(SNAME("success_color"), SNAME("Editor")));
		get_ok_button()->set_disabled(false);
	} else {
		path_status->set_text(error);
		path_status->add_theme_color_override("font_color", path_status->get_theme_color(SNAME("error_color"), SNAME("Editor")));
		get_ok_button()->set_disabled(true);
	}
}

void FBXImporterManager::_select_file(const String &p_path) {
	fbx_path->set_text(p_path);
	_validate_path(p_path);
}

void FBXImporterManager::_path_confirmed() {
	String path = fbx_path->get_text();
	EditorSettings::get_singleton()->set("filesystem/import/fbx/fbx2gltf_path", path);
	EditorSettings::get_singleton()->save();
}

void FBXImporterManager::_browse_install() {
	if (fbx_path->get_text() != String()) {
		browse_dialog->set_current_file(fbx_path->get_text());
	}

	browse_dialog->popup_centered_ratio();
}

FBXImporterManager *FBXImporterManager::singleton = nullptr;

FBXImporterManager::FBXImporterManager() {
	singleton = this;

	set_title(TTR("Configure FBX Importer"));

	VBoxContainer *vb = memnew(VBoxContainer);
	vb->add_child(memnew(Label(TTR("FBX2glTF is required for importing FBX files.\nPlease download it and provide a valid path to the binary:"))));
	LinkButton *lb = memnew(LinkButton);
	lb->set_text(TTR("Click this link to download FBX2glTF"));
	lb->set_uri("https://godotengine.org/fbx-import");
	vb->add_child(lb);

	HBoxContainer *hb = memnew(HBoxContainer);

	fbx_path = memnew(LineEdit);
	fbx_path->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	hb->add_child(fbx_path);
	fbx_path_browse = memnew(Button);
	hb->add_child(fbx_path_browse);
	fbx_path_browse->set_text(TTR("Browse"));
	fbx_path_browse->connect("pressed", callable_mp(this, &FBXImporterManager::_browse_install));
	hb->set_h_size_flags(Control::SIZE_EXPAND_FILL);
	hb->set_custom_minimum_size(Size2(400 * EDSCALE, 0));

	vb->add_child(hb);

	path_status = memnew(Label);
	vb->add_child(path_status);

	add_child(vb);

	fbx_path->connect("text_changed", callable_mp(this, &FBXImporterManager::_validate_path));

	get_ok_button()->set_text(TTR("Confirm Path"));

	browse_dialog = memnew(EditorFileDialog);
	browse_dialog->set_access(EditorFileDialog::ACCESS_FILESYSTEM);
	browse_dialog->set_file_mode(EditorFileDialog::FILE_MODE_OPEN_FILE);
#ifdef WINDOWS_ENABLED
	browse_dialog->add_filter("*.exe");
#endif

	browse_dialog->connect("file_selected", callable_mp(this, &FBXImporterManager::_select_file));

	add_child(browse_dialog);
}
