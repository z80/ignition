/**************************************************************************/
/*  register_types.cpp                                                    */
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

#include "register_types.h"

#include "bullet_physics_server.h"
#include "servers/physics_server_3d_wrap_mt.h"
#include "core/object/class_db.h"
#include "core/config/project_settings.h"

/**
	@author AndreaCatania
	FatherTed
*/

#ifndef _3D_DISABLED
static PhysicsServer3D *_createBulletPhysics3DCallback()
{
	bool using_threads = GLOBAL_GET("physics/3d/run_on_separate_thread");

	PhysicsServer3D *physics_server_3d = memnew( BulletPhysicsServer() );

	return memnew(PhysicsServer3DWrapMT(physics_server_3d, using_threads));
}

#endif

void initialize_bullet_module( ModuleInitializationLevel p_level )
{
    if (p_level != MODULE_INITIALIZATION_LEVEL_SERVERS)
        return;

#ifndef _3D_DISABLED
	PhysicsServer3DManager::get_singleton()->register_server("Bullet (port from Godot-3.5.2)", callable_mp_static(_createBulletPhysics3DCallback));
	//PhysicsServer3DManager::get_singleton()->set_default_server("BulletPhysics3D");
#endif
}

void uninitialize_bullet_module( ModuleInitializationLevel p_level )
{
}
