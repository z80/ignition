/*************************************************************************/
/*  script_text_editor.cpp                                               */
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

#include "script_text_editor.h"

#include "core/os/keyboard.h"
#include "editor/editor_node.h"
#include "editor/editor_settings.h"
#include "editor/script_editor_debugger.h"

Vector<String> ScriptTextEditor::get_functions() {

	String errortxt;
	int line = -1, col;
	TextEdit *te = code_editor->get_text_edit();
	String text = te->get_text();
	List<String> fnc;

	if (script->get_language()->validate(text, line, col, errortxt, script->get_path(), &fnc)) {

		//if valid rewrite functions to latest
		functions.clear();
		for (List<String>::Element *E = fnc.front(); E; E = E->next()) {

			functions.push_back(E->get());
		}
	}

	return functions;
}

void ScriptTextEditor::apply_code() {

	if (script.is_null())
		return;
	script->set_source_code(code_editor->get_text_edit()->get_text());
	script->update_exports();
	_update_member_keywords();
}

RES ScriptTextEditor::get_edited_resource() const {
	return script;
}

void ScriptTextEditor::set_edited_resource(const RES &p_res) {
	ERR_FAIL_COND(!script.is_null());

	script = p_res;
	_set_theme_for_script();

	code_editor->get_text_edit()->set_text(script->get_source_code());
	code_editor->get_text_edit()->clear_undo_history();
	code_editor->get_text_edit()->tag_saved_version();

	emit_signal("name_changed");
	code_editor->update_line_and_column();

	_validate_script();
}

void ScriptTextEditor::_update_member_keywords() {
	member_keywords.clear();
	code_editor->get_text_edit()->clear_member_keywords();
	Color member_variable_color = EDITOR_GET("text_editor/highlighting/member_variable_color");

	StringName instance_base = script->get_instance_base_type();

	if (instance_base == StringName())
		return;
	List<PropertyInfo> plist;
	ClassDB::get_property_list(instance_base, &plist);

	for (List<PropertyInfo>::Element *E = plist.front(); E; E = E->next()) {
		String name = E->get().name;
		if (E->get().usage & PROPERTY_USAGE_CATEGORY || E->get().usage & PROPERTY_USAGE_GROUP)
			continue;
		if (name.find("/") != -1)
			continue;

		code_editor->get_text_edit()->add_member_keyword(name, member_variable_color);
	}

	List<String> clist;
	ClassDB::get_integer_constant_list(instance_base, &clist);

	for (List<String>::Element *E = clist.front(); E; E = E->next()) {

		code_editor->get_text_edit()->add_member_keyword(E->get(), member_variable_color);
	}
}

void ScriptTextEditor::_load_theme_settings() {

	TextEdit *text_edit = code_editor->get_text_edit();

	text_edit->clear_colors();

	Color background_color = EDITOR_GET("text_editor/highlighting/background_color");
	Color completion_background_color = EDITOR_GET("text_editor/highlighting/completion_background_color");
	Color completion_selected_color = EDITOR_GET("text_editor/highlighting/completion_selected_color");
	Color completion_existing_color = EDITOR_GET("text_editor/highlighting/completion_existing_color");
	Color completion_scroll_color = EDITOR_GET("text_editor/highlighting/completion_scroll_color");
	Color completion_font_color = EDITOR_GET("text_editor/highlighting/completion_font_color");
	Color text_color = EDITOR_GET("text_editor/highlighting/text_color");
	Color line_number_color = EDITOR_GET("text_editor/highlighting/line_number_color");
	Color safe_line_number_color = EDITOR_GET("text_editor/highlighting/safe_line_number_color");
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
	Color basetype_color = EDITOR_GET("text_editor/highlighting/base_type_color");
	Color type_color = EDITOR_GET("text_editor/highlighting/engine_type_color");
	Color comment_color = EDITOR_GET("text_editor/highlighting/comment_color");
	Color string_color = EDITOR_GET("text_editor/highlighting/string_color");

	text_edit->add_color_override("background_color", background_color);
	text_edit->add_color_override("completion_background_color", completion_background_color);
	text_edit->add_color_override("completion_selected_color", completion_selected_color);
	text_edit->add_color_override("completion_existing_color", completion_existing_color);
	text_edit->add_color_override("completion_scroll_color", completion_scroll_color);
	text_edit->add_color_override("completion_font_color", completion_font_color);
	text_edit->add_color_override("font_color", text_color);
	text_edit->add_color_override("line_number_color", line_number_color);
	text_edit->add_color_override("safe_line_number_color", safe_line_number_color);
	text_edit->add_color_override("caret_color", caret_color);
	text_edit->add_color_override("caret_background_color", caret_background_color);
	text_edit->add_color_override("font_color_selected", text_selected_color);
	text_edit->add_color_override("selection_color", selection_color);
	text_edit->add_color_override("brace_mismatch_color", brace_mismatch_color);
	text_edit->add_color_override("current_line_color", current_line_color);
	text_edit->add_color_override("line_length_guideline_color", line_length_guideline_color);
	text_edit->add_color_override("word_highlighted_color", word_highlighted_color);
	text_edit->add_color_override("number_color", number_color);
	text_edit->add_color_override("function_color", function_color);
	text_edit->add_color_override("member_variable_color", member_variable_color);
	text_edit->add_color_override("breakpoint_color", breakpoint_color);
	text_edit->add_color_override("mark_color", mark_color);
	text_edit->add_color_override("code_folding_color", code_folding_color);
	text_edit->add_color_override("search_result_color", search_result_color);
	text_edit->add_color_override("search_result_border_color", search_result_border_color);
	text_edit->add_color_override("symbol_color", symbol_color);

	text_edit->add_constant_override("line_spacing", EDITOR_DEF("text_editor/theme/line_spacing", 6));

	colors_cache.symbol_color = symbol_color;
	colors_cache.keyword_color = keyword_color;
	colors_cache.basetype_color = basetype_color;
	colors_cache.type_color = type_color;
	colors_cache.comment_color = comment_color;
	colors_cache.string_color = string_color;

	theme_loaded = true;
	if (!script.is_null())
		_set_theme_for_script();
}

void ScriptTextEditor::_set_theme_for_script() {

	if (!theme_loaded)
		return;

	TextEdit *text_edit = code_editor->get_text_edit();

	List<String> keywords;
	script->get_language()->get_reserved_words(&keywords);

	for (List<String>::Element *E = keywords.front(); E; E = E->next()) {

		text_edit->add_keyword_color(E->get(), colors_cache.keyword_color);
	}

	//colorize core types
	const Color basetype_color = colors_cache.basetype_color;
	text_edit->add_keyword_color("String", basetype_color);
	text_edit->add_keyword_color("Vector2", basetype_color);
	text_edit->add_keyword_color("Rect2", basetype_color);
	text_edit->add_keyword_color("Transform2D", basetype_color);
	text_edit->add_keyword_color("Vector3", basetype_color);
	text_edit->add_keyword_color("AABB", basetype_color);
	text_edit->add_keyword_color("Basis", basetype_color);
	text_edit->add_keyword_color("Plane", basetype_color);
	text_edit->add_keyword_color("Transform", basetype_color);
	text_edit->add_keyword_color("Quat", basetype_color);
	text_edit->add_keyword_color("Color", basetype_color);
	text_edit->add_keyword_color("Object", basetype_color);
	text_edit->add_keyword_color("NodePath", basetype_color);
	text_edit->add_keyword_color("RID", basetype_color);
	text_edit->add_keyword_color("Dictionary", basetype_color);
	text_edit->add_keyword_color("Array", basetype_color);
	text_edit->add_keyword_color("PoolByteArray", basetype_color);
	text_edit->add_keyword_color("PoolIntArray", basetype_color);
	text_edit->add_keyword_color("PoolRealArray", basetype_color);
	text_edit->add_keyword_color("PoolStringArray", basetype_color);
	text_edit->add_keyword_color("PoolVector2Array", basetype_color);
	text_edit->add_keyword_color("PoolVector3Array", basetype_color);
	text_edit->add_keyword_color("PoolColorArray", basetype_color);

	//colorize engine types
	List<StringName> types;
	ClassDB::get_class_list(&types);

	for (List<StringName>::Element *E = types.front(); E; E = E->next()) {

		String n = E->get();
		if (n.begins_with("_"))
			n = n.substr(1, n.length());

		text_edit->add_keyword_color(n, colors_cache.type_color);
	}
	_update_member_keywords();

	//colorize comments
	List<String> comments;
	script->get_language()->get_comment_delimiters(&comments);

	for (List<String>::Element *E = comments.front(); E; E = E->next()) {

		String comment = E->get();
		String beg = comment.get_slice(" ", 0);
		String end = comment.get_slice_count(" ") > 1 ? comment.get_slice(" ", 1) : String();

		text_edit->add_color_region(beg, end, colors_cache.comment_color, end == "");
	}

	//colorize strings
	List<String> strings;
	script->get_language()->get_string_delimiters(&strings);
	for (List<String>::Element *E = strings.front(); E; E = E->next()) {

		String string = E->get();
		String beg = string.get_slice(" ", 0);
		String end = string.get_slice_count(" ") > 1 ? string.get_slice(" ", 1) : String();
		text_edit->add_color_region(beg, end, colors_cache.string_color, end == "");
	}
}

void ScriptTextEditor::_show_warnings_panel(bool p_show) {
	warnings_panel->set_visible(p_show);
}

void ScriptTextEditor::_error_pressed() {
	code_editor->goto_error();
}

void ScriptTextEditor::_warning_clicked(Variant p_line) {
	if (p_line.get_type() == Variant::INT) {
		code_editor->get_text_edit()->cursor_set_line(p_line.operator int64_t());
	} else if (p_line.get_type() == Variant::DICTIONARY) {
		Dictionary meta = p_line.operator Dictionary();
		code_editor->get_text_edit()->insert_at("# warning-ignore:" + meta["code"].operator String(), meta["line"].operator int64_t() - 1);
		_validate_script();
	}
}

void ScriptTextEditor::reload_text() {

	ERR_FAIL_COND(script.is_null());

	TextEdit *te = code_editor->get_text_edit();
	int column = te->cursor_get_column();
	int row = te->cursor_get_line();
	int h = te->get_h_scroll();
	int v = te->get_v_scroll();

	te->set_text(script->get_source_code());
	te->clear_undo_history();
	te->cursor_set_line(row);
	te->cursor_set_column(column);
	te->set_h_scroll(h);
	te->set_v_scroll(v);

	te->tag_saved_version();

	code_editor->update_line_and_column();
}

void ScriptTextEditor::_notification(int p_what) {

	switch (p_what) {
		case NOTIFICATION_READY:
			_load_theme_settings();
			_change_syntax_highlighter(EditorSettings::get_singleton()->get_project_metadata("script_text_editor", "syntax_highlighter", 0));
			break;
	}
}

void ScriptTextEditor::add_callback(const String &p_function, PoolStringArray p_args) {

	String code = code_editor->get_text_edit()->get_text();
	int pos = script->get_language()->find_function(p_function, code);
	if (pos == -1) {
		//does not exist
		code_editor->get_text_edit()->deselect();
		pos = code_editor->get_text_edit()->get_line_count() + 2;
		String func = script->get_language()->make_function("", p_function, p_args);
		//code=code+func;
		code_editor->get_text_edit()->cursor_set_line(pos + 1);
		code_editor->get_text_edit()->cursor_set_column(1000000); //none shall be that big
		code_editor->get_text_edit()->insert_text_at_cursor("\n\n" + func);
	}
	code_editor->get_text_edit()->cursor_set_line(pos);
	code_editor->get_text_edit()->cursor_set_column(1);
}

bool ScriptTextEditor::show_members_overview() {
	return true;
}

void ScriptTextEditor::update_settings() {

	code_editor->update_editor_settings();
}

bool ScriptTextEditor::is_unsaved() {

	return code_editor->get_text_edit()->get_version() != code_editor->get_text_edit()->get_saved_version();
}

Variant ScriptTextEditor::get_edit_state() {

	return code_editor->get_edit_state();
}

void ScriptTextEditor::set_edit_state(const Variant &p_state) {

	code_editor->set_edit_state(p_state);
}

void ScriptTextEditor::_convert_case(CodeTextEditor::CaseStyle p_case) {

	code_editor->convert_case(p_case);
}

void ScriptTextEditor::trim_trailing_whitespace() {

	code_editor->trim_trailing_whitespace();
}

void ScriptTextEditor::convert_indent_to_spaces() {

	code_editor->convert_indent_to_spaces();
}

void ScriptTextEditor::convert_indent_to_tabs() {

	code_editor->convert_indent_to_tabs();
}

void ScriptTextEditor::tag_saved_version() {

	code_editor->get_text_edit()->tag_saved_version();
}

void ScriptTextEditor::goto_line(int p_line, bool p_with_error) {

	code_editor->goto_line(p_line);
}

void ScriptTextEditor::goto_line_selection(int p_line, int p_begin, int p_end) {

	code_editor->goto_line_selection(p_line, p_begin, p_end);
}

void ScriptTextEditor::ensure_focus() {

	code_editor->get_text_edit()->grab_focus();
}

String ScriptTextEditor::get_name() {
	String name;

	if (script->get_path().find("local://") == -1 && script->get_path().find("::") == -1) {
		name = script->get_path().get_file();
		if (is_unsaved()) {
			name += "(*)";
		}
	} else if (script->get_name() != "")
		name = script->get_name();
	else
		name = script->get_class() + "(" + itos(script->get_instance_id()) + ")";

	return name;
}

Ref<Texture> ScriptTextEditor::get_icon() {

	if (get_parent_control() && get_parent_control()->has_icon(script->get_class(), "EditorIcons")) {
		return get_parent_control()->get_icon(script->get_class(), "EditorIcons");
	}

	return Ref<Texture>();
}

void ScriptTextEditor::_validate_script() {

	String errortxt;
	int line = -1, col;
	TextEdit *te = code_editor->get_text_edit();

	String text = te->get_text();
	List<String> fnc;
	Set<int> safe_lines;
	List<ScriptLanguage::Warning> warnings;

	if (!script->get_language()->validate(text, line, col, errortxt, script->get_path(), &fnc, &warnings, &safe_lines)) {
		String error_text = "error(" + itos(line) + "," + itos(col) + "): " + errortxt;
		code_editor->set_error(error_text);
		code_editor->set_error_pos(line - 1, col - 1);
	} else {
		code_editor->set_error("");
		line = -1;
		if (!script->is_tool()) {
			script->set_source_code(text);
			script->update_exports();
			_update_member_keywords();
			//script->reload(); //will update all the variables in property editors
		}

		functions.clear();
		for (List<String>::Element *E = fnc.front(); E; E = E->next()) {

			functions.push_back(E->get());
		}
	}

	code_editor->set_warning_nb(warnings.size());
	warnings_panel->clear();
	warnings_panel->push_table(3);
	for (List<ScriptLanguage::Warning>::Element *E = warnings.front(); E; E = E->next()) {
		ScriptLanguage::Warning w = E->get();

		warnings_panel->push_cell();
		warnings_panel->push_meta(w.line - 1);
		warnings_panel->push_color(warnings_panel->get_color("warning_color", "Editor"));
		warnings_panel->add_text(TTR("Line") + " " + itos(w.line));
		warnings_panel->add_text(" (" + w.string_code + "):");
		warnings_panel->pop(); // Color
		warnings_panel->pop(); // Meta goto
		warnings_panel->pop(); // Cell

		warnings_panel->push_cell();
		warnings_panel->add_text(w.message);
		warnings_panel->pop(); // Cell

		Dictionary ignore_meta;
		ignore_meta["line"] = w.line;
		ignore_meta["code"] = w.string_code.to_lower();
		warnings_panel->push_cell();
		warnings_panel->push_meta(ignore_meta);
		warnings_panel->add_text(TTR("(ignore)"));
		warnings_panel->pop(); // Meta ignore
		warnings_panel->pop(); // Cell
		//warnings_panel->add_newline();
	}
	warnings_panel->pop(); // Table

	line--;
	bool highlight_safe = EDITOR_DEF("text_editor/highlighting/highlight_type_safe_lines", true);
	bool last_is_safe = false;
	for (int i = 0; i < te->get_line_count(); i++) {
		te->set_line_as_marked(i, line == i);
		if (highlight_safe) {
			if (safe_lines.has(i + 1)) {
				te->set_line_as_safe(i, true);
				last_is_safe = true;
			} else if (last_is_safe && (te->is_line_comment(i) || te->get_line(i).strip_edges().empty())) {
				te->set_line_as_safe(i, true);
			} else {
				te->set_line_as_safe(i, false);
				last_is_safe = false;
			}
		} else {
			te->set_line_as_safe(i, false);
		}
	}

	emit_signal("name_changed");
	emit_signal("edited_script_changed");
}

static Node *_find_node_for_script(Node *p_base, Node *p_current, const Ref<Script> &p_script) {

	if (p_current->get_owner() != p_base && p_base != p_current)
		return NULL;
	Ref<Script> c = p_current->get_script();
	if (c == p_script)
		return p_current;
	for (int i = 0; i < p_current->get_child_count(); i++) {
		Node *found = _find_node_for_script(p_base, p_current->get_child(i), p_script);
		if (found)
			return found;
	}

	return NULL;
}

static void _find_changed_scripts_for_external_editor(Node *p_base, Node *p_current, Set<Ref<Script> > &r_scripts) {

	if (p_current->get_owner() != p_base && p_base != p_current)
		return;
	Ref<Script> c = p_current->get_script();

	if (c.is_valid())
		r_scripts.insert(c);

	for (int i = 0; i < p_current->get_child_count(); i++) {
		_find_changed_scripts_for_external_editor(p_base, p_current->get_child(i), r_scripts);
	}
}

void ScriptEditor::_update_modified_scripts_for_external_editor(Ref<Script> p_for_script) {

	if (!bool(EditorSettings::get_singleton()->get("text_editor/external/use_external_editor")))
		return;

	ERR_FAIL_COND(!get_tree());

	Set<Ref<Script> > scripts;

	Node *base = get_tree()->get_edited_scene_root();
	if (base) {
		_find_changed_scripts_for_external_editor(base, base, scripts);
	}

	for (Set<Ref<Script> >::Element *E = scripts.front(); E; E = E->next()) {

		Ref<Script> script = E->get();

		if (p_for_script.is_valid() && p_for_script != script)
			continue;

		if (script->get_path() == "" || script->get_path().find("local://") != -1 || script->get_path().find("::") != -1) {

			continue; //internal script, who cares, though weird
		}

		uint64_t last_date = script->get_last_modified_time();
		uint64_t date = FileAccess::get_modified_time(script->get_path());

		if (last_date != date) {

			Ref<Script> rel_script = ResourceLoader::load(script->get_path(), script->get_class(), true);
			ERR_CONTINUE(!rel_script.is_valid());
			script->set_source_code(rel_script->get_source_code());
			script->set_last_modified_time(rel_script->get_last_modified_time());
			script->update_exports();
		}
	}
}

void ScriptTextEditor::_code_complete_scripts(void *p_ud, const String &p_code, List<String> *r_options, bool &r_force) {

	ScriptTextEditor *ste = (ScriptTextEditor *)p_ud;
	ste->_code_complete_script(p_code, r_options, r_force);
}

void ScriptTextEditor::_code_complete_script(const String &p_code, List<String> *r_options, bool &r_force) {

	if (color_panel->is_visible_in_tree()) return;
	Node *base = get_tree()->get_edited_scene_root();
	if (base) {
		base = _find_node_for_script(base, base, script);
	}
	String hint;
	Error err = script->get_language()->complete_code(p_code, script->get_path().get_base_dir(), base, r_options, r_force, hint);
	if (err == OK && hint != "") {
		code_editor->get_text_edit()->set_code_hint(hint);
	}
}

void ScriptTextEditor::_breakpoint_toggled(int p_row) {

	ScriptEditor::get_singleton()->get_debugger()->set_breakpoint(script->get_path(), p_row + 1, code_editor->get_text_edit()->is_line_set_as_breakpoint(p_row));
}

void ScriptTextEditor::_lookup_symbol(const String &p_symbol, int p_row, int p_column) {

	Node *base = get_tree()->get_edited_scene_root();
	if (base) {
		base = _find_node_for_script(base, base, script);
	}

	ScriptLanguage::LookupResult result;
	if (ScriptServer::is_global_class(p_symbol)) {
		EditorNode::get_singleton()->load_resource(ScriptServer::get_global_class_path(p_symbol));
	} else if (p_symbol.is_resource_file()) {
		List<String> scene_extensions;
		ResourceLoader::get_recognized_extensions_for_type("PackedScene", &scene_extensions);

		if (scene_extensions.find(p_symbol.get_extension())) {
			EditorNode::get_singleton()->load_scene(p_symbol);
		} else {
			EditorNode::get_singleton()->load_resource(p_symbol);
		}

	} else if (script->get_language()->lookup_code(code_editor->get_text_edit()->get_text_for_lookup_completion(), p_symbol, script->get_path().get_base_dir(), base, result) == OK) {

		_goto_line(p_row);

		result.class_name = result.class_name.trim_prefix("_");

		switch (result.type) {
			case ScriptLanguage::LookupResult::RESULT_SCRIPT_LOCATION: {

				if (result.script.is_valid()) {
					emit_signal("request_open_script_at_line", result.script, result.location - 1);
				} else {
					emit_signal("request_save_history");
					_goto_line(result.location - 1);
				}
			} break;
			case ScriptLanguage::LookupResult::RESULT_CLASS: {
				emit_signal("go_to_help", "class_name:" + result.class_name);
			} break;
			case ScriptLanguage::LookupResult::RESULT_CLASS_CONSTANT: {

				StringName cname = result.class_name;
				bool success;
				while (true) {
					ClassDB::get_integer_constant(cname, result.class_member, &success);
					if (success) {
						result.class_name = cname;
						cname = ClassDB::get_parent_class(cname);
					} else {
						break;
					}
				}

				emit_signal("go_to_help", "class_constant:" + result.class_name + ":" + result.class_member);

			} break;
			case ScriptLanguage::LookupResult::RESULT_CLASS_PROPERTY: {
				emit_signal("go_to_help", "class_property:" + result.class_name + ":" + result.class_member);

			} break;
			case ScriptLanguage::LookupResult::RESULT_CLASS_METHOD: {

				StringName cname = result.class_name;

				while (true) {
					if (ClassDB::has_method(cname, result.class_member)) {
						result.class_name = cname;
						cname = ClassDB::get_parent_class(cname);
					} else {
						break;
					}
				}

				emit_signal("go_to_help", "class_method:" + result.class_name + ":" + result.class_member);

			} break;
			case ScriptLanguage::LookupResult::RESULT_CLASS_ENUM: {

				StringName cname = result.class_name;
				StringName success;
				while (true) {
					success = ClassDB::get_integer_constant_enum(cname, result.class_member, true);
					if (success != StringName()) {
						result.class_name = cname;
						cname = ClassDB::get_parent_class(cname);
					} else {
						break;
					}
				}

				emit_signal("go_to_help", "class_enum:" + result.class_name + ":" + result.class_member);

			} break;
			case ScriptLanguage::LookupResult::RESULT_CLASS_TBD_GLOBALSCOPE: {
				emit_signal("go_to_help", "class_global:" + result.class_name + ":" + result.class_member);
			} break;
		}
	}
}

void ScriptTextEditor::_edit_option(int p_op) {

	TextEdit *tx = code_editor->get_text_edit();

	switch (p_op) {
		case EDIT_UNDO: {

			tx->undo();
			tx->call_deferred("grab_focus");
		} break;
		case EDIT_REDO: {

			tx->redo();
			tx->call_deferred("grab_focus");
		} break;
		case EDIT_CUT: {

			tx->cut();
			tx->call_deferred("grab_focus");
		} break;
		case EDIT_COPY: {

			tx->copy();
			tx->call_deferred("grab_focus");
		} break;
		case EDIT_PASTE: {

			tx->paste();
			tx->call_deferred("grab_focus");
		} break;
		case EDIT_SELECT_ALL: {

			tx->select_all();
			tx->call_deferred("grab_focus");
		} break;
		case EDIT_MOVE_LINE_UP: {

			code_editor->move_lines_up();
		} break;
		case EDIT_MOVE_LINE_DOWN: {

			code_editor->move_lines_down();
		} break;
		case EDIT_INDENT_LEFT: {

			Ref<Script> scr = script;
			if (scr.is_null())
				return;

			tx->indent_left();
		} break;
		case EDIT_INDENT_RIGHT: {

			Ref<Script> scr = script;
			if (scr.is_null())
				return;

			tx->indent_right();
		} break;
		case EDIT_DELETE_LINE: {

			code_editor->delete_lines();
		} break;
		case EDIT_CLONE_DOWN: {

			code_editor->clone_lines_down();
		} break;
		case EDIT_TOGGLE_FOLD_LINE: {

			tx->toggle_fold_line(tx->cursor_get_line());
			tx->update();
		} break;
		case EDIT_FOLD_ALL_LINES: {

			tx->fold_all_lines();
			tx->update();
		} break;
		case EDIT_UNFOLD_ALL_LINES: {

			tx->unhide_all_lines();
			tx->update();
		} break;
		case EDIT_TOGGLE_COMMENT: {

			Ref<Script> scr = script;
			if (scr.is_null())
				return;

			String delimiter = "#";
			List<String> comment_delimiters;
			scr->get_language()->get_comment_delimiters(&comment_delimiters);

			for (List<String>::Element *E = comment_delimiters.front(); E; E = E->next()) {
				String script_delimiter = E->get();
				if (script_delimiter.find(" ") == -1) {
					delimiter = script_delimiter;
					break;
				}
			}

			tx->begin_complex_operation();
			if (tx->is_selection_active()) {
				int begin = tx->get_selection_from_line();
				int end = tx->get_selection_to_line();

				// End of selection ends on the first column of the last line, ignore it.
				if (tx->get_selection_to_column() == 0)
					end -= 1;

				int col_to = tx->get_selection_to_column();
				int cursor_pos = tx->cursor_get_column();

				// Check if all lines in the selected block are commented
				bool is_commented = true;
				for (int i = begin; i <= end; i++) {
					if (!tx->get_line(i).begins_with(delimiter)) {
						is_commented = false;
						break;
					}
				}
				for (int i = begin; i <= end; i++) {
					String line_text = tx->get_line(i);

					if (line_text.strip_edges().empty()) {
						line_text = delimiter;
					} else {
						if (is_commented) {
							line_text = line_text.substr(delimiter.length(), line_text.length());
						} else {
							line_text = delimiter + line_text;
						}
					}
					tx->set_line(i, line_text);
				}

				// Adjust selection & cursor position.
				int offset = is_commented ? -1 : 1;
				int col_from = tx->get_selection_from_column() > 0 ? tx->get_selection_from_column() + offset : 0;

				if (is_commented && tx->cursor_get_column() == tx->get_line(tx->cursor_get_line()).length() + 1)
					cursor_pos += 1;

				if (tx->get_selection_to_column() != 0 && col_to != tx->get_line(tx->get_selection_to_line()).length() + 1)
					col_to += offset;

				if (tx->cursor_get_column() != 0)
					cursor_pos += offset;

				tx->select(begin, col_from, tx->get_selection_to_line(), col_to);
				tx->cursor_set_column(cursor_pos);

			} else {
				int begin = tx->cursor_get_line();
				String line_text = tx->get_line(begin);

				int col = tx->cursor_get_column();
				if (line_text.begins_with(delimiter)) {
					line_text = line_text.substr(delimiter.length(), line_text.length());
					col -= 1;
				} else {
					line_text = delimiter + line_text;
					col += 1;
				}

				tx->set_line(begin, line_text);
				tx->cursor_set_column(col);
			}
			tx->end_complex_operation();
			tx->update();

		} break;
		case EDIT_COMPLETE: {

			tx->query_code_comple();
		} break;
		case EDIT_AUTO_INDENT: {

			String text = tx->get_text();
			Ref<Script> scr = script;
			if (scr.is_null())
				return;

			tx->begin_complex_operation();
			int begin, end;
			if (tx->is_selection_active()) {
				begin = tx->get_selection_from_line();
				end = tx->get_selection_to_line();
				// ignore if the cursor is not past the first column
				if (tx->get_selection_to_column() == 0) {
					end--;
				}
			} else {
				begin = 0;
				end = tx->get_line_count() - 1;
			}
			scr->get_language()->auto_indent_code(text, begin, end);
			Vector<String> lines = text.split("\n");
			for (int i = begin; i <= end; ++i) {
				tx->set_line(i, lines[i]);
			}

			tx->end_complex_operation();
		} break;
		case EDIT_TRIM_TRAILING_WHITESAPCE: {

			trim_trailing_whitespace();
		} break;
		case EDIT_CONVERT_INDENT_TO_SPACES: {

			convert_indent_to_spaces();
		} break;
		case EDIT_CONVERT_INDENT_TO_TABS: {

			convert_indent_to_tabs();
		} break;
		case EDIT_PICK_COLOR: {

			color_panel->popup();
		} break;
		case EDIT_TO_UPPERCASE: {

			_convert_case(CodeTextEditor::UPPER);
		} break;
		case EDIT_TO_LOWERCASE: {

			_convert_case(CodeTextEditor::LOWER);
		} break;
		case EDIT_CAPITALIZE: {

			_convert_case(CodeTextEditor::CAPITALIZE);
		} break;
		case SEARCH_FIND: {

			code_editor->get_find_replace_bar()->popup_search();
		} break;
		case SEARCH_FIND_NEXT: {

			code_editor->get_find_replace_bar()->search_next();
		} break;
		case SEARCH_FIND_PREV: {

			code_editor->get_find_replace_bar()->search_prev();
		} break;
		case SEARCH_REPLACE: {

			code_editor->get_find_replace_bar()->popup_replace();
		} break;
		case SEARCH_IN_FILES: {

			String selected_text = code_editor->get_text_edit()->get_selection_text();

			// Yep, because it doesn't make sense to instance this dialog for every single script open...
			// So this will be delegated to the ScriptEditor
			emit_signal("search_in_files_requested", selected_text);

		} break;
		case SEARCH_LOCATE_FUNCTION: {

			quick_open->popup_dialog(get_functions());
			quick_open->set_title(TTR("Go to Function"));
		} break;
		case SEARCH_GOTO_LINE: {

			goto_line_dialog->popup_find_line(tx);
		} break;
		case DEBUG_TOGGLE_BREAKPOINT: {

			int line = tx->cursor_get_line();
			bool dobreak = !tx->is_line_set_as_breakpoint(line);
			tx->set_line_as_breakpoint(line, dobreak);
			ScriptEditor::get_singleton()->get_debugger()->set_breakpoint(script->get_path(), line + 1, dobreak);
		} break;
		case DEBUG_REMOVE_ALL_BREAKPOINTS: {

			List<int> bpoints;
			tx->get_breakpoints(&bpoints);

			for (List<int>::Element *E = bpoints.front(); E; E = E->next()) {
				int line = E->get();
				bool dobreak = !tx->is_line_set_as_breakpoint(line);
				tx->set_line_as_breakpoint(line, dobreak);
				ScriptEditor::get_singleton()->get_debugger()->set_breakpoint(script->get_path(), line + 1, dobreak);
			}
		} break;
		case DEBUG_GOTO_NEXT_BREAKPOINT: {

			List<int> bpoints;
			tx->get_breakpoints(&bpoints);
			if (bpoints.size() <= 0) {
				return;
			}

			int line = tx->cursor_get_line();

			// wrap around
			if (line >= bpoints[bpoints.size() - 1]) {
				tx->unfold_line(bpoints[0]);
				tx->cursor_set_line(bpoints[0]);
			} else {
				for (List<int>::Element *E = bpoints.front(); E; E = E->next()) {
					int bline = E->get();
					if (bline > line) {
						tx->unfold_line(bline);
						tx->cursor_set_line(bline);
						return;
					}
				}
			}

		} break;
		case DEBUG_GOTO_PREV_BREAKPOINT: {

			List<int> bpoints;
			tx->get_breakpoints(&bpoints);
			if (bpoints.size() <= 0) {
				return;
			}

			int line = tx->cursor_get_line();
			// wrap around
			if (line <= bpoints[0]) {
				tx->unfold_line(bpoints[bpoints.size() - 1]);
				tx->cursor_set_line(bpoints[bpoints.size() - 1]);
			} else {
				for (List<int>::Element *E = bpoints.back(); E; E = E->prev()) {
					int bline = E->get();
					if (bline < line) {
						tx->unfold_line(bline);
						tx->cursor_set_line(bline);
						return;
					}
				}
			}

		} break;
		case HELP_CONTEXTUAL: {

			String text = tx->get_selection_text();
			if (text == "")
				text = tx->get_word_under_cursor();
			if (text != "") {
				emit_signal("request_help", text);
			}
		} break;
		case LOOKUP_SYMBOL: {

			String text = tx->get_word_under_cursor();
			if (text == "")
				text = tx->get_selection_text();
			if (text != "") {
				_lookup_symbol(text, tx->cursor_get_line(), tx->cursor_get_column());
			}
		} break;
	}
}

void ScriptTextEditor::add_syntax_highlighter(SyntaxHighlighter *p_highlighter) {
	highlighters[p_highlighter->get_name()] = p_highlighter;
	highlighter_menu->add_radio_check_item(p_highlighter->get_name());
}

void ScriptTextEditor::set_syntax_highlighter(SyntaxHighlighter *p_highlighter) {
	TextEdit *te = code_editor->get_text_edit();
	te->_set_syntax_highlighting(p_highlighter);
	if (p_highlighter != NULL)
		highlighter_menu->set_item_checked(highlighter_menu->get_item_idx_from_text(p_highlighter->get_name()), true);
	else
		highlighter_menu->set_item_checked(highlighter_menu->get_item_idx_from_text(TTR("Standard")), true);
}

void ScriptTextEditor::_change_syntax_highlighter(int p_idx) {
	Map<String, SyntaxHighlighter *>::Element *el = highlighters.front();
	while (el != NULL) {
		highlighter_menu->set_item_checked(highlighter_menu->get_item_idx_from_text(el->key()), false);
		el = el->next();
	}
	// highlighter_menu->set_item_checked(p_idx, true);
	set_syntax_highlighter(highlighters[highlighter_menu->get_item_text(p_idx)]);
	EditorSettings::get_singleton()->set_project_metadata("script_text_editor", "syntax_highlighter", p_idx);
}

void ScriptTextEditor::_bind_methods() {

	ClassDB::bind_method("_validate_script", &ScriptTextEditor::_validate_script);
	ClassDB::bind_method("_load_theme_settings", &ScriptTextEditor::_load_theme_settings);
	ClassDB::bind_method("_breakpoint_toggled", &ScriptTextEditor::_breakpoint_toggled);
	ClassDB::bind_method("_change_syntax_highlighter", &ScriptTextEditor::_change_syntax_highlighter);
	ClassDB::bind_method("_edit_option", &ScriptTextEditor::_edit_option);
	ClassDB::bind_method("_goto_line", &ScriptTextEditor::_goto_line);
	ClassDB::bind_method("_lookup_symbol", &ScriptTextEditor::_lookup_symbol);
	ClassDB::bind_method("_text_edit_gui_input", &ScriptTextEditor::_text_edit_gui_input);
	ClassDB::bind_method("_show_warnings_panel", &ScriptTextEditor::_show_warnings_panel);
	ClassDB::bind_method("_error_pressed", &ScriptTextEditor::_error_pressed);
	ClassDB::bind_method("_warning_clicked", &ScriptTextEditor::_warning_clicked);
	ClassDB::bind_method("_color_changed", &ScriptTextEditor::_color_changed);

	ClassDB::bind_method("get_drag_data_fw", &ScriptTextEditor::get_drag_data_fw);
	ClassDB::bind_method("can_drop_data_fw", &ScriptTextEditor::can_drop_data_fw);
	ClassDB::bind_method("drop_data_fw", &ScriptTextEditor::drop_data_fw);
}

Control *ScriptTextEditor::get_edit_menu() {

	return edit_hb;
}

void ScriptTextEditor::clear_edit_menu() {
	memdelete(edit_hb);
}

void ScriptTextEditor::reload(bool p_soft) {

	TextEdit *te = code_editor->get_text_edit();
	Ref<Script> scr = script;
	if (scr.is_null())
		return;
	scr->set_source_code(te->get_text());
	bool soft = p_soft || scr->get_instance_base_type() == "EditorPlugin"; //always soft-reload editor plugins

	scr->get_language()->reload_tool_script(scr, soft);
}

void ScriptTextEditor::get_breakpoints(List<int> *p_breakpoints) {

	code_editor->get_text_edit()->get_breakpoints(p_breakpoints);
}

void ScriptTextEditor::set_tooltip_request_func(String p_method, Object *p_obj) {

	code_editor->get_text_edit()->set_tooltip_request_func(p_obj, p_method, this);
}

void ScriptTextEditor::set_debugger_active(bool p_active) {
}

Variant ScriptTextEditor::get_drag_data_fw(const Point2 &p_point, Control *p_from) {

	return Variant();
}

bool ScriptTextEditor::can_drop_data_fw(const Point2 &p_point, const Variant &p_data, Control *p_from) const {

	Dictionary d = p_data;
	if (d.has("type") && (String(d["type"]) == "resource" ||
								 String(d["type"]) == "files" ||
								 String(d["type"]) == "nodes")) {

		return true;
	}

	return false;
}

#ifdef TOOLS_ENABLED

static Node *_find_script_node(Node *p_edited_scene, Node *p_current_node, const Ref<Script> &script) {

	if (p_edited_scene != p_current_node && p_current_node->get_owner() != p_edited_scene)
		return NULL;

	Ref<Script> scr = p_current_node->get_script();

	if (scr.is_valid() && scr == script)
		return p_current_node;

	for (int i = 0; i < p_current_node->get_child_count(); i++) {
		Node *n = _find_script_node(p_edited_scene, p_current_node->get_child(i), script);
		if (n)
			return n;
	}

	return NULL;
}

#else

static Node *_find_script_node(Node *p_edited_scene, Node *p_current_node, const Ref<Script> &script) {

	return NULL;
}
#endif

void ScriptTextEditor::drop_data_fw(const Point2 &p_point, const Variant &p_data, Control *p_from) {

	Dictionary d = p_data;

	TextEdit *te = code_editor->get_text_edit();
	int row, col;
	te->_get_mouse_pos(p_point, row, col);

	if (d.has("type") && String(d["type"]) == "resource") {

		Ref<Resource> res = d["resource"];
		if (!res.is_valid()) {
			return;
		}

		if (res->get_path().is_resource_file()) {
			EditorNode::get_singleton()->show_warning(TTR("Only resources from filesystem can be dropped."));
			return;
		}

		te->cursor_set_line(row);
		te->cursor_set_column(col);
		te->insert_text_at_cursor(res->get_path());
	}

	if (d.has("type") && String(d["type"]) == "files") {

		Array files = d["files"];

		String text_to_drop;
		for (int i = 0; i < files.size(); i++) {

			if (i > 0)
				text_to_drop += ",";
			text_to_drop += "\"" + String(files[i]).c_escape() + "\"";
		}

		te->cursor_set_line(row);
		te->cursor_set_column(col);
		te->insert_text_at_cursor(text_to_drop);
	}

	if (d.has("type") && String(d["type"]) == "nodes") {

		Node *sn = _find_script_node(get_tree()->get_edited_scene_root(), get_tree()->get_edited_scene_root(), script);

		if (!sn) {
			EditorNode::get_singleton()->show_warning("Can't drop nodes because script '" + get_name() + "' is not used in this scene.");
			return;
		}

		Array nodes = d["nodes"];
		String text_to_drop;
		for (int i = 0; i < nodes.size(); i++) {

			if (i > 0)
				text_to_drop += ",";

			NodePath np = nodes[i];
			Node *node = get_node(np);
			if (!node) {
				continue;
			}

			String path = sn->get_path_to(node);
			text_to_drop += "\"" + path.c_escape() + "\"";
		}

		te->cursor_set_line(row);
		te->cursor_set_column(col);
		te->insert_text_at_cursor(text_to_drop);
	}
}

void ScriptTextEditor::_text_edit_gui_input(const Ref<InputEvent> &ev) {

	Ref<InputEventMouseButton> mb = ev;

	if (mb.is_valid()) {

		if (mb->get_button_index() == BUTTON_RIGHT && mb->is_pressed()) {
			int col, row;
			TextEdit *tx = code_editor->get_text_edit();
			tx->_get_mouse_pos(mb->get_global_position() - tx->get_global_position(), row, col);
			Vector2 mpos = mb->get_global_position() - tx->get_global_position();

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

			String word_at_mouse = tx->get_word_at_pos(mpos);
			if (word_at_mouse == "")
				word_at_mouse = tx->get_word_under_cursor();
			if (word_at_mouse == "")
				word_at_mouse = tx->get_selection_text();

			bool has_color = (word_at_mouse == "Color");
			bool foldable = tx->can_fold(row) || tx->is_folded(row);
			bool open_docs = false;
			bool goto_definition = false;

			if (word_at_mouse.is_resource_file()) {
				open_docs = true;
			} else {

				Node *base = get_tree()->get_edited_scene_root();
				if (base) {
					base = _find_node_for_script(base, base, script);
				}
				ScriptLanguage::LookupResult result;
				if (script->get_language()->lookup_code(code_editor->get_text_edit()->get_text_for_lookup_completion(), word_at_mouse, script->get_path().get_base_dir(), base, result) == OK) {
					open_docs = true;
				}
			}

			if (has_color) {
				String line = tx->get_line(row);
				color_position.x = row;
				color_position.y = col;

				int begin = 0;
				int end = 0;
				bool valid = false;
				for (int i = col; i < line.length(); i++) {
					if (line[i] == '(') {
						begin = i;
						continue;
					} else if (line[i] == ')') {
						end = i + 1;
						valid = true;
						break;
					}
				}
				if (valid) {
					color_args = line.substr(begin, end - begin);
					String stripped = color_args.replace(" ", "").replace("(", "").replace(")", "");
					Vector<float> color = stripped.split_floats(",");
					if (color.size() > 2) {
						float alpha = color.size() > 3 ? color[3] : 1.0f;
						color_picker->set_pick_color(Color(color[0], color[1], color[2], alpha));
					}
					color_panel->set_position(get_global_transform().xform(get_local_mouse_position()));
				} else {
					has_color = false;
				}
			}
			_make_context_menu(tx->is_selection_active(), has_color, foldable, open_docs, goto_definition);
		}
	}
}

void ScriptTextEditor::_color_changed(const Color &p_color) {
	String new_args;
	if (p_color.a == 1.0f) {
		new_args = String("(" + rtos(p_color.r) + ", " + rtos(p_color.g) + ", " + rtos(p_color.b) + ")");
	} else {
		new_args = String("(" + rtos(p_color.r) + ", " + rtos(p_color.g) + ", " + rtos(p_color.b) + ", " + rtos(p_color.a) + ")");
	}

	String line = code_editor->get_text_edit()->get_line(color_position.x);
	int color_args_pos = line.find(color_args, color_position.y);
	String line_with_replaced_args = line;
	line_with_replaced_args.erase(color_args_pos, color_args.length());
	line_with_replaced_args = line_with_replaced_args.insert(color_args_pos, new_args);

	color_args = new_args;
	code_editor->get_text_edit()->set_line(color_position.x, line_with_replaced_args);
	code_editor->get_text_edit()->update();
}

void ScriptTextEditor::_make_context_menu(bool p_selection, bool p_color, bool p_foldable, bool p_open_docs, bool p_goto_definition) {

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

	if (p_selection) {
		context_menu->add_separator();
		context_menu->add_shortcut(ED_GET_SHORTCUT("script_text_editor/convert_to_uppercase"), EDIT_TO_UPPERCASE);
		context_menu->add_shortcut(ED_GET_SHORTCUT("script_text_editor/convert_to_lowercase"), EDIT_TO_LOWERCASE);
	}
	if (p_foldable)
		context_menu->add_shortcut(ED_GET_SHORTCUT("script_text_editor/toggle_fold_line"), EDIT_TOGGLE_FOLD_LINE);

	if (p_color || p_open_docs || p_goto_definition) {
		context_menu->add_separator();
		if (p_open_docs)
			context_menu->add_item(TTR("Lookup Symbol"), LOOKUP_SYMBOL);
		if (p_color)
			context_menu->add_item(TTR("Pick Color"), EDIT_PICK_COLOR);
	}

	context_menu->set_position(get_global_transform().xform(get_local_mouse_position()));
	context_menu->set_size(Vector2(1, 1));
	context_menu->popup();
}

ScriptTextEditor::ScriptTextEditor() {

	theme_loaded = false;

	VSplitContainer *editor_box = memnew(VSplitContainer);
	add_child(editor_box);
	editor_box->set_anchors_and_margins_preset(Control::PRESET_WIDE);
	editor_box->set_v_size_flags(SIZE_EXPAND_FILL);

	code_editor = memnew(CodeTextEditor);
	editor_box->add_child(code_editor);
	code_editor->add_constant_override("separation", 2);
	code_editor->set_anchors_and_margins_preset(Control::PRESET_WIDE);
	code_editor->connect("validate_script", this, "_validate_script");
	code_editor->connect("load_theme_settings", this, "_load_theme_settings");
	code_editor->set_code_complete_func(_code_complete_scripts, this);
	code_editor->get_text_edit()->connect("breakpoint_toggled", this, "_breakpoint_toggled");
	code_editor->get_text_edit()->connect("symbol_lookup", this, "_lookup_symbol");
	code_editor->set_v_size_flags(SIZE_EXPAND_FILL);

	warnings_panel = memnew(RichTextLabel);
	editor_box->add_child(warnings_panel);
	warnings_panel->set_custom_minimum_size(Size2(0, 100 * EDSCALE));
	warnings_panel->set_h_size_flags(SIZE_EXPAND_FILL);
	warnings_panel->set_meta_underline(true);
	warnings_panel->set_selection_enabled(true);
	warnings_panel->set_focus_mode(FOCUS_CLICK);
	warnings_panel->hide();

	code_editor->connect("error_pressed", this, "_error_pressed");
	code_editor->connect("show_warnings_panel", this, "_show_warnings_panel");
	warnings_panel->connect("meta_clicked", this, "_warning_clicked");

	update_settings();

	code_editor->get_text_edit()->set_callhint_settings(
			EditorSettings::get_singleton()->get("text_editor/completion/put_callhint_tooltip_below_current_line"),
			EditorSettings::get_singleton()->get("text_editor/completion/callhint_tooltip_offset"));

	code_editor->get_text_edit()->set_select_identifiers_on_hover(true);
	code_editor->get_text_edit()->set_context_menu_enabled(false);
	code_editor->get_text_edit()->connect("gui_input", this, "_text_edit_gui_input");

	context_menu = memnew(PopupMenu);
	add_child(context_menu);
	context_menu->connect("id_pressed", this, "_edit_option");
	context_menu->set_hide_on_window_lose_focus(true);

	color_panel = memnew(PopupPanel);
	add_child(color_panel);
	color_picker = memnew(ColorPicker);
	color_picker->set_deferred_mode(true);
	color_panel->add_child(color_picker);
	color_picker->connect("color_changed", this, "_color_changed");

	edit_hb = memnew(HBoxContainer);

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
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/toggle_fold_line"), EDIT_TOGGLE_FOLD_LINE);
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/fold_all_lines"), EDIT_FOLD_ALL_LINES);
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/unfold_all_lines"), EDIT_UNFOLD_ALL_LINES);
	edit_menu->get_popup()->add_separator();
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/clone_down"), EDIT_CLONE_DOWN);
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/complete_symbol"), EDIT_COMPLETE);
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/trim_trailing_whitespace"), EDIT_TRIM_TRAILING_WHITESAPCE);
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/convert_indent_to_spaces"), EDIT_CONVERT_INDENT_TO_SPACES);
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/convert_indent_to_tabs"), EDIT_CONVERT_INDENT_TO_TABS);
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/auto_indent"), EDIT_AUTO_INDENT);
	edit_menu->get_popup()->connect("id_pressed", this, "_edit_option");
	edit_menu->get_popup()->add_separator();
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/toggle_breakpoint"), DEBUG_TOGGLE_BREAKPOINT);
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/remove_all_breakpoints"), DEBUG_REMOVE_ALL_BREAKPOINTS);
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/goto_next_breakpoint"), DEBUG_GOTO_NEXT_BREAKPOINT);
	edit_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/goto_previous_breakpoint"), DEBUG_GOTO_PREV_BREAKPOINT);
	edit_menu->get_popup()->add_separator();

	PopupMenu *convert_case = memnew(PopupMenu);
	convert_case->set_name("convert_case");
	edit_menu->get_popup()->add_child(convert_case);
	edit_menu->get_popup()->add_submenu_item(TTR("Convert Case"), "convert_case");
	convert_case->add_shortcut(ED_SHORTCUT("script_text_editor/convert_to_uppercase", TTR("Uppercase"), KEY_MASK_SHIFT | KEY_F4), EDIT_TO_UPPERCASE);
	convert_case->add_shortcut(ED_SHORTCUT("script_text_editor/convert_to_lowercase", TTR("Lowercase"), KEY_MASK_SHIFT | KEY_F5), EDIT_TO_LOWERCASE);
	convert_case->add_shortcut(ED_SHORTCUT("script_text_editor/capitalize", TTR("Capitalize"), KEY_MASK_SHIFT | KEY_F6), EDIT_CAPITALIZE);
	convert_case->connect("id_pressed", this, "_edit_option");

	highlighters[TTR("Standard")] = NULL;
	highlighter_menu = memnew(PopupMenu);
	highlighter_menu->set_name("highlighter_menu");
	edit_menu->get_popup()->add_child(highlighter_menu);
	edit_menu->get_popup()->add_submenu_item(TTR("Syntax Highlighter"), "highlighter_menu");
	highlighter_menu->add_radio_check_item(TTR("Standard"));
	highlighter_menu->connect("id_pressed", this, "_change_syntax_highlighter");

	search_menu = memnew(MenuButton);
	edit_hb->add_child(search_menu);
	search_menu->set_text(TTR("Search"));
	search_menu->set_switch_on_hover(true);
	search_menu->get_popup()->set_hide_on_window_lose_focus(true);
	search_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/find"), SEARCH_FIND);
	search_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/find_next"), SEARCH_FIND_NEXT);
	search_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/find_previous"), SEARCH_FIND_PREV);
	search_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/replace"), SEARCH_REPLACE);
	search_menu->get_popup()->add_separator();
	search_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/find_in_files"), SEARCH_IN_FILES);
	search_menu->get_popup()->add_separator();
	search_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/goto_function"), SEARCH_LOCATE_FUNCTION);
	search_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/goto_line"), SEARCH_GOTO_LINE);
	search_menu->get_popup()->add_separator();
	search_menu->get_popup()->add_shortcut(ED_GET_SHORTCUT("script_text_editor/contextual_help"), HELP_CONTEXTUAL);

	search_menu->get_popup()->connect("id_pressed", this, "_edit_option");

	edit_hb->add_child(edit_menu);

	quick_open = memnew(ScriptEditorQuickOpen);
	add_child(quick_open);
	quick_open->connect("goto_line", this, "_goto_line");

	goto_line_dialog = memnew(GotoLineDialog);
	add_child(goto_line_dialog);

	code_editor->get_text_edit()->set_drag_forwarding(this);
}

static ScriptEditorBase *create_editor(const RES &p_resource) {

	if (Object::cast_to<Script>(*p_resource)) {
		return memnew(ScriptTextEditor);
	}
	return NULL;
}

void ScriptTextEditor::register_editor() {

	ED_SHORTCUT("script_text_editor/undo", TTR("Undo"), KEY_MASK_CMD | KEY_Z);
	ED_SHORTCUT("script_text_editor/redo", TTR("Redo"), KEY_MASK_CMD | KEY_Y);
	ED_SHORTCUT("script_text_editor/cut", TTR("Cut"), KEY_MASK_CMD | KEY_X);
	ED_SHORTCUT("script_text_editor/copy", TTR("Copy"), KEY_MASK_CMD | KEY_C);
	ED_SHORTCUT("script_text_editor/paste", TTR("Paste"), KEY_MASK_CMD | KEY_V);
	ED_SHORTCUT("script_text_editor/select_all", TTR("Select All"), KEY_MASK_CMD | KEY_A);
	ED_SHORTCUT("script_text_editor/move_up", TTR("Move Up"), KEY_MASK_ALT | KEY_UP);
	ED_SHORTCUT("script_text_editor/move_down", TTR("Move Down"), KEY_MASK_ALT | KEY_DOWN);
	ED_SHORTCUT("script_text_editor/delete_line", TTR("Delete Line"), KEY_MASK_CMD | KEY_MASK_SHIFT | KEY_K);

	//leave these at zero, same can be accomplished with tab/shift-tab, including selection
	//the next/previous in history shortcut in this case makes a lot more sene.

	ED_SHORTCUT("script_text_editor/indent_left", TTR("Indent Left"), 0);
	ED_SHORTCUT("script_text_editor/indent_right", TTR("Indent Right"), 0);
	ED_SHORTCUT("script_text_editor/toggle_comment", TTR("Toggle Comment"), KEY_MASK_CMD | KEY_K);
	ED_SHORTCUT("script_text_editor/toggle_fold_line", TTR("Fold/Unfold Line"), KEY_MASK_ALT | KEY_F);
	ED_SHORTCUT("script_text_editor/fold_all_lines", TTR("Fold All Lines"), 0);
	ED_SHORTCUT("script_text_editor/unfold_all_lines", TTR("Unfold All Lines"), 0);
#ifdef OSX_ENABLED
	ED_SHORTCUT("script_text_editor/clone_down", TTR("Clone Down"), KEY_MASK_SHIFT | KEY_MASK_CMD | KEY_C);
	ED_SHORTCUT("script_text_editor/complete_symbol", TTR("Complete Symbol"), KEY_MASK_CTRL | KEY_SPACE);
#else
	ED_SHORTCUT("script_text_editor/clone_down", TTR("Clone Down"), KEY_MASK_CMD | KEY_B);
	ED_SHORTCUT("script_text_editor/complete_symbol", TTR("Complete Symbol"), KEY_MASK_CMD | KEY_SPACE);
#endif
	ED_SHORTCUT("script_text_editor/trim_trailing_whitespace", TTR("Trim Trailing Whitespace"), KEY_MASK_CMD | KEY_MASK_ALT | KEY_T);
	ED_SHORTCUT("script_text_editor/convert_indent_to_spaces", TTR("Convert Indent to Spaces"), KEY_MASK_CMD | KEY_MASK_SHIFT | KEY_Y);
	ED_SHORTCUT("script_text_editor/convert_indent_to_tabs", TTR("Convert Indent to Tabs"), KEY_MASK_CMD | KEY_MASK_SHIFT | KEY_I);
	ED_SHORTCUT("script_text_editor/auto_indent", TTR("Auto Indent"), KEY_MASK_CMD | KEY_I);

#ifdef OSX_ENABLED
	ED_SHORTCUT("script_text_editor/toggle_breakpoint", TTR("Toggle Breakpoint"), KEY_MASK_CMD | KEY_MASK_SHIFT | KEY_B);
#else
	ED_SHORTCUT("script_text_editor/toggle_breakpoint", TTR("Toggle Breakpoint"), KEY_F9);
#endif
	ED_SHORTCUT("script_text_editor/remove_all_breakpoints", TTR("Remove All Breakpoints"), KEY_MASK_CMD | KEY_MASK_SHIFT | KEY_F9);
	ED_SHORTCUT("script_text_editor/goto_next_breakpoint", TTR("Go to Next Breakpoint"), KEY_MASK_CMD | KEY_PERIOD);
	ED_SHORTCUT("script_text_editor/goto_previous_breakpoint", TTR("Go to Previous Breakpoint"), KEY_MASK_CMD | KEY_COMMA);

	ED_SHORTCUT("script_text_editor/find", TTR("Find..."), KEY_MASK_CMD | KEY_F);
#ifdef OSX_ENABLED
	ED_SHORTCUT("script_text_editor/find_next", TTR("Find Next"), KEY_MASK_CMD | KEY_G);
	ED_SHORTCUT("script_text_editor/find_previous", TTR("Find Previous"), KEY_MASK_CMD | KEY_MASK_SHIFT | KEY_G);
	ED_SHORTCUT("script_text_editor/replace", TTR("Replace..."), KEY_MASK_ALT | KEY_MASK_CMD | KEY_F);
#else
	ED_SHORTCUT("script_text_editor/find_next", TTR("Find Next"), KEY_F3);
	ED_SHORTCUT("script_text_editor/find_previous", TTR("Find Previous"), KEY_MASK_SHIFT | KEY_F3);
	ED_SHORTCUT("script_text_editor/replace", TTR("Replace..."), KEY_MASK_CMD | KEY_R);
#endif

	ED_SHORTCUT("script_text_editor/find_in_files", TTR("Find in Files..."), KEY_MASK_CMD | KEY_MASK_SHIFT | KEY_F);

#ifdef OSX_ENABLED
	ED_SHORTCUT("script_text_editor/goto_function", TTR("Go to Function..."), KEY_MASK_CTRL | KEY_MASK_CMD | KEY_J);
#else
	ED_SHORTCUT("script_text_editor/goto_function", TTR("Go to Function..."), KEY_MASK_ALT | KEY_MASK_CMD | KEY_F);
#endif
	ED_SHORTCUT("script_text_editor/goto_line", TTR("Go to Line..."), KEY_MASK_CMD | KEY_L);

#ifdef OSX_ENABLED
	ED_SHORTCUT("script_text_editor/contextual_help", TTR("Contextual Help"), KEY_MASK_ALT | KEY_MASK_SHIFT | KEY_SPACE);
#else
	ED_SHORTCUT("script_text_editor/contextual_help", TTR("Contextual Help"), KEY_MASK_SHIFT | KEY_F1);
#endif

	ScriptEditor::register_create_script_editor_function(create_editor);
}
