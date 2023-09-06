/**************************************************************************/
/*  gradle_export_util.cpp                                                */
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

#include "gradle_export_util.h"

#include "core/config/project_settings.h"

int _get_android_orientation_value(DisplayServer::ScreenOrientation screen_orientation) {
	switch (screen_orientation) {
		case DisplayServer::SCREEN_PORTRAIT:
			return 1;
		case DisplayServer::SCREEN_REVERSE_LANDSCAPE:
			return 8;
		case DisplayServer::SCREEN_REVERSE_PORTRAIT:
			return 9;
		case DisplayServer::SCREEN_SENSOR_LANDSCAPE:
			return 11;
		case DisplayServer::SCREEN_SENSOR_PORTRAIT:
			return 12;
		case DisplayServer::SCREEN_SENSOR:
			return 13;
		case DisplayServer::SCREEN_LANDSCAPE:
		default:
			return 0;
	}
}

String _get_android_orientation_label(DisplayServer::ScreenOrientation screen_orientation) {
	switch (screen_orientation) {
		case DisplayServer::SCREEN_PORTRAIT:
			return "portrait";
		case DisplayServer::SCREEN_REVERSE_LANDSCAPE:
			return "reverseLandscape";
		case DisplayServer::SCREEN_REVERSE_PORTRAIT:
			return "reversePortrait";
		case DisplayServer::SCREEN_SENSOR_LANDSCAPE:
			return "userLandscape";
		case DisplayServer::SCREEN_SENSOR_PORTRAIT:
			return "userPortrait";
		case DisplayServer::SCREEN_SENSOR:
			return "fullUser";
		case DisplayServer::SCREEN_LANDSCAPE:
		default:
			return "landscape";
	}
}

int _get_app_category_value(int category_index) {
	switch (category_index) {
		case APP_CATEGORY_ACCESSIBILITY:
			return 8;
		case APP_CATEGORY_AUDIO:
			return 1;
		case APP_CATEGORY_IMAGE:
			return 3;
		case APP_CATEGORY_MAPS:
			return 6;
		case APP_CATEGORY_NEWS:
			return 5;
		case APP_CATEGORY_PRODUCTIVITY:
			return 7;
		case APP_CATEGORY_SOCIAL:
			return 4;
		case APP_CATEGORY_VIDEO:
			return 2;
		case APP_CATEGORY_GAME:
		default:
			return 0;
	}
}

String _get_app_category_label(int category_index) {
	switch (category_index) {
		case APP_CATEGORY_ACCESSIBILITY:
			return "accessibility";
		case APP_CATEGORY_AUDIO:
			return "audio";
		case APP_CATEGORY_IMAGE:
			return "image";
		case APP_CATEGORY_MAPS:
			return "maps";
		case APP_CATEGORY_NEWS:
			return "news";
		case APP_CATEGORY_PRODUCTIVITY:
			return "productivity";
		case APP_CATEGORY_SOCIAL:
			return "social";
		case APP_CATEGORY_VIDEO:
			return "video";
		case APP_CATEGORY_GAME:
		default:
			return "game";
	}
}

// Utility method used to create a directory.
Error create_directory(const String &p_dir) {
	if (!DirAccess::exists(p_dir)) {
		Ref<DirAccess> filesystem_da = DirAccess::create(DirAccess::ACCESS_RESOURCES);
		ERR_FAIL_COND_V_MSG(filesystem_da.is_null(), ERR_CANT_CREATE, "Cannot create directory '" + p_dir + "'.");
		Error err = filesystem_da->make_dir_recursive(p_dir);
		ERR_FAIL_COND_V_MSG(err, ERR_CANT_CREATE, "Cannot create directory '" + p_dir + "'.");
	}
	return OK;
}

// Writes p_data into a file at p_path, creating directories if necessary.
// Note: this will overwrite the file at p_path if it already exists.
Error store_file_at_path(const String &p_path, const Vector<uint8_t> &p_data) {
	String dir = p_path.get_base_dir();
	Error err = create_directory(dir);
	if (err != OK) {
		return err;
	}
	Ref<FileAccess> fa = FileAccess::open(p_path, FileAccess::WRITE);
	ERR_FAIL_COND_V_MSG(fa.is_null(), ERR_CANT_CREATE, "Cannot create file '" + p_path + "'.");
	fa->store_buffer(p_data.ptr(), p_data.size());
	return OK;
}

// Writes string p_data into a file at p_path, creating directories if necessary.
// Note: this will overwrite the file at p_path if it already exists.
Error store_string_at_path(const String &p_path, const String &p_data) {
	String dir = p_path.get_base_dir();
	Error err = create_directory(dir);
	if (err != OK) {
		if (OS::get_singleton()->is_stdout_verbose()) {
			print_error("Unable to write data into " + p_path);
		}
		return err;
	}
	Ref<FileAccess> fa = FileAccess::open(p_path, FileAccess::WRITE);
	ERR_FAIL_COND_V_MSG(fa.is_null(), ERR_CANT_CREATE, "Cannot create file '" + p_path + "'.");
	fa->store_string(p_data);
	return OK;
}

// Implementation of EditorExportSaveFunction.
// This method will only be called as an input to export_project_files.
// It is used by the export_project_files method to save all the asset files into the gradle project.
// It's functionality mirrors that of the method save_apk_file.
// This method will be called ONLY when gradle build is enabled.
Error rename_and_store_file_in_gradle_project(void *p_userdata, const String &p_path, const Vector<uint8_t> &p_data, int p_file, int p_total, const Vector<String> &p_enc_in_filters, const Vector<String> &p_enc_ex_filters, const Vector<uint8_t> &p_key) {
	CustomExportData *export_data = static_cast<CustomExportData *>(p_userdata);
	String dst_path = p_path.replace_first("res://", export_data->assets_directory + "/");
	print_verbose("Saving project files from " + p_path + " into " + dst_path);
	Error err = store_file_at_path(dst_path, p_data);
	return err;
}

String _android_xml_escape(const String &p_string) {
	// Android XML requires strings to be both valid XML (`xml_escape()`) but also
	// to escape characters which are valid XML but have special meaning in Android XML.
	// https://developer.android.com/guide/topics/resources/string-resource.html#FormattingAndStyling
	// Note: Didn't handle U+XXXX unicode chars, could be done if needed.
	return p_string
			.replace("@", "\\@")
			.replace("?", "\\?")
			.replace("'", "\\'")
			.replace("\"", "\\\"")
			.replace("\n", "\\n")
			.replace("\t", "\\t")
			.xml_escape(false);
}

// Creates strings.xml files inside the gradle project for different locales.
Error _create_project_name_strings_files(const Ref<EditorExportPreset> &p_preset, const String &project_name) {
	print_verbose("Creating strings resources for supported locales for project " + project_name);
	// Stores the string into the default values directory.
	String processed_default_xml_string = vformat(godot_project_name_xml_string, _android_xml_escape(project_name));
	store_string_at_path("res://android/build/res/values/godot_project_name_string.xml", processed_default_xml_string);

	// Searches the Gradle project res/ directory to find all supported locales
	Ref<DirAccess> da = DirAccess::open("res://android/build/res");
	if (da.is_null()) {
		if (OS::get_singleton()->is_stdout_verbose()) {
			print_error("Unable to open Android resources directory.");
		}
		return ERR_CANT_OPEN;
	}
	da->list_dir_begin();
	Dictionary appnames = GLOBAL_GET("application/config/name_localized");
	while (true) {
		String file = da->get_next();
		if (file.is_empty()) {
			break;
		}
		if (!file.begins_with("values-")) {
			// NOTE: This assumes all directories that start with "values-" are for localization.
			continue;
		}
		String locale = file.replace("values-", "").replace("-r", "_");
		String locale_directory = "res://android/build/res/" + file + "/godot_project_name_string.xml";
		if (appnames.has(locale)) {
			String locale_project_name = appnames[locale];
			String processed_xml_string = vformat(godot_project_name_xml_string, _android_xml_escape(locale_project_name));
			print_verbose("Storing project name for locale " + locale + " under " + locale_directory);
			store_string_at_path(locale_directory, processed_xml_string);
		} else {
			// TODO: Once the legacy build system is deprecated we don't need to have xml files for this else branch
			store_string_at_path(locale_directory, processed_default_xml_string);
		}
	}
	da->list_dir_end();
	return OK;
}

String bool_to_string(bool v) {
	return v ? "true" : "false";
}

String _get_gles_tag() {
	return "    <uses-feature android:glEsVersion=\"0x00030000\" android:required=\"true\" />\n";
}

String _get_screen_sizes_tag(const Ref<EditorExportPreset> &p_preset) {
	String manifest_screen_sizes = "    <supports-screens \n        tools:node=\"replace\"";
	String sizes[] = { "small", "normal", "large", "xlarge" };
	size_t num_sizes = sizeof(sizes) / sizeof(sizes[0]);
	for (size_t i = 0; i < num_sizes; i++) {
		String feature_name = vformat("screen/support_%s", sizes[i]);
		String feature_support = bool_to_string(p_preset->get(feature_name));
		String xml_entry = vformat("\n        android:%sScreens=\"%s\"", sizes[i], feature_support);
		manifest_screen_sizes += xml_entry;
	}
	manifest_screen_sizes += " />\n";
	return manifest_screen_sizes;
}

String _get_xr_features_tag(const Ref<EditorExportPreset> &p_preset, bool p_uses_vulkan) {
	String manifest_xr_features;
	int xr_mode_index = (int)(p_preset->get("xr_features/xr_mode"));
	bool uses_xr = xr_mode_index == XR_MODE_OPENXR;
	if (uses_xr) {
		int hand_tracking_index = p_preset->get("xr_features/hand_tracking"); // 0: none, 1: optional, 2: required
		if (hand_tracking_index == XR_HAND_TRACKING_OPTIONAL) {
			manifest_xr_features += "    <uses-feature tools:node=\"replace\" android:name=\"oculus.software.handtracking\" android:required=\"false\" />\n";
		} else if (hand_tracking_index == XR_HAND_TRACKING_REQUIRED) {
			manifest_xr_features += "    <uses-feature tools:node=\"replace\" android:name=\"oculus.software.handtracking\" android:required=\"true\" />\n";
		}

		int passthrough_mode = p_preset->get("xr_features/passthrough");
		if (passthrough_mode == XR_PASSTHROUGH_OPTIONAL) {
			manifest_xr_features += "    <uses-feature tools:node=\"replace\" android:name=\"com.oculus.feature.PASSTHROUGH\" android:required=\"false\" />\n";
		} else if (passthrough_mode == XR_PASSTHROUGH_REQUIRED) {
			manifest_xr_features += "    <uses-feature tools:node=\"replace\" android:name=\"com.oculus.feature.PASSTHROUGH\" android:required=\"true\" />\n";
		}
	}

	if (p_uses_vulkan) {
		manifest_xr_features += "    <uses-feature tools:node=\"replace\" android:name=\"android.hardware.vulkan.level\" android:required=\"false\" android:version=\"1\" />\n";
		manifest_xr_features += "    <uses-feature tools:node=\"replace\" android:name=\"android.hardware.vulkan.version\" android:required=\"true\" android:version=\"0x400003\" />\n";
	}
	return manifest_xr_features;
}

String _get_activity_tag(const Ref<EditorExportPreset> &p_preset, bool p_uses_xr) {
	String orientation = _get_android_orientation_label(DisplayServer::ScreenOrientation(int(GLOBAL_GET("display/window/handheld/orientation"))));
	String manifest_activity_text = vformat(
			"        <activity android:name=\"com.godot.game.GodotApp\" "
			"tools:replace=\"android:screenOrientation,android:excludeFromRecents,android:resizeableActivity\" "
			"tools:node=\"mergeOnlyAttributes\" "
			"android:excludeFromRecents=\"%s\" "
			"android:screenOrientation=\"%s\" "
			"android:resizeableActivity=\"%s\">\n",
			bool_to_string(p_preset->get("package/exclude_from_recents")),
			orientation,
			bool_to_string(bool(GLOBAL_GET("display/window/size/resizable"))));

	if (p_uses_xr) {
		manifest_activity_text += "            <intent-filter>\n"
								  "                <action android:name=\"android.intent.action.MAIN\" />\n"
								  "                <category android:name=\"android.intent.category.LAUNCHER\" />\n"
								  "\n"
								  "                <!-- Enable access to OpenXR on Oculus mobile devices, no-op on other Android\n"
								  "                platforms. -->\n"
								  "                <category android:name=\"com.oculus.intent.category.VR\" />\n"
								  "\n"
								  "                <!-- OpenXR category tag to indicate the activity starts in an immersive OpenXR mode. \n"
								  "                See https://registry.khronos.org/OpenXR/specs/1.0/html/xrspec.html#android-runtime-category. -->\n"
								  "                <category android:name=\"org.khronos.openxr.intent.category.IMMERSIVE_HMD\" />\n"
								  "\n"
								  "                <!-- Enable VR access on HTC Vive Focus devices. -->\n"
								  "                <category android:name=\"com.htc.intent.category.VRAPP\" />\n"
								  "            </intent-filter>\n";
	} else {
		manifest_activity_text += "            <intent-filter>\n"
								  "                <action android:name=\"android.intent.action.MAIN\" />\n"
								  "                <category android:name=\"android.intent.category.LAUNCHER\" />\n"
								  "            </intent-filter>\n";
	}

	manifest_activity_text += "        </activity>\n";
	return manifest_activity_text;
}

String _get_application_tag(const Ref<EditorExportPreset> &p_preset, bool p_has_read_write_storage_permission) {
	int app_category_index = (int)(p_preset->get("package/app_category"));
	bool is_game = app_category_index == APP_CATEGORY_GAME;

	int xr_mode_index = (int)(p_preset->get("xr_features/xr_mode"));
	bool uses_xr = xr_mode_index == XR_MODE_OPENXR;

	String manifest_application_text = vformat(
			"    <application android:label=\"@string/godot_project_name_string\"\n"
			"        android:allowBackup=\"%s\"\n"
			"        android:icon=\"@mipmap/icon\"\n"
			"        android:appCategory=\"%s\"\n"
			"        android:isGame=\"%s\"\n"
			"        android:hasFragileUserData=\"%s\"\n"
			"        android:requestLegacyExternalStorage=\"%s\"\n"
			"        tools:replace=\"android:allowBackup,android:appCategory,android:isGame,android:hasFragileUserData,android:requestLegacyExternalStorage\"\n"
			"        tools:ignore=\"GoogleAppIndexingWarning\">\n\n",
			bool_to_string(p_preset->get("user_data_backup/allow")),
			_get_app_category_label(app_category_index),
			bool_to_string(is_game),
			bool_to_string(p_preset->get("package/retain_data_on_uninstall")),
			bool_to_string(p_has_read_write_storage_permission));

	if (uses_xr) {
		bool hand_tracking_enabled = (int)(p_preset->get("xr_features/hand_tracking")) > XR_HAND_TRACKING_NONE;
		if (hand_tracking_enabled) {
			int hand_tracking_frequency_index = p_preset->get("xr_features/hand_tracking_frequency");
			String hand_tracking_frequency = hand_tracking_frequency_index == XR_HAND_TRACKING_FREQUENCY_LOW ? "LOW" : "HIGH";
			manifest_application_text += vformat(
					"        <meta-data tools:node=\"replace\" android:name=\"com.oculus.handtracking.frequency\" android:value=\"%s\" />\n",
					hand_tracking_frequency);
			manifest_application_text += "        <meta-data tools:node=\"replace\" android:name=\"com.oculus.handtracking.version\" android:value=\"V2.0\" />\n";
		}
	}
	manifest_application_text += _get_activity_tag(p_preset, uses_xr);
	manifest_application_text += "    </application>\n";
	return manifest_application_text;
}
