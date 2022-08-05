/*************************************************************************/
/*  packed_scene_gltf.h                                                  */
/*************************************************************************/
/*                       This file is part of:                           */
/*                           GODOT ENGINE                                */
/*                      https://godotengine.org                          */
/*************************************************************************/
/* Copyright (c) 2007-2022 Juan Linietsky, Ariel Manzur.                 */
/* Copyright (c) 2014-2022 Godot Engine contributors (cf. AUTHORS.md).   */
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

#ifndef PACKED_SCENE_GLTF_H
#define PACKED_SCENE_GLTF_H

#ifdef TOOLS_ENABLED

#include "scene/main/node.h"
#include "scene/resources/packed_scene.h"

#include "gltf_state.h"

class PackedSceneGLTF : public PackedScene {
	GDCLASS(PackedSceneGLTF, PackedScene);

protected:
	static void _bind_methods();

public:
	virtual void save_scene(Node *p_node, const String &p_path, const String &p_src_path,
			uint32_t p_flags, int p_bake_fps,
			List<String> *r_missing_deps, Error *r_err = nullptr);
	virtual void _build_parent_hierachy(Ref<GLTFState> state);
	virtual Error export_gltf(Node *p_root, String p_path, int32_t p_flags = 0,
			real_t p_bake_fps = 1000.0f);
	virtual Node *import_scene(const String &p_path, uint32_t p_flags,
			int p_bake_fps, uint32_t p_compress_flags,
			List<String> *r_missing_deps,
			Error *r_err,
			Ref<GLTFState> r_state);
	virtual Node *import_gltf_scene(const String &p_path, uint32_t p_flags, float p_bake_fps, uint32_t p_compress_flags, Ref<GLTFState> r_state = Ref<GLTFState>());
	virtual void pack_gltf(String p_path, int32_t p_flags = 0,
			real_t p_bake_fps = 1000.0f, uint32_t p_compress_flags = Mesh::ARRAY_COMPRESS_DEFAULT, Ref<GLTFState> r_state = Ref<GLTFState>());
};

#endif // TOOLS_ENABLED

#endif // PACKED_SCENE_GLTF_H
