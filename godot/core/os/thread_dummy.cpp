/*************************************************************************/
/*  thread_dummy.cpp                                                     */
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

#include "thread_dummy.h"

#include "core/os/memory.h"

Thread *ThreadDummy::create(ThreadCreateCallback p_callback, void *p_user, const Thread::Settings &p_settings) {
	return memnew(ThreadDummy);
};

void ThreadDummy::make_default() {
	Thread::create_func = &ThreadDummy::create;
};

Mutex *MutexDummy::create(bool p_recursive) {
	return memnew(MutexDummy);
};

void MutexDummy::make_default() {
	Mutex::create_func = &MutexDummy::create;
};

Semaphore *SemaphoreDummy::create() {
	return memnew(SemaphoreDummy);
};

void SemaphoreDummy::make_default() {
	Semaphore::create_func = &SemaphoreDummy::create;
};

RWLock *RWLockDummy::create() {
	return memnew(RWLockDummy);
};

void RWLockDummy::make_default() {
	RWLock::create_func = &RWLockDummy::create;
};
