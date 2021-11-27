/*************************************************************************/
/*  register_types.cpp                                                   */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2021 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2021 Godot Engine contributors (cf. AUTHORS.md).   */
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

#include "register_types.h"

#include "editor/editor_node.h"
#include "editor_scene_exporter_gltf_plugin.h"
#include "editor_scene_importer_gltf.h"
#include "gltf_accessor.h"
#include "gltf_animation.h"
#include "gltf_buffer_view.h"
#include "gltf_camera.h"
#include "gltf_document.h"
#include "gltf_light.h"
#include "gltf_mesh.h"
#include "gltf_node.h"
#include "gltf_skeleton.h"
#include "gltf_skin.h"
#include "gltf_spec_gloss.h"
#include "gltf_state.h"
#include "gltf_texture.h"

#ifndef _3D_DISABLED
#ifdef TOOLS_ENABLED
static void _editor_init() {
	Ref<EditorSceneImporterGLTF> import_gltf;
	import_gltf.instance();
	ResourceImporterScene::get_singleton()->add_importer(import_gltf);
}
#endif
#endif

void register_gltf_types() {
#ifndef _3D_DISABLED
#ifdef TOOLS_ENABLED
	ClassDB::APIType prev_api = ClassDB::get_current_api();
	ClassDB::set_current_api(ClassDB::API_EDITOR);
	ClassDB::register_class<EditorSceneImporterGLTF>();
	ClassDB::register_class<GLTFMesh>();
	EditorPlugins::add_by_type<SceneExporterGLTFPlugin>();
	ClassDB::set_current_api(prev_api);
	EditorNode::add_init_callback(_editor_init);
#endif
	ClassDB::register_class<GLTFSpecGloss>();
	ClassDB::register_class<GLTFNode>();
	ClassDB::register_class<GLTFAnimation>();
	ClassDB::register_class<GLTFBufferView>();
	ClassDB::register_class<GLTFAccessor>();
	ClassDB::register_class<GLTFTexture>();
	ClassDB::register_class<GLTFSkeleton>();
	ClassDB::register_class<GLTFSkin>();
	ClassDB::register_class<GLTFCamera>();
	ClassDB::register_class<GLTFLight>();
	ClassDB::register_class<GLTFState>();
	ClassDB::register_class<GLTFDocument>();
	ClassDB::register_class<PackedSceneGLTF>();
#endif
}

void unregister_gltf_types() {
}
