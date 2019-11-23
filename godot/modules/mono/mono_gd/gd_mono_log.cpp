/*************************************************************************/
/*  gd_mono_log.cpp                                                      */
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

#include "gd_mono_log.h"

#include <mono/utils/mono-logger.h>
#include <stdlib.h> // abort

#include "core/os/dir_access.h"
#include "core/os/os.h"

#include "../godotsharp_dirs.h"

static int log_level_get_id(const char *p_log_level) {

	const char *valid_log_levels[] = { "error", "critical", "warning", "message", "info", "debug", NULL };

	int i = 0;
	while (valid_log_levels[i]) {
		if (!strcmp(valid_log_levels[i], p_log_level))
			return i;
		i++;
	}

	return -1;
}

static void mono_log_callback(const char *log_domain, const char *log_level, const char *message, mono_bool fatal, void *user_data) {

	FileAccess *f = GDMonoLog::get_singleton()->get_log_file();

	if (GDMonoLog::get_singleton()->get_log_level_id() >= log_level_get_id(log_level)) {
		String text(message);
		text += " (in domain ";
		text += log_domain;
		if (log_level) {
			text += ", ";
			text += log_level;
		}
		text += ")\n";

		f->seek_end();
		f->store_string(text);
	}

	if (fatal) {
		ERR_PRINTS("Mono: FATAL ERROR, ABORTING! Logfile: " + GDMonoLog::get_singleton()->get_log_file_path() + "\n");
		// Make sure to flush before aborting
		f->flush();
		f->close();
		memdelete(f);

		abort();
	}
}

GDMonoLog *GDMonoLog::singleton = NULL;

bool GDMonoLog::_try_create_logs_dir(const String &p_logs_dir) {

	if (!DirAccess::exists(p_logs_dir)) {
		DirAccessRef diraccess = DirAccess::create(DirAccess::ACCESS_FILESYSTEM);
		ERR_FAIL_COND_V(!diraccess, false);
		Error logs_mkdir_err = diraccess->make_dir_recursive(p_logs_dir);
		ERR_EXPLAIN("Failed to create mono logs directory");
		ERR_FAIL_COND_V(logs_mkdir_err != OK, false);
	}

	return true;
}

void GDMonoLog::_delete_old_log_files(const String &p_logs_dir) {

	static const uint64_t MAX_SECS = 5 * 86400; // 5 days

	DirAccessRef da = DirAccess::create(DirAccess::ACCESS_FILESYSTEM);
	ERR_FAIL_COND(!da);

	Error err = da->change_dir(p_logs_dir);
	ERR_FAIL_COND(err != OK);

	ERR_FAIL_COND(da->list_dir_begin() != OK);

	String current;
	while ((current = da->get_next()).length()) {
		if (da->current_is_dir())
			continue;
		if (!current.ends_with(".txt"))
			continue;

		uint64_t modified_time = FileAccess::get_modified_time(da->get_current_dir().plus_file(current));

		if (OS::get_singleton()->get_unix_time() - modified_time > MAX_SECS) {
			da->remove(current);
		}
	}

	da->list_dir_end();
}

static String format(const char *p_fmt, ...) {
	va_list args;

	va_start(args, p_fmt);
	int len = vsnprintf(NULL, 0, p_fmt, args);
	va_end(args);

	len += 1; // for the trailing '/0'

	char *buffer(memnew_arr(char, len));

	va_start(args, p_fmt);
	vsnprintf(buffer, len, p_fmt, args);
	va_end(args);

	String res(buffer);
	memdelete_arr(buffer);

	return res;
}

void GDMonoLog::initialize() {

	CharString log_level = OS::get_singleton()->get_environment("GODOT_MONO_LOG_LEVEL").utf8();

	if (log_level.length() != 0 && log_level_get_id(log_level.get_data()) == -1) {
		ERR_PRINTS(String() + "Mono: Ignoring invalid log level (GODOT_MONO_LOG_LEVEL): " + log_level.get_data());
		log_level = CharString();
	}

	if (log_level.length() == 0) {
#ifdef DEBUG_ENABLED
		log_level = String("info").utf8();
#else
		log_level = String("warning").utf8();
#endif
	}

	String logs_dir = GodotSharpDirs::get_mono_logs_dir();

	if (_try_create_logs_dir(logs_dir)) {
		_delete_old_log_files(logs_dir);

		OS::Date date_now = OS::get_singleton()->get_date();
		OS::Time time_now = OS::get_singleton()->get_time();
		int pid = OS::get_singleton()->get_process_id();

		String log_file_name = format("%d_%02d_%02d %02d.%02d.%02d (%d).txt",
				date_now.year, date_now.month, date_now.day,
				time_now.hour, time_now.min, time_now.sec, pid);

		log_file_path = logs_dir.plus_file(log_file_name);

		log_file = FileAccess::open(log_file_path, FileAccess::WRITE);
		if (!log_file) {
			ERR_PRINT("Mono: Cannot create log file");
		}
	}

	mono_trace_set_level_string(log_level.get_data());
	log_level_id = log_level_get_id(log_level.get_data());

	if (log_file) {
		OS::get_singleton()->print("Mono: Logfile is: %s\n", log_file_path.utf8().get_data());
		mono_trace_set_log_handler(mono_log_callback, this);
	} else {
		OS::get_singleton()->printerr("Mono: No log file, using default log handler\n");
	}
}

GDMonoLog::GDMonoLog() {

	singleton = this;

	log_level_id = -1;
}

GDMonoLog::~GDMonoLog() {

	singleton = NULL;

	if (log_file) {
		log_file->close();
		memdelete(log_file);
	}
}
