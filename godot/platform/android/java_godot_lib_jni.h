/*************************************************************************/
/*  java_godot_lib_jni.h                                                 */
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

#ifndef JAVA_GODOT_LIB_JNI_H
#define JAVA_GODOT_LIB_JNI_H

#include <android/log.h>
#include <jni.h>

// These functions can be called from within JAVA and are the means by which our JAVA implementation calls back into our C++ code.
// See java/src/org/godotengine/godot/GodotLib.java for the JAVA side of this (yes that's why we have the long names)
extern "C" {
JNIEXPORT void JNICALL Java_org_godotengine_godot_GodotLib_initialize(JNIEnv *env, jclass clazz, jobject activity, jobject p_asset_manager, jboolean p_use_apk_expansion);
JNIEXPORT void JNICALL Java_org_godotengine_godot_GodotLib_ondestroy(JNIEnv *env, jclass clazz, jobject activity);
JNIEXPORT void JNICALL Java_org_godotengine_godot_GodotLib_setup(JNIEnv *env, jclass clazz, jobjectArray p_cmdline);
JNIEXPORT void JNICALL Java_org_godotengine_godot_GodotLib_resize(JNIEnv *env, jclass clazz, jint width, jint height);
JNIEXPORT void JNICALL Java_org_godotengine_godot_GodotLib_newcontext(JNIEnv *env, jclass clazz, jboolean p_32_bits);
JNIEXPORT void JNICALL Java_org_godotengine_godot_GodotLib_step(JNIEnv *env, jclass clazz);
JNIEXPORT void JNICALL Java_org_godotengine_godot_GodotLib_back(JNIEnv *env, jclass clazz);
JNIEXPORT void JNICALL Java_org_godotengine_godot_GodotLib_touch(JNIEnv *env, jclass clazz, jint ev, jint pointer, jint count, jintArray positions);
JNIEXPORT void JNICALL Java_org_godotengine_godot_GodotLib_hover(JNIEnv *env, jclass clazz, jint p_type, jint p_x, jint p_y);
JNIEXPORT void JNICALL Java_org_godotengine_godot_GodotLib_doubletap(JNIEnv *env, jclass clazz, jint p_x, jint p_y);
JNIEXPORT void JNICALL Java_org_godotengine_godot_GodotLib_scroll(JNIEnv *env, jclass clazz, jint p_x, jint p_y);
JNIEXPORT void JNICALL Java_org_godotengine_godot_GodotLib_key(JNIEnv *env, jclass clazz, jint p_scancode, jint p_unicode_char, jboolean p_pressed);
JNIEXPORT void JNICALL Java_org_godotengine_godot_GodotLib_joybutton(JNIEnv *env, jclass clazz, jint p_device, jint p_button, jboolean p_pressed);
JNIEXPORT void JNICALL Java_org_godotengine_godot_GodotLib_joyaxis(JNIEnv *env, jclass clazz, jint p_device, jint p_axis, jfloat p_value);
JNIEXPORT void JNICALL Java_org_godotengine_godot_GodotLib_joyhat(JNIEnv *env, jclass clazz, jint p_device, jint p_hat_x, jint p_hat_y);
JNIEXPORT void JNICALL Java_org_godotengine_godot_GodotLib_joyconnectionchanged(JNIEnv *env, jclass clazz, jint p_device, jboolean p_connected, jstring p_name);
JNIEXPORT void JNICALL Java_org_godotengine_godot_GodotLib_audio(JNIEnv *env, jclass clazz);
JNIEXPORT void JNICALL Java_org_godotengine_godot_GodotLib_accelerometer(JNIEnv *env, jclass clazz, jfloat x, jfloat y, jfloat z);
JNIEXPORT void JNICALL Java_org_godotengine_godot_GodotLib_gravity(JNIEnv *env, jclass clazz, jfloat x, jfloat y, jfloat z);
JNIEXPORT void JNICALL Java_org_godotengine_godot_GodotLib_magnetometer(JNIEnv *env, jclass clazz, jfloat x, jfloat y, jfloat z);
JNIEXPORT void JNICALL Java_org_godotengine_godot_GodotLib_gyroscope(JNIEnv *env, jclass clazz, jfloat x, jfloat y, jfloat z);
JNIEXPORT void JNICALL Java_org_godotengine_godot_GodotLib_focusin(JNIEnv *env, jclass clazz);
JNIEXPORT void JNICALL Java_org_godotengine_godot_GodotLib_focusout(JNIEnv *env, jclass clazz);
JNIEXPORT jstring JNICALL Java_org_godotengine_godot_GodotLib_getGlobal(JNIEnv *env, jclass clazz, jstring path);
JNIEXPORT void JNICALL Java_org_godotengine_godot_GodotLib_callobject(JNIEnv *env, jclass clazz, jlong ID, jstring method, jobjectArray params);
JNIEXPORT void JNICALL Java_org_godotengine_godot_GodotLib_calldeferred(JNIEnv *env, jclass clazz, jlong ID, jstring method, jobjectArray params);
JNIEXPORT void JNICALL Java_org_godotengine_godot_GodotLib_setVirtualKeyboardHeight(JNIEnv *env, jclass clazz, jint p_height);
JNIEXPORT void JNICALL Java_org_godotengine_godot_GodotLib_requestPermissionResult(JNIEnv *env, jclass clazz, jstring p_permission, jboolean p_result);
JNIEXPORT void JNICALL Java_org_godotengine_godot_GodotLib_onRendererResumed(JNIEnv *env, jclass clazz);
JNIEXPORT void JNICALL Java_org_godotengine_godot_GodotLib_onRendererPaused(JNIEnv *env, jclass clazz);
}

#endif /* !JAVA_GODOT_LIB_JNI_H */
