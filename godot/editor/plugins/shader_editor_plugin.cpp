/*************************************************************************/
/*  shader_editor_plugin.cpp                                             */
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

#include "shader_editor_plugin.h"

#include "core/io/resource_loader.h"
#include "core/io/resource_saver.h"
#include "core/os/keyboard.h"
#include "core/os/os.h"
#include "editor/editor_node.h"
#include "editor/editor_settings.h"
#include "editor/property_editor.h"
#include "servers/visual/shader_types.h"

/*** SHADER SCRIPT EDITOR ****/

Ref<Shader> ShaderTextEditor::get_edited_shader() const {

	return shader;
}
void ShaderTextEditor::set_edited_shader(const Ref<Shader> &p_shader) {

	if (shader == p_shader) {
		return;
	}
	shader = p_shader;

	_load_theme_settings();

	get_text_edit()->set_text(p_shader->get_code());

	_validate_script();
	_line_col_changed();
}

void ShaderTextEditor::_load_theme_settings() {

	get_text_edit()->clear_colors();

	Color background_color = EDITOR_GET("text_editor/highlighting/background_color");
	Color completion_background_color = EDITOR_GET("text_editor/highlighting/completion_background_color");
	Color completion_selected_color = EDITOR_GET("text_editor/highlighting/completion_selected_color");
	Color completion_existing_color = EDITOR_GET("text_editor/highlighting/completion_existing_color");
	Color completion_scroll_color = EDITOR_GET("text_editor/highlighting/completion_scroll_color");
	Color completion_font_color = EDITOR_GET("text_editor/highlighting/completion_font_color");
	Color text_color = EDITOR_GET("text_editor/highlighting/text_color");
	Color line_number_color = EDITOR_GET("text_editor/highlighting/line_number_color");
	Color caret_color = EDITOR_GET("text_editor/highlighting/caret_color");
	Color caret_background_color = EDITOR_GET("text_editor/highlighting/caret_background_color");
	Color text_selected_color = EDITOR_GET("text_editor/highlighting/text_selected_color");
	Color selection_color = EDITOR_GET("text_editor/highlighting/selection_color");
	Color brace_mismatch_color = EDITOR_GET("text_editor/highlighting/brace_mismatch_color");
	Color current_line_color = EDITOR_GET("text_editor/highlighting/current_line_color");
	Color line_length_guideline_color = EDITOR_GET("text_editor/highlighting/line_length_guideline_color");
	Color word_highlighted_color = EDITOR_GET("text_editor/highlighting/word_highlighted_color");
	Color number_color = EDITOR_GET("text_editor/highlighting/number_color");
	Color function_color = EDITOR_GET("text_editor/highlighting/function_color");
	Color member_variable_color = EDITOR_GET("text_editor/highlighting/member_variable_color");
	Color mark_color = EDITOR_GET("text_editor/highlighting/mark_color");
	Color breakpoint_color = EDITOR_GET("text_editor/highlighting/breakpoint_color");
	Color code_folding_color = EDITOR_GET("text_editor/highlighting/code_folding_color");
	Color search_result_color = EDITOR_GET("text_editor/highlighting/search_result_color");
	Color search_result_border_color = EDITOR_GET("text_editor/highlighting/search_result_border_color");
	Color symbol_color = EDITOR_GET("text_editor/highlighting/symbol_color");
	Color keyword_color = EDITOR_GET("text_editor/highlighting/keyword_color");
	Color comment_color = EDITOR_GET("text_editor/highlighting/comment_color");

	get_text_edit()->add_color_override("background_color", background_color);
	get_text_edit()->add_color_override("completion_background_color", completion_background_color);
	get_text_edit()->add_color_override("completion_selected_color", completion_selected_color);
	get_text_edit()->add_color_override("completion_existing_color", completion_existing_color);
	get_text_edit()->add_color_override("completion_scroll_color", completion_scroll_color);
	get_text_edit()->add_color_override("completion_font_color", completion_font_color);
	get_text_edit()->add_color_override("font_color", text_color);
	get_text_edit()->add_color_override("line_number_color", line_number_color);
	get_text_edit()->add_color_override("caret_color", caret_color);
	get_text_edit()->add_color_override("caret_background_color", caret_background_color);
	get_text_edit()->add_color_override("font_selected_color", text_selected_color);
	get_text_edit()->add_color_override("selection_color", selection_color);
	get_text_edit()->add_color_override("brace_mismatch_color", brace_mismatch_color);
	get_text_edit()->add_color_override("current_line_color", current_line_color);
	get_text_edit()->add_color_override("line_length_guideline_color", line_length_guideline_color);
	get_text_edit()->add_color_override("word_highlighted_color", word_highlighted_color);
	get_text_edit()->add_color_override("number_color", number_color);
	get_text_edit()->add_color_override("function_color", function_color);
	get_text_edit()->add_color_override("member_variable_color", member_variable_color);
	get_text_edit()->add_color_override("mark_color", mark_color);
	get_text_edit()->add_color_override("breakpoint_color", breakpoint_color);
	get_text_edit()->add_color_override("code_folding_color", code_folding_color);
	get_text_edit()->add_color_override("search_result_color", search_result_color);
	get_text_edit()->add_color_override("search_result_border_color", search_result_border_color);
	get_text_edit()->add_color_override("symbol_color", symbol_color);

	List<String> keywords;
	ShaderLanguage::get_keyword_list(&keywords);

	if (shader.is_valid()) {

		for (const Map<StringName, ShaderLanguage::FunctionInfo>::Element *E = ShaderTypes::get_singleton()->get_functions(VisualServer::ShaderMode(shader->get_mode())).front(); E; E = E->next()) {

			for (const Map<StringName, ShaderLanguage::BuiltInInfo>::Element *F = E->get().built_ins.front(); F; F = F->next()) {
				keywords.push_back(F->key());
			}
		}

		for (int i = 0; i < ShaderTypes::get_singleton()->get_modes(VisualServer::ShaderMode(shader->get_mode())).size(); i++) {

			keywords.push_back(ShaderTypes::get_singleton()->get_modes(VisualServer::ShaderMode(shader->get_mode()))[i]);
		}
	}

	for (List<String>::Element *E = keywords.front(); E; E = E->next()) {

		get_text_edit()->add_keyword_color(E->get(), keyword_color);
	}

	//colorize comments
	get_text_edit()->add_color_region("/*", "*/", comment_color, false);
	get_text_edit()->add_color_region("//", "", comment_color, false);
}

void ShaderTextEditor::_check_shader_mode() {

	String type = ShaderLanguage::get_shader_type(get_text_edit()->get_text());

	Shader::Mode mode;

	if (type == "canvas_item") {
		mode = Shader::MODE_CANVAS_ITEM;
	} else if (type == "particles") {
		mode = Shader::MODE_PARTICLES;
	} else {
		mode = Shader::MODE_SPATIAL;
	}

	if (shader->get_mode() != mode) {
		shader->set_code(get_text_edit()->get_text());
		_load_theme_settings();
	}
}

void ShaderTextEditor::_code_complete_script(const String &p_code, List<String> *r_options) {

	_check_shader_mode();

	ShaderLanguage sl;
	String calltip;

	Error err = sl.complete(p_code, ShaderTypes::get_singleton()->get_functions(VisualServer::ShaderMode(shader->get_mode())), ShaderTypes::get_singleton()->get_modes(VisualServer::ShaderMode(shader->get_mode())), ShaderTypes::get_singleton()->get_types(), r_options, calltip);
	if (err != OK)
		ERR_PRINT("Shaderlang complete failed");

	if (calltip != "") {
		get_text_edit()->set_code_hint(calltip);
	}
}

void ShaderTextEditor::_validate_script() {

	_check_shader_mode();

	String code = get_text_edit()->get_text();
	//List<StringName> params;
	//shader->get_param_list(&params);

	ShaderLanguage sl;

	Error err = sl.compile(code, ShaderTypes::get_singleton()->get_functions(VisualServer::ShaderMode(shader->get_mode())), ShaderTypes::get_singleton()->get_modes(VisualServer::ShaderMode(shader->get_mode())), ShaderTypes::get_singleton()->get_types());

	if (err != OK) {
		String error_text = "error(" + itos(sl.get_error_line()) + "): " + sl.get_error_text();
		set_error(error_text);
		set_error_pos(sl.get_error_line() - 1, 0);
		for (int i = 0; i < get_text_edit()->get_line_count(); i++)
			get_text_edit()->set_line_as_marked(i, false);
		get_text_edit()->set_line_as_marked(sl.get_error_line() - 1, true);

	} else {
		for (int i = 0; i < get_text_edit()->get_line_count(); i++)
			get_text_edit()->set_line_as_marked(i, false);
		set_error("");
	}

	emit_signal("script_changed");
}

void ShaderTextEditor::_bind_methods() {
}

ShaderTextEditor::ShaderTextEditor() {
}

/*** SCRIPT EDITOR ******/

void ShaderEditor::_menu_option(int p_option) {

	switch (p_option) {
		case EDIT_UNDO: {
			shader_editor->get_text_edit()->undo();
		} break;
		case EDIT_REDO: {
			shader_editor->get_text_edit()->redo();
		} break;
		case EDIT_CUT: {
			shader_editor->get_text_edit()->cut();
		} break;
		case EDIT_COPY: {
			shader_editor->get_text_edit()->copy();
		} break;
		case EDIT_PASTE: {
			shader_editor->get_text_edit()->paste();
		} break;
		case EDIT_SELECT_ALL: {
			shader_editor->get_text_edit()->select_all();
		} break;
		case EDIT_MOVE_LINE_UP: {
			shader_editor->move_lines_up();
		} break;
		case EDIT_MOVE_LINE_DOWN: {
			shader_editor->move_lines_down();
		} break;
		case EDIT_INDENT_LEFT: {

			TextEdit *tx = shader_editor->get_text_edit();
			if (shader.is_null())
				return;

			tx->indent_left();

		} break;
		case EDIT_INDENT_RIGHT: {

			TextEdit *tx = shader_editor->get_text_edit();
			if (shader.is_null())
				return;

			tx->indent_right();

		} break;
		case EDIT_DELETE_LINE: {
			shader_editor->delete_lines();
		} break;
		case EDIT_CLONE_DOWN: {
			shader_editor->clone_lines_down();
		} break;
		case EDIT_TOGGLE_COMMENT: {

			TextEdit *tx = shader_editor->get_text_edit();
			if (shader.is_null())
				return;

			tx->begin_complex_operation();
			if (tx->is_selection_active()) {
				int begin = tx->get_selection_from_line();
				int end = tx->get_selection_to_line();

				// End of selection ends on the first column of the last line, ignore it.
				if (tx->get_selection_to_column() == 0)
					end -= 1;

				// Check if all lines in the selected block are commented
				bool is_commented = true;
				for (int i = begin; i <= end; i++) {
					if (!tx->get_line(i).begins_with("//")) {
						is_commented = false;
						break;
					}
				}
				for (int i = begin; i <= end; i++) {
					String line_text = tx->get_line(i);

					if (line_text.strip_edges().empty()) {
						line_text = "//";
					} else {
						if (is_commented) {
							line_text = line_text.substr(2, line_text.length());
						} else {
							line_text = "//" + line_text;
						}
					}
					tx->set_line(i, line_text);
				}
			} else {
				int begin = tx->cursor_get_line();
				String line_text = tx->get_line(begin);

				if (line_text.begins_with("//"))
					line_text = line_text.substr(2, line_text.length());
				else
					line_text = "//" + line_text;
				tx->set_line(begin, line_text);
			}
			tx->end_complex_operation();
			tx->update();
			//tx->deselect();

		} break;
		case EDIT_COMPLETE: {

			shader_editor->get_text_edit()->query_code_comple();
		} break;
		case SEARCH_FIND: {

			shader_editor->get_find_replace_bar()->popup_search();
		} break;
		case SEARCH_FIND_NEXT: {

			shader_editor->get_find_replace_bar()->search_next();
		} break;
		case SEARCH_FIND_PREV: {

			shader_editor->get_find_replace_bar()->search_prev();
		} break;
		case SEARCH_REPLACE: {

			shader_editor->get_find_replace_bar()->popup_replace();
		} break;
		case SEARCH_GOTO_LINE: {

			goto_line_dialog->popup_find_line(shader_editor->get_text_edit());
		} break;
	}
	if (p_option != SEARCH_FIND && p_option != SEARCH_REPLACE && p_option != SEARCH_GOTO_LINE) {
		shader_editor->get_text_edit()->call_deferred("grab_focus");
	}
}

void ShaderEditor::_notification(int p_what) {

	if (p_what == NOTIFICATION_VISIBILITY_CHANGED) {
		//if (is_visible_in_tree())
		//	shader_editor->get_text_edit()->grab_focus();
	}
}

void ShaderEditor::_params_changed() {

	shader_editor->_validate_script();
}

void ShaderEditor::_editor_settings_changed() {

	shader_editor->get_text_edit()->set_auto_brace_completion(EditorSettings::get_singleton()->get("text_editor/completion/auto_brace_complete"));
	shader_editor->get_text_edit()->set_scroll_pass_end_of_file(EditorSettings::get_singleton()->get("text_editor/cursor/scroll_past_end_of_file"));
	shader_editor->get_text_edit()->set_indent_size(EditorSettings::get_singleton()->get("text_editor/indent/size"));
	shader_editor->get_text_edit()->set_indent_using_spaces(EditorSettings::get_singleton()->get("text_editor/indent/type"));
	shader_editor->get_text_edit()->set_auto_indent(EditorSettings::get_singleton()->get("text_editor/indent/auto_indent"));
	shader_editor->get_text_edit()->set_draw_tabs(EditorSettings::get_singleton()->get("text_editor/indent/draw_tabs"));
	shader_editor->get_text_edit()->set_show_line_numbers(EditorSettings::get_singleton()->get("text_editor/line_numbers/show_line_numbers"));
	shader_editor->get_text_edit()->set_syntax_coloring(EditorSettings::get_singleton()->get("text_editor/highlighting/syntax_highlighting"));
	shader_editor->get_text_edit()->set_highlight_all_occurrences(EditorSettings::get_singleton()->get("text_editor/highlighting/highlight_all_occurrences"));
	shader_editor->get_text_edit()->set_highlight_current_line(EditorSettings::get_singleton()->get("text_editor/highlighting/highlight_current_line"));
	shader_editor->get_text_edit()->cursor_set_blink_enabled(EditorSettings::get_singleton()->get("text_editor/cursor/caret_blink"));
	shader_editor->get_text_edit()->cursor_set_blink_speed(EditorSettings::get_singleton()->get("text_editor/cursor/caret_blink_speed"));
	shader_editor->get_text_edit()->add_constant_override("line_spacing", EditorSettings::get_singleton()->get("text_editor/theme/line_spacing"));
	shader_editor->get_text_edit()->cursor_set_block_mode(EditorSettings::get_singleton()->get("text_editor/cursor/block_caret"));
	shader_editor->get_text_edit()->set_smooth_scroll_enabled(EditorSettings::get_singleton()->get("text_editor/open_scripts/smooth_scrolling"));
	shader_editor->get_text_edit()->set_v_scroll_speed(EditorSettings::get_singleton()->get("text_editor/open_scripts/v_scroll_speed"));
}

void ShaderEditor::_bind_methods() {

	ClassDB::bind_method("_editor_settings_changed", &ShaderEditor::_editor_settings_changed);
	ClassDB::bind_method("_text_edit_gui_input", &ShaderEditor::_text_edit_gui_input);

	ClassDB::bind_method("_menu_option", &ShaderEditor::_menu_option);
	ClassDB::bind_method("_params_changed", &ShaderEditor::_params_changed);
	ClassDB::bind_method("apply_shaders", &ShaderEditor::apply_shaders);
}

void ShaderEditor::ensure_select_current() {

	/*
	if (tab_container->get_child_count() && tab_container->get_current_tab()>=0) {

		ShaderTextEditor *ste = Object::cast_to<ShaderTextEditor>(tab_container->get_child(tab_container->get_current_tab()));
		if (!ste)
			return;
		Ref<Shader> shader = ste->get_edited_shader();
		get_scene()->get_root_node()->call("_resource_selected",shader);
	}*/
}

void ShaderEditor::goto_line_selection(int p_line, int p_begin, int p_end) {

	shader_editor->goto_line_selection(p_line, p_begin, p_end);
}

void ShaderEditor::edit(const Ref<Shader> &p_shader) {

	if (p_shader.is_null() || !p_shader->is_text_shader())
		return;

	if (shader == p_shader)
		return;

	shader = p_shader;

	shader_editor->set_edited_shader(p_shader);

	//vertex_editor->set_edited_shader(shader,ShaderLanguage::SHADER_MATERIAL_VERTEX);
	// see if already has it
}

void ShaderEditor::save_external_data() {

	if (shader.is_null())
		return;
	apply_shaders();

	if (shader->get_path() != "" && shader->get_path().find("local://") == -1 && shader->get_path().find("::") == -1) {
		//external shader, save it
		ResourceSaver::save(shader->get_path(), shader);
	}
}

void ShaderEditor::apply_shaders() {

	if (shader.is_valid()) {
		String shader_code = shader->get_code();
		String editor_code = shader_editor->get_text_edit()->get_text();
		if (shader_code != editor_code) {
			shader->set_code(editor_code);
			shader->set_edited(true);
		}
	}
}

void ShaderEditor::_text_edit_gui_input(const Ref<InputEvent> &ev) {

	Ref<InputEventMouseButton> mb = ev;

	if (mb.is_valid()) {

		if (mb->get_button_index() == BUTTON_RIGHT && mb->is_pressed()) {

			int col, row;
			TextEdit *tx = shader_editor->get_text_edit();
			tx->_get_mouse_pos(mb->get_global_position() - tx->get_global_position(), row, col);
			tx->set_right_click_moves_caret(EditorSettings::get_singleton()->get("text_editor/cursor/right_click_moves_caret"));

			if (tx->is_right_click_moving_caret()) {
				if (tx->is_selection_active()) {

					int from_line = tx->get_selection_from_line();
					int to_line = tx->get_selection_to_line();
					int from_column = tx->get_selection_from_column();
					int to_column = tx->get_selection_to_column();

					if (row < from_line || row > to_line || (row == from_line && col < from_column) || (row == to_line && col > to_column)) {
						// Right click is outside the selected text
						tx->deselect();
					}
				}
				if (!tx->is_selection_active()) {
					tx->cursor_set_line(row, true, false);
					tx->cursor_set_column(col);
				}
			}
			_make_context_menu(tx->is_selection_active());
		}
	}
}

void ShaderEditor::_make_context_menu(bool p_selection) {

	context_menu->clear();
	if (p_selection) {
		context_menu->add_shortcut(ED_GET_SHORTCUT("script_text_editor/cut"), EDIT_CUT);
		context_menu->add_shortcut(ED_GET_SHORTCUT("script_text_editor/copy"), EDIT_COPY);
	}

	context_menu->add_shortcut(ED_GET_SHORTCUT("script_text_editor/paste"), EDIT_PASTE);
	context_menu->add_separator();
	context_menu->add_shortcut(ED_GET_SHORTCUT("script_text_editor/select_all"), EDIT_SELECT_ALL);
	context_menu->add_shortcut(ED_GET_SHORTCUT("script_text_editor/undo"), EDIT_UNDO);
	context_menu->add_shortcut(ED_GET_SHORTCUT("script_text_editor/redo"), EDIT_REDO);

	context_menu->add_separator();
	context_menu->add_shortcut(ED_GET_SHORTCUT("script_text_editor/indent_left"), EDIT_INDENT_LEFT);
	context_menu->add_shortcut(ED_GET_SHORTCUT("script_text_editor/indent_right"), EDIT_INDENT_RIGHT);
	context_menu->add_shortcut(ED_GET_SHORTCUT("script_text_editor/toggle_comment"), EDIT_TOGGLE_COMMENT);

	context_menu->set_position(get_global_transform().xform(get_local_mouse_position()));
	context_menu->set_size(Vector2(1, 1));
	context_menu->popup();
}

ShaderEditor::ShaderEditor(EditorNode *p_node) {

	shader_editor = memnew(ShaderTextEditor);
	shader_editor->set_v_size_flags(SIZE_EXPAND_FILL);
	shader_editor->add_constant_override("separation", 0);
	shader_editor->set_anchors_and_margins_preset(Control::PRESET_WIDE);

	shader_editor->connect("script_changed", this, "apply_shaders");
	EditorSettings::get_singleton()->connect("settings_changed", this, "_editor_settings_changed");

	shader_editor->get_text_edit()->set_callhint_settings(
			EditorSettings::get_singleton()->get("text_editor/completion/put_callhint_tooltip_below_current_line"),
			EditorSettings::get_singleton()->get("text_editor/completion/callhint_tooltip_offset"));

	shader_editor->get_text_edit()->set_select_identifiers_on_hover(true);
	shader_editor->get_text_edit()->set_context_menu_enabled(false);
	shader_editor->get_text_edit()->connect("gui_input", this, "_text_edit_gui_input");

	shader_editor->update_editor_settings();

	context_menu = memnew(PopupMenu);
	add_child(context_menu);
	context_menu->connect("id_pressed", this, "_menu_option");
	context_menu->set_hide_on_window_lose_focus(true);

	VBoxContainer *main_container = memnew(VBoxContainer);
	HBoxContainer *hbc = memnew(HBoxContainer);

	edit_menu = memnew(MenuButton);
	edit_menu->set_text(TTR("Edit"));
	edit_menu->set_switch_on_hover(true);
	edit_menu->get_popup()->set_hide_on_window_lose_focus(true);
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/undo"), EDIT_UNDO);
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/redo"), EDIT_REDO);
	edit_menu->get_popup()->add_separator();
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/cut"), EDIT_CUT);
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/copy"), EDIT_COPY);
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/paste"), EDIT_PASTE);
	edit_menu->get_popup()->add_separator();
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/select_all"), EDIT_SELECT_ALL);
	edit_menu->get_popup()->add_separator();
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/move_up"), EDIT_MOVE_LINE_UP);
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/move_down"), EDIT_MOVE_LINE_DOWN);
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/indent_left"), EDIT_INDENT_LEFT);
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/indent_right"), EDIT_INDENT_RIGHT);
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/delete_line"), EDIT_DELETE_LINE);
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/toggle_comment"), EDIT_TOGGLE_COMMENT);
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/clone_down"), EDIT_CLONE_DOWN);
	edit_menu->get_popup()->add_separator();
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/complete_symbol"), EDIT_COMPLETE);
	edit_menu->get_popup()->connect("id_pressed", this, "_menu_option");

	search_menu = memnew(MenuButton);
	search_menu->set_text(TTR("Search"));
	search_menu->set_switch_on_hover(true);
	search_menu->get_popup()->set_hide_on_window_lose_focus(true);
	search_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/find"), SEARCH_FIND);
	search_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/find_next"), SEARCH_FIND_NEXT);
	search_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/find_previous"), SEARCH_FIND_PREV);
	search_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/replace"), SEARCH_REPLACE);
	search_menu->get_popup()->add_separator();
	search_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/goto_line"), SEARCH_GOTO_LINE);
	search_menu->get_popup()->connect("id_pressed", this, "_menu_option");

	add_child(main_container);
	main_container->add_child(hbc);
	hbc->add_child(search_menu);
	hbc->add_child(edit_menu);
	hbc->add_style_override("panel", p_node->get_gui_base()->get_stylebox("ScriptEditorPanel", "EditorStyles"));
	main_container->add_child(shader_editor);

	goto_line_dialog = memnew(GotoLineDialog);
	add_child(goto_line_dialog);

	_editor_settings_changed();
}

void ShaderEditorPlugin::edit(Object *p_object) {

	Shader *s = Object::cast_to<Shader>(p_object);
	shader_editor->edit(s);
}

bool ShaderEditorPlugin::handles(Object *p_object) const {

	Shader *shader = Object::cast_to<Shader>(p_object);
	return shader != NULL && shader->is_text_shader();
}

void ShaderEditorPlugin::make_visible(bool p_visible) {

	if (p_visible) {
		button->show();
		editor->make_bottom_panel_item_visible(shader_editor);

	} else {

		button->hide();
		if (shader_editor->is_visible_in_tree())
			editor->hide_bottom_panel();
		shader_editor->apply_shaders();
	}
}

void ShaderEditorPlugin::selected_notify() {

	shader_editor->ensure_select_current();
}

void ShaderEditorPlugin::save_external_data() {

	shader_editor->save_external_data();
}

void ShaderEditorPlugin::apply_changes() {

	shader_editor->apply_shaders();
}

ShaderEditorPlugin::ShaderEditorPlugin(EditorNode *p_node) {

	editor = p_node;
	shader_editor = memnew(ShaderEditor(p_node));

	shader_editor->set_custom_minimum_size(Size2(0, 300));
	button = editor->add_bottom_panel_item(TTR("Shader"), shader_editor);
	button->hide();
}

ShaderEditorPlugin::~ShaderEditorPlugin() {
}
