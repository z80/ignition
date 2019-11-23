/*************************************************************************/
/*  android_gdn.cpp                                                      */
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

#include "modules/gdnative/gdnative.h"

// Code by Paritosh97 with minor tweaks by Mux213
// These entry points are only for the android platform and are simple stubs in all others.

#ifdef __ANDROID__
#include "platform/android/thread_jandroid.h"
#else
#define JNIEnv void
#define jobject void *
#endif

#ifdef __cplusplus
extern "C" {
#endif

JNIEnv *GDAPI godot_android_get_env() {
#ifdef __ANDROID__
	return ThreadAndroid::get_env();
#else
	return NULL;
#endif
}

jobject GDAPI godot_android_get_activity() {
#ifdef __ANDROID__
	JNIEnv *env = ThreadAndroid::get_env();

	jclass activityThread = env->FindClass("android/app/ActivityThread");
	jmethodID currentActivityThread = env->GetStaticMethodID(activityThread, "currentActivityThread", "()Landroid/app/ActivityThread;");
	jobject at = env->CallStaticObjectMethod(activityThread, currentActivityThread);
	jmethodID getApplication = env->GetMethodID(activityThread, "getApplication", "()Landroid/app/Application;");
	jobject context = env->CallObjectMethod(at, getApplication);

	return env->NewGlobalRef(context);
#else
	return NULL;
#endif
}

#ifdef __cplusplus
}
#endif