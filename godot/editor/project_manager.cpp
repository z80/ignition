/*************************************************************************/
/*  project_manager.cpp                                                  */
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

#include "project_manager.h"

#include "core/io/config_file.h"
#include "core/io/resource_saver.h"
#include "core/io/stream_peer_ssl.h"
#include "core/io/zip_io.h"
#include "core/os/dir_access.h"
#include "core/os/file_access.h"
#include "core/os/keyboard.h"
#include "core/os/os.h"
#include "core/translation.h"
#include "core/version.h"
#include "core/version_hash.gen.h"
#include "editor_scale.h"
#include "editor_settings.h"
#include "editor_themes.h"
#include "scene/gui/center_container.h"
#include "scene/gui/line_edit.h"
#include "scene/gui/margin_container.h"
#include "scene/gui/panel_container.h"
#include "scene/gui/separator.h"
#include "scene/gui/texture_rect.h"
#include "scene/gui/tool_button.h"

class ProjectDialog : public ConfirmationDialog {

	GDCLASS(ProjectDialog, ConfirmationDialog);

public:
	enum Mode {
		MODE_NEW,
		MODE_IMPORT,
		MODE_INSTALL,
		MODE_RENAME
	};

private:
	enum MessageType {
		MESSAGE_ERROR,
		MESSAGE_WARNING,
		MESSAGE_SUCCESS
	};

	enum InputType {
		PROJECT_PATH,
		INSTALL_PATH
	};

	Mode mode;
	Button *browse;
	Button *install_browse;
	Button *create_dir;
	Container *name_container;
	Container *path_container;
	Container *install_path_container;
	Container *rasterizer_container;
	Ref<ButtonGroup> rasterizer_button_group;
	Label *msg;
	LineEdit *project_path;
	LineEdit *project_name;
	LineEdit *install_path;
	TextureRect *status_rect;
	TextureRect *install_status_rect;
	FileDialog *fdialog;
	FileDialog *fdialog_install;
	String zip_path;
	String zip_title;
	AcceptDialog *dialog_error;
	String fav_dir;

	String created_folder_path;

	void set_message(const String &p_msg, MessageType p_type = MESSAGE_SUCCESS, InputType input_type = PROJECT_PATH) {

		msg->set_text(p_msg);
		Ref<Texture> current_path_icon = status_rect->get_texture();
		Ref<Texture> current_install_icon = install_status_rect->get_texture();
		Ref<Texture> new_icon;

		switch (p_type) {

			case MESSAGE_ERROR: {

				msg->add_color_override("font_color", get_color("error_color", "Editor"));
				msg->set_modulate(Color(1, 1, 1, 1));
				new_icon = get_icon("StatusError", "EditorIcons");

			} break;
			case MESSAGE_WARNING: {

				msg->add_color_override("font_color", get_color("warning_color", "Editor"));
				msg->set_modulate(Color(1, 1, 1, 1));
				new_icon = get_icon("StatusWarning", "EditorIcons");

			} break;
			case MESSAGE_SUCCESS: {

				msg->set_modulate(Color(1, 1, 1, 0));
				new_icon = get_icon("StatusSuccess", "EditorIcons");

			} break;
		}

		if (current_path_icon != new_icon && input_type == PROJECT_PATH) {
			status_rect->set_texture(new_icon);
		} else if (current_install_icon != new_icon && input_type == INSTALL_PATH) {
			install_status_rect->set_texture(new_icon);
		}

		set_size(Size2(500, 0) * EDSCALE);
	}

	String _test_path() {

		DirAccess *d = DirAccess::create(DirAccess::ACCESS_FILESYSTEM);
		String valid_path, valid_install_path;
		if (d->change_dir(project_path->get_text()) == OK) {
			valid_path = project_path->get_text();
		} else if (d->change_dir(project_path->get_text().strip_edges()) == OK) {
			valid_path = project_path->get_text().strip_edges();
		} else if (project_path->get_text().ends_with(".zip")) {
			if (d->file_exists(project_path->get_text())) {
				valid_path = project_path->get_text();
			}
		} else if (project_path->get_text().strip_edges().ends_with(".zip")) {
			if (d->file_exists(project_path->get_text().strip_edges())) {
				valid_path = project_path->get_text().strip_edges();
			}
		}

		if (valid_path == "") {
			set_message(TTR("The path does not exist."), MESSAGE_ERROR);
			memdelete(d);
			get_ok()->set_disabled(true);
			return "";
		}

		if (mode == MODE_IMPORT && valid_path.ends_with(".zip")) {
			if (d->change_dir(install_path->get_text()) == OK) {
				valid_install_path = install_path->get_text();
			} else if (d->change_dir(install_path->get_text().strip_edges()) == OK) {
				valid_install_path = install_path->get_text().strip_edges();
			}

			if (valid_install_path == "") {
				set_message(TTR("The path does not exist."), MESSAGE_ERROR, INSTALL_PATH);
				memdelete(d);
				get_ok()->set_disabled(true);
				return "";
			}
		}

		if (mode == MODE_IMPORT || mode == MODE_RENAME) {

			if (valid_path != "" && !d->file_exists("project.godot")) {

				if (valid_path.ends_with(".zip")) {
					FileAccess *src_f = NULL;
					zlib_filefunc_def io = zipio_create_io_from_file(&src_f);

					unzFile pkg = unzOpen2(valid_path.utf8().get_data(), &io);
					if (!pkg) {

						set_message(TTR("Error opening package file, not in zip format."), MESSAGE_ERROR);
						memdelete(d);
						get_ok()->set_disabled(true);
						unzClose(pkg);
						return "";
					}

					int ret = unzGoToFirstFile(pkg);
					while (ret == UNZ_OK) {
						unz_file_info info;
						char fname[16384];
						ret = unzGetCurrentFileInfo(pkg, &info, fname, 16384, NULL, 0, NULL, 0);

						if (String(fname).ends_with("project.godot")) {
							break;
						}

						ret = unzGoToNextFile(pkg);
					}

					if (ret == UNZ_END_OF_LIST_OF_FILE) {
						set_message(TTR("Invalid '.zip' project file, does not contain a 'project.godot' file."), MESSAGE_ERROR);
						memdelete(d);
						get_ok()->set_disabled(true);
						unzClose(pkg);
						return "";
					}

					unzClose(pkg);

					// check if the specified install folder is empty, even though this is not an error, it is good to check here
					d->list_dir_begin();
					bool is_empty = true;
					String n = d->get_next();
					while (n != String()) {
						if (n != "." && n != "..") {
							is_empty = false;
							break;
						}
						n = d->get_next();
					}
					d->list_dir_end();

					if (!is_empty) {

						set_message(TTR("Please choose an empty folder."), MESSAGE_WARNING, INSTALL_PATH);
						memdelete(d);
						get_ok()->set_disabled(true);
						return "";
					}

				} else {
					set_message(TTR("Please choose a 'project.godot' or '.zip' file."), MESSAGE_ERROR);
					memdelete(d);
					install_path_container->hide();
					get_ok()->set_disabled(true);
					return "";
				}

			} else if (valid_path.ends_with("zip")) {

				set_message(TTR("Directory already contains a Godot project."), MESSAGE_ERROR, INSTALL_PATH);
				memdelete(d);
				get_ok()->set_disabled(true);
				return "";
			}

		} else {

			// check if the specified folder is empty, even though this is not an error, it is good to check here
			d->list_dir_begin();
			bool is_empty = true;
			String n = d->get_next();
			while (n != String()) {
				if (n != "." && n != "..") { // i don't know if this is enough to guarantee an empty dir
					is_empty = false;
					break;
				}
				n = d->get_next();
			}
			d->list_dir_end();

			if (!is_empty) {

				set_message(TTR("Please choose an empty folder."), MESSAGE_ERROR);
				memdelete(d);
				get_ok()->set_disabled(true);
				return "";
			}
		}

		set_message("");
		set_message("", MESSAGE_SUCCESS, INSTALL_PATH);
		memdelete(d);
		get_ok()->set_disabled(false);
		return valid_path;
	}

	void _path_text_changed(const String &p_path) {

		String sp = _test_path();
		if (sp != "") {

			// set the project name to the select folder name
			if (project_name->get_text() == "") {
				sp = sp.replace("\\", "/");
				int lidx = sp.find_last("/");

				if (lidx != -1) {
					sp = sp.substr(lidx + 1, sp.length());
				}
				if (sp == "" && mode == MODE_IMPORT)
					sp = TTR("Imported Project");

				project_name->set_text(sp);
				_text_changed(sp);
			}
		}

		if (created_folder_path != "" && created_folder_path != p_path) {
			_remove_created_folder();
		}
	}

	void _file_selected(const String &p_path) {

		String p = p_path;
		if (mode == MODE_IMPORT) {
			if (p.ends_with("project.godot")) {
				p = p.get_base_dir();
				install_path_container->hide();
				get_ok()->set_disabled(false);
			} else if (p.ends_with(".zip")) {
				install_path->set_text(p.get_base_dir());
				install_path_container->show();
				get_ok()->set_disabled(false);
			} else {
				set_message(TTR("Please choose a 'project.godot' or '.zip' file."), MESSAGE_ERROR);
				get_ok()->set_disabled(true);
				return;
			}
		}
		String sp = p.simplify_path();
		project_path->set_text(sp);
		_path_text_changed(sp);
		if (p.ends_with(".zip")) {
			install_path->call_deferred("grab_focus");
		} else {
			get_ok()->call_deferred("grab_focus");
		}
	}

	void _path_selected(const String &p_path) {

		String p = p_path;
		String sp = p.simplify_path();
		project_path->set_text(sp);
		_path_text_changed(sp);
		get_ok()->call_deferred("grab_focus");
	}

	void _install_path_selected(const String &p_path) {
		String p = p_path;
		String sp = p.simplify_path();
		install_path->set_text(sp);
		_path_text_changed(sp);
		get_ok()->call_deferred("grab_focus");
	}

	void _browse_path() {

		fdialog->set_current_dir(project_path->get_text());

		if (mode == MODE_IMPORT) {

			fdialog->set_mode(FileDialog::MODE_OPEN_FILE);
			fdialog->clear_filters();
			fdialog->add_filter("project.godot ; " VERSION_NAME " Project");
			fdialog->add_filter("*.zip ; Zip File");
		} else {
			fdialog->set_mode(FileDialog::MODE_OPEN_DIR);
		}
		fdialog->popup_centered_ratio();
	}

	void _browse_install_path() {
		fdialog_install->set_current_dir(install_path->get_text());
		fdialog_install->set_mode(FileDialog::MODE_OPEN_DIR);
		fdialog_install->popup_centered_ratio();
	}

	void _create_folder() {

		if (project_name->get_text() == "" || created_folder_path != "" || project_name->get_text().ends_with(".") || project_name->get_text().ends_with(" ")) {
			set_message(TTR("Invalid Project Name."), MESSAGE_WARNING);
			return;
		}

		DirAccess *d = DirAccess::create(DirAccess::ACCESS_FILESYSTEM);
		if (d->change_dir(project_path->get_text()) == OK) {

			if (!d->dir_exists(project_name->get_text())) {

				if (d->make_dir(project_name->get_text()) == OK) {

					d->change_dir(project_name->get_text());
					String dir_str = d->get_current_dir();
					project_path->set_text(dir_str);
					_path_text_changed(dir_str);
					created_folder_path = d->get_current_dir();
					create_dir->set_disabled(true);
				} else {

					dialog_error->set_text(TTR("Couldn't create folder."));
					dialog_error->popup_centered_minsize();
				}
			} else {

				dialog_error->set_text(TTR("There is already a folder in this path with the specified name."));
				dialog_error->popup_centered_minsize();
			}
		}

		memdelete(d);
	}

	void _text_changed(const String &p_text) {

		if (mode != MODE_NEW)
			return;

		_test_path();

		if (p_text == "")
			set_message(TTR("It would be a good idea to name your project."), MESSAGE_WARNING);
	}

	void ok_pressed() {

		String dir = project_path->get_text();

		if (mode == MODE_RENAME) {

			String dir2 = _test_path();
			if (dir2 == "") {
				set_message(TTR("Invalid project path (changed anything?)."), MESSAGE_ERROR);
				return;
			}

			ProjectSettings *current = memnew(ProjectSettings);

			int err = current->setup(dir2, "");
			if (err != OK) {
				set_message(vformat(TTR("Couldn't load project.godot in project path (error %d). It may be missing or corrupted."), err), MESSAGE_ERROR);
			} else {
				ProjectSettings::CustomMap edited_settings;
				edited_settings["application/config/name"] = project_name->get_text();

				if (current->save_custom(dir2.plus_file("project.godot"), edited_settings, Vector<String>(), true) != OK) {
					set_message(TTR("Couldn't edit project.godot in project path."), MESSAGE_ERROR);
				}
			}

			hide();
			emit_signal("projects_updated");

		} else {

			if (mode == MODE_IMPORT) {

				if (project_path->get_text().ends_with(".zip")) {

					mode = MODE_INSTALL;
					ok_pressed();

					return;
				}

			} else {
				if (mode == MODE_NEW) {

					ProjectSettings::CustomMap initial_settings;
					if (rasterizer_button_group->get_pressed_button()->get_meta("driver_name") == "GLES3") {
						initial_settings["rendering/quality/driver/driver_name"] = "GLES3";
					} else {
						initial_settings["rendering/quality/driver/driver_name"] = "GLES2";
						initial_settings["rendering/vram_compression/import_etc2"] = false;
						initial_settings["rendering/vram_compression/import_etc"] = true;
					}
					initial_settings["application/config/name"] = project_name->get_text();
					initial_settings["application/config/icon"] = "res://icon.png";
					initial_settings["rendering/environment/default_environment"] = "res://default_env.tres";

					if (ProjectSettings::get_singleton()->save_custom(dir.plus_file("project.godot"), initial_settings, Vector<String>(), false) != OK) {
						set_message(TTR("Couldn't create project.godot in project path."), MESSAGE_ERROR);
					} else {
						ResourceSaver::save(dir.plus_file("icon.png"), get_icon("DefaultProjectIcon", "EditorIcons"));

						FileAccess *f = FileAccess::open(dir.plus_file("default_env.tres"), FileAccess::WRITE);
						if (!f) {
							set_message(TTR("Couldn't create project.godot in project path."), MESSAGE_ERROR);
						} else {
							f->store_line("[gd_resource type=\"Environment\" load_steps=2 format=2]");
							f->store_line("[sub_resource type=\"ProceduralSky\" id=1]");
							f->store_line("[resource]");
							f->store_line("background_mode = 2");
							f->store_line("background_sky = SubResource( 1 )");
							memdelete(f);
						}
					}

				} else if (mode == MODE_INSTALL) {

					if (project_path->get_text().ends_with(".zip")) {
						dir = install_path->get_text();
						zip_path = project_path->get_text();
					}

					FileAccess *src_f = NULL;
					zlib_filefunc_def io = zipio_create_io_from_file(&src_f);

					unzFile pkg = unzOpen2(zip_path.utf8().get_data(), &io);
					if (!pkg) {

						dialog_error->set_text(TTR("Error opening package file, not in zip format."));
						dialog_error->popup_centered_minsize();
						return;
					}

					int ret = unzGoToFirstFile(pkg);

					Vector<String> failed_files;

					int idx = 0;
					while (ret == UNZ_OK) {

						//get filename
						unz_file_info info;
						char fname[16384];
						ret = unzGetCurrentFileInfo(pkg, &info, fname, 16384, NULL, 0, NULL, 0);

						String path = fname;

						int depth = 1; //stuff from github comes with tag
						bool skip = false;
						while (depth > 0) {
							int pp = path.find("/");
							if (pp == -1) {
								skip = true;
								break;
							}
							path = path.substr(pp + 1, path.length());
							depth--;
						}

						if (skip || path == String()) {
							//
						} else if (path.ends_with("/")) { // a dir

							path = path.substr(0, path.length() - 1);

							DirAccess *da = DirAccess::create(DirAccess::ACCESS_FILESYSTEM);
							da->make_dir(dir.plus_file(path));
							memdelete(da);

						} else {

							Vector<uint8_t> data;
							data.resize(info.uncompressed_size);

							//read
							unzOpenCurrentFile(pkg);
							unzReadCurrentFile(pkg, data.ptrw(), data.size());
							unzCloseCurrentFile(pkg);

							FileAccess *f = FileAccess::open(dir.plus_file(path), FileAccess::WRITE);

							if (f) {
								f->store_buffer(data.ptr(), data.size());
								memdelete(f);
							} else {
								failed_files.push_back(path);
							}
						}

						idx++;
						ret = unzGoToNextFile(pkg);
					}

					unzClose(pkg);

					if (failed_files.size()) {
						String msg = TTR("The following files failed extraction from package:") + "\n\n";
						for (int i = 0; i < failed_files.size(); i++) {

							if (i > 15) {
								msg += "\nAnd " + itos(failed_files.size() - i) + " more files.";
								break;
							}
							msg += failed_files[i] + "\n";
						}

						dialog_error->set_text(msg);
						dialog_error->popup_centered_minsize();

					} else if (!project_path->get_text().ends_with(".zip")) {
						dialog_error->set_text(TTR("Package installed successfully!"));
						dialog_error->popup_centered_minsize();
					}
				}
			}

			dir = dir.replace("\\", "/");
			if (dir.ends_with("/"))
				dir = dir.substr(0, dir.length() - 1);
			String proj = dir.replace("/", "::");
			EditorSettings::get_singleton()->set("projects/" + proj, dir);
			EditorSettings::get_singleton()->save();

			hide();
			emit_signal("project_created", dir);
		}
	}

	void _remove_created_folder() {

		if (created_folder_path != "") {
			DirAccess *d = DirAccess::create(DirAccess::ACCESS_FILESYSTEM);
			d->remove(created_folder_path);
			memdelete(d);

			create_dir->set_disabled(false);
			created_folder_path = "";
		}
	}

	void cancel_pressed() {

		_remove_created_folder();

		project_path->clear();
		_path_text_changed("");
		project_name->clear();
		_text_changed("");

		if (status_rect->get_texture() == get_icon("StatusError", "EditorIcons"))
			msg->show();

		if (install_status_rect->get_texture() == get_icon("StatusError", "EditorIcons"))
			msg->show();
	}

	void _notification(int p_what) {

		if (p_what == MainLoop::NOTIFICATION_WM_QUIT_REQUEST)
			_remove_created_folder();
	}

protected:
	static void _bind_methods() {

		ClassDB::bind_method("_browse_path", &ProjectDialog::_browse_path);
		ClassDB::bind_method("_create_folder", &ProjectDialog::_create_folder);
		ClassDB::bind_method("_text_changed", &ProjectDialog::_text_changed);
		ClassDB::bind_method("_path_text_changed", &ProjectDialog::_path_text_changed);
		ClassDB::bind_method("_path_selected", &ProjectDialog::_path_selected);
		ClassDB::bind_method("_file_selected", &ProjectDialog::_file_selected);
		ClassDB::bind_method("_install_path_selected", &ProjectDialog::_install_path_selected);
		ClassDB::bind_method("_browse_install_path", &ProjectDialog::_browse_install_path);
		ADD_SIGNAL(MethodInfo("project_created"));
		ADD_SIGNAL(MethodInfo("projects_updated"));
	}

public:
	void set_zip_path(const String &p_path) {
		zip_path = p_path;
	}
	void set_zip_title(const String &p_title) {
		zip_title = p_title;
	}

	void set_mode(Mode p_mode) {

		mode = p_mode;
	}

	void set_project_path(const String &p_path) {
		project_path->set_text(p_path);
	}

	void show_dialog() {

		if (mode == MODE_RENAME) {

			project_path->set_editable(false);
			browse->hide();
			install_browse->hide();

			set_title(TTR("Rename Project"));
			get_ok()->set_text(TTR("Rename"));
			name_container->show();
			status_rect->hide();
			msg->hide();
			install_path_container->hide();
			install_status_rect->hide();
			rasterizer_container->hide();
			get_ok()->set_disabled(false);

			ProjectSettings *current = memnew(ProjectSettings);

			int err = current->setup(project_path->get_text(), "");
			if (err != OK) {
				set_message(vformat(TTR("Couldn't load project.godot in project path (error %d). It may be missing or corrupted."), err), MESSAGE_ERROR);
				status_rect->show();
				msg->show();
				get_ok()->set_disabled(true);
			} else if (current->has_setting("application/config/name")) {
				String proj = current->get("application/config/name");
				project_name->set_text(proj);
				_text_changed(proj);
			}

			project_name->call_deferred("grab_focus");

			create_dir->hide();

		} else {

			fav_dir = EditorSettings::get_singleton()->get("filesystem/directories/default_project_path");
			if (fav_dir != "") {
				project_path->set_text(fav_dir);
				fdialog->set_current_dir(fav_dir);
			} else {
				DirAccess *d = DirAccess::create(DirAccess::ACCESS_FILESYSTEM);
				project_path->set_text(d->get_current_dir());
				fdialog->set_current_dir(d->get_current_dir());
				memdelete(d);
			}
			String proj = TTR("New Game Project");
			project_name->set_text(proj);
			_text_changed(proj);

			project_path->set_editable(true);
			browse->set_disabled(false);
			browse->show();
			install_browse->set_disabled(false);
			install_browse->show();
			create_dir->show();
			status_rect->show();
			install_status_rect->show();
			msg->show();

			if (mode == MODE_IMPORT) {
				set_title(TTR("Import Existing Project"));
				get_ok()->set_text(TTR("Import & Edit"));
				name_container->hide();
				install_path_container->hide();
				rasterizer_container->hide();
				project_path->grab_focus();

			} else if (mode == MODE_NEW) {

				set_title(TTR("Create New Project"));
				get_ok()->set_text(TTR("Create & Edit"));
				name_container->show();
				install_path_container->hide();
				rasterizer_container->show();
				project_name->call_deferred("grab_focus");
				project_name->call_deferred("select_all");

			} else if (mode == MODE_INSTALL) {

				set_title(TTR("Install Project:") + " " + zip_title);
				get_ok()->set_text(TTR("Install & Edit"));
				name_container->hide();
				install_path_container->hide();
				rasterizer_container->hide();
				project_path->grab_focus();
			}

			_test_path();
		}

		popup_centered_minsize(Size2(500, 0) * EDSCALE);
	}

	ProjectDialog() {

		VBoxContainer *vb = memnew(VBoxContainer);
		add_child(vb);

		name_container = memnew(VBoxContainer);
		vb->add_child(name_container);

		Label *l = memnew(Label);
		l->set_text(TTR("Project Name:"));
		name_container->add_child(l);

		HBoxContainer *pnhb = memnew(HBoxContainer);
		name_container->add_child(pnhb);

		project_name = memnew(LineEdit);
		project_name->set_h_size_flags(SIZE_EXPAND_FILL);
		pnhb->add_child(project_name);

		create_dir = memnew(Button);
		pnhb->add_child(create_dir);
		create_dir->set_text(TTR("Create folder"));
		create_dir->connect("pressed", this, "_create_folder");

		path_container = memnew(VBoxContainer);
		vb->add_child(path_container);

		l = memnew(Label);
		l->set_text(TTR("Project Path:"));
		path_container->add_child(l);

		HBoxContainer *pphb = memnew(HBoxContainer);
		path_container->add_child(pphb);

		project_path = memnew(LineEdit);
		project_path->set_h_size_flags(SIZE_EXPAND_FILL);
		pphb->add_child(project_path);

		install_path_container = memnew(VBoxContainer);
		vb->add_child(install_path_container);

		l = memnew(Label);
		l->set_text(TTR("Project Installation Path:"));
		install_path_container->add_child(l);

		HBoxContainer *iphb = memnew(HBoxContainer);
		install_path_container->add_child(iphb);

		install_path = memnew(LineEdit);
		install_path->set_h_size_flags(SIZE_EXPAND_FILL);
		iphb->add_child(install_path);

		// status icon
		status_rect = memnew(TextureRect);
		status_rect->set_stretch_mode(TextureRect::STRETCH_KEEP_CENTERED);
		pphb->add_child(status_rect);

		browse = memnew(Button);
		browse->set_text(TTR("Browse"));
		browse->connect("pressed", this, "_browse_path");
		pphb->add_child(browse);

		// install status icon
		install_status_rect = memnew(TextureRect);
		install_status_rect->set_stretch_mode(TextureRect::STRETCH_KEEP_CENTERED);
		iphb->add_child(install_status_rect);

		install_browse = memnew(Button);
		install_browse->set_text(TTR("Browse"));
		install_browse->connect("pressed", this, "_browse_install_path");
		iphb->add_child(install_browse);

		msg = memnew(Label);
		msg->set_align(Label::ALIGN_CENTER);
		vb->add_child(msg);

		// rasterizer selection
		rasterizer_container = memnew(VBoxContainer);
		vb->add_child(rasterizer_container);
		l = memnew(Label);
		l->set_text(TTR("Renderer:"));
		rasterizer_container->add_child(l);
		Container *rshb = memnew(HBoxContainer);
		rasterizer_container->add_child(rshb);
		rasterizer_button_group.instance();

		Container *rvb = memnew(VBoxContainer);
		rvb->set_h_size_flags(SIZE_EXPAND_FILL);
		rshb->add_child(rvb);
		Button *rs_button = memnew(CheckBox);
		rs_button->set_button_group(rasterizer_button_group);
		rs_button->set_text(TTR("OpenGL ES 3.0"));
		rs_button->set_meta("driver_name", "GLES3");
		rs_button->set_pressed(true);
		rvb->add_child(rs_button);
		l = memnew(Label);
		l->set_text(TTR("Higher visual quality\nAll features available\nIncompatible with older hardware\nNot recommended for web games"));
		rvb->add_child(l);

		rshb->add_child(memnew(VSeparator));

		rvb = memnew(VBoxContainer);
		rvb->set_h_size_flags(SIZE_EXPAND_FILL);
		rshb->add_child(rvb);
		rs_button = memnew(CheckBox);
		rs_button->set_button_group(rasterizer_button_group);
		rs_button->set_text(TTR("OpenGL ES 2.0"));
		rs_button->set_meta("driver_name", "GLES2");
		rvb->add_child(rs_button);
		l = memnew(Label);
		l->set_text(TTR("Lower visual quality\nSome features not available\nWorks on most hardware\nRecommended for web games"));
		rvb->add_child(l);

		l = memnew(Label);
		l->set_text(TTR("Renderer can be changed later, but scenes may need to be adjusted."));
		l->set_align(Label::ALIGN_CENTER);
		rasterizer_container->add_child(l);

		fdialog = memnew(FileDialog);
		fdialog->set_access(FileDialog::ACCESS_FILESYSTEM);
		fdialog_install = memnew(FileDialog);
		fdialog_install->set_access(FileDialog::ACCESS_FILESYSTEM);
		add_child(fdialog);
		add_child(fdialog_install);
		project_name->connect("text_changed", this, "_text_changed");
		project_path->connect("text_changed", this, "_path_text_changed");
		install_path->connect("text_changed", this, "_path_text_changed");
		fdialog->connect("dir_selected", this, "_path_selected");
		fdialog->connect("file_selected", this, "_file_selected");
		fdialog_install->connect("dir_selected", this, "_install_path_selected");
		fdialog_install->connect("file_selected", this, "_install_path_selected");

		set_hide_on_ok(false);
		mode = MODE_NEW;

		dialog_error = memnew(AcceptDialog);
		add_child(dialog_error);
	}
};

struct ProjectItem {
	String project;
	String path;
	String conf;
	int config_version;
	uint64_t last_modified;
	bool favorite;
	bool grayed;
	bool ordered_latest_modification;
	ProjectItem() {}
	ProjectItem(const String &p_project, const String &p_path, const String &p_conf, int p_config_version, uint64_t p_last_modified, bool p_favorite = false, bool p_grayed = false, const bool p_ordered_latest_modification = true) {
		project = p_project;
		path = p_path;
		conf = p_conf;
		config_version = p_config_version;
		last_modified = p_last_modified;
		favorite = p_favorite;
		grayed = p_grayed;
		ordered_latest_modification = p_ordered_latest_modification;
	}
	_FORCE_INLINE_ bool operator<(const ProjectItem &l) const {
		if (ordered_latest_modification)
			return last_modified > l.last_modified;
		return project < l.project;
	}
	_FORCE_INLINE_ bool operator==(const ProjectItem &l) const { return project == l.project; }
};

void ProjectManager::_notification(int p_what) {

	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {

			Engine::get_singleton()->set_editor_hint(false);
		} break;
		case NOTIFICATION_READY: {

			if (scroll_children->get_child_count() == 0 && StreamPeerSSL::is_available())
				open_templates->popup_centered_minsize();
		} break;
		case NOTIFICATION_VISIBILITY_CHANGED: {

			set_process_unhandled_input(is_visible_in_tree());
		} break;
	}
}

void ProjectManager::_panel_draw(Node *p_hb) {

	HBoxContainer *hb = Object::cast_to<HBoxContainer>(p_hb);

	hb->draw_line(Point2(0, hb->get_size().y + 1), Point2(hb->get_size().x - 10, hb->get_size().y + 1), get_color("guide_color", "Tree"));

	if (selected_list.has(hb->get_meta("name"))) {
		hb->draw_style_box(gui_base->get_stylebox("selected", "Tree"), Rect2(Point2(), hb->get_size() - Size2(10, 0) * EDSCALE));
	}
}

void ProjectManager::_update_project_buttons() {
	for (int i = 0; i < scroll_children->get_child_count(); i++) {

		CanvasItem *item = Object::cast_to<CanvasItem>(scroll_children->get_child(i));
		item->update();
	}

	bool empty_selection = selected_list.empty();
	erase_btn->set_disabled(empty_selection);
	open_btn->set_disabled(empty_selection);
	rename_btn->set_disabled(empty_selection);
	run_btn->set_disabled(empty_selection);
}

void ProjectManager::_panel_input(const Ref<InputEvent> &p_ev, Node *p_hb) {

	Ref<InputEventMouseButton> mb = p_ev;

	if (mb.is_valid() && mb->is_pressed() && mb->get_button_index() == BUTTON_LEFT) {

		String clicked = p_hb->get_meta("name");
		String clicked_main_scene = p_hb->get_meta("main_scene");

		if (mb->get_shift() && selected_list.size() > 0 && last_clicked != "" && clicked != last_clicked) {

			int clicked_id = -1;
			int last_clicked_id = -1;
			for (int i = 0; i < scroll_children->get_child_count(); i++) {
				HBoxContainer *hb = Object::cast_to<HBoxContainer>(scroll_children->get_child(i));
				if (!hb) continue;
				if (hb->get_meta("name") == clicked) clicked_id = i;
				if (hb->get_meta("name") == last_clicked) last_clicked_id = i;
			}

			if (last_clicked_id != -1 && clicked_id != -1) {
				int min = clicked_id < last_clicked_id ? clicked_id : last_clicked_id;
				int max = clicked_id > last_clicked_id ? clicked_id : last_clicked_id;
				for (int i = 0; i < scroll_children->get_child_count(); ++i) {
					HBoxContainer *hb = Object::cast_to<HBoxContainer>(scroll_children->get_child(i));
					if (!hb) continue;
					if (i != clicked_id && (i < min || i > max) && !mb->get_control()) {
						selected_list.erase(hb->get_meta("name"));
					} else if (i >= min && i <= max) {
						selected_list.insert(hb->get_meta("name"), hb->get_meta("main_scene"));
					}
				}
			}

		} else if (selected_list.has(clicked) && mb->get_control()) {

			selected_list.erase(clicked);

		} else {

			last_clicked = clicked;
			if (mb->get_control() || selected_list.size() == 0) {
				selected_list.insert(clicked, clicked_main_scene);
			} else {
				selected_list.clear();
				selected_list.insert(clicked, clicked_main_scene);
			}
		}

		_update_project_buttons();

		if (mb->is_doubleclick())
			_open_selected_projects_ask(); //open if doubleclicked
	}
}

void ProjectManager::_unhandled_input(const Ref<InputEvent> &p_ev) {

	Ref<InputEventKey> k = p_ev;

	if (k.is_valid()) {

		if (!k->is_pressed())
			return;

		if (tabs->get_current_tab() != 0)
			return;

		bool scancode_handled = true;

		switch (k->get_scancode()) {

			case KEY_ENTER: {

				_open_selected_projects_ask();
			} break;
			case KEY_DELETE: {

				_erase_project();
			} break;
			case KEY_HOME: {

				for (int i = 0; i < scroll_children->get_child_count(); i++) {

					HBoxContainer *hb = Object::cast_to<HBoxContainer>(scroll_children->get_child(i));
					if (hb) {
						selected_list.clear();
						selected_list.insert(hb->get_meta("name"), hb->get_meta("main_scene"));
						scroll->set_v_scroll(0);
						_update_project_buttons();
						break;
					}
				}

			} break;
			case KEY_END: {

				for (int i = scroll_children->get_child_count() - 1; i >= 0; i--) {

					HBoxContainer *hb = Object::cast_to<HBoxContainer>(scroll_children->get_child(i));
					if (hb) {
						selected_list.clear();
						selected_list.insert(hb->get_meta("name"), hb->get_meta("main_scene"));
						scroll->set_v_scroll(scroll_children->get_size().y);
						_update_project_buttons();
						break;
					}
				}

			} break;
			case KEY_UP: {

				if (k->get_shift())
					break;

				if (selected_list.size()) {

					bool found = false;

					for (int i = scroll_children->get_child_count() - 1; i >= 0; i--) {

						HBoxContainer *hb = Object::cast_to<HBoxContainer>(scroll_children->get_child(i));
						if (!hb) continue;

						String current = hb->get_meta("name");

						if (found) {
							selected_list.clear();
							selected_list.insert(current, hb->get_meta("main_scene"));

							int offset_diff = scroll->get_v_scroll() - hb->get_position().y;

							if (offset_diff > 0)
								scroll->set_v_scroll(scroll->get_v_scroll() - offset_diff);

							_update_project_buttons();

							break;

						} else if (current == selected_list.back()->key()) {

							found = true;
						}
					}

					break;
				}
				// else fallthrough to key_down
			}
			case KEY_DOWN: {

				if (k->get_shift())
					break;

				bool found = selected_list.empty();

				for (int i = 0; i < scroll_children->get_child_count(); i++) {

					HBoxContainer *hb = Object::cast_to<HBoxContainer>(scroll_children->get_child(i));
					if (!hb) continue;

					String current = hb->get_meta("name");

					if (found) {
						selected_list.clear();
						selected_list.insert(current, hb->get_meta("main_scene"));

						int last_y_visible = scroll->get_v_scroll() + scroll->get_size().y;
						int offset_diff = (hb->get_position().y + hb->get_size().y) - last_y_visible;

						if (offset_diff > 0)
							scroll->set_v_scroll(scroll->get_v_scroll() + offset_diff);

						_update_project_buttons();

						break;

					} else if (current == selected_list.back()->key()) {

						found = true;
					}
				}

			} break;
			case KEY_F: {
				if (k->get_command())
					this->project_filter->search_box->grab_focus();
				else
					scancode_handled = false;
			} break;
			default: {
				scancode_handled = false;
			} break;
		}

		if (scancode_handled) {
			accept_event();
		}
	}
}

void ProjectManager::_favorite_pressed(Node *p_hb) {

	String clicked = p_hb->get_meta("name");
	bool favorite = !p_hb->get_meta("favorite");
	String proj = clicked.replace(":::", ":/");
	proj = proj.replace("::", "/");

	if (favorite) {
		EditorSettings::get_singleton()->set("favorite_projects/" + clicked, proj);
	} else {
		EditorSettings::get_singleton()->erase("favorite_projects/" + clicked);
	}
	EditorSettings::get_singleton()->save();
	call_deferred("_load_recent_projects");
}

void ProjectManager::_load_recent_projects() {

	ProjectListFilter::FilterOption filter_option = project_filter->get_filter_option();
	String search_term = project_filter->get_search_term();

	while (scroll_children->get_child_count() > 0) {
		memdelete(scroll_children->get_child(0));
	}

	Map<String, String> selected_list_copy = selected_list;

	List<PropertyInfo> properties;
	EditorSettings::get_singleton()->get_property_list(&properties);

	Color font_color = gui_base->get_color("font_color", "Tree");

	bool set_ordered_latest_modification;
	ProjectListFilter::FilterOption filter_order_option = project_order_filter->get_filter_option();
	if (filter_order_option == ProjectListFilter::FILTER_NAME) {
		set_ordered_latest_modification = false;
	} else {
		set_ordered_latest_modification = true;
	}
	EditorSettings::get_singleton()->set("project_manager/sorting_order", (int)filter_order_option);

	List<ProjectItem> projects;
	List<ProjectItem> favorite_projects;

	for (List<PropertyInfo>::Element *E = properties.front(); E; E = E->next()) {

		String _name = E->get().name;
		if (!_name.begins_with("projects/") && !_name.begins_with("favorite_projects/"))
			continue;

		String path = EditorSettings::get_singleton()->get(_name);
		if (filter_option == ProjectListFilter::FILTER_PATH && search_term != "" && path.findn(search_term) == -1)
			continue;

		String project = _name.get_slice("/", 1);
		String conf = path.plus_file("project.godot");
		int config_version = 0; // Assume 0 until we know better
		bool favorite = (_name.begins_with("favorite_projects/")) ? true : false;
		bool grayed = false;

		uint64_t last_modified = 0;
		if (FileAccess::exists(conf)) {
			last_modified = FileAccess::get_modified_time(conf);

			String fscache = path.plus_file(".fscache");
			if (FileAccess::exists(fscache)) {
				uint64_t cache_modified = FileAccess::get_modified_time(fscache);
				if (cache_modified > last_modified)
					last_modified = cache_modified;
			}
		} else {
			grayed = true;
		}

		ProjectItem item(project, path, conf, config_version, last_modified, favorite, grayed, set_ordered_latest_modification);
		if (favorite)
			favorite_projects.push_back(item);
		else
			projects.push_back(item);
	}
	projects.sort();
	favorite_projects.sort();

	for (List<ProjectItem>::Element *E = projects.front(); E;) {
		List<ProjectItem>::Element *next = E->next();
		if (favorite_projects.find(E->get()) != NULL)
			projects.erase(E->get());
		E = next;
	}
	for (List<ProjectItem>::Element *E = favorite_projects.back(); E; E = E->prev()) {
		projects.push_front(E->get());
	}

	Ref<Texture> favorite_icon = get_icon("Favorites", "EditorIcons");

	for (List<ProjectItem>::Element *E = projects.front(); E; E = E->next()) {

		ProjectItem &item = E->get();
		String project = item.project;
		String path = item.path;
		String conf = item.conf;

		Ref<ConfigFile> cf = memnew(ConfigFile);
		Error cf_err = cf->load(conf);

		String project_name = TTR("Unnamed Project");
		if (cf_err == OK && cf->has_section_key("application", "config/name")) {
			project_name = static_cast<String>(cf->get_value("application", "config/name")).xml_unescape();
		}

		if (filter_option == ProjectListFilter::FILTER_NAME && search_term != "" && project_name.findn(search_term) == -1)
			continue;

		Ref<Texture> icon;
		String main_scene;

		if (cf_err == OK) {
			item.config_version = (int)cf->get_value("", "config_version", 0);
			if (item.config_version > ProjectSettings::CONFIG_VERSION) {
				// Comes from an incompatible (more recent) Godot version, grey it out
				item.grayed = true;
			}

			String appicon = cf->get_value("application", "config/icon", "");
			if (appicon != "") {
				Ref<Image> img;
				img.instance();
				Error err = img->load(appicon.replace_first("res://", path + "/"));
				if (err == OK) {

					Ref<Texture> default_icon = get_icon("DefaultProjectIcon", "EditorIcons");
					img->resize(default_icon->get_width(), default_icon->get_height());
					Ref<ImageTexture> it = memnew(ImageTexture);
					it->create_from_image(img);
					icon = it;
				}
			}

			main_scene = cf->get_value("application", "run/main_scene", "");
		}

		if (icon.is_null()) {
			icon = get_icon("DefaultProjectIcon", "EditorIcons");
		}

		selected_list_copy.erase(project);

		bool is_favorite = item.favorite;
		bool is_grayed = item.grayed;

		HBoxContainer *hb = memnew(HBoxContainer);
		hb->set_meta("name", project);
		hb->set_meta("main_scene", main_scene);
		hb->set_meta("favorite", is_favorite);
		hb->connect("draw", this, "_panel_draw", varray(hb));
		hb->connect("gui_input", this, "_panel_input", varray(hb));
		hb->add_constant_override("separation", 10 * EDSCALE);

		VBoxContainer *favorite_box = memnew(VBoxContainer);
		TextureButton *favorite = memnew(TextureButton);
		favorite->set_normal_texture(favorite_icon);
		if (!is_favorite)
			favorite->set_modulate(Color(1, 1, 1, 0.2));
		favorite->connect("pressed", this, "_favorite_pressed", varray(hb));
		favorite_box->add_child(favorite);
		favorite_box->set_alignment(BoxContainer::ALIGN_CENTER);
		hb->add_child(favorite_box);

		TextureRect *tf = memnew(TextureRect);
		tf->set_texture(icon);
		hb->add_child(tf);

		VBoxContainer *vb = memnew(VBoxContainer);
		if (is_grayed)
			vb->set_modulate(Color(0.5, 0.5, 0.5));
		vb->set_name("project");
		vb->set_h_size_flags(SIZE_EXPAND_FILL);
		hb->add_child(vb);
		Control *ec = memnew(Control);
		ec->set_custom_minimum_size(Size2(0, 1));
		ec->set_mouse_filter(MOUSE_FILTER_PASS);
		vb->add_child(ec);
		Label *title = memnew(Label(project_name));
		title->add_font_override("font", gui_base->get_font("title", "EditorFonts"));
		title->add_color_override("font_color", font_color);
		title->set_clip_text(true);
		vb->add_child(title);

		HBoxContainer *path_hb = memnew(HBoxContainer);
		path_hb->set_name("path_box");
		path_hb->set_h_size_flags(SIZE_EXPAND_FILL);
		vb->add_child(path_hb);

		Button *show = memnew(Button);
		show->set_name("show");
		show->set_icon(get_icon("Filesystem", "EditorIcons"));
		show->set_flat(true);
		show->set_modulate(Color(1, 1, 1, 0.5));
		path_hb->add_child(show);
		show->connect("pressed", this, "_show_project", varray(path));
		show->set_tooltip(TTR("Show in File Manager"));

		Label *fpath = memnew(Label(path));
		fpath->set_name("path");
		path_hb->add_child(fpath);
		fpath->set_h_size_flags(SIZE_EXPAND_FILL);
		fpath->set_modulate(Color(1, 1, 1, 0.5));
		fpath->add_color_override("font_color", font_color);
		fpath->set_clip_text(true);

		scroll_children->add_child(hb);
	}

	for (Map<String, String>::Element *E = selected_list_copy.front(); E; E = E->next()) {
		String key = E->key();
		selected_list.erase(key);
	}

	scroll->set_v_scroll(0);

	_update_project_buttons();

	EditorSettings::get_singleton()->save();

	tabs->set_current_tab(0);
}

void ProjectManager::_on_projects_updated() {
	_load_recent_projects();
}

void ProjectManager::_on_project_created(const String &dir) {
	project_filter->clear();
	bool has_already = false;
	for (int i = 0; i < scroll_children->get_child_count(); i++) {
		HBoxContainer *hb = Object::cast_to<HBoxContainer>(scroll_children->get_child(i));
		Label *fpath = Object::cast_to<Label>(hb->get_node(NodePath("project/path_box/path")));
		if (fpath->get_text() == dir) {
			has_already = true;
			break;
		}
	}
	if (has_already) {
		_update_scroll_position(dir);
	} else {
		_load_recent_projects();
		_update_scroll_position(dir);
	}
	_open_selected_projects_ask();
}

void ProjectManager::_update_scroll_position(const String &dir) {
	for (int i = 0; i < scroll_children->get_child_count(); i++) {
		HBoxContainer *hb = Object::cast_to<HBoxContainer>(scroll_children->get_child(i));
		Label *fpath = Object::cast_to<Label>(hb->get_node(NodePath("project/path_box/path")));
		if (fpath->get_text() == dir) {
			last_clicked = hb->get_meta("name");
			selected_list.clear();
			selected_list.insert(hb->get_meta("name"), hb->get_meta("main_scene"));
			_update_project_buttons();
			int last_y_visible = scroll->get_v_scroll() + scroll->get_size().y;
			int offset_diff = (hb->get_position().y + hb->get_size().y) - last_y_visible;

			if (offset_diff > 0)
				scroll->set_v_scroll(scroll->get_v_scroll() + offset_diff);
			break;
		}
	}
}

void ProjectManager::_confirm_update_settings() {
	_open_selected_projects();
}

void ProjectManager::_open_selected_projects() {

	for (const Map<String, String>::Element *E = selected_list.front(); E; E = E->next()) {
		const String &selected = E->key();
		String path = EditorSettings::get_singleton()->get("projects/" + selected);
		String conf = path.plus_file("project.godot");
		if (!FileAccess::exists(conf)) {
			dialog_error->set_text(vformat(TTR("Can't open project at '%s'."), path));
			dialog_error->popup_centered_minsize();
			return;
		}

		print_line("Editing project: " + path + " (" + selected + ")");

		List<String> args;

		args.push_back("--path");
		args.push_back(path);

		args.push_back("--editor");

		if (OS::get_singleton()->is_disable_crash_handler()) {
			args.push_back("--disable-crash-handler");
		}

		String exec = OS::get_singleton()->get_executable_path();

		OS::ProcessID pid = 0;
		Error err = OS::get_singleton()->execute(exec, args, false, &pid);
		ERR_FAIL_COND(err);
	}

	get_tree()->quit();
}

void ProjectManager::_open_selected_projects_ask() {

	if (selected_list.size() < 1) {
		return;
	}

	if (selected_list.size() > 1) {
		multi_open_ask->set_text(TTR("Are you sure to open more than one project?"));
		multi_open_ask->popup_centered_minsize();
		return;
	}

	// Update the project settings or don't open
	String path = EditorSettings::get_singleton()->get("projects/" + selected_list.front()->key());
	String conf = path.plus_file("project.godot");

	// FIXME: We already parse those in _load_recent_projects, we could instead make
	// its `projects` list global and reuse its parsed metadata here.
	Ref<ConfigFile> cf = memnew(ConfigFile);
	Error cf_err = cf->load(conf);

	if (cf_err != OK) {
		dialog_error->set_text(vformat(TTR("Can't open project at '%s'."), path));
		dialog_error->popup_centered_minsize();
		return;
	}

	int config_version = (int)cf->get_value("", "config_version", 0);

	// Check if the config_version property was empty or 0
	if (config_version == 0) {
		ask_update_settings->set_text(vformat(TTR("The following project settings file does not specify the version of Godot through which it was created.\n\n%s\n\nIf you proceed with opening it, it will be converted to Godot's current configuration file format.\nWarning: You will not be able to open the project with previous versions of the engine anymore."), conf));
		ask_update_settings->popup_centered_minsize();
		return;
	}
	// Check if we need to convert project settings from an earlier engine version
	if (config_version < ProjectSettings::CONFIG_VERSION) {
		ask_update_settings->set_text(vformat(TTR("The following project settings file was generated by an older engine version, and needs to be converted for this version:\n\n%s\n\nDo you want to convert it?\nWarning: You will not be able to open the project with previous versions of the engine anymore."), conf));
		ask_update_settings->popup_centered_minsize();
		return;
	}
	// Check if the file was generated by a newer, incompatible engine version
	if (config_version > ProjectSettings::CONFIG_VERSION) {
		dialog_error->set_text(vformat(TTR("Can't open project at '%s'.") + "\n" + TTR("The project settings were created by a newer engine version, whose settings are not compatible with this version."), path));
		dialog_error->popup_centered_minsize();
		return;
	}

	// Open if the project is up-to-date
	_open_selected_projects();
}

void ProjectManager::_run_project_confirm() {

	for (Map<String, String>::Element *E = selected_list.front(); E; E = E->next()) {

		const String &selected_main = E->get();
		if (selected_main == "") {
			run_error_diag->set_text(TTR("Can't run project: no main scene defined.\nPlease edit the project and set the main scene in \"Project Settings\" under the \"Application\" category."));
			run_error_diag->popup_centered();
			return;
		}

		const String &selected = E->key();
		String path = EditorSettings::get_singleton()->get("projects/" + selected);

		if (!DirAccess::exists(path + "/.import")) {
			run_error_diag->set_text(TTR("Can't run project: Assets need to be imported.\nPlease edit the project to trigger the initial import."));
			run_error_diag->popup_centered();
			return;
		}

		print_line("Running project: " + path + " (" + selected + ")");

		List<String> args;

		args.push_back("--path");
		args.push_back(path);

		if (OS::get_singleton()->is_disable_crash_handler()) {
			args.push_back("--disable-crash-handler");
		}

		String exec = OS::get_singleton()->get_executable_path();

		OS::ProcessID pid = 0;
		Error err = OS::get_singleton()->execute(exec, args, false, &pid);
		ERR_FAIL_COND(err);
	}
}

void ProjectManager::_run_project() {

	if (selected_list.size() < 1) {
		return;
	}

	if (selected_list.size() > 1) {
		multi_run_ask->set_text(TTR("Are you sure to run more than one project?"));
		multi_run_ask->popup_centered_minsize();
	} else {
		_run_project_confirm();
	}
}

void ProjectManager::_show_project(const String &p_path) {

	OS::get_singleton()->shell_open(String("file://") + p_path);
}

void ProjectManager::_scan_dir(DirAccess *da, float pos, float total, List<String> *r_projects) {

	List<String> subdirs;
	da->list_dir_begin();
	String n = da->get_next();
	while (n != String()) {
		if (da->current_is_dir() && !n.begins_with(".")) {
			subdirs.push_front(n);
		} else if (n == "project.godot") {
			r_projects->push_back(da->get_current_dir());
		}
		n = da->get_next();
	}
	da->list_dir_end();
	int m = 0;
	for (List<String>::Element *E = subdirs.front(); E; E = E->next()) {

		da->change_dir(E->get());

		float slice = total / subdirs.size();
		_scan_dir(da, pos + slice * m, slice, r_projects);
		da->change_dir("..");
		m++;
	}
}

void ProjectManager::_scan_begin(const String &p_base) {

	print_line("Scanning projects at: " + p_base);
	List<String> projects;
	DirAccess *da = DirAccess::create(DirAccess::ACCESS_FILESYSTEM);
	da->change_dir(p_base);
	_scan_dir(da, 0, 1, &projects);
	memdelete(da);
	print_line("Found " + itos(projects.size()) + " projects.");

	for (List<String>::Element *E = projects.front(); E; E = E->next()) {
		String proj = E->get().replace("/", "::");
		EditorSettings::get_singleton()->set("projects/" + proj, E->get());
	}
	EditorSettings::get_singleton()->save();
	_load_recent_projects();
}

void ProjectManager::_scan_projects() {

	scan_dir->popup_centered_ratio();
}

void ProjectManager::_new_project() {

	npdialog->set_mode(ProjectDialog::MODE_NEW);
	npdialog->show_dialog();
}

void ProjectManager::_import_project() {

	npdialog->set_mode(ProjectDialog::MODE_IMPORT);
	npdialog->show_dialog();
}

void ProjectManager::_rename_project() {

	if (selected_list.size() == 0) {
		return;
	}

	for (Map<String, String>::Element *E = selected_list.front(); E; E = E->next()) {
		const String &selected = E->key();
		String path = EditorSettings::get_singleton()->get("projects/" + selected);
		npdialog->set_project_path(path);
		npdialog->set_mode(ProjectDialog::MODE_RENAME);
		npdialog->show_dialog();
	}
}

void ProjectManager::_erase_project_confirm() {

	if (selected_list.size() == 0) {
		return;
	}
	for (Map<String, String>::Element *E = selected_list.front(); E; E = E->next()) {
		EditorSettings::get_singleton()->erase("projects/" + E->key());
		EditorSettings::get_singleton()->erase("favorite_projects/" + E->key());
	}
	EditorSettings::get_singleton()->save();
	selected_list.clear();
	last_clicked = "";
	_load_recent_projects();
}

void ProjectManager::_erase_project() {

	if (selected_list.size() == 0)
		return;

	erase_ask->set_text(TTR("Remove project from the list? (Folder contents will not be modified)"));
	erase_ask->popup_centered_minsize();
}

void ProjectManager::_language_selected(int p_id) {

	String lang = language_btn->get_item_metadata(p_id);
	EditorSettings::get_singleton()->set("interface/editor/editor_language", lang);
	language_btn->set_text(lang);
	language_btn->set_icon(get_icon("Environment", "EditorIcons"));

	language_restart_ask->set_text(TTR("Language changed.\nThe UI will update next time the editor or project manager starts."));
	language_restart_ask->popup_centered();
}

void ProjectManager::_restart_confirm() {

	List<String> args = OS::get_singleton()->get_cmdline_args();
	String exec = OS::get_singleton()->get_executable_path();
	OS::ProcessID pid = 0;
	Error err = OS::get_singleton()->execute(exec, args, false, &pid);
	ERR_FAIL_COND(err);

	get_tree()->quit();
}

void ProjectManager::_exit_dialog() {

	get_tree()->quit();
}

void ProjectManager::_install_project(const String &p_zip_path, const String &p_title) {

	npdialog->set_mode(ProjectDialog::MODE_INSTALL);
	npdialog->set_zip_path(p_zip_path);
	npdialog->set_zip_title(p_title);
	npdialog->show_dialog();
}

void ProjectManager::_files_dropped(PoolStringArray p_files, int p_screen) {
	Set<String> folders_set;
	DirAccess *da = DirAccess::create(DirAccess::ACCESS_FILESYSTEM);
	for (int i = 0; i < p_files.size(); i++) {
		String file = p_files[i];
		folders_set.insert(da->dir_exists(file) ? file : file.get_base_dir());
	}
	memdelete(da);
	if (folders_set.size() > 0) {
		PoolStringArray folders;
		for (Set<String>::Element *E = folders_set.front(); E; E = E->next()) {
			folders.append(E->get());
		}

		bool confirm = true;
		if (folders.size() == 1) {
			DirAccess *dir = DirAccess::create(DirAccess::ACCESS_FILESYSTEM);
			if (dir->change_dir(folders[0]) == OK) {
				dir->list_dir_begin();
				String file = dir->get_next();
				while (confirm && file != String()) {
					if (!dir->current_is_dir() && file.ends_with("project.godot")) {
						confirm = false;
					}
					file = dir->get_next();
				}
				dir->list_dir_end();
			}
			memdelete(dir);
		}
		if (confirm) {
			multi_scan_ask->get_ok()->disconnect("pressed", this, "_scan_multiple_folders");
			multi_scan_ask->get_ok()->connect("pressed", this, "_scan_multiple_folders", varray(folders));
			multi_scan_ask->set_text(vformat(TTR("You are about the scan %s folders for existing Godot projects. Do you confirm?"), folders.size()));
			multi_scan_ask->popup_centered_minsize();
		} else {
			_scan_multiple_folders(folders);
		}
	}
}

void ProjectManager::_scan_multiple_folders(PoolStringArray p_files) {
	for (int i = 0; i < p_files.size(); i++) {
		_scan_begin(p_files.get(i));
	}
}

void ProjectManager::_bind_methods() {

	ClassDB::bind_method("_open_selected_projects_ask", &ProjectManager::_open_selected_projects_ask);
	ClassDB::bind_method("_open_selected_projects", &ProjectManager::_open_selected_projects);
	ClassDB::bind_method("_run_project", &ProjectManager::_run_project);
	ClassDB::bind_method("_run_project_confirm", &ProjectManager::_run_project_confirm);
	ClassDB::bind_method("_show_project", &ProjectManager::_show_project);
	ClassDB::bind_method("_scan_projects", &ProjectManager::_scan_projects);
	ClassDB::bind_method("_scan_begin", &ProjectManager::_scan_begin);
	ClassDB::bind_method("_import_project", &ProjectManager::_import_project);
	ClassDB::bind_method("_new_project", &ProjectManager::_new_project);
	ClassDB::bind_method("_rename_project", &ProjectManager::_rename_project);
	ClassDB::bind_method("_erase_project", &ProjectManager::_erase_project);
	ClassDB::bind_method("_erase_project_confirm", &ProjectManager::_erase_project_confirm);
	ClassDB::bind_method("_language_selected", &ProjectManager::_language_selected);
	ClassDB::bind_method("_restart_confirm", &ProjectManager::_restart_confirm);
	ClassDB::bind_method("_exit_dialog", &ProjectManager::_exit_dialog);
	ClassDB::bind_method("_load_recent_projects", &ProjectManager::_load_recent_projects);
	ClassDB::bind_method("_on_projects_updated", &ProjectManager::_on_projects_updated);
	ClassDB::bind_method("_on_project_created", &ProjectManager::_on_project_created);
	ClassDB::bind_method("_update_scroll_position", &ProjectManager::_update_scroll_position);
	ClassDB::bind_method("_panel_draw", &ProjectManager::_panel_draw);
	ClassDB::bind_method("_panel_input", &ProjectManager::_panel_input);
	ClassDB::bind_method("_unhandled_input", &ProjectManager::_unhandled_input);
	ClassDB::bind_method("_favorite_pressed", &ProjectManager::_favorite_pressed);
	ClassDB::bind_method("_install_project", &ProjectManager::_install_project);
	ClassDB::bind_method("_files_dropped", &ProjectManager::_files_dropped);
	ClassDB::bind_method("_open_asset_library", &ProjectManager::_open_asset_library);
	ClassDB::bind_method("_confirm_update_settings", &ProjectManager::_confirm_update_settings);
	ClassDB::bind_method(D_METHOD("_scan_multiple_folders", "files"), &ProjectManager::_scan_multiple_folders);
}

void ProjectManager::_open_asset_library() {
	asset_library->disable_community_support();
	tabs->set_current_tab(1);
}

ProjectManager::ProjectManager() {

	// load settings
	if (!EditorSettings::get_singleton())
		EditorSettings::create();

	EditorSettings::get_singleton()->set_optimize_save(false); //just write settings as they came

	{
		int display_scale = EditorSettings::get_singleton()->get("interface/editor/display_scale");
		float custom_display_scale = EditorSettings::get_singleton()->get("interface/editor/custom_display_scale");

		switch (display_scale) {
			case 0: {
				// Try applying a suitable display scale automatically
				const int screen = OS::get_singleton()->get_current_screen();
				editor_set_scale(OS::get_singleton()->get_screen_dpi(screen) >= 192 && OS::get_singleton()->get_screen_size(screen).x > 2000 ? 2.0 : 1.0);
			} break;

			case 1: {
				editor_set_scale(0.75);
			} break;

			case 2: {
				editor_set_scale(1.0);
			} break;

			case 3: {
				editor_set_scale(1.25);
			} break;

			case 4: {
				editor_set_scale(1.5);
			} break;

			case 5: {
				editor_set_scale(1.75);
			} break;

			case 6: {
				editor_set_scale(2.0);
			} break;

			default: {
				editor_set_scale(custom_display_scale);
			} break;
		}

#ifndef OSX_ENABLED
		// The macOS platform implementation uses its own hiDPI window resizing code
		// TODO: Resize windows on hiDPI displays on Windows and Linux and remove the line below
		OS::get_singleton()->set_window_size(OS::get_singleton()->get_window_size() * MAX(1, EDSCALE));
#endif
	}

	FileDialog::set_default_show_hidden_files(EditorSettings::get_singleton()->get("filesystem/file_dialog/show_hidden_files"));

	set_anchors_and_margins_preset(Control::PRESET_WIDE);
	set_theme(create_editor_theme());

	gui_base = memnew(Control);
	add_child(gui_base);
	gui_base->set_anchors_and_margins_preset(Control::PRESET_WIDE);
	gui_base->set_theme(create_custom_theme());

	Panel *panel = memnew(Panel);
	gui_base->add_child(panel);
	panel->set_anchors_and_margins_preset(Control::PRESET_WIDE);
	panel->add_style_override("panel", gui_base->get_stylebox("Background", "EditorStyles"));

	VBoxContainer *vb = memnew(VBoxContainer);
	panel->add_child(vb);
	vb->set_anchors_and_margins_preset(Control::PRESET_WIDE, Control::PRESET_MODE_MINSIZE, 8 * EDSCALE);
	vb->add_constant_override("separation", 8 * EDSCALE);

	String cp;
	cp += 0xA9;
	OS::get_singleton()->set_window_title(VERSION_NAME + String(" - ") + TTR("Project Manager") + " - " + cp + " 2007-2019 Juan Linietsky, Ariel Manzur & Godot Contributors");

	HBoxContainer *top_hb = memnew(HBoxContainer);
	vb->add_child(top_hb);
	Label *l = memnew(Label);
	l->set_text(VERSION_NAME + String(" - ") + TTR("Project Manager"));
	top_hb->add_child(l);
	top_hb->add_spacer();
	l = memnew(Label);
	String hash = String(VERSION_HASH);
	if (hash.length() != 0)
		hash = "." + hash.left(7);
	l->set_text("v" VERSION_FULL_BUILD "" + hash);
	l->set_align(Label::ALIGN_CENTER);
	top_hb->add_child(l);

	Control *center_box = memnew(Control);
	center_box->set_v_size_flags(SIZE_EXPAND_FILL);
	vb->add_child(center_box);

	tabs = memnew(TabContainer);
	center_box->add_child(tabs);
	tabs->set_anchors_and_margins_preset(Control::PRESET_WIDE);

	HBoxContainer *tree_hb = memnew(HBoxContainer);
	projects_hb = tree_hb;

	projects_hb->set_name(TTR("Project List"));

	tabs->add_child(tree_hb);

	VBoxContainer *search_tree_vb = memnew(VBoxContainer);
	tree_hb->add_child(search_tree_vb);
	search_tree_vb->set_h_size_flags(SIZE_EXPAND_FILL);

	HBoxContainer *sort_filters = memnew(HBoxContainer);
	Label *sort_label = memnew(Label);
	sort_label->set_text(TTR("Sort:"));
	sort_filters->add_child(sort_label);
	Vector<String> sort_filter_titles;
	sort_filter_titles.push_back("Name");
	sort_filter_titles.push_back("Last Modified");
	project_order_filter = memnew(ProjectListFilter);
	project_order_filter->_setup_filters(sort_filter_titles);
	project_order_filter->set_filter_size(150);
	sort_filters->add_child(project_order_filter);
	project_order_filter->connect("filter_changed", this, "_load_recent_projects");
	project_order_filter->set_custom_minimum_size(Size2(180, 10) * EDSCALE);

	int projects_sorting_order = (int)EditorSettings::get_singleton()->get("project_manager/sorting_order");
	project_order_filter->set_filter_option((ProjectListFilter::FilterOption)projects_sorting_order);

	sort_filters->add_spacer(true);
	Label *search_label = memnew(Label);
	search_label->set_text(TTR("Search:"));
	sort_filters->add_child(search_label);

	HBoxContainer *search_filters = memnew(HBoxContainer);
	Vector<String> vec2;
	vec2.push_back("Name");
	vec2.push_back("Path");
	project_filter = memnew(ProjectListFilter);
	project_filter->_setup_filters(vec2);
	project_filter->add_search_box();
	search_filters->add_child(project_filter);
	project_filter->connect("filter_changed", this, "_load_recent_projects");
	project_filter->set_custom_minimum_size(Size2(280, 10) * EDSCALE);
	sort_filters->add_child(search_filters);

	search_tree_vb->add_child(sort_filters);

	PanelContainer *pc = memnew(PanelContainer);
	pc->add_style_override("panel", gui_base->get_stylebox("bg", "Tree"));
	search_tree_vb->add_child(pc);
	pc->set_v_size_flags(SIZE_EXPAND_FILL);

	scroll = memnew(ScrollContainer);
	pc->add_child(scroll);
	scroll->set_enable_h_scroll(false);

	VBoxContainer *tree_vb = memnew(VBoxContainer);
	tree_hb->add_child(tree_vb);
	scroll_children = memnew(VBoxContainer);
	scroll_children->set_h_size_flags(SIZE_EXPAND_FILL);
	scroll->add_child(scroll_children);

	Button *open = memnew(Button);
	open->set_text(TTR("Edit"));
	tree_vb->add_child(open);
	open->connect("pressed", this, "_open_selected_projects_ask");
	open_btn = open;

	Button *run = memnew(Button);
	run->set_text(TTR("Run"));
	tree_vb->add_child(run);
	run->connect("pressed", this, "_run_project");
	run_btn = run;

	tree_vb->add_child(memnew(HSeparator));

	Button *scan = memnew(Button);
	scan->set_text(TTR("Scan"));
	tree_vb->add_child(scan);
	scan->connect("pressed", this, "_scan_projects");

	tree_vb->add_child(memnew(HSeparator));

	scan_dir = memnew(FileDialog);
	scan_dir->set_access(FileDialog::ACCESS_FILESYSTEM);
	scan_dir->set_mode(FileDialog::MODE_OPEN_DIR);
	scan_dir->set_title(TTR("Select a Folder to Scan")); // must be after mode or it's overridden
	scan_dir->set_current_dir(EditorSettings::get_singleton()->get("filesystem/directories/default_project_path"));
	gui_base->add_child(scan_dir);
	scan_dir->connect("dir_selected", this, "_scan_begin");

	Button *create = memnew(Button);
	create->set_text(TTR("New Project"));
	tree_vb->add_child(create);
	create->connect("pressed", this, "_new_project");

	Button *import = memnew(Button);
	import->set_text(TTR("Import"));
	tree_vb->add_child(import);
	import->connect("pressed", this, "_import_project");

	Button *rename = memnew(Button);
	rename->set_text(TTR("Rename"));
	tree_vb->add_child(rename);
	rename->connect("pressed", this, "_rename_project");
	rename_btn = rename;

	Button *erase = memnew(Button);
	erase->set_text(TTR("Remove"));
	tree_vb->add_child(erase);
	erase->connect("pressed", this, "_erase_project");
	erase_btn = erase;

	tree_vb->add_spacer();

	if (StreamPeerSSL::is_available()) {
		asset_library = memnew(EditorAssetLibrary(true));
		asset_library->set_name(TTR("Templates"));
		tabs->add_child(asset_library);
		asset_library->connect("install_asset", this, "_install_project");
	} else {
		WARN_PRINT("Asset Library not available, as it requires SSL to work.");
	}

	HBoxContainer *settings_hb = memnew(HBoxContainer);
	settings_hb->set_alignment(BoxContainer::ALIGN_END);
	settings_hb->set_h_grow_direction(Control::GROW_DIRECTION_BEGIN);

	language_btn = memnew(OptionButton);
	language_btn->set_flat(true);
	language_btn->set_focus_mode(Control::FOCUS_NONE);

	Vector<String> editor_languages;
	List<PropertyInfo> editor_settings_properties;
	EditorSettings::get_singleton()->get_property_list(&editor_settings_properties);
	for (List<PropertyInfo>::Element *E = editor_settings_properties.front(); E; E = E->next()) {
		PropertyInfo &pi = E->get();
		if (pi.name == "interface/editor/editor_language") {
			editor_languages = pi.hint_string.split(",");
		}
	}
	String current_lang = EditorSettings::get_singleton()->get("interface/editor/editor_language");
	for (int i = 0; i < editor_languages.size(); i++) {
		String lang = editor_languages[i];
		String lang_name = TranslationServer::get_singleton()->get_locale_name(lang);
		language_btn->add_item(lang_name + " [" + lang + "]", i);
		language_btn->set_item_metadata(i, lang);
		if (current_lang == lang) {
			language_btn->select(i);
			language_btn->set_text(lang);
		}
	}
	language_btn->set_icon(get_icon("Environment", "EditorIcons"));

	settings_hb->add_child(language_btn);
	language_btn->connect("item_selected", this, "_language_selected");

	center_box->add_child(settings_hb);
	settings_hb->set_anchors_and_margins_preset(Control::PRESET_TOP_RIGHT);

	CenterContainer *cc = memnew(CenterContainer);
	Button *cancel = memnew(Button);
	cancel->set_text(TTR("Exit"));
	cancel->set_custom_minimum_size(Size2(100, 1) * EDSCALE);
	cc->add_child(cancel);
	cancel->connect("pressed", this, "_exit_dialog");
	vb->add_child(cc);

	//////////////////////////////////////////////////////////////

	language_restart_ask = memnew(ConfirmationDialog);
	language_restart_ask->get_ok()->set_text(TTR("Restart Now"));
	language_restart_ask->get_ok()->connect("pressed", this, "_restart_confirm");
	language_restart_ask->get_cancel()->set_text(TTR("Continue"));
	gui_base->add_child(language_restart_ask);

	erase_ask = memnew(ConfirmationDialog);
	erase_ask->get_ok()->set_text(TTR("Remove"));
	erase_ask->get_ok()->connect("pressed", this, "_erase_project_confirm");
	gui_base->add_child(erase_ask);

	multi_open_ask = memnew(ConfirmationDialog);
	multi_open_ask->get_ok()->set_text(TTR("Edit"));
	multi_open_ask->get_ok()->connect("pressed", this, "_open_selected_projects");
	gui_base->add_child(multi_open_ask);

	multi_run_ask = memnew(ConfirmationDialog);
	multi_run_ask->get_ok()->set_text(TTR("Run"));
	multi_run_ask->get_ok()->connect("pressed", this, "_run_project_confirm");
	gui_base->add_child(multi_run_ask);

	multi_scan_ask = memnew(ConfirmationDialog);
	multi_scan_ask->get_ok()->set_text(TTR("Scan"));
	gui_base->add_child(multi_scan_ask);

	ask_update_settings = memnew(ConfirmationDialog);
	ask_update_settings->get_ok()->connect("pressed", this, "_confirm_update_settings");
	gui_base->add_child(ask_update_settings);

	OS::get_singleton()->set_low_processor_usage_mode(true);

	npdialog = memnew(ProjectDialog);
	gui_base->add_child(npdialog);

	npdialog->connect("projects_updated", this, "_on_projects_updated");
	npdialog->connect("project_created", this, "_on_project_created");
	_load_recent_projects();

	if (EditorSettings::get_singleton()->get("filesystem/directories/autoscan_project_path")) {
		_scan_begin(EditorSettings::get_singleton()->get("filesystem/directories/autoscan_project_path"));
	}

	last_clicked = "";

	SceneTree::get_singleton()->connect("files_dropped", this, "_files_dropped");

	run_error_diag = memnew(AcceptDialog);
	gui_base->add_child(run_error_diag);
	run_error_diag->set_title(TTR("Can't run project"));

	dialog_error = memnew(AcceptDialog);
	gui_base->add_child(dialog_error);

	open_templates = memnew(ConfirmationDialog);
	open_templates->set_text(TTR("You don't currently have any projects.\nWould you like to explore the official example projects in the Asset Library?"));
	open_templates->get_ok()->set_text(TTR("Open Asset Library"));
	open_templates->connect("confirmed", this, "_open_asset_library");
	add_child(open_templates);
}

ProjectManager::~ProjectManager() {

	if (EditorSettings::get_singleton())
		EditorSettings::destroy();
}

void ProjectListFilter::_setup_filters(Vector<String> options) {

	filter_option->clear();
	for (int i = 0; i < options.size(); i++)
		filter_option->add_item(TTR(options[i]));
}

void ProjectListFilter::_search_text_changed(const String &p_newtext) {
	emit_signal("filter_changed");
}

String ProjectListFilter::get_search_term() {
	return search_box->get_text().strip_edges();
}

ProjectListFilter::FilterOption ProjectListFilter::get_filter_option() {
	return _current_filter;
}

void ProjectListFilter::set_filter_option(FilterOption option) {
	filter_option->select((int)option);
	_filter_option_selected(0);
}

void ProjectListFilter::_filter_option_selected(int p_idx) {
	FilterOption selected = (FilterOption)(filter_option->get_selected());
	if (_current_filter != selected) {
		_current_filter = selected;
		emit_signal("filter_changed");
	}
}

void ProjectListFilter::_notification(int p_what) {

	if (p_what == NOTIFICATION_ENTER_TREE && has_search_box) {
		search_box->set_right_icon(get_icon("Search", "EditorIcons"));
		search_box->set_clear_button_enabled(true);
	}
}

void ProjectListFilter::_bind_methods() {

	ClassDB::bind_method(D_METHOD("_search_text_changed"), &ProjectListFilter::_search_text_changed);
	ClassDB::bind_method(D_METHOD("_filter_option_selected"), &ProjectListFilter::_filter_option_selected);

	ADD_SIGNAL(MethodInfo("filter_changed"));
}

void ProjectListFilter::add_search_box() {
	search_box = memnew(LineEdit);
	search_box->connect("text_changed", this, "_search_text_changed");
	search_box->set_h_size_flags(SIZE_EXPAND_FILL);
	add_child(search_box);
	has_search_box = true;
}

void ProjectListFilter::set_filter_size(int h_size) {
	filter_option->set_custom_minimum_size(Size2(h_size * EDSCALE, 10 * EDSCALE));
}

ProjectListFilter::ProjectListFilter() {

	_current_filter = FILTER_NAME;

	filter_option = memnew(OptionButton);
	set_filter_size(80);
	filter_option->set_clip_text(true);
	filter_option->connect("item_selected", this, "_filter_option_selected");
	add_child(filter_option);

	has_search_box = false;
}

void ProjectListFilter::clear() {
	if (has_search_box) {
		search_box->clear();
	}
}
