/*************************************************************************/
/*  export.cpp                                                           */
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

#include "export.h"

#include "core/io/image_loader.h"
#include "core/io/marshalls.h"
#include "core/io/zip_io.h"
#include "core/os/dir_access.h"
#include "core/os/file_access.h"
#include "core/os/os.h"
#include "core/project_settings.h"
#include "core/version.h"
#include "drivers/png/png_driver_common.h"
#include "editor/editor_export.h"
#include "editor/editor_log.h"
#include "editor/editor_node.h"
#include "editor/editor_settings.h"
#include "platform/android/logo.gen.h"
#include "platform/android/plugin/godot_plugin_config.h"
#include "platform/android/run_icon.gen.h"

#include <string.h>

static const char *android_perms[] = {
	"ACCESS_CHECKIN_PROPERTIES",
	"ACCESS_COARSE_LOCATION",
	"ACCESS_FINE_LOCATION",
	"ACCESS_LOCATION_EXTRA_COMMANDS",
	"ACCESS_MOCK_LOCATION",
	"ACCESS_NETWORK_STATE",
	"ACCESS_SURFACE_FLINGER",
	"ACCESS_WIFI_STATE",
	"ACCOUNT_MANAGER",
	"ADD_VOICEMAIL",
	"AUTHENTICATE_ACCOUNTS",
	"BATTERY_STATS",
	"BIND_ACCESSIBILITY_SERVICE",
	"BIND_APPWIDGET",
	"BIND_DEVICE_ADMIN",
	"BIND_INPUT_METHOD",
	"BIND_NFC_SERVICE",
	"BIND_NOTIFICATION_LISTENER_SERVICE",
	"BIND_PRINT_SERVICE",
	"BIND_REMOTEVIEWS",
	"BIND_TEXT_SERVICE",
	"BIND_VPN_SERVICE",
	"BIND_WALLPAPER",
	"BLUETOOTH",
	"BLUETOOTH_ADMIN",
	"BLUETOOTH_PRIVILEGED",
	"BRICK",
	"BROADCAST_PACKAGE_REMOVED",
	"BROADCAST_SMS",
	"BROADCAST_STICKY",
	"BROADCAST_WAP_PUSH",
	"CALL_PHONE",
	"CALL_PRIVILEGED",
	"CAMERA",
	"CAPTURE_AUDIO_OUTPUT",
	"CAPTURE_SECURE_VIDEO_OUTPUT",
	"CAPTURE_VIDEO_OUTPUT",
	"CHANGE_COMPONENT_ENABLED_STATE",
	"CHANGE_CONFIGURATION",
	"CHANGE_NETWORK_STATE",
	"CHANGE_WIFI_MULTICAST_STATE",
	"CHANGE_WIFI_STATE",
	"CLEAR_APP_CACHE",
	"CLEAR_APP_USER_DATA",
	"CONTROL_LOCATION_UPDATES",
	"DELETE_CACHE_FILES",
	"DELETE_PACKAGES",
	"DEVICE_POWER",
	"DIAGNOSTIC",
	"DISABLE_KEYGUARD",
	"DUMP",
	"EXPAND_STATUS_BAR",
	"FACTORY_TEST",
	"FLASHLIGHT",
	"FORCE_BACK",
	"GET_ACCOUNTS",
	"GET_PACKAGE_SIZE",
	"GET_TASKS",
	"GET_TOP_ACTIVITY_INFO",
	"GLOBAL_SEARCH",
	"HARDWARE_TEST",
	"INJECT_EVENTS",
	"INSTALL_LOCATION_PROVIDER",
	"INSTALL_PACKAGES",
	"INSTALL_SHORTCUT",
	"INTERNAL_SYSTEM_WINDOW",
	"INTERNET",
	"KILL_BACKGROUND_PROCESSES",
	"LOCATION_HARDWARE",
	"MANAGE_ACCOUNTS",
	"MANAGE_APP_TOKENS",
	"MANAGE_DOCUMENTS",
	"MASTER_CLEAR",
	"MEDIA_CONTENT_CONTROL",
	"MODIFY_AUDIO_SETTINGS",
	"MODIFY_PHONE_STATE",
	"MOUNT_FORMAT_FILESYSTEMS",
	"MOUNT_UNMOUNT_FILESYSTEMS",
	"NFC",
	"PERSISTENT_ACTIVITY",
	"PROCESS_OUTGOING_CALLS",
	"READ_CALENDAR",
	"READ_CALL_LOG",
	"READ_CONTACTS",
	"READ_EXTERNAL_STORAGE",
	"READ_FRAME_BUFFER",
	"READ_HISTORY_BOOKMARKS",
	"READ_INPUT_STATE",
	"READ_LOGS",
	"READ_PHONE_STATE",
	"READ_PROFILE",
	"READ_SMS",
	"READ_SOCIAL_STREAM",
	"READ_SYNC_SETTINGS",
	"READ_SYNC_STATS",
	"READ_USER_DICTIONARY",
	"REBOOT",
	"RECEIVE_BOOT_COMPLETED",
	"RECEIVE_MMS",
	"RECEIVE_SMS",
	"RECEIVE_WAP_PUSH",
	"RECORD_AUDIO",
	"REORDER_TASKS",
	"RESTART_PACKAGES",
	"SEND_RESPOND_VIA_MESSAGE",
	"SEND_SMS",
	"SET_ACTIVITY_WATCHER",
	"SET_ALARM",
	"SET_ALWAYS_FINISH",
	"SET_ANIMATION_SCALE",
	"SET_DEBUG_APP",
	"SET_ORIENTATION",
	"SET_POINTER_SPEED",
	"SET_PREFERRED_APPLICATIONS",
	"SET_PROCESS_LIMIT",
	"SET_TIME",
	"SET_TIME_ZONE",
	"SET_WALLPAPER",
	"SET_WALLPAPER_HINTS",
	"SIGNAL_PERSISTENT_PROCESSES",
	"STATUS_BAR",
	"SUBSCRIBED_FEEDS_READ",
	"SUBSCRIBED_FEEDS_WRITE",
	"SYSTEM_ALERT_WINDOW",
	"TRANSMIT_IR",
	"UNINSTALL_SHORTCUT",
	"UPDATE_DEVICE_STATS",
	"USE_CREDENTIALS",
	"USE_SIP",
	"VIBRATE",
	"WAKE_LOCK",
	"WRITE_APN_SETTINGS",
	"WRITE_CALENDAR",
	"WRITE_CALL_LOG",
	"WRITE_CONTACTS",
	"WRITE_EXTERNAL_STORAGE",
	"WRITE_GSERVICES",
	"WRITE_HISTORY_BOOKMARKS",
	"WRITE_PROFILE",
	"WRITE_SECURE_SETTINGS",
	"WRITE_SETTINGS",
	"WRITE_SMS",
	"WRITE_SOCIAL_STREAM",
	"WRITE_SYNC_SETTINGS",
	"WRITE_USER_DICTIONARY",
	NULL
};

struct LauncherIcon {
	const char *export_path;
	int dimensions;
};

static const int icon_densities_count = 6;
static const char *launcher_icon_option = "launcher_icons/main_192x192";
static const char *launcher_adaptive_icon_foreground_option = "launcher_icons/adaptive_foreground_432x432";
static const char *launcher_adaptive_icon_background_option = "launcher_icons/adaptive_background_432x432";

static const LauncherIcon launcher_icons[icon_densities_count] = {
	{ "res/mipmap-xxxhdpi-v4/icon.png", 192 },
	{ "res/mipmap-xxhdpi-v4/icon.png", 144 },
	{ "res/mipmap-xhdpi-v4/icon.png", 96 },
	{ "res/mipmap-hdpi-v4/icon.png", 72 },
	{ "res/mipmap-mdpi-v4/icon.png", 48 },
	{ "res/mipmap/icon.png", 192 }
};

static const LauncherIcon launcher_adaptive_icon_foregrounds[icon_densities_count] = {
	{ "res/mipmap-xxxhdpi-v4/icon_foreground.png", 432 },
	{ "res/mipmap-xxhdpi-v4/icon_foreground.png", 324 },
	{ "res/mipmap-xhdpi-v4/icon_foreground.png", 216 },
	{ "res/mipmap-hdpi-v4/icon_foreground.png", 162 },
	{ "res/mipmap-mdpi-v4/icon_foreground.png", 108 },
	{ "res/mipmap/icon_foreground.png", 432 }
};

static const LauncherIcon launcher_adaptive_icon_backgrounds[icon_densities_count] = {
	{ "res/mipmap-xxxhdpi-v4/icon_background.png", 432 },
	{ "res/mipmap-xxhdpi-v4/icon_background.png", 324 },
	{ "res/mipmap-xhdpi-v4/icon_background.png", 216 },
	{ "res/mipmap-hdpi-v4/icon_background.png", 162 },
	{ "res/mipmap-mdpi-v4/icon_background.png", 108 },
	{ "res/mipmap/icon_background.png", 432 }
};

class EditorExportPlatformAndroid : public EditorExportPlatform {

	GDCLASS(EditorExportPlatformAndroid, EditorExportPlatform);

	Ref<ImageTexture> logo;
	Ref<ImageTexture> run_icon;

	struct Device {

		String id;
		String name;
		String description;
		int api_level;
	};

	struct APKExportData {

		zipFile apk;
		EditorProgress *ep;
	};

	Vector<PluginConfig> plugins;
	String last_plugin_names;
	uint64_t last_custom_build_time = 0;
	volatile bool plugins_changed;
	Mutex *plugins_lock;
	Vector<Device> devices;
	volatile bool devices_changed;
	Mutex *device_lock;
	Thread *check_for_changes_thread;
	volatile bool quit_request;

	static void _check_for_changes_poll_thread(void *ud) {
		EditorExportPlatformAndroid *ea = (EditorExportPlatformAndroid *)ud;

		while (!ea->quit_request) {
			// Check for plugins updates
			{
				// Nothing to do if we already know the plugins have changed.
				if (!ea->plugins_changed) {
					Vector<PluginConfig> loaded_plugins = get_plugins();

					ea->plugins_lock->lock();

					if (ea->plugins.size() != loaded_plugins.size()) {
						ea->plugins_changed = true;
					} else {
						for (int i = 0; i < ea->plugins.size(); i++) {
							if (ea->plugins[i].name != loaded_plugins[i].name) {
								ea->plugins_changed = true;
								break;
							}
						}
					}

					if (ea->plugins_changed) {
						ea->plugins = loaded_plugins;
					}

					ea->plugins_lock->unlock();
				}
			}

			// Check for devices updates
			String adb = EditorSettings::get_singleton()->get("export/android/adb");
			if (FileAccess::exists(adb)) {
				String devices;
				List<String> args;
				args.push_back("devices");
				int ec;
				OS::get_singleton()->execute(adb, args, true, NULL, &devices, &ec);

				Vector<String> ds = devices.split("\n");
				Vector<String> ldevices;
				for (int i = 1; i < ds.size(); i++) {

					String d = ds[i];
					int dpos = d.find("device");
					if (dpos == -1) {
						continue;
					}
					d = d.substr(0, dpos).strip_edges();
					ldevices.push_back(d);
				}

				ea->device_lock->lock();

				bool different = false;

				if (ea->devices.size() != ldevices.size()) {
					different = true;
				} else {

					for (int i = 0; i < ea->devices.size(); i++) {
						if (ea->devices[i].id != ldevices[i]) {
							different = true;
							break;
						}
					}
				}

				if (different) {
					Vector<Device> ndevices;

					for (int i = 0; i < ldevices.size(); i++) {
						Device d;
						d.id = ldevices[i];
						for (int j = 0; j < ea->devices.size(); j++) {
							if (ea->devices[j].id == ldevices[i]) {
								d.description = ea->devices[j].description;
								d.name = ea->devices[j].name;
								d.api_level = ea->devices[j].api_level;
							}
						}

						if (d.description == "") {
							//in the oven, request!
							args.clear();
							args.push_back("-s");
							args.push_back(d.id);
							args.push_back("shell");
							args.push_back("getprop");
							int ec2;
							String dp;

							OS::get_singleton()->execute(adb, args, true, NULL, &dp, &ec2);

							Vector<String> props = dp.split("\n");
							String vendor;
							String device;
							d.description = "Device ID: " + d.id + "\n";
							d.api_level = 0;
							for (int j = 0; j < props.size(); j++) {

								// got information by `shell cat /system/build.prop` before and its format is "property=value"
								// it's now changed to use `shell getporp` because of permission issue with Android 8.0 and above
								// its format is "[property]: [value]" so changed it as like build.prop
								String p = props[j];
								p = p.replace("]: ", "=");
								p = p.replace("[", "");
								p = p.replace("]", "");

								if (p.begins_with("ro.product.model=")) {
									device = p.get_slice("=", 1).strip_edges();
								} else if (p.begins_with("ro.product.brand=")) {
									vendor = p.get_slice("=", 1).strip_edges().capitalize();
								} else if (p.begins_with("ro.build.display.id=")) {
									d.description += "Build: " + p.get_slice("=", 1).strip_edges() + "\n";
								} else if (p.begins_with("ro.build.version.release=")) {
									d.description += "Release: " + p.get_slice("=", 1).strip_edges() + "\n";
								} else if (p.begins_with("ro.build.version.sdk=")) {
									d.api_level = p.get_slice("=", 1).to_int();
								} else if (p.begins_with("ro.product.cpu.abi=")) {
									d.description += "CPU: " + p.get_slice("=", 1).strip_edges() + "\n";
								} else if (p.begins_with("ro.product.manufacturer=")) {
									d.description += "Manufacturer: " + p.get_slice("=", 1).strip_edges() + "\n";
								} else if (p.begins_with("ro.board.platform=")) {
									d.description += "Chipset: " + p.get_slice("=", 1).strip_edges() + "\n";
								} else if (p.begins_with("ro.opengles.version=")) {
									uint32_t opengl = p.get_slice("=", 1).to_int();
									d.description += "OpenGL: " + itos(opengl >> 16) + "." + itos((opengl >> 8) & 0xFF) + "." + itos((opengl)&0xFF) + "\n";
								}
							}

							d.name = vendor + " " + device;
							if (device == String()) continue;
						}

						ndevices.push_back(d);
					}

					ea->devices = ndevices;
					ea->devices_changed = true;
				}

				ea->device_lock->unlock();
			}

			uint64_t sleep = OS::get_singleton()->get_power_state() == OS::POWERSTATE_ON_BATTERY ? 1000 : 100;
			uint64_t wait = 3000000;
			uint64_t time = OS::get_singleton()->get_ticks_usec();
			while (OS::get_singleton()->get_ticks_usec() - time < wait) {
				OS::get_singleton()->delay_usec(1000 * sleep);
				if (ea->quit_request)
					break;
			}
		}

		if (EditorSettings::get_singleton()->get("export/android/shutdown_adb_on_exit")) {
			String adb = EditorSettings::get_singleton()->get("export/android/adb");
			if (!FileAccess::exists(adb)) {
				return; //adb not configured
			}

			List<String> args;
			args.push_back("kill-server");
			OS::get_singleton()->execute(adb, args, true);
		};
	}

	String get_project_name(const String &p_name) const {

		String aname;
		if (p_name != "") {
			aname = p_name;
		} else {
			aname = ProjectSettings::get_singleton()->get("application/config/name");
		}

		if (aname == "") {
			aname = VERSION_NAME;
		}

		return aname;
	}

	String get_package_name(const String &p_package) const {

		String pname = p_package;
		String basename = ProjectSettings::get_singleton()->get("application/config/name");
		basename = basename.to_lower();

		String name;
		bool first = true;
		for (int i = 0; i < basename.length(); i++) {
			CharType c = basename[i];
			if (c >= '0' && c <= '9' && first) {
				continue;
			}
			if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9')) {
				name += String::chr(c);
				first = false;
			}
		}
		if (name == "")
			name = "noname";

		pname = pname.replace("$genname", name);

		return pname;
	}

	bool is_package_name_valid(const String &p_package, String *r_error = NULL) const {

		String pname = p_package;

		if (pname.length() == 0) {
			if (r_error) {
				*r_error = TTR("Package name is missing.");
			}
			return false;
		}

		int segments = 0;
		bool first = true;
		for (int i = 0; i < pname.length(); i++) {
			CharType c = pname[i];
			if (first && c == '.') {
				if (r_error) {
					*r_error = TTR("Package segments must be of non-zero length.");
				}
				return false;
			}
			if (c == '.') {
				segments++;
				first = true;
				continue;
			}
			if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '_')) {
				if (r_error) {
					*r_error = vformat(TTR("The character '%s' is not allowed in Android application package names."), String::chr(c));
				}
				return false;
			}
			if (first && (c >= '0' && c <= '9')) {
				if (r_error) {
					*r_error = TTR("A digit cannot be the first character in a package segment.");
				}
				return false;
			}
			if (first && c == '_') {
				if (r_error) {
					*r_error = vformat(TTR("The character '%s' cannot be the first character in a package segment."), String::chr(c));
				}
				return false;
			}
			first = false;
		}

		if (segments == 0) {
			if (r_error) {
				*r_error = TTR("The package must have at least one '.' separator.");
			}
			return false;
		}

		if (first) {
			if (r_error) {
				*r_error = TTR("Package segments must be of non-zero length.");
			}
			return false;
		}

		return true;
	}

	static bool _should_compress_asset(const String &p_path, const Vector<uint8_t> &p_data) {

		/*
		 *  By not compressing files with little or not benefit in doing so,
		 *  a performance gain is expected attime. Moreover, if the APK is
		 *  zip-aligned, assets stored as they are can be efficiently read by
		 *  Android by memory-mapping them.
		 */

		// -- Unconditional uncompress to mimic AAPT plus some other

		static const char *unconditional_compress_ext[] = {
			// From https://github.com/android/platform_frameworks_base/blob/master/tools/aapt/Package.cpp
			// These formats are already compressed, or don't compress well:
			".jpg", ".jpeg", ".png", ".gif",
			".wav", ".mp2", ".mp3", ".ogg", ".aac",
			".mpg", ".mpeg", ".mid", ".midi", ".smf", ".jet",
			".rtttl", ".imy", ".xmf", ".mp4", ".m4a",
			".m4v", ".3gp", ".3gpp", ".3g2", ".3gpp2",
			".amr", ".awb", ".wma", ".wmv",
			// Godot-specific:
			".webp", // Same reasoning as .png
			".cfb", // Don't let small config files slow-down startup
			".scn", // Binary scenes are usually already compressed
			".stex", // Streamable textures are usually already compressed
			// Trailer for easier processing
			NULL
		};

		for (const char **ext = unconditional_compress_ext; *ext; ++ext) {
			if (p_path.to_lower().ends_with(String(*ext))) {
				return false;
			}
		}

		// -- Compressed resource?

		if (p_data.size() >= 4 && p_data[0] == 'R' && p_data[1] == 'S' && p_data[2] == 'C' && p_data[3] == 'C') {
			// Already compressed
			return false;
		}

		// --- TODO: Decide on texture resources according to their image compression setting

		return true;
	}

	static zip_fileinfo get_zip_fileinfo() {

		OS::Time time = OS::get_singleton()->get_time();
		OS::Date date = OS::get_singleton()->get_date();

		zip_fileinfo zipfi;
		zipfi.tmz_date.tm_hour = time.hour;
		zipfi.tmz_date.tm_mday = date.day;
		zipfi.tmz_date.tm_min = time.min;
		zipfi.tmz_date.tm_mon = date.month;
		zipfi.tmz_date.tm_sec = time.sec;
		zipfi.tmz_date.tm_year = date.year;
		zipfi.dosDate = 0;
		zipfi.external_fa = 0;
		zipfi.internal_fa = 0;

		return zipfi;
	}

	static Vector<String> get_abis() {
		Vector<String> abis;
		abis.push_back("armeabi-v7a");
		abis.push_back("arm64-v8a");
		abis.push_back("x86");
		abis.push_back("x86_64");
		return abis;
	}

	/// List the gdap files in the directory specified by the p_path parameter.
	static Vector<String> list_gdap_files(const String &p_path) {
		Vector<String> dir_files;
		DirAccessRef da = DirAccess::open(p_path);
		if (da) {
			da->list_dir_begin();
			while (true) {
				String file = da->get_next();
				if (file == "") {
					break;
				}

				if (da->current_is_dir() || da->current_is_hidden()) {
					continue;
				}

				if (file.ends_with(PLUGIN_CONFIG_EXT)) {
					dir_files.push_back(file);
				}
			}
			da->list_dir_end();
		}

		return dir_files;
	}

	static Vector<PluginConfig> get_plugins() {
		Vector<PluginConfig> loaded_plugins;

		String plugins_dir = ProjectSettings::get_singleton()->get_resource_path().plus_file("android/plugins");

		// Add the prebuilt plugins
		loaded_plugins.append_array(get_prebuilt_plugins(plugins_dir));

		if (DirAccess::exists(plugins_dir)) {
			Vector<String> plugins_filenames = list_gdap_files(plugins_dir);

			if (!plugins_filenames.empty()) {
				Ref<ConfigFile> config_file = memnew(ConfigFile);
				for (int i = 0; i < plugins_filenames.size(); i++) {
					PluginConfig config = load_plugin_config(config_file, plugins_dir.plus_file(plugins_filenames[i]));
					if (config.valid_config) {
						loaded_plugins.push_back(config);
					} else {
						print_error("Invalid plugin config file " + plugins_filenames[i]);
					}
				}
			}
		}

		return loaded_plugins;
	}

	static Vector<PluginConfig> get_enabled_plugins(const Ref<EditorExportPreset> &p_presets) {
		Vector<PluginConfig> enabled_plugins;
		Vector<PluginConfig> all_plugins = get_plugins();
		for (int i = 0; i < all_plugins.size(); i++) {
			PluginConfig plugin = all_plugins[i];
			bool enabled = p_presets->get("plugins/" + plugin.name);
			if (enabled) {
				enabled_plugins.push_back(plugin);
			}
		}

		return enabled_plugins;
	}

	static Error store_in_apk(APKExportData *ed, const String &p_path, const Vector<uint8_t> &p_data, int compression_method = Z_DEFLATED) {
		zip_fileinfo zipfi = get_zip_fileinfo();
		zipOpenNewFileInZip(ed->apk,
				p_path.utf8().get_data(),
				&zipfi,
				NULL,
				0,
				NULL,
				0,
				NULL,
				compression_method,
				Z_DEFAULT_COMPRESSION);

		zipWriteInFileInZip(ed->apk, p_data.ptr(), p_data.size());
		zipCloseFileInZip(ed->apk);

		return OK;
	}

	static Error save_apk_so(void *p_userdata, const SharedObject &p_so) {
		if (!p_so.path.get_file().begins_with("lib")) {
			String err = "Android .so file names must start with \"lib\", but got: " + p_so.path;
			ERR_PRINTS(err);
			return FAILED;
		}
		APKExportData *ed = (APKExportData *)p_userdata;
		Vector<String> abis = get_abis();
		bool exported = false;
		for (int i = 0; i < p_so.tags.size(); ++i) {
			// shared objects can be fat (compatible with multiple ABIs)
			int abi_index = abis.find(p_so.tags[i]);
			if (abi_index != -1) {
				exported = true;
				String abi = abis[abi_index];
				String dst_path = String("lib").plus_file(abi).plus_file(p_so.path.get_file());
				Vector<uint8_t> array = FileAccess::get_file_as_array(p_so.path);
				Error store_err = store_in_apk(ed, dst_path, array);
				ERR_FAIL_COND_V_MSG(store_err, store_err, "Cannot store in apk file '" + dst_path + "'.");
			}
		}
		if (!exported) {
			String abis_string = String(" ").join(abis);
			String err = "Cannot determine ABI for library \"" + p_so.path + "\". One of the supported ABIs must be used as a tag: " + abis_string;
			ERR_PRINTS(err);
			return FAILED;
		}
		return OK;
	}

	static Error save_apk_file(void *p_userdata, const String &p_path, const Vector<uint8_t> &p_data, int p_file, int p_total) {
		APKExportData *ed = (APKExportData *)p_userdata;
		String dst_path = p_path.replace_first("res://", "assets/");

		store_in_apk(ed, dst_path, p_data, _should_compress_asset(p_path, p_data) ? Z_DEFLATED : 0);
		return OK;
	}

	static Error ignore_apk_file(void *p_userdata, const String &p_path, const Vector<uint8_t> &p_data, int p_file, int p_total) {
		return OK;
	}

	void _fix_manifest(const Ref<EditorExportPreset> &p_preset, Vector<uint8_t> &p_manifest, bool p_give_internet) {

		// Leaving the unused types commented because looking these constants up
		// again later would be annoying
		// const int CHUNK_AXML_FILE = 0x00080003;
		// const int CHUNK_RESOURCEIDS = 0x00080180;
		const int CHUNK_STRINGS = 0x001C0001;
		// const int CHUNK_XML_END_NAMESPACE = 0x00100101;
		const int CHUNK_XML_END_TAG = 0x00100103;
		// const int CHUNK_XML_START_NAMESPACE = 0x00100100;
		const int CHUNK_XML_START_TAG = 0x00100102;
		// const int CHUNK_XML_TEXT = 0x00100104;
		const int UTF8_FLAG = 0x00000100;

		Vector<String> string_table;

		uint32_t ofs = 8;

		uint32_t string_count = 0;
		//uint32_t styles_count = 0;
		uint32_t string_flags = 0;
		uint32_t string_data_offset = 0;

		//uint32_t styles_offset = 0;
		uint32_t string_table_begins = 0;
		uint32_t string_table_ends = 0;
		Vector<uint8_t> stable_extra;

		String version_name = p_preset->get("version/name");
		int version_code = p_preset->get("version/code");
		String package_name = p_preset->get("package/unique_name");

		int orientation = p_preset->get("screen/orientation");

		bool min_gles3 = ProjectSettings::get_singleton()->get("rendering/quality/driver/driver_name") == "GLES3" &&
						 !ProjectSettings::get_singleton()->get("rendering/quality/driver/fallback_to_gles2");
		bool screen_support_small = p_preset->get("screen/support_small");
		bool screen_support_normal = p_preset->get("screen/support_normal");
		bool screen_support_large = p_preset->get("screen/support_large");
		bool screen_support_xlarge = p_preset->get("screen/support_xlarge");

		int xr_mode_index = p_preset->get("xr_features/xr_mode");
		bool focus_awareness = p_preset->get("xr_features/focus_awareness");

		String plugins_names = get_plugins_names(get_enabled_plugins(p_preset));

		Vector<String> perms;

		const char **aperms = android_perms;
		while (*aperms) {

			bool enabled = p_preset->get("permissions/" + String(*aperms).to_lower());
			if (enabled)
				perms.push_back("android.permission." + String(*aperms));
			aperms++;
		}

		PoolStringArray user_perms = p_preset->get("permissions/custom_permissions");

		for (int i = 0; i < user_perms.size(); i++) {
			String user_perm = user_perms[i].strip_edges();
			if (!user_perm.empty()) {
				perms.push_back(user_perm);
			}
		}

		if (p_give_internet) {
			if (perms.find("android.permission.INTERNET") == -1)
				perms.push_back("android.permission.INTERNET");
		}

		while (ofs < (uint32_t)p_manifest.size()) {

			uint32_t chunk = decode_uint32(&p_manifest[ofs]);
			uint32_t size = decode_uint32(&p_manifest[ofs + 4]);

			switch (chunk) {

				case CHUNK_STRINGS: {

					int iofs = ofs + 8;

					string_count = decode_uint32(&p_manifest[iofs]);
					//styles_count = decode_uint32(&p_manifest[iofs + 4]);
					string_flags = decode_uint32(&p_manifest[iofs + 8]);
					string_data_offset = decode_uint32(&p_manifest[iofs + 12]);
					//styles_offset = decode_uint32(&p_manifest[iofs + 16]);
					/*
					printf("string count: %i\n",string_count);
					printf("flags: %i\n",string_flags);
					printf("sdata ofs: %i\n",string_data_offset);
					printf("styles ofs: %i\n",styles_offset);
					*/
					uint32_t st_offset = iofs + 20;
					string_table.resize(string_count);
					uint32_t string_end = 0;

					string_table_begins = st_offset;

					for (uint32_t i = 0; i < string_count; i++) {

						uint32_t string_at = decode_uint32(&p_manifest[st_offset + i * 4]);
						string_at += st_offset + string_count * 4;

						ERR_FAIL_COND_MSG(string_flags & UTF8_FLAG, "Unimplemented, can't read UTF-8 string table.");

						if (string_flags & UTF8_FLAG) {

						} else {
							uint32_t len = decode_uint16(&p_manifest[string_at]);
							Vector<CharType> ucstring;
							ucstring.resize(len + 1);
							for (uint32_t j = 0; j < len; j++) {
								uint16_t c = decode_uint16(&p_manifest[string_at + 2 + 2 * j]);
								ucstring.write[j] = c;
							}
							string_end = MAX(string_at + 2 + 2 * len, string_end);
							ucstring.write[len] = 0;
							string_table.write[i] = ucstring.ptr();
						}
					}

					for (uint32_t i = string_end; i < (ofs + size); i++) {
						stable_extra.push_back(p_manifest[i]);
					}

					string_table_ends = ofs + size;

				} break;
				case CHUNK_XML_START_TAG: {

					int iofs = ofs + 8;
					uint32_t name = decode_uint32(&p_manifest[iofs + 12]);

					String tname = string_table[name];
					uint32_t attrcount = decode_uint32(&p_manifest[iofs + 20]);
					iofs += 28;

					for (uint32_t i = 0; i < attrcount; i++) {
						uint32_t attr_nspace = decode_uint32(&p_manifest[iofs]);
						uint32_t attr_name = decode_uint32(&p_manifest[iofs + 4]);
						uint32_t attr_value = decode_uint32(&p_manifest[iofs + 8]);
						uint32_t attr_resid = decode_uint32(&p_manifest[iofs + 16]);

						const String value = (attr_value != 0xFFFFFFFF) ? string_table[attr_value] : "Res #" + itos(attr_resid);
						String attrname = string_table[attr_name];
						const String nspace = (attr_nspace != 0xFFFFFFFF) ? string_table[attr_nspace] : "";

						//replace project information
						if (tname == "manifest" && attrname == "package") {
							string_table.write[attr_value] = get_package_name(package_name);
						}

						if (tname == "manifest" && attrname == "versionCode") {
							encode_uint32(version_code, &p_manifest.write[iofs + 16]);
						}

						if (tname == "manifest" && attrname == "versionName") {
							if (attr_value == 0xFFFFFFFF) {
								WARN_PRINT("Version name in a resource, should be plain text");
							} else
								string_table.write[attr_value] = version_name;
						}

						if (tname == "instrumentation" && attrname == "targetPackage") {
							string_table.write[attr_value] = get_package_name(package_name);
						}

						if (tname == "activity" && attrname == "screenOrientation") {

							encode_uint32(orientation == 0 ? 0 : 1, &p_manifest.write[iofs + 16]);
						}

						if (tname == "supports-screens") {

							if (attrname == "smallScreens") {

								encode_uint32(screen_support_small ? 0xFFFFFFFF : 0, &p_manifest.write[iofs + 16]);

							} else if (attrname == "normalScreens") {

								encode_uint32(screen_support_normal ? 0xFFFFFFFF : 0, &p_manifest.write[iofs + 16]);

							} else if (attrname == "largeScreens") {

								encode_uint32(screen_support_large ? 0xFFFFFFFF : 0, &p_manifest.write[iofs + 16]);

							} else if (attrname == "xlargeScreens") {

								encode_uint32(screen_support_xlarge ? 0xFFFFFFFF : 0, &p_manifest.write[iofs + 16]);
							}
						}

						if (tname == "uses-feature" && attrname == "glEsVersion") {

							encode_uint32(min_gles3 ? 0x00030000 : 0x00020000, &p_manifest.write[iofs + 16]);
						}

						// FIXME: `attr_value != 0xFFFFFFFF` below added as a stopgap measure for GH-32553,
						// but the issue should be debugged further and properly addressed.
						if (tname == "meta-data" && attrname == "name" && value == "xr_mode_metadata_name") {
							// Update the meta-data 'android:name' attribute based on the selected XR mode.
							if (xr_mode_index == 1 /* XRMode.OVR */) {
								string_table.write[attr_value] = "com.samsung.android.vr.application.mode";
							}
						}

						if (tname == "meta-data" && attrname == "value" && value == "xr_mode_metadata_value") {
							// Update the meta-data 'android:value' attribute based on the selected XR mode.
							if (xr_mode_index == 1 /* XRMode.OVR */) {
								string_table.write[attr_value] = "vr_only";
							}
						}

						if (tname == "meta-data" && attrname == "value" && value == "oculus_focus_aware_value") {
							// Update the focus awareness meta-data value
							string_table.write[attr_value] = xr_mode_index == /* XRMode.OVR */ 1 && focus_awareness ? "true" : "false";
						}

						if (tname == "meta-data" && attrname == "value" && value == "plugins_value" && !plugins_names.empty()) {
							// Update the meta-data 'android:value' attribute with the list of enabled plugins.
							string_table.write[attr_value] = plugins_names;
						}

						iofs += 20;
					}

				} break;
				case CHUNK_XML_END_TAG: {
					int iofs = ofs + 8;
					uint32_t name = decode_uint32(&p_manifest[iofs + 12]);
					String tname = string_table[name];

					if (tname == "uses-feature") {
						Vector<String> feature_names;
						Vector<bool> feature_required_list;
						Vector<int> feature_versions;

						if (xr_mode_index == 1 /* XRMode.OVR */) {
							// Check for degrees of freedom
							int dof_index = p_preset->get("xr_features/degrees_of_freedom"); // 0: none, 1: 3dof and 6dof, 2: 6dof

							if (dof_index > 0) {
								feature_names.push_back("android.hardware.vr.headtracking");
								feature_required_list.push_back(dof_index == 2);
								feature_versions.push_back(1);
							}

							// Check for hand tracking
							int hand_tracking_index = p_preset->get("xr_features/hand_tracking"); // 0: none, 1: optional, 2: required
							if (hand_tracking_index > 0) {
								feature_names.push_back("oculus.software.handtracking");
								feature_required_list.push_back(hand_tracking_index == 2);
								feature_versions.push_back(-1); // no version attribute should be added.

								if (perms.find("com.oculus.permission.HAND_TRACKING") == -1) {
									perms.push_back("com.oculus.permission.HAND_TRACKING");
								}
							}
						}

						if (feature_names.size() > 0) {
							ofs += 24; // skip over end tag

							// save manifest ending so we can restore it
							Vector<uint8_t> manifest_end;
							uint32_t manifest_cur_size = p_manifest.size();

							manifest_end.resize(p_manifest.size() - ofs);
							memcpy(manifest_end.ptrw(), &p_manifest[ofs], manifest_end.size());

							int32_t attr_name_string = string_table.find("name");
							ERR_FAIL_COND_MSG(attr_name_string == -1, "Template does not have 'name' attribute.");

							int32_t ns_android_string = string_table.find("http://schemas.android.com/apk/res/android");
							if (ns_android_string == -1) {
								string_table.push_back("http://schemas.android.com/apk/res/android");
								ns_android_string = string_table.size() - 1;
							}

							int32_t attr_uses_feature_string = string_table.find("uses-feature");
							if (attr_uses_feature_string == -1) {
								string_table.push_back("uses-feature");
								attr_uses_feature_string = string_table.size() - 1;
							}

							int32_t attr_required_string = string_table.find("required");
							if (attr_required_string == -1) {
								string_table.push_back("required");
								attr_required_string = string_table.size() - 1;
							}

							for (int i = 0; i < feature_names.size(); i++) {
								String feature_name = feature_names[i];
								bool feature_required = feature_required_list[i];
								int feature_version = feature_versions[i];
								bool has_version_attribute = feature_version != -1;

								print_line("Adding feature " + feature_name);

								int32_t feature_string = string_table.find(feature_name);
								if (feature_string == -1) {
									string_table.push_back(feature_name);
									feature_string = string_table.size() - 1;
								}

								String required_value_string = feature_required ? "true" : "false";
								int32_t required_value = string_table.find(required_value_string);
								if (required_value == -1) {
									string_table.push_back(required_value_string);
									required_value = string_table.size() - 1;
								}

								int32_t attr_version_string = -1;
								int32_t version_value = -1;
								int tag_size;
								int attr_count;
								if (has_version_attribute) {
									attr_version_string = string_table.find("version");
									if (attr_version_string == -1) {
										string_table.push_back("version");
										attr_version_string = string_table.size() - 1;
									}

									version_value = string_table.find(itos(feature_version));
									if (version_value == -1) {
										string_table.push_back(itos(feature_version));
										version_value = string_table.size() - 1;
									}

									tag_size = 96; // node and three attrs + end node
									attr_count = 3;
								} else {
									tag_size = 76; // node and two attrs + end node
									attr_count = 2;
								}
								manifest_cur_size += tag_size + 24;
								p_manifest.resize(manifest_cur_size);

								// start tag
								encode_uint16(0x102, &p_manifest.write[ofs]); // type
								encode_uint16(16, &p_manifest.write[ofs + 2]); // headersize
								encode_uint32(tag_size, &p_manifest.write[ofs + 4]); // size
								encode_uint32(0, &p_manifest.write[ofs + 8]); // lineno
								encode_uint32(-1, &p_manifest.write[ofs + 12]); // comment
								encode_uint32(-1, &p_manifest.write[ofs + 16]); // ns
								encode_uint32(attr_uses_feature_string, &p_manifest.write[ofs + 20]); // name
								encode_uint16(20, &p_manifest.write[ofs + 24]); // attr_start
								encode_uint16(20, &p_manifest.write[ofs + 26]); // attr_size
								encode_uint16(attr_count, &p_manifest.write[ofs + 28]); // num_attrs
								encode_uint16(0, &p_manifest.write[ofs + 30]); // id_index
								encode_uint16(0, &p_manifest.write[ofs + 32]); // class_index
								encode_uint16(0, &p_manifest.write[ofs + 34]); // style_index

								// android:name attribute
								encode_uint32(ns_android_string, &p_manifest.write[ofs + 36]); // ns
								encode_uint32(attr_name_string, &p_manifest.write[ofs + 40]); // 'name'
								encode_uint32(feature_string, &p_manifest.write[ofs + 44]); // raw_value
								encode_uint16(8, &p_manifest.write[ofs + 48]); // typedvalue_size
								p_manifest.write[ofs + 50] = 0; // typedvalue_always0
								p_manifest.write[ofs + 51] = 0x03; // typedvalue_type (string)
								encode_uint32(feature_string, &p_manifest.write[ofs + 52]); // typedvalue reference

								// android:required attribute
								encode_uint32(ns_android_string, &p_manifest.write[ofs + 56]); // ns
								encode_uint32(attr_required_string, &p_manifest.write[ofs + 60]); // 'name'
								encode_uint32(required_value, &p_manifest.write[ofs + 64]); // raw_value
								encode_uint16(8, &p_manifest.write[ofs + 68]); // typedvalue_size
								p_manifest.write[ofs + 70] = 0; // typedvalue_always0
								p_manifest.write[ofs + 71] = 0x03; // typedvalue_type (string)
								encode_uint32(required_value, &p_manifest.write[ofs + 72]); // typedvalue reference

								ofs += 76;

								if (has_version_attribute) {
									// android:version attribute
									encode_uint32(ns_android_string, &p_manifest.write[ofs]); // ns
									encode_uint32(attr_version_string, &p_manifest.write[ofs + 4]); // 'name'
									encode_uint32(version_value, &p_manifest.write[ofs + 8]); // raw_value
									encode_uint16(8, &p_manifest.write[ofs + 12]); // typedvalue_size
									p_manifest.write[ofs + 14] = 0; // typedvalue_always0
									p_manifest.write[ofs + 15] = 0x03; // typedvalue_type (string)
									encode_uint32(version_value, &p_manifest.write[ofs + 16]); // typedvalue reference

									ofs += 20;
								}

								// end tag
								encode_uint16(0x103, &p_manifest.write[ofs]); // type
								encode_uint16(16, &p_manifest.write[ofs + 2]); // headersize
								encode_uint32(24, &p_manifest.write[ofs + 4]); // size
								encode_uint32(0, &p_manifest.write[ofs + 8]); // lineno
								encode_uint32(-1, &p_manifest.write[ofs + 12]); // comment
								encode_uint32(-1, &p_manifest.write[ofs + 16]); // ns
								encode_uint32(attr_uses_feature_string, &p_manifest.write[ofs + 20]); // name

								ofs += 24;
							}
							memcpy(&p_manifest.write[ofs], manifest_end.ptr(), manifest_end.size());
							ofs -= 24; // go back over back end
						}
					}
					if (tname == "manifest") {

						// save manifest ending so we can restore it
						Vector<uint8_t> manifest_end;
						uint32_t manifest_cur_size = p_manifest.size();

						manifest_end.resize(p_manifest.size() - ofs);
						memcpy(manifest_end.ptrw(), &p_manifest[ofs], manifest_end.size());

						int32_t attr_name_string = string_table.find("name");
						ERR_FAIL_COND_MSG(attr_name_string == -1, "Template does not have 'name' attribute.");

						int32_t ns_android_string = string_table.find("android");
						ERR_FAIL_COND_MSG(ns_android_string == -1, "Template does not have 'android' namespace.");

						int32_t attr_uses_permission_string = string_table.find("uses-permission");
						if (attr_uses_permission_string == -1) {
							string_table.push_back("uses-permission");
							attr_uses_permission_string = string_table.size() - 1;
						}

						for (int i = 0; i < perms.size(); ++i) {
							print_line("Adding permission " + perms[i]);

							manifest_cur_size += 56 + 24; // node + end node
							p_manifest.resize(manifest_cur_size);

							// Add permission to the string pool
							int32_t perm_string = string_table.find(perms[i]);
							if (perm_string == -1) {
								string_table.push_back(perms[i]);
								perm_string = string_table.size() - 1;
							}

							// start tag
							encode_uint16(0x102, &p_manifest.write[ofs]); // type
							encode_uint16(16, &p_manifest.write[ofs + 2]); // headersize
							encode_uint32(56, &p_manifest.write[ofs + 4]); // size
							encode_uint32(0, &p_manifest.write[ofs + 8]); // lineno
							encode_uint32(-1, &p_manifest.write[ofs + 12]); // comment
							encode_uint32(-1, &p_manifest.write[ofs + 16]); // ns
							encode_uint32(attr_uses_permission_string, &p_manifest.write[ofs + 20]); // name
							encode_uint16(20, &p_manifest.write[ofs + 24]); // attr_start
							encode_uint16(20, &p_manifest.write[ofs + 26]); // attr_size
							encode_uint16(1, &p_manifest.write[ofs + 28]); // num_attrs
							encode_uint16(0, &p_manifest.write[ofs + 30]); // id_index
							encode_uint16(0, &p_manifest.write[ofs + 32]); // class_index
							encode_uint16(0, &p_manifest.write[ofs + 34]); // style_index

							// attribute
							encode_uint32(ns_android_string, &p_manifest.write[ofs + 36]); // ns
							encode_uint32(attr_name_string, &p_manifest.write[ofs + 40]); // 'name'
							encode_uint32(perm_string, &p_manifest.write[ofs + 44]); // raw_value
							encode_uint16(8, &p_manifest.write[ofs + 48]); // typedvalue_size
							p_manifest.write[ofs + 50] = 0; // typedvalue_always0
							p_manifest.write[ofs + 51] = 0x03; // typedvalue_type (string)
							encode_uint32(perm_string, &p_manifest.write[ofs + 52]); // typedvalue reference

							ofs += 56;

							// end tag
							encode_uint16(0x103, &p_manifest.write[ofs]); // type
							encode_uint16(16, &p_manifest.write[ofs + 2]); // headersize
							encode_uint32(24, &p_manifest.write[ofs + 4]); // size
							encode_uint32(0, &p_manifest.write[ofs + 8]); // lineno
							encode_uint32(-1, &p_manifest.write[ofs + 12]); // comment
							encode_uint32(-1, &p_manifest.write[ofs + 16]); // ns
							encode_uint32(attr_uses_permission_string, &p_manifest.write[ofs + 20]); // name

							ofs += 24;
						}

						// copy footer back in
						memcpy(&p_manifest.write[ofs], manifest_end.ptr(), manifest_end.size());
					}
				} break;
			}

			ofs += size;
		}

		//create new andriodmanifest binary

		Vector<uint8_t> ret;
		ret.resize(string_table_begins + string_table.size() * 4);

		for (uint32_t i = 0; i < string_table_begins; i++) {

			ret.write[i] = p_manifest[i];
		}

		ofs = 0;
		for (int i = 0; i < string_table.size(); i++) {

			encode_uint32(ofs, &ret.write[string_table_begins + i * 4]);
			ofs += string_table[i].length() * 2 + 2 + 2;
		}

		ret.resize(ret.size() + ofs);
		string_data_offset = ret.size() - ofs;
		uint8_t *chars = &ret.write[string_data_offset];
		for (int i = 0; i < string_table.size(); i++) {

			String s = string_table[i];
			encode_uint16(s.length(), chars);
			chars += 2;
			for (int j = 0; j < s.length(); j++) {
				encode_uint16(s[j], chars);
				chars += 2;
			}
			encode_uint16(0, chars);
			chars += 2;
		}

		for (int i = 0; i < stable_extra.size(); i++) {
			ret.push_back(stable_extra[i]);
		}

		//pad
		while (ret.size() % 4)
			ret.push_back(0);

		uint32_t new_stable_end = ret.size();

		uint32_t extra = (p_manifest.size() - string_table_ends);
		ret.resize(new_stable_end + extra);
		for (uint32_t i = 0; i < extra; i++)
			ret.write[new_stable_end + i] = p_manifest[string_table_ends + i];

		while (ret.size() % 4)
			ret.push_back(0);
		encode_uint32(ret.size(), &ret.write[4]); //update new file size

		encode_uint32(new_stable_end - 8, &ret.write[12]); //update new string table size
		encode_uint32(string_table.size(), &ret.write[16]); //update new number of strings
		encode_uint32(string_data_offset - 8, &ret.write[28]); //update new string data offset

		p_manifest = ret;
	}

	static String _parse_string(const uint8_t *p_bytes, bool p_utf8) {

		uint32_t offset = 0;
		uint32_t len = 0;

		if (p_utf8) {
			uint8_t byte = p_bytes[offset];
			if (byte & 0x80)
				offset += 2;
			else
				offset += 1;
			byte = p_bytes[offset];
			offset++;
			if (byte & 0x80) {
				len = byte & 0x7F;
				len = (len << 8) + p_bytes[offset];
				offset++;
			} else {
				len = byte;
			}
		} else {
			len = decode_uint16(&p_bytes[offset]);
			offset += 2;
			if (len & 0x8000) {
				len &= 0x7FFF;
				len = (len << 16) + decode_uint16(&p_bytes[offset]);
				offset += 2;
			}
		}

		if (p_utf8) {

			Vector<uint8_t> str8;
			str8.resize(len + 1);
			for (uint32_t i = 0; i < len; i++) {
				str8.write[i] = p_bytes[offset + i];
			}
			str8.write[len] = 0;
			String str;
			str.parse_utf8((const char *)str8.ptr());
			return str;
		} else {

			String str;
			for (uint32_t i = 0; i < len; i++) {
				CharType c = decode_uint16(&p_bytes[offset + i * 2]);
				if (c == 0)
					break;
				str += String::chr(c);
			}
			return str;
		}
	}
	void _fix_resources(const Ref<EditorExportPreset> &p_preset, Vector<uint8_t> &p_manifest) {

		const int UTF8_FLAG = 0x00000100;

		uint32_t string_block_len = decode_uint32(&p_manifest[16]);
		uint32_t string_count = decode_uint32(&p_manifest[20]);
		uint32_t string_flags = decode_uint32(&p_manifest[28]);
		const uint32_t string_table_begins = 40;

		Vector<String> string_table;

		String package_name = p_preset->get("package/name");

		for (uint32_t i = 0; i < string_count; i++) {

			uint32_t offset = decode_uint32(&p_manifest[string_table_begins + i * 4]);
			offset += string_table_begins + string_count * 4;

			String str = _parse_string(&p_manifest[offset], string_flags & UTF8_FLAG);

			if (str.begins_with("godot-project-name")) {

				if (str == "godot-project-name") {
					//project name
					str = get_project_name(package_name);

				} else {

					String lang = str.substr(str.find_last("-") + 1, str.length()).replace("-", "_");
					String prop = "application/config/name_" + lang;
					if (ProjectSettings::get_singleton()->has_setting(prop)) {
						str = ProjectSettings::get_singleton()->get(prop);
					} else {
						str = get_project_name(package_name);
					}
				}
			}

			string_table.push_back(str);
		}

		//write a new string table, but use 16 bits
		Vector<uint8_t> ret;
		ret.resize(string_table_begins + string_table.size() * 4);

		for (uint32_t i = 0; i < string_table_begins; i++) {

			ret.write[i] = p_manifest[i];
		}

		int ofs = 0;
		for (int i = 0; i < string_table.size(); i++) {

			encode_uint32(ofs, &ret.write[string_table_begins + i * 4]);
			ofs += string_table[i].length() * 2 + 2 + 2;
		}

		ret.resize(ret.size() + ofs);
		uint8_t *chars = &ret.write[ret.size() - ofs];
		for (int i = 0; i < string_table.size(); i++) {

			String s = string_table[i];
			encode_uint16(s.length(), chars);
			chars += 2;
			for (int j = 0; j < s.length(); j++) {
				encode_uint16(s[j], chars);
				chars += 2;
			}
			encode_uint16(0, chars);
			chars += 2;
		}

		//pad
		while (ret.size() % 4)
			ret.push_back(0);

		//change flags to not use utf8
		encode_uint32(string_flags & ~0x100, &ret.write[28]);
		//change length
		encode_uint32(ret.size() - 12, &ret.write[16]);
		//append the rest...
		int rest_from = 12 + string_block_len;
		int rest_to = ret.size();
		int rest_len = (p_manifest.size() - rest_from);
		ret.resize(ret.size() + (p_manifest.size() - rest_from));
		for (int i = 0; i < rest_len; i++) {
			ret.write[rest_to + i] = p_manifest[rest_from + i];
		}
		//finally update the size
		encode_uint32(ret.size(), &ret.write[4]);

		p_manifest = ret;
		//printf("end\n");
	}

	void _process_launcher_icons(const String &p_processing_file_name, const Ref<Image> &p_source_image, const LauncherIcon p_icon, Vector<uint8_t> &p_data) {
		if (p_processing_file_name == p_icon.export_path) {
			Ref<Image> working_image = p_source_image;

			if (p_source_image->get_width() != p_icon.dimensions || p_source_image->get_height() != p_icon.dimensions) {
				working_image = p_source_image->duplicate();
				working_image->resize(p_icon.dimensions, p_icon.dimensions, Image::Interpolation::INTERPOLATE_LANCZOS);
			}

			PoolVector<uint8_t> png_buffer;
			Error err = PNGDriverCommon::image_to_png(working_image, png_buffer);
			if (err == OK) {
				p_data.resize(png_buffer.size());
				memcpy(p_data.ptrw(), png_buffer.read().ptr(), p_data.size());
			} else {
				String err_str = String("Failed to convert resized icon (") + p_processing_file_name + ") to png.";
				WARN_PRINT(err_str.utf8().get_data());
			}
		}
	}

	static Vector<String> get_enabled_abis(const Ref<EditorExportPreset> &p_preset) {
		Vector<String> abis = get_abis();
		Vector<String> enabled_abis;
		for (int i = 0; i < abis.size(); ++i) {
			bool is_enabled = p_preset->get("architectures/" + abis[i]);
			if (is_enabled) {
				enabled_abis.push_back(abis[i]);
			}
		}
		return enabled_abis;
	}

public:
	typedef Error (*EditorExportSaveFunction)(void *p_userdata, const String &p_path, const Vector<uint8_t> &p_data, int p_file, int p_total);

public:
	virtual void get_preset_features(const Ref<EditorExportPreset> &p_preset, List<String> *r_features) {

		String driver = ProjectSettings::get_singleton()->get("rendering/quality/driver/driver_name");
		if (driver == "GLES2") {
			r_features->push_back("etc");
		} else if (driver == "GLES3") {
			r_features->push_back("etc2");
			if (ProjectSettings::get_singleton()->get("rendering/quality/driver/fallback_to_gles2")) {
				r_features->push_back("etc");
			}
		}

		Vector<String> abis = get_enabled_abis(p_preset);
		for (int i = 0; i < abis.size(); ++i) {
			r_features->push_back(abis[i]);
		}
	}

	virtual void get_export_options(List<ExportOption> *r_options) {

		r_options->push_back(ExportOption(PropertyInfo(Variant::BOOL, "graphics/32_bits_framebuffer"), true));
		r_options->push_back(ExportOption(PropertyInfo(Variant::INT, "xr_features/xr_mode", PROPERTY_HINT_ENUM, "Regular,Oculus Mobile VR"), 0));
		r_options->push_back(ExportOption(PropertyInfo(Variant::INT, "xr_features/degrees_of_freedom", PROPERTY_HINT_ENUM, "None,3DOF and 6DOF,6DOF"), 0));
		r_options->push_back(ExportOption(PropertyInfo(Variant::INT, "xr_features/hand_tracking", PROPERTY_HINT_ENUM, "None,Optional,Required"), 0));
		r_options->push_back(ExportOption(PropertyInfo(Variant::BOOL, "xr_features/focus_awareness"), false));
		r_options->push_back(ExportOption(PropertyInfo(Variant::BOOL, "one_click_deploy/clear_previous_install"), false));
		r_options->push_back(ExportOption(PropertyInfo(Variant::STRING, "custom_template/debug", PROPERTY_HINT_GLOBAL_FILE, "*.apk"), ""));
		r_options->push_back(ExportOption(PropertyInfo(Variant::STRING, "custom_template/release", PROPERTY_HINT_GLOBAL_FILE, "*.apk"), ""));
		r_options->push_back(ExportOption(PropertyInfo(Variant::BOOL, "custom_template/use_custom_build"), false));

		Vector<PluginConfig> plugins_configs = get_plugins();
		for (int i = 0; i < plugins_configs.size(); i++) {
			print_verbose("Found Android plugin " + plugins_configs[i].name);
			r_options->push_back(ExportOption(PropertyInfo(Variant::BOOL, "plugins/" + plugins_configs[i].name), false));
		}
		plugins_changed = false;

		r_options->push_back(ExportOption(PropertyInfo(Variant::STRING, "command_line/extra_args"), ""));
		r_options->push_back(ExportOption(PropertyInfo(Variant::INT, "version/code", PROPERTY_HINT_RANGE, "1,4096,1,or_greater"), 1));
		r_options->push_back(ExportOption(PropertyInfo(Variant::STRING, "version/name"), "1.0"));
		r_options->push_back(ExportOption(PropertyInfo(Variant::STRING, "package/unique_name", PROPERTY_HINT_PLACEHOLDER_TEXT, "ext.domain.name"), "org.godotengine.$genname"));
		r_options->push_back(ExportOption(PropertyInfo(Variant::STRING, "package/name", PROPERTY_HINT_PLACEHOLDER_TEXT, "Game Name [default if blank]"), ""));
		r_options->push_back(ExportOption(PropertyInfo(Variant::BOOL, "package/signed"), true));
		r_options->push_back(ExportOption(PropertyInfo(Variant::BOOL, "screen/immersive_mode"), true));
		r_options->push_back(ExportOption(PropertyInfo(Variant::INT, "screen/orientation", PROPERTY_HINT_ENUM, "Landscape,Portrait"), 0));
		r_options->push_back(ExportOption(PropertyInfo(Variant::BOOL, "screen/support_small"), true));
		r_options->push_back(ExportOption(PropertyInfo(Variant::BOOL, "screen/support_normal"), true));
		r_options->push_back(ExportOption(PropertyInfo(Variant::BOOL, "screen/support_large"), true));
		r_options->push_back(ExportOption(PropertyInfo(Variant::BOOL, "screen/support_xlarge"), true));
		r_options->push_back(ExportOption(PropertyInfo(Variant::BOOL, "screen/opengl_debug"), false));
		r_options->push_back(ExportOption(PropertyInfo(Variant::STRING, launcher_icon_option, PROPERTY_HINT_FILE, "*.png"), ""));
		r_options->push_back(ExportOption(PropertyInfo(Variant::STRING, launcher_adaptive_icon_foreground_option, PROPERTY_HINT_FILE, "*.png"), ""));
		r_options->push_back(ExportOption(PropertyInfo(Variant::STRING, launcher_adaptive_icon_background_option, PROPERTY_HINT_FILE, "*.png"), ""));
		r_options->push_back(ExportOption(PropertyInfo(Variant::STRING, "keystore/debug", PROPERTY_HINT_GLOBAL_FILE, "*.keystore"), ""));
		r_options->push_back(ExportOption(PropertyInfo(Variant::STRING, "keystore/debug_user"), ""));
		r_options->push_back(ExportOption(PropertyInfo(Variant::STRING, "keystore/debug_password"), ""));
		r_options->push_back(ExportOption(PropertyInfo(Variant::STRING, "keystore/release", PROPERTY_HINT_GLOBAL_FILE, "*.keystore"), ""));
		r_options->push_back(ExportOption(PropertyInfo(Variant::STRING, "keystore/release_user"), ""));
		r_options->push_back(ExportOption(PropertyInfo(Variant::STRING, "keystore/release_password"), ""));
		r_options->push_back(ExportOption(PropertyInfo(Variant::BOOL, "apk_expansion/enable"), false));
		r_options->push_back(ExportOption(PropertyInfo(Variant::STRING, "apk_expansion/SALT"), ""));
		r_options->push_back(ExportOption(PropertyInfo(Variant::STRING, "apk_expansion/public_key", PROPERTY_HINT_MULTILINE_TEXT), ""));

		Vector<String> abis = get_abis();
		for (int i = 0; i < abis.size(); ++i) {
			String abi = abis[i];
			bool is_default = (abi == "armeabi-v7a" || abi == "arm64-v8a");
			r_options->push_back(ExportOption(PropertyInfo(Variant::BOOL, "architectures/" + abi), is_default));
		}

		r_options->push_back(ExportOption(PropertyInfo(Variant::POOL_STRING_ARRAY, "permissions/custom_permissions"), PoolStringArray()));

		const char **perms = android_perms;
		while (*perms) {

			r_options->push_back(ExportOption(PropertyInfo(Variant::BOOL, "permissions/" + String(*perms).to_lower()), false));
			perms++;
		}
	}

	virtual String get_name() const {
		return "Android";
	}

	virtual String get_os_name() const {
		return "Android";
	}

	virtual Ref<Texture> get_logo() const {
		return logo;
	}

	virtual bool should_update_export_options() {
		bool export_options_changed = plugins_changed;
		if (export_options_changed) {
			// don't clear unless we're reporting true, to avoid race
			plugins_changed = false;
		}
		return export_options_changed;
	}

	virtual bool poll_export() {

		bool dc = devices_changed;
		if (dc) {
			// don't clear unless we're reporting true, to avoid race
			devices_changed = false;
		}
		return dc;
	}

	virtual int get_options_count() const {

		device_lock->lock();
		int dc = devices.size();
		device_lock->unlock();

		return dc;
	}

	virtual String get_options_tooltip() const {

		return TTR("Select device from the list");
	}

	virtual String get_option_label(int p_index) const {

		ERR_FAIL_INDEX_V(p_index, devices.size(), "");
		device_lock->lock();
		String s = devices[p_index].name;
		device_lock->unlock();
		return s;
	}

	virtual String get_option_tooltip(int p_index) const {

		ERR_FAIL_INDEX_V(p_index, devices.size(), "");
		device_lock->lock();
		String s = devices[p_index].description;
		if (devices.size() == 1) {
			// Tooltip will be:
			// Name
			// Description
			s = devices[p_index].name + "\n\n" + s;
		}
		device_lock->unlock();
		return s;
	}

	virtual Error run(const Ref<EditorExportPreset> &p_preset, int p_device, int p_debug_flags) {

		ERR_FAIL_INDEX_V(p_device, devices.size(), ERR_INVALID_PARAMETER);

		String can_export_error;
		bool can_export_missing_templates;
		if (!can_export(p_preset, can_export_error, can_export_missing_templates)) {
			EditorNode::add_io_error(can_export_error);
			return ERR_UNCONFIGURED;
		}

		device_lock->lock();

		EditorProgress ep("run", "Running on " + devices[p_device].name, 3);

		String adb = EditorSettings::get_singleton()->get("export/android/adb");

		// Export_temp APK.
		if (ep.step("Exporting APK...", 0)) {
			device_lock->unlock();
			return ERR_SKIP;
		}

		const bool use_remote = (p_debug_flags & DEBUG_FLAG_REMOTE_DEBUG) || (p_debug_flags & DEBUG_FLAG_DUMB_CLIENT);
		const bool use_reverse = devices[p_device].api_level >= 21;

		if (use_reverse)
			p_debug_flags |= DEBUG_FLAG_REMOTE_DEBUG_LOCALHOST;

		String tmp_export_path = EditorSettings::get_singleton()->get_cache_dir().plus_file("tmpexport.apk");

#define CLEANUP_AND_RETURN(m_err)                         \
	{                                                     \
		DirAccess::remove_file_or_error(tmp_export_path); \
		device_lock->unlock();                            \
		return m_err;                                     \
	}

		// Export to temporary APK before sending to device.
		Error err = export_project(p_preset, true, tmp_export_path, p_debug_flags);

		if (err != OK) {
			CLEANUP_AND_RETURN(err);
		}

		List<String> args;
		int rv;

		bool remove_prev = p_preset->get("one_click_deploy/clear_previous_install");
		String version_name = p_preset->get("version/name");
		String package_name = p_preset->get("package/unique_name");

		if (remove_prev) {
			if (ep.step("Uninstalling...", 1)) {
				CLEANUP_AND_RETURN(ERR_SKIP);
			}

			print_line("Uninstalling previous version: " + devices[p_device].name);

			args.push_back("-s");
			args.push_back(devices[p_device].id);
			args.push_back("uninstall");
			args.push_back(get_package_name(package_name));

			err = OS::get_singleton()->execute(adb, args, true, NULL, NULL, &rv);
		}

		print_line("Installing to device (please wait...): " + devices[p_device].name);
		if (ep.step("Installing to device, please wait...", 2)) {
			CLEANUP_AND_RETURN(ERR_SKIP);
		}

		args.clear();
		args.push_back("-s");
		args.push_back(devices[p_device].id);
		args.push_back("install");
		args.push_back("-r");
		args.push_back(tmp_export_path);

		err = OS::get_singleton()->execute(adb, args, true, NULL, NULL, &rv);
		if (err || rv != 0) {
			EditorNode::add_io_error("Could not install to device.");
			CLEANUP_AND_RETURN(ERR_CANT_CREATE);
		}

		if (use_remote) {
			if (use_reverse) {

				static const char *const msg = "--- Device API >= 21; debugging over USB ---";
				EditorNode::get_singleton()->get_log()->add_message(msg, EditorLog::MSG_TYPE_EDITOR);
				print_line(String(msg).to_upper());

				args.clear();
				args.push_back("-s");
				args.push_back(devices[p_device].id);
				args.push_back("reverse");
				args.push_back("--remove-all");
				OS::get_singleton()->execute(adb, args, true, NULL, NULL, &rv);

				if (p_debug_flags & DEBUG_FLAG_REMOTE_DEBUG) {

					int dbg_port = EditorSettings::get_singleton()->get("network/debug/remote_port");
					args.clear();
					args.push_back("-s");
					args.push_back(devices[p_device].id);
					args.push_back("reverse");
					args.push_back("tcp:" + itos(dbg_port));
					args.push_back("tcp:" + itos(dbg_port));

					OS::get_singleton()->execute(adb, args, true, NULL, NULL, &rv);
					print_line("Reverse result: " + itos(rv));
				}

				if (p_debug_flags & DEBUG_FLAG_DUMB_CLIENT) {

					int fs_port = EditorSettings::get_singleton()->get("filesystem/file_server/port");

					args.clear();
					args.push_back("-s");
					args.push_back(devices[p_device].id);
					args.push_back("reverse");
					args.push_back("tcp:" + itos(fs_port));
					args.push_back("tcp:" + itos(fs_port));

					err = OS::get_singleton()->execute(adb, args, true, NULL, NULL, &rv);
					print_line("Reverse result2: " + itos(rv));
				}
			} else {

				static const char *const msg = "--- Device API < 21; debugging over Wi-Fi ---";
				EditorNode::get_singleton()->get_log()->add_message(msg, EditorLog::MSG_TYPE_EDITOR);
				print_line(String(msg).to_upper());
			}
		}

		if (ep.step("Running on device...", 3)) {
			CLEANUP_AND_RETURN(ERR_SKIP);
		}
		args.clear();
		args.push_back("-s");
		args.push_back(devices[p_device].id);
		args.push_back("shell");
		args.push_back("am");
		args.push_back("start");
		if ((bool)EditorSettings::get_singleton()->get("export/android/force_system_user") && devices[p_device].api_level >= 17) { // Multi-user introduced in Android 17
			args.push_back("--user");
			args.push_back("0");
		}
		args.push_back("-a");
		args.push_back("android.intent.action.MAIN");
		args.push_back("-n");
		args.push_back(get_package_name(package_name) + "/com.godot.game.GodotApp");

		err = OS::get_singleton()->execute(adb, args, true, NULL, NULL, &rv);
		if (err || rv != 0) {
			EditorNode::add_io_error("Could not execute on device.");
			CLEANUP_AND_RETURN(ERR_CANT_CREATE);
		}

		CLEANUP_AND_RETURN(OK);
#undef CLEANUP_AND_RETURN
	}

	virtual Ref<Texture> get_run_icon() const {
		return run_icon;
	}

	virtual bool can_export(const Ref<EditorExportPreset> &p_preset, String &r_error, bool &r_missing_templates) const {

		String err;
		bool valid = false;

		// Look for export templates (first official, and if defined custom templates).

		if (!bool(p_preset->get("custom_template/use_custom_build"))) {
			String template_err;
			bool dvalid = false;
			bool rvalid = false;

			if (p_preset->get("custom_template/debug") != "") {
				dvalid = FileAccess::exists(p_preset->get("custom_template/debug"));
				if (!dvalid) {
					template_err += TTR("Custom debug template not found.") + "\n";
				}
			} else {
				dvalid = exists_export_template("android_debug.apk", &template_err);
			}

			if (p_preset->get("custom_template/release") != "") {
				rvalid = FileAccess::exists(p_preset->get("custom_template/release"));
				if (!rvalid) {
					template_err += TTR("Custom release template not found.") + "\n";
				}
			} else {
				rvalid = exists_export_template("android_release.apk", &template_err);
			}

			valid = dvalid || rvalid;
			if (!valid) {
				err += template_err;
			}
		} else {
			valid = exists_export_template("android_source.zip", &err);
		}
		r_missing_templates = !valid;

		// Validate the rest of the configuration.

		String adb = EditorSettings::get_singleton()->get("export/android/adb");

		if (!FileAccess::exists(adb)) {

			valid = false;
			err += TTR("ADB executable not configured in the Editor Settings.") + "\n";
		}

		String js = EditorSettings::get_singleton()->get("export/android/jarsigner");

		if (!FileAccess::exists(js)) {

			valid = false;
			err += TTR("OpenJDK jarsigner not configured in the Editor Settings.") + "\n";
		}

		String dk = p_preset->get("keystore/debug");

		if (!FileAccess::exists(dk)) {

			dk = EditorSettings::get_singleton()->get("export/android/debug_keystore");
			if (!FileAccess::exists(dk)) {
				valid = false;
				err += TTR("Debug keystore not configured in the Editor Settings nor in the preset.") + "\n";
			}
		}

		String rk = p_preset->get("keystore/release");

		if (!rk.empty() && !FileAccess::exists(rk)) {
			valid = false;
			err += TTR("Release keystore incorrectly configured in the export preset.") + "\n";
		}

		if (bool(p_preset->get("custom_template/use_custom_build"))) {
			String sdk_path = EditorSettings::get_singleton()->get("export/android/custom_build_sdk_path");
			if (sdk_path == "") {
				err += TTR("Custom build requires a valid Android SDK path in Editor Settings.") + "\n";
				valid = false;
			} else {
				Error errn;
				DirAccessRef da = DirAccess::open(sdk_path.plus_file("platform-tools"), &errn);
				if (errn != OK) {
					err += TTR("Invalid Android SDK path for custom build in Editor Settings.") + "\n";
					valid = false;
				}
			}

			if (!FileAccess::exists("res://android/build/build.gradle")) {

				err += TTR("Android build template not installed in the project. Install it from the Project menu.") + "\n";
				valid = false;
			}
		}

		bool apk_expansion = p_preset->get("apk_expansion/enable");

		if (apk_expansion) {

			String apk_expansion_pkey = p_preset->get("apk_expansion/public_key");

			if (apk_expansion_pkey == "") {
				valid = false;

				err += TTR("Invalid public key for APK expansion.") + "\n";
			}
		}

		String pn = p_preset->get("package/unique_name");
		String pn_err;

		if (!is_package_name_valid(get_package_name(pn), &pn_err)) {

			valid = false;
			err += TTR("Invalid package name:") + " " + pn_err + "\n";
		}

		String etc_error = test_etc2();
		if (etc_error != String()) {
			valid = false;
			err += etc_error;
		}

		// The GodotPaymentV3 module was converted to the external GodotGooglePlayBilling plugin in Godot 3.2.2,
		// this check helps users to notice the change to ensure that they change their settings.
		String modules = ProjectSettings::get_singleton()->get("android/modules");
		if (modules.find("org/godotengine/godot/GodotPaymentV3") != -1) {
			bool godot_google_play_billing_enabled = p_preset->get("plugins/GodotGooglePlayBilling");
			if (!godot_google_play_billing_enabled) {
				valid = false;
				err += TTR("Invalid \"GodotPaymentV3\" module included in the \"android/modules\" project setting (changed in Godot 3.2.2).\n"
						   "Replace it with the first-party \"GodotGooglePlayBilling\" plugin.\n"
						   "Note that the singleton was also renamed from \"GodotPayments\" to \"GodotGooglePlayBilling\".");
				err += "\n";
			}
		}

		// Ensure that `Use Custom Build` is enabled if a plugin is selected.
		String enabled_plugins_names = get_plugins_names(get_enabled_plugins(p_preset));
		bool custom_build_enabled = p_preset->get("custom_template/use_custom_build");
		if (!enabled_plugins_names.empty() && !custom_build_enabled) {
			valid = false;
			err += TTR("\"Use Custom Build\" must be enabled to use the plugins.");
			err += "\n";
		}

		// Validate the Xr features are properly populated
		int xr_mode_index = p_preset->get("xr_features/xr_mode");
		int degrees_of_freedom = p_preset->get("xr_features/degrees_of_freedom");
		int hand_tracking = p_preset->get("xr_features/hand_tracking");
		bool focus_awareness = p_preset->get("xr_features/focus_awareness");
		if (xr_mode_index != /* XRMode.OVR*/ 1) {
			if (degrees_of_freedom > 0) {
				valid = false;
				err += TTR("\"Degrees Of Freedom\" is only valid when \"Xr Mode\" is \"Oculus Mobile VR\".");
				err += "\n";
			}

			if (hand_tracking > 0) {
				valid = false;
				err += TTR("\"Hand Tracking\" is only valid when \"Xr Mode\" is \"Oculus Mobile VR\".");
				err += "\n";
			}

			if (focus_awareness) {
				valid = false;
				err += TTR("\"Focus Awareness\" is only valid when \"Xr Mode\" is \"Oculus Mobile VR\".");
				err += "\n";
			}
		}

		r_error = err;
		return valid;
	}

	virtual List<String> get_binary_extensions(const Ref<EditorExportPreset> &p_preset) const {
		List<String> list;
		list.push_back("apk");
		return list;
	}

	void _update_custom_build_project() {

		DirAccessRef da = DirAccess::open("res://android");

		ERR_FAIL_COND_MSG(!da, "Cannot open directory 'res://android'.");
		Map<String, List<String> > directory_paths;
		Map<String, List<String> > manifest_sections;
		Map<String, List<String> > gradle_sections;
		da->list_dir_begin();
		String d = da->get_next();
		while (d != String()) {

			if (!d.begins_with(".") && d != "build" && da->current_is_dir()) { //a dir and not the build dir
				//add directories found
				DirAccessRef ds = DirAccess::open(String("res://android").plus_file(d));
				if (ds) {
					ds->list_dir_begin();
					String sd = ds->get_next();
					while (sd != String()) {

						if (!sd.begins_with(".") && ds->current_is_dir()) {
							String key = sd.to_upper();
							if (!directory_paths.has(key)) {
								directory_paths[key] = List<String>();
							}
							String path = ProjectSettings::get_singleton()->get_resource_path().plus_file("android").plus_file(d).plus_file(sd);
							directory_paths[key].push_back(path);
							print_line("Add: " + sd + ":" + path);
						}

						sd = ds->get_next();
					}
					ds->list_dir_end();
				}
				//parse manifest
				{
					FileAccessRef f = FileAccess::open(String("res://android").plus_file(d).plus_file("AndroidManifest.conf"), FileAccess::READ);
					if (f) {

						String section;
						while (!f->eof_reached()) {
							String l = f->get_line();
							String k = l.strip_edges();
							if (k.begins_with("[")) {
								section = k.substr(1, k.length() - 2).strip_edges().to_upper();
								print_line("Section: " + section);
							} else if (k != String()) {
								if (!manifest_sections.has(section)) {
									manifest_sections[section] = List<String>();
								}
								manifest_sections[section].push_back(l);
							}
						}

						f->close();
					}
				}
				//parse gradle
				{
					FileAccessRef f = FileAccess::open(String("res://android").plus_file(d).plus_file("gradle.conf"), FileAccess::READ);
					if (f) {

						String section;
						while (!f->eof_reached()) {
							String l = f->get_line().strip_edges();
							String k = l.strip_edges();
							if (k.begins_with("[")) {
								section = k.substr(1, k.length() - 2).strip_edges().to_upper();
								print_line("Section: " + section);
							} else if (k != String()) {
								if (!gradle_sections.has(section)) {
									gradle_sections[section] = List<String>();
								}
								gradle_sections[section].push_back(l);
							}
						}
					}
				}
			}
			d = da->get_next();
		}
		da->list_dir_end();

		{ //fix gradle build

			String new_file;
			{
				FileAccessRef f = FileAccess::open("res://android/build/build.gradle", FileAccess::READ);
				if (f) {

					while (!f->eof_reached()) {
						String l = f->get_line();

						bool append_line = false;
						if (l.begins_with("//CHUNK_")) {
							String text = l.replace_first("//CHUNK_", "");
							int begin_pos = text.find("_BEGIN");
							if (begin_pos != -1) {
								text = text.substr(0, begin_pos);
								text = text.to_upper(); //just in case

								String end_marker = "//CHUNK_" + text + "_END";
								size_t pos = f->get_position();
								bool found = false;
								while (!f->eof_reached()) {
									l = f->get_line();
									if (l.begins_with(end_marker)) {
										found = true;
										break;
									}
								}

								new_file += "//CHUNK_" + text + "_BEGIN\n";

								if (!found) {
									ERR_PRINTS("No end marker found in build.gradle for chunk: " + text);
									f->seek(pos);
								} else {

									//add chunk lines
									if (gradle_sections.has(text)) {
										for (List<String>::Element *E = gradle_sections[text].front(); E; E = E->next()) {
											new_file += E->get() + "\n";
										}
									}
									if (f->eof_reached()) {
										new_file += end_marker;
									} else {
										new_file += end_marker + "\n";
									}
								}
							} else {
								append_line = true;
							}
						} else if (l.begins_with("//DIR_")) {
							String text = l.replace_first("//DIR_", "");
							int begin_pos = text.find("_BEGIN");
							if (begin_pos != -1) {
								text = text.substr(0, begin_pos);
								text = text.to_upper(); //just in case

								String end_marker = "//DIR_" + text + "_END";
								size_t pos = f->get_position();
								bool found = false;
								while (!f->eof_reached()) {
									l = f->get_line();
									if (l.begins_with(end_marker)) {
										found = true;
										break;
									}
								}

								new_file += "//DIR_" + text + "_BEGIN\n";

								if (!found) {
									ERR_PRINTS("No end marker found in build.gradle for dir: " + text);
									f->seek(pos);
								} else {
									//add chunk lines
									if (directory_paths.has(text)) {
										for (List<String>::Element *E = directory_paths[text].front(); E; E = E->next()) {
											new_file += ",'" + E->get().replace("'", "\'") + "'";
											new_file += "\n";
										}
									}
									if (f->eof_reached()) {
										new_file += end_marker;
									} else {
										new_file += end_marker + "\n";
									}
								}
							} else {
								append_line = true;
							}
						} else {
							append_line = true;
						}

						if (append_line) {
							if (f->eof_reached()) {
								new_file += l;
							} else {
								new_file += l + "\n";
							}
						}
					}
				}
			}

			FileAccessRef f = FileAccess::open("res://android/build/build.gradle", FileAccess::WRITE);
			f->store_string(new_file);
			f->close();
		}

		{ //fix manifest

			String new_file;
			{
				FileAccessRef f = FileAccess::open("res://android/build/AndroidManifest.xml", FileAccess::READ);
				if (f) {

					while (!f->eof_reached()) {
						String l = f->get_line();

						bool append_line = false;
						if (l.begins_with("<!--CHUNK_")) {
							String text = l.replace_first("<!--CHUNK_", "");
							int begin_pos = text.find("_BEGIN-->");
							if (begin_pos != -1) {
								text = text.substr(0, begin_pos);
								text = text.to_upper(); //just in case

								String end_marker = "<!--CHUNK_" + text + "_END-->";
								size_t pos = f->get_position();
								bool found = false;
								while (!f->eof_reached()) {
									l = f->get_line();
									if (l.begins_with(end_marker)) {
										found = true;
										break;
									}
								}

								new_file += "<!--CHUNK_" + text + "_BEGIN-->\n";

								if (!found) {
									ERR_PRINTS("No end marker found in AndroidManifest.xml for chunk: " + text);
									f->seek(pos);
								} else {
									//add chunk lines
									if (manifest_sections.has(text)) {
										for (List<String>::Element *E = manifest_sections[text].front(); E; E = E->next()) {
											new_file += E->get() + "\n";
										}
									}
									if (f->eof_reached()) {
										new_file += end_marker;
									} else {
										new_file += end_marker + "\n";
									}
								}
							} else {
								append_line = true;
							}

						} else if (l.strip_edges().begins_with("<application")) {
							String last_tag = "android:icon=\"@mipmap/icon\"";
							int last_tag_pos = l.find(last_tag);
							if (last_tag_pos == -1) {
								ERR_PRINTS("Not adding application attributes as the expected tag was not found in '<application': " + last_tag);
								append_line = true;
							} else {
								String base = l.substr(0, last_tag_pos + last_tag.length());
								if (manifest_sections.has("application_attribs")) {
									for (List<String>::Element *E = manifest_sections["application_attribs"].front(); E; E = E->next()) {
										String to_add = E->get().strip_edges();
										base += " " + to_add + " ";
									}
								}
								base += ">\n";
								new_file += base;
							}
						} else {
							append_line = true;
						}

						if (append_line) {
							new_file += l;
							if (!f->eof_reached()) {
								new_file += "\n";
							}
						}
					}
				}
			}

			FileAccessRef f = FileAccess::open("res://android/build/AndroidManifest.xml", FileAccess::WRITE);
			f->store_string(new_file);
			f->close();
		}
	}

	inline bool is_clean_build_required(Vector<PluginConfig> enabled_plugins) {
		String plugin_names = get_plugins_names(enabled_plugins);
		bool first_build = last_custom_build_time == 0;
		bool have_plugins_changed = false;

		if (!first_build) {
			have_plugins_changed = plugin_names != last_plugin_names;
			if (!have_plugins_changed) {
				for (int i = 0; i < enabled_plugins.size(); i++) {
					if (enabled_plugins.get(i).last_updated > last_custom_build_time) {
						have_plugins_changed = true;
						break;
					}
				}
			}
		}

		last_custom_build_time = OS::get_singleton()->get_unix_time();
		last_plugin_names = plugin_names;

		return have_plugins_changed || first_build;
	}

	virtual Error export_project(const Ref<EditorExportPreset> &p_preset, bool p_debug, const String &p_path, int p_flags = 0) {

		ExportNotifier notifier(*this, p_preset, p_debug, p_path, p_flags);

		String src_apk;

		EditorProgress ep("export", "Exporting for Android", 105, true);

		if (bool(p_preset->get("custom_template/use_custom_build"))) { //custom build
			//re-generate build.gradle and AndroidManifest.xml

			{ //test that installed build version is alright
				FileAccessRef f = FileAccess::open("res://android/.build_version", FileAccess::READ);
				if (!f) {
					EditorNode::get_singleton()->show_warning(TTR("Trying to build from a custom built template, but no version info for it exists. Please reinstall from the 'Project' menu."));
					return ERR_UNCONFIGURED;
				}
				String version = f->get_line().strip_edges();
				if (version != VERSION_FULL_CONFIG) {
					EditorNode::get_singleton()->show_warning(vformat(TTR("Android build version mismatch:\n   Template installed: %s\n   Godot Version: %s\nPlease reinstall Android build template from 'Project' menu."), version, VERSION_FULL_CONFIG));
					return ERR_UNCONFIGURED;
				}
			}
			//build project if custom build is enabled
			String sdk_path = EDITOR_GET("export/android/custom_build_sdk_path");

			ERR_FAIL_COND_V_MSG(sdk_path == "", ERR_UNCONFIGURED, "Android SDK path must be configured in Editor Settings at 'export/android/custom_build_sdk_path'.");

			_update_custom_build_project();

			OS::get_singleton()->set_environment("ANDROID_HOME", sdk_path); //set and overwrite if required

			String build_command;
#ifdef WINDOWS_ENABLED
			build_command = "gradlew.bat";
#else
			build_command = "gradlew";
#endif

			String build_path = ProjectSettings::get_singleton()->get_resource_path().plus_file("android/build");

			build_command = build_path.plus_file(build_command);

			String package_name = get_package_name(p_preset->get("package/unique_name"));

			Vector<PluginConfig> enabled_plugins = get_enabled_plugins(p_preset);
			String local_plugins_binaries = get_plugins_binaries(BINARY_TYPE_LOCAL, enabled_plugins);
			String remote_plugins_binaries = get_plugins_binaries(BINARY_TYPE_REMOTE, enabled_plugins);
			String custom_maven_repos = get_plugins_custom_maven_repos(enabled_plugins);
			bool clean_build_required = is_clean_build_required(enabled_plugins);

			List<String> cmdline;
			if (clean_build_required) {
				cmdline.push_back("clean");
			}
			cmdline.push_back("build");
			cmdline.push_back("-Pexport_package_name=" + package_name); // argument to specify the package name.
			cmdline.push_back("-Pplugins_local_binaries=" + local_plugins_binaries); // argument to specify the list of plugins local dependencies.
			cmdline.push_back("-Pplugins_remote_binaries=" + remote_plugins_binaries); // argument to specify the list of plugins remote dependencies.
			cmdline.push_back("-Pplugins_maven_repos=" + custom_maven_repos); // argument to specify the list of custom maven repos for the plugins dependencies.
			cmdline.push_back("-p"); // argument to specify the start directory.
			cmdline.push_back(build_path); // start directory.
			/*{ used for debug
				int ec;
				String pipe;
				OS::get_singleton()->execute(build_command, cmdline, true, NULL, NULL, &ec);
				print_line("exit code: " + itos(ec));
			}
			*/
			int result = EditorNode::get_singleton()->execute_and_show_output(TTR("Building Android Project (gradle)"), build_command, cmdline);
			if (result != 0) {
				EditorNode::get_singleton()->show_warning(TTR("Building of Android project failed, check output for the error.\nAlternatively visit docs.godotengine.org for Android build documentation."));
				return ERR_CANT_CREATE;
			}
			if (p_debug) {
				src_apk = build_path.plus_file("build/outputs/apk/debug/android_debug.apk");
			} else {
				src_apk = build_path.plus_file("build/outputs/apk/release/android_release.apk");
			}

			if (!FileAccess::exists(src_apk)) {
				EditorNode::get_singleton()->show_warning(TTR("No build apk generated at: ") + "\n" + src_apk);
				return ERR_CANT_CREATE;
			}

		} else {

			if (p_debug)
				src_apk = p_preset->get("custom_template/debug");
			else
				src_apk = p_preset->get("custom_template/release");

			src_apk = src_apk.strip_edges();
			if (src_apk == "") {
				if (p_debug) {
					src_apk = find_export_template("android_debug.apk");
				} else {
					src_apk = find_export_template("android_release.apk");
				}
				if (src_apk == "") {
					EditorNode::add_io_error("Package not found: " + src_apk);
					return ERR_FILE_NOT_FOUND;
				}
			}
		}

		if (!DirAccess::exists(p_path.get_base_dir())) {
			return ERR_FILE_BAD_PATH;
		}

		FileAccess *src_f = NULL;
		zlib_filefunc_def io = zipio_create_io_from_file(&src_f);

		if (ep.step("Creating APK...", 0)) {
			return ERR_SKIP;
		}

		unzFile pkg = unzOpen2(src_apk.utf8().get_data(), &io);
		if (!pkg) {

			EditorNode::add_io_error("Could not find template APK to export:\n" + src_apk);
			return ERR_FILE_NOT_FOUND;
		}

		int ret = unzGoToFirstFile(pkg);

		zlib_filefunc_def io2 = io;
		FileAccess *dst_f = NULL;
		io2.opaque = &dst_f;

		String tmp_unaligned_path = EditorSettings::get_singleton()->get_cache_dir().plus_file("tmpexport-unaligned.apk");

#define CLEANUP_AND_RETURN(m_err)                            \
	{                                                        \
		DirAccess::remove_file_or_error(tmp_unaligned_path); \
		return m_err;                                        \
	}

		zipFile unaligned_apk = zipOpen2(tmp_unaligned_path.utf8().get_data(), APPEND_STATUS_CREATE, NULL, &io2);

		bool use_32_fb = p_preset->get("graphics/32_bits_framebuffer");
		bool immersive = p_preset->get("screen/immersive_mode");
		bool debug_opengl = p_preset->get("screen/opengl_debug");

		bool _signed = p_preset->get("package/signed");

		bool apk_expansion = p_preset->get("apk_expansion/enable");

		String cmdline = p_preset->get("command_line/extra_args");

		int version_code = p_preset->get("version/code");
		String version_name = p_preset->get("version/name");
		String package_name = p_preset->get("package/unique_name");

		String apk_expansion_pkey = p_preset->get("apk_expansion/public_key");

		String release_keystore = p_preset->get("keystore/release");
		String release_username = p_preset->get("keystore/release_user");
		String release_password = p_preset->get("keystore/release_password");

		Vector<String> enabled_abis = get_enabled_abis(p_preset);

		String project_icon_path = ProjectSettings::get_singleton()->get("application/config/icon");

		// Prepare images to be resized for the icons. If some image ends up being uninitialized, the default image from the export template will be used.
		Ref<Image> launcher_icon_image;
		Ref<Image> launcher_adaptive_icon_foreground_image;
		Ref<Image> launcher_adaptive_icon_background_image;

		launcher_icon_image.instance();
		launcher_adaptive_icon_foreground_image.instance();
		launcher_adaptive_icon_background_image.instance();

		// Regular icon: user selection -> project icon -> default.
		String path = static_cast<String>(p_preset->get(launcher_icon_option)).strip_edges();
		if (path.empty() || ImageLoader::load_image(path, launcher_icon_image) != OK) {
			ImageLoader::load_image(project_icon_path, launcher_icon_image);
		}

		// Adaptive foreground: user selection -> regular icon (user selection -> project icon -> default).
		path = static_cast<String>(p_preset->get(launcher_adaptive_icon_foreground_option)).strip_edges();
		if (path.empty() || ImageLoader::load_image(path, launcher_adaptive_icon_foreground_image) != OK) {
			launcher_adaptive_icon_foreground_image = launcher_icon_image;
		}

		// Adaptive background: user selection -> default.
		path = static_cast<String>(p_preset->get(launcher_adaptive_icon_background_option)).strip_edges();
		if (!path.empty()) {
			ImageLoader::load_image(path, launcher_adaptive_icon_background_image);
		}

		Vector<String> invalid_abis(enabled_abis);
		while (ret == UNZ_OK) {

			//get filename
			unz_file_info info;
			char fname[16384];
			ret = unzGetCurrentFileInfo(pkg, &info, fname, 16384, NULL, 0, NULL, 0);

			bool skip = false;

			String file = fname;

			Vector<uint8_t> data;
			data.resize(info.uncompressed_size);

			//read
			unzOpenCurrentFile(pkg);
			unzReadCurrentFile(pkg, data.ptrw(), data.size());
			unzCloseCurrentFile(pkg);

			//write

			if (file == "AndroidManifest.xml") {
				_fix_manifest(p_preset, data, p_flags & (DEBUG_FLAG_DUMB_CLIENT | DEBUG_FLAG_REMOTE_DEBUG));
			}

			if (file == "resources.arsc") {
				_fix_resources(p_preset, data);
			}

			for (int i = 0; i < icon_densities_count; ++i) {
				if (launcher_icon_image.is_valid() && !launcher_icon_image->empty()) {
					_process_launcher_icons(file, launcher_icon_image, launcher_icons[i], data);
				}
				if (launcher_adaptive_icon_foreground_image.is_valid() && !launcher_adaptive_icon_foreground_image->empty()) {
					_process_launcher_icons(file, launcher_adaptive_icon_foreground_image, launcher_adaptive_icon_foregrounds[i], data);
				}
				if (launcher_adaptive_icon_background_image.is_valid() && !launcher_adaptive_icon_background_image->empty()) {
					_process_launcher_icons(file, launcher_adaptive_icon_background_image, launcher_adaptive_icon_backgrounds[i], data);
				}
			}

			if (file.ends_with(".so")) {
				bool enabled = false;
				for (int i = 0; i < enabled_abis.size(); ++i) {
					if (file.begins_with("lib/" + enabled_abis[i] + "/")) {
						invalid_abis.erase(enabled_abis[i]);
						enabled = true;
						break;
					}
				}
				if (!enabled) {
					skip = true;
				}
			}

			if (file.begins_with("META-INF") && _signed) {
				skip = true;
			}

			if (!skip) {
				print_line("ADDING: " + file);

				// Respect decision on compression made by AAPT for the export template
				const bool uncompressed = info.compression_method == 0;

				zip_fileinfo zipfi = get_zip_fileinfo();

				zipOpenNewFileInZip(unaligned_apk,
						file.utf8().get_data(),
						&zipfi,
						NULL,
						0,
						NULL,
						0,
						NULL,
						uncompressed ? 0 : Z_DEFLATED,
						Z_DEFAULT_COMPRESSION);

				zipWriteInFileInZip(unaligned_apk, data.ptr(), data.size());
				zipCloseFileInZip(unaligned_apk);
			}

			ret = unzGoToNextFile(pkg);
		}

		if (!invalid_abis.empty()) {
			String unsupported_arch = String(", ").join(invalid_abis);
			EditorNode::add_io_error("Missing libraries in the export template for the selected architectures: " + unsupported_arch + ".\n" +
									 "Please build a template with all required libraries, or uncheck the missing architectures in the export preset.");
			CLEANUP_AND_RETURN(ERR_FILE_NOT_FOUND);
		}

		if (ep.step("Adding files...", 1)) {
			CLEANUP_AND_RETURN(ERR_SKIP);
		}
		Error err = OK;
		Vector<String> cl = cmdline.strip_edges().split(" ");
		for (int i = 0; i < cl.size(); i++) {
			if (cl[i].strip_edges().length() == 0) {
				cl.remove(i);
				i--;
			}
		}

		gen_export_flags(cl, p_flags);

		if (p_flags & DEBUG_FLAG_DUMB_CLIENT) {

			APKExportData ed;
			ed.ep = &ep;
			ed.apk = unaligned_apk;
			err = export_project_files(p_preset, ignore_apk_file, &ed, save_apk_so);
		} else {
			//all files

			if (apk_expansion) {

				String apkfname = "main." + itos(version_code) + "." + get_package_name(package_name) + ".obb";
				String fullpath = p_path.get_base_dir().plus_file(apkfname);
				err = save_pack(p_preset, fullpath);

				if (err != OK) {
					unzClose(pkg);
					EditorNode::add_io_error("Could not write expansion package file: " + apkfname);

					CLEANUP_AND_RETURN(ERR_SKIP);
				}

				cl.push_back("--use_apk_expansion");
				cl.push_back("--apk_expansion_md5");
				cl.push_back(FileAccess::get_md5(fullpath));
				cl.push_back("--apk_expansion_key");
				cl.push_back(apk_expansion_pkey.strip_edges());

			} else {

				APKExportData ed;
				ed.ep = &ep;
				ed.apk = unaligned_apk;

				err = export_project_files(p_preset, save_apk_file, &ed, save_apk_so);
			}
		}

		int xr_mode_index = p_preset->get("xr_features/xr_mode");
		if (xr_mode_index == 1 /* XRMode.OVR */) {
			cl.push_back("--xr_mode_ovr");
		} else {
			// XRMode.REGULAR is the default.
			cl.push_back("--xr_mode_regular");
		}

		if (use_32_fb)
			cl.push_back("--use_depth_32");

		if (immersive)
			cl.push_back("--use_immersive");

		if (debug_opengl)
			cl.push_back("--debug_opengl");

		if (cl.size()) {
			//add comandline
			Vector<uint8_t> clf;
			clf.resize(4);
			encode_uint32(cl.size(), &clf.write[0]);
			for (int i = 0; i < cl.size(); i++) {

				print_line(itos(i) + " param: " + cl[i]);
				CharString txt = cl[i].utf8();
				int base = clf.size();
				int length = txt.length();
				if (!length)
					continue;
				clf.resize(base + 4 + length);
				encode_uint32(length, &clf.write[base]);
				copymem(&clf.write[base + 4], txt.ptr(), length);
			}

			zip_fileinfo zipfi = get_zip_fileinfo();

			zipOpenNewFileInZip(unaligned_apk,
					"assets/_cl_",
					&zipfi,
					NULL,
					0,
					NULL,
					0,
					NULL,
					0, // No compress (little size gain and potentially slower startup)
					Z_DEFAULT_COMPRESSION);

			zipWriteInFileInZip(unaligned_apk, clf.ptr(), clf.size());
			zipCloseFileInZip(unaligned_apk);
		}

		zipClose(unaligned_apk, NULL);
		unzClose(pkg);

		if (err != OK) {
			CLEANUP_AND_RETURN(err);
		}

		if (_signed) {

			String jarsigner = EditorSettings::get_singleton()->get("export/android/jarsigner");
			if (!FileAccess::exists(jarsigner)) {
				EditorNode::add_io_error("'jarsigner' could not be found.\nPlease supply a path in the Editor Settings.\nThe resulting APK is unsigned.");
				CLEANUP_AND_RETURN(OK);
			}

			String keystore;
			String password;
			String user;
			if (p_debug) {

				keystore = p_preset->get("keystore/debug");
				password = p_preset->get("keystore/debug_password");
				user = p_preset->get("keystore/debug_user");

				if (keystore.empty()) {

					keystore = EditorSettings::get_singleton()->get("export/android/debug_keystore");
					password = EditorSettings::get_singleton()->get("export/android/debug_keystore_pass");
					user = EditorSettings::get_singleton()->get("export/android/debug_keystore_user");
				}

				if (ep.step("Signing debug APK...", 103)) {
					CLEANUP_AND_RETURN(ERR_SKIP);
				}

			} else {
				keystore = release_keystore;
				password = release_password;
				user = release_username;

				if (ep.step("Signing release APK...", 103)) {
					CLEANUP_AND_RETURN(ERR_SKIP);
				}
			}

			if (!FileAccess::exists(keystore)) {
				EditorNode::add_io_error("Could not find keystore, unable to export.");
				CLEANUP_AND_RETURN(ERR_FILE_CANT_OPEN);
			}

			List<String> args;
			args.push_back("-digestalg");
			args.push_back("SHA-256");
			args.push_back("-sigalg");
			args.push_back("SHA256withRSA");
			String tsa_url = EditorSettings::get_singleton()->get("export/android/timestamping_authority_url");
			if (tsa_url != "") {
				args.push_back("-tsa");
				args.push_back(tsa_url);
			}
			args.push_back("-verbose");
			args.push_back("-keystore");
			args.push_back(keystore);
			args.push_back("-storepass");
			args.push_back(password);
			args.push_back(tmp_unaligned_path);
			args.push_back(user);
			int retval;
			OS::get_singleton()->execute(jarsigner, args, true, NULL, NULL, &retval);
			if (retval) {
				EditorNode::add_io_error("'jarsigner' returned with error #" + itos(retval));
				CLEANUP_AND_RETURN(ERR_CANT_CREATE);
			}

			if (ep.step("Verifying APK...", 104)) {
				CLEANUP_AND_RETURN(ERR_SKIP);
			}

			args.clear();
			args.push_back("-verify");
			args.push_back("-keystore");
			args.push_back(keystore);
			args.push_back(tmp_unaligned_path);
			args.push_back("-verbose");

			OS::get_singleton()->execute(jarsigner, args, true, NULL, NULL, &retval);
			if (retval) {
				EditorNode::add_io_error("'jarsigner' verification of APK failed. Make sure to use a jarsigner from OpenJDK 8.");
				CLEANUP_AND_RETURN(ERR_CANT_CREATE);
			}
		}

		// Let's zip-align (must be done after signing)

		static const int ZIP_ALIGNMENT = 4;

		if (ep.step("Aligning APK...", 105)) {
			CLEANUP_AND_RETURN(ERR_SKIP);
		}

		unzFile tmp_unaligned = unzOpen2(tmp_unaligned_path.utf8().get_data(), &io);
		if (!tmp_unaligned) {

			EditorNode::add_io_error("Could not unzip temporary unaligned APK.");
			CLEANUP_AND_RETURN(ERR_FILE_NOT_FOUND);
		}

		ret = unzGoToFirstFile(tmp_unaligned);

		io2 = io;
		dst_f = NULL;
		io2.opaque = &dst_f;
		zipFile final_apk = zipOpen2(p_path.utf8().get_data(), APPEND_STATUS_CREATE, NULL, &io2);

		// Take files from the unaligned APK and write them out to the aligned one
		// in raw mode, i.e. not uncompressing and recompressing, aligning them as needed,
		// following what is done in https://github.com/android/platform_build/blob/master/tools/zipalign/ZipAlign.cpp
		int bias = 0;
		while (ret == UNZ_OK) {

			unz_file_info info;
			memset(&info, 0, sizeof(info));

			char fname[16384];
			char extra[16384];
			ret = unzGetCurrentFileInfo(tmp_unaligned, &info, fname, 16384, extra, 16384 - ZIP_ALIGNMENT, NULL, 0);

			String file = fname;

			Vector<uint8_t> data;
			data.resize(info.compressed_size);

			// read
			int method, level;
			unzOpenCurrentFile2(tmp_unaligned, &method, &level, 1); // raw read
			long file_offset = unzGetCurrentFileZStreamPos64(tmp_unaligned);
			unzReadCurrentFile(tmp_unaligned, data.ptrw(), data.size());
			unzCloseCurrentFile(tmp_unaligned);

			// align
			int padding = 0;
			if (!info.compression_method) {
				// Uncompressed file => Align
				long new_offset = file_offset + bias;
				padding = (ZIP_ALIGNMENT - (new_offset % ZIP_ALIGNMENT)) % ZIP_ALIGNMENT;
			}

			memset(extra + info.size_file_extra, 0, padding);

			// write
			zip_fileinfo zipfi = get_zip_fileinfo();

			zipOpenNewFileInZip2(final_apk,
					file.utf8().get_data(),
					&zipfi,
					extra,
					info.size_file_extra + padding,
					NULL,
					0,
					NULL,
					method,
					level,
					1); // raw write
			zipWriteInFileInZip(final_apk, data.ptr(), data.size());
			zipCloseFileInZipRaw(final_apk, info.uncompressed_size, info.crc);

			bias += padding;

			ret = unzGoToNextFile(tmp_unaligned);
		}

		zipClose(final_apk, NULL);
		unzClose(tmp_unaligned);

		CLEANUP_AND_RETURN(OK);
	}

	virtual void get_platform_features(List<String> *r_features) {

		r_features->push_back("mobile");
		r_features->push_back("Android");
	}

	virtual void resolve_platform_feature_priorities(const Ref<EditorExportPreset> &p_preset, Set<String> &p_features) {
	}

	EditorExportPlatformAndroid() {

		Ref<Image> img = memnew(Image(_android_logo));
		logo.instance();
		logo->create_from_image(img);

		img = Ref<Image>(memnew(Image(_android_run_icon)));
		run_icon.instance();
		run_icon->create_from_image(img);

		device_lock = Mutex::create();
		devices_changed = true;

		plugins_lock = Mutex::create();
		plugins_changed = true;
		quit_request = false;
		check_for_changes_thread = Thread::create(_check_for_changes_poll_thread, this);
	}

	~EditorExportPlatformAndroid() {
		quit_request = true;
		Thread::wait_to_finish(check_for_changes_thread);
		memdelete(plugins_lock);
		memdelete(device_lock);
		memdelete(check_for_changes_thread);
	}
};

void register_android_exporter() {

	String exe_ext;
	if (OS::get_singleton()->get_name() == "Windows") {
		exe_ext = "*.exe";
	}

	EDITOR_DEF("export/android/adb", "");
	EditorSettings::get_singleton()->add_property_hint(PropertyInfo(Variant::STRING, "export/android/adb", PROPERTY_HINT_GLOBAL_FILE, exe_ext));
	EDITOR_DEF("export/android/jarsigner", "");
	EditorSettings::get_singleton()->add_property_hint(PropertyInfo(Variant::STRING, "export/android/jarsigner", PROPERTY_HINT_GLOBAL_FILE, exe_ext));
	EDITOR_DEF("export/android/debug_keystore", "");
	EditorSettings::get_singleton()->add_property_hint(PropertyInfo(Variant::STRING, "export/android/debug_keystore", PROPERTY_HINT_GLOBAL_FILE, "*.keystore"));
	EDITOR_DEF("export/android/debug_keystore_user", "androiddebugkey");
	EDITOR_DEF("export/android/debug_keystore_pass", "android");
	EDITOR_DEF("export/android/force_system_user", false);
	EDITOR_DEF("export/android/custom_build_sdk_path", "");
	EditorSettings::get_singleton()->add_property_hint(PropertyInfo(Variant::STRING, "export/android/custom_build_sdk_path", PROPERTY_HINT_GLOBAL_DIR));

	EDITOR_DEF("export/android/timestamping_authority_url", "");
	EDITOR_DEF("export/android/shutdown_adb_on_exit", true);

	Ref<EditorExportPlatformAndroid> exporter = Ref<EditorExportPlatformAndroid>(memnew(EditorExportPlatformAndroid));
	EditorExport::get_singleton()->add_export_platform(exporter);
}
