/*************************************************************************/
/*  os_iphone.cpp                                                        */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2020 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2020 Godot Engine contributors (cf. AUTHORS.md).   */
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

#ifdef IPHONE_ENABLED

#include "os_iphone.h"

#include "drivers/gles2/rasterizer_gles2.h"
#include "drivers/gles3/rasterizer_gles3.h"
#include "servers/visual/visual_server_raster.h"
#include "servers/visual/visual_server_wrap_mt.h"

#include "main/main.h"

#include "core/io/file_access_pack.h"
#include "core/os/dir_access.h"
#include "core/os/file_access.h"
#include "core/project_settings.h"
#include "drivers/unix/syslog_logger.h"

#include "semaphore_iphone.h"

#include <dlfcn.h>

int OSIPhone::get_video_driver_count() const {

	return 2;
};

const char *OSIPhone::get_video_driver_name(int p_driver) const {

	switch (p_driver) {
		case VIDEO_DRIVER_GLES3:
			return "GLES3";
		case VIDEO_DRIVER_GLES2:
			return "GLES2";
	}
	ERR_FAIL_V_MSG(NULL, "Invalid video driver index: " + itos(p_driver) + ".");
};

OSIPhone *OSIPhone::get_singleton() {

	return (OSIPhone *)OS::get_singleton();
};

extern int gl_view_base_fb; // from gl_view.mm

void OSIPhone::set_data_dir(String p_dir) {

	DirAccess *da = DirAccess::open(p_dir);

	data_dir = da->get_current_dir();
	printf("setting data dir to %ls from %ls\n", data_dir.c_str(), p_dir.c_str());
	memdelete(da);
};

void OSIPhone::set_unique_id(String p_id) {

	unique_id = p_id;
};

String OSIPhone::get_unique_id() const {

	return unique_id;
};

void OSIPhone::initialize_core() {

	OS_Unix::initialize_core();
	SemaphoreIphone::make_default();

	set_data_dir(data_dir);
};

int OSIPhone::get_current_video_driver() const {
	return video_driver_index;
}

extern bool gles3_available; // from gl_view.mm

Error OSIPhone::initialize(const VideoMode &p_desired, int p_video_driver, int p_audio_driver) {

	bool use_gl3 = GLOBAL_GET("rendering/quality/driver/driver_name") == "GLES3";
	bool gl_initialization_error = false;

	while (true) {
		if (use_gl3) {
			if (RasterizerGLES3::is_viable() == OK && gles3_available) {
				RasterizerGLES3::register_config();
				RasterizerGLES3::make_current();
				break;
			} else {
				if (GLOBAL_GET("rendering/quality/driver/fallback_to_gles2")) {
					p_video_driver = VIDEO_DRIVER_GLES2;
					use_gl3 = false;
					continue;
				} else {
					gl_initialization_error = true;
					break;
				}
			}
		} else {
			if (RasterizerGLES2::is_viable() == OK) {
				RasterizerGLES2::register_config();
				RasterizerGLES2::make_current();
				break;
			} else {
				gl_initialization_error = true;
				break;
			}
		}
	}

	if (gl_initialization_error) {
		OS::get_singleton()->alert("Your device does not support any of the supported OpenGL versions.",
				"Unable to initialize Video driver");
		return ERR_UNAVAILABLE;
	}

	video_driver_index = p_video_driver;
	visual_server = memnew(VisualServerRaster);
	// FIXME: Reimplement threaded rendering
	if (get_render_thread_mode() != RENDER_THREAD_UNSAFE) {
		visual_server = memnew(VisualServerWrapMT(visual_server, false));
	}

	visual_server->init();
	//visual_server->cursor_set_visible(false, 0);

	// reset this to what it should be, it will have been set to 0 after visual_server->init() is called
	if (use_gl3)
		RasterizerStorageGLES3::system_fbo = gl_view_base_fb;
	else
		RasterizerStorageGLES2::system_fbo = gl_view_base_fb;

	AudioDriverManager::initialize(p_audio_driver);

	input = memnew(InputDefault);

#ifdef GAME_CENTER_ENABLED
	game_center = memnew(GameCenter);
	Engine::get_singleton()->add_singleton(Engine::Singleton("GameCenter", game_center));
	game_center->connect();
#endif

#ifdef STOREKIT_ENABLED
	store_kit = memnew(InAppStore);
	Engine::get_singleton()->add_singleton(Engine::Singleton("InAppStore", store_kit));
#endif

#ifdef ICLOUD_ENABLED
	icloud = memnew(ICloud);
	Engine::get_singleton()->add_singleton(Engine::Singleton("ICloud", icloud));
	//icloud->connect();
#endif
	ios = memnew(iOS);
	Engine::get_singleton()->add_singleton(Engine::Singleton("iOS", ios));

	return OK;
};

MainLoop *OSIPhone::get_main_loop() const {

	return main_loop;
};

void OSIPhone::set_main_loop(MainLoop *p_main_loop) {

	main_loop = p_main_loop;

	if (main_loop) {
		input->set_main_loop(p_main_loop);
		main_loop->init();
	}
};

bool OSIPhone::iterate() {

	if (!main_loop)
		return true;

	if (main_loop) {
		for (int i = 0; i < event_count; i++) {

			input->parse_input_event(event_queue[i]);
		};
	};
	event_count = 0;

	return Main::iteration();
};

void OSIPhone::key(uint32_t p_key, bool p_pressed) {

	Ref<InputEventKey> ev;
	ev.instance();
	ev->set_echo(false);
	ev->set_pressed(p_pressed);
	ev->set_scancode(p_key);
	ev->set_unicode(p_key);
	queue_event(ev);
};

void OSIPhone::touch_press(int p_idx, int p_x, int p_y, bool p_pressed, bool p_doubleclick) {

	if (!GLOBAL_DEF("debug/disable_touch", false)) {
		Ref<InputEventScreenTouch> ev;
		ev.instance();

		ev->set_index(p_idx);
		ev->set_pressed(p_pressed);
		ev->set_position(Vector2(p_x, p_y));
		queue_event(ev);
	};

	touch_list.pressed[p_idx] = p_pressed;
};

void OSIPhone::touch_drag(int p_idx, int p_prev_x, int p_prev_y, int p_x, int p_y) {

	if (!GLOBAL_DEF("debug/disable_touch", false)) {

		Ref<InputEventScreenDrag> ev;
		ev.instance();
		ev->set_index(p_idx);
		ev->set_position(Vector2(p_x, p_y));
		ev->set_relative(Vector2(p_x - p_prev_x, p_y - p_prev_y));
		queue_event(ev);
	};
};

void OSIPhone::queue_event(const Ref<InputEvent> &p_event) {

	ERR_FAIL_INDEX(event_count, MAX_EVENTS);

	event_queue[event_count++] = p_event;
};

void OSIPhone::touches_cancelled() {

	for (int i = 0; i < MAX_MOUSE_COUNT; i++) {

		if (touch_list.pressed[i]) {

			// send a mouse_up outside the screen
			touch_press(i, -1, -1, false, false);
		};
	};
};

static const float ACCEL_RANGE = 1;

void OSIPhone::update_gravity(float p_x, float p_y, float p_z) {
	input->set_gravity(Vector3(p_x, p_y, p_z));
};

void OSIPhone::update_accelerometer(float p_x, float p_y, float p_z) {

	// Found out the Z should not be negated! Pass as is!
	input->set_accelerometer(Vector3(p_x / (float)ACCEL_RANGE, p_y / (float)ACCEL_RANGE, p_z / (float)ACCEL_RANGE));

	/*
	if (p_x != last_accel.x) {
		//printf("updating accel x %f\n", p_x);
		InputEvent ev;
		ev.type = InputEvent::JOYPAD_MOTION;
		ev.device = 0;
		ev.joy_motion.axis = JOY_ANALOG_0;
		ev.joy_motion.axis_value = (p_x / (float)ACCEL_RANGE);
		last_accel.x = p_x;
		queue_event(ev);
	};
	if (p_y != last_accel.y) {
		//printf("updating accel y %f\n", p_y);
		InputEvent ev;
		ev.type = InputEvent::JOYPAD_MOTION;
		ev.device = 0;
		ev.joy_motion.axis = JOY_ANALOG_1;
		ev.joy_motion.axis_value = (p_y / (float)ACCEL_RANGE);
		last_accel.y = p_y;
		queue_event(ev);
	};
	if (p_z != last_accel.z) {
		//printf("updating accel z %f\n", p_z);
		InputEvent ev;
		ev.type = InputEvent::JOYPAD_MOTION;
		ev.device = 0;
		ev.joy_motion.axis = JOY_ANALOG_2;
		ev.joy_motion.axis_value = ( (1.0 - p_z) / (float)ACCEL_RANGE);
		last_accel.z = p_z;
		queue_event(ev);
	};
	*/
};

void OSIPhone::update_magnetometer(float p_x, float p_y, float p_z) {
	input->set_magnetometer(Vector3(p_x, p_y, p_z));
};

void OSIPhone::update_gyroscope(float p_x, float p_y, float p_z) {
	input->set_gyroscope(Vector3(p_x, p_y, p_z));
};

int OSIPhone::get_unused_joy_id() {
	return input->get_unused_joy_id();
};

void OSIPhone::joy_connection_changed(int p_idx, bool p_connected, String p_name) {
	input->joy_connection_changed(p_idx, p_connected, p_name);
};

void OSIPhone::joy_button(int p_device, int p_button, bool p_pressed) {
	input->joy_button(p_device, p_button, p_pressed);
};

void OSIPhone::joy_axis(int p_device, int p_axis, const InputDefault::JoyAxis &p_value) {
	input->joy_axis(p_device, p_axis, p_value);
};

void OSIPhone::delete_main_loop() {

	if (main_loop) {
		main_loop->finish();
		memdelete(main_loop);
	};

	main_loop = NULL;
};

void OSIPhone::finalize() {

	delete_main_loop();

	memdelete(input);
	memdelete(ios);

#ifdef GAME_CENTER_ENABLED
	memdelete(game_center);
#endif

#ifdef STOREKIT_ENABLED
	memdelete(store_kit);
#endif

#ifdef ICLOUD_ENABLED
	memdelete(icloud);
#endif

	visual_server->finish();
	memdelete(visual_server);
	//	memdelete(rasterizer);

	// Free unhandled events before close
	for (int i = 0; i < MAX_EVENTS; i++) {
		event_queue[i].unref();
	};
	event_count = 0;
};

void OSIPhone::set_mouse_show(bool p_show){};
void OSIPhone::set_mouse_grab(bool p_grab){};

bool OSIPhone::is_mouse_grab_enabled() const {

	return true;
};

Point2 OSIPhone::get_mouse_position() const {

	return Point2();
};

int OSIPhone::get_mouse_button_state() const {

	return 0;
};

void OSIPhone::set_window_title(const String &p_title){};

void OSIPhone::alert(const String &p_alert, const String &p_title) {

	const CharString utf8_alert = p_alert.utf8();
	const CharString utf8_title = p_title.utf8();
	iOS::alert(utf8_alert.get_data(), utf8_title.get_data());
}

Error OSIPhone::open_dynamic_library(const String p_path, void *&p_library_handle, bool p_also_set_library_path) {
	if (p_path.length() == 0) {
		p_library_handle = RTLD_SELF;
		return OK;
	}
	return OS_Unix::open_dynamic_library(p_path, p_library_handle, p_also_set_library_path);
}

Error OSIPhone::close_dynamic_library(void *p_library_handle) {
	if (p_library_handle == RTLD_SELF) {
		return OK;
	}
	return OS_Unix::close_dynamic_library(p_library_handle);
}

HashMap<String, void *> OSIPhone::dynamic_symbol_lookup_table;
void register_dynamic_symbol(char *name, void *address) {
	OSIPhone::dynamic_symbol_lookup_table[String(name)] = address;
}

Error OSIPhone::get_dynamic_library_symbol_handle(void *p_library_handle, const String p_name, void *&p_symbol_handle, bool p_optional) {
	if (p_library_handle == RTLD_SELF) {
		void **ptr = OSIPhone::dynamic_symbol_lookup_table.getptr(p_name);
		if (ptr) {
			p_symbol_handle = *ptr;
			return OK;
		}
	}
	return OS_Unix::get_dynamic_library_symbol_handle(p_library_handle, p_name, p_symbol_handle, p_optional);
}

void OSIPhone::set_video_mode(const VideoMode &p_video_mode, int p_screen) {

	video_mode = p_video_mode;
};

OS::VideoMode OSIPhone::get_video_mode(int p_screen) const {

	return video_mode;
};

void OSIPhone::get_fullscreen_mode_list(List<VideoMode> *p_list, int p_screen) const {

	p_list->push_back(video_mode);
};

bool OSIPhone::can_draw() const {

	if (native_video_is_playing())
		return false;
	return true;
};

int OSIPhone::set_base_framebuffer(int p_fb) {

	// gl_view_base_fb has not been updated yet
	RasterizerStorageGLES3::system_fbo = p_fb;

	return 0;
};

bool OSIPhone::has_virtual_keyboard() const {
	return true;
};

extern void _show_keyboard(String p_existing);
extern void _hide_keyboard();
extern Error _shell_open(String p_uri);
extern void _set_keep_screen_on(bool p_enabled);
extern void _vibrate();

void OSIPhone::show_virtual_keyboard(const String &p_existing_text, const Rect2 &p_screen_rect, int p_max_input_length, int p_cursor_start, int p_cursor_end) {
	_show_keyboard(p_existing_text);
};

void OSIPhone::hide_virtual_keyboard() {
	_hide_keyboard();
};

void OSIPhone::set_virtual_keyboard_height(int p_height) {
	virtual_keyboard_height = p_height;
}

int OSIPhone::get_virtual_keyboard_height() const {
	return virtual_keyboard_height;
}

Error OSIPhone::shell_open(String p_uri) {
	return _shell_open(p_uri);
};

void OSIPhone::set_keep_screen_on(bool p_enabled) {
	OS::set_keep_screen_on(p_enabled);
	_set_keep_screen_on(p_enabled);
};

String OSIPhone::get_user_data_dir() const {

	return data_dir;
};

String OSIPhone::get_name() const {

	return "iOS";
};

String OSIPhone::get_model_name() const {

	String model = ios->get_model();
	if (model != "")
		return model;

	return OS_Unix::get_model_name();
}

Size2 OSIPhone::get_window_size() const {

	return Vector2(video_mode.width, video_mode.height);
}

extern Rect2 _get_ios_window_safe_area(float p_window_width, float p_window_height);

Rect2 OSIPhone::get_window_safe_area() const {
	return _get_ios_window_safe_area(video_mode.width, video_mode.height);
}

bool OSIPhone::has_touchscreen_ui_hint() const {

	return true;
}

void OSIPhone::set_locale(String p_locale) {
	locale_code = p_locale;
}

String OSIPhone::get_locale() const {
	return locale_code;
}

extern bool _play_video(String p_path, float p_volume, String p_audio_track, String p_subtitle_track);
extern bool _is_video_playing();
extern void _pause_video();
extern void _unpause_video();
extern void _stop_video();
extern void _focus_out_video();

Error OSIPhone::native_video_play(String p_path, float p_volume, String p_audio_track, String p_subtitle_track) {
	FileAccess *f = FileAccess::open(p_path, FileAccess::READ);
	bool exists = f && f->is_open();

	String tempFile = get_user_data_dir();
	if (!exists)
		return FAILED;

	if (p_path.begins_with("res://")) {
		if (PackedData::get_singleton()->has_path(p_path)) {
			print("Unable to play %S using the native player as it resides in a .pck file\n", p_path.c_str());
			return ERR_INVALID_PARAMETER;
		} else {
			p_path = p_path.replace("res:/", ProjectSettings::get_singleton()->get_resource_path());
		}
	} else if (p_path.begins_with("user://"))
		p_path = p_path.replace("user:/", get_user_data_dir());

	memdelete(f);

	print("Playing video: %S\n", p_path.c_str());
	if (_play_video(p_path, p_volume, p_audio_track, p_subtitle_track))
		return OK;
	return FAILED;
}

bool OSIPhone::native_video_is_playing() const {
	return _is_video_playing();
}

void OSIPhone::native_video_pause() {
	if (native_video_is_playing())
		_pause_video();
}

void OSIPhone::native_video_unpause() {
	_unpause_video();
};

void OSIPhone::native_video_focus_out() {
	_focus_out_video();
};

void OSIPhone::native_video_stop() {
	if (native_video_is_playing())
		_stop_video();
}

void OSIPhone::vibrate_handheld(int p_duration_ms) {
	// iOS does not support duration for vibration
	_vibrate();
}

bool OSIPhone::_check_internal_feature_support(const String &p_feature) {

	return p_feature == "mobile";
}

// Initialization order between compilation units is not guaranteed,
// so we use this as a hack to ensure certain code is called before
// everything else, but after all units are initialized.
typedef void (*init_callback)();
static init_callback *ios_init_callbacks = NULL;
static int ios_init_callbacks_count = 0;
static int ios_init_callbacks_capacity = 0;

void add_ios_init_callback(init_callback cb) {
	if (ios_init_callbacks_count == ios_init_callbacks_capacity) {
		void *new_ptr = realloc(ios_init_callbacks, sizeof(cb) * 32);
		if (new_ptr) {
			ios_init_callbacks = (init_callback *)(new_ptr);
			ios_init_callbacks_capacity += 32;
		}
	}
	if (ios_init_callbacks_capacity > ios_init_callbacks_count) {
		ios_init_callbacks[ios_init_callbacks_count] = cb;
		++ios_init_callbacks_count;
	}
}

OSIPhone::OSIPhone(int width, int height, String p_data_dir) {
	for (int i = 0; i < ios_init_callbacks_count; ++i) {
		ios_init_callbacks[i]();
	}
	free(ios_init_callbacks);
	ios_init_callbacks = NULL;
	ios_init_callbacks_count = 0;
	ios_init_callbacks_capacity = 0;

	main_loop = NULL;
	visual_server = NULL;

	VideoMode vm;
	vm.fullscreen = true;
	vm.width = width;
	vm.height = height;
	vm.resizable = false;
	set_video_mode(vm);
	event_count = 0;
	virtual_keyboard_height = 0;

	// can't call set_data_dir from here, since it requires DirAccess
	// which is initialized in initialize_core
	data_dir = p_data_dir;

	Vector<Logger *> loggers;
	loggers.push_back(memnew(SyslogLogger));
#ifdef DEBUG_ENABLED
	// it seems iOS app's stdout/stderr is only obtainable if you launch it from Xcode
	loggers.push_back(memnew(StdLogger));
#endif
	_set_logger(memnew(CompositeLogger(loggers)));

	AudioDriverManager::add_driver(&audio_driver);
};

OSIPhone::~OSIPhone() {
}

#endif
