/*************************************************************************/
/*  pvrtc_compress.cpp                                                   */
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

#include "pvrtc_compress.h"

#include "core/io/resource_loader.h"
#include "core/io/resource_saver.h"
#include "core/os/file_access.h"
#include "core/os/os.h"
#include "editor_settings.h"
#include "scene/resources/texture.h"

static void (*_base_image_compress_pvrtc2_func)(Image *) = NULL;
static void (*_base_image_compress_pvrtc4_func)(Image *) = NULL;

static void _compress_image(Image::CompressMode p_mode, Image *p_image) {

	String ttpath = EditorSettings::get_singleton()->get("filesystem/import/pvrtc_texture_tool");

	if (ttpath.strip_edges() == "" || !FileAccess::exists(ttpath)) {
		switch (p_mode) {

			case Image::COMPRESS_PVRTC2:
				if (_base_image_compress_pvrtc2_func)
					_base_image_compress_pvrtc2_func(p_image);
				else if (_base_image_compress_pvrtc4_func)
					_base_image_compress_pvrtc4_func(p_image);

				break;
			case Image::COMPRESS_PVRTC4:
				if (_base_image_compress_pvrtc4_func)
					_base_image_compress_pvrtc4_func(p_image);

				break;
			default: ERR_FAIL();
		}
		return;
	}
	String tmppath = EditorSettings::get_singleton()->get_cache_dir();

	List<String> args;

	String src_img = tmppath.plus_file("_tmp_src_img.png");
	String dst_img = tmppath.plus_file("_tmp_dst_img.pvr");

	args.push_back("-i");
	args.push_back(src_img);
	args.push_back("-o");
	args.push_back(dst_img);
	args.push_back("-f");
	switch (p_mode) {

		case Image::COMPRESS_PVRTC2: args.push_back("PVRTC2"); break;
		case Image::COMPRESS_PVRTC4: args.push_back("PVRTC4"); break;
		case Image::COMPRESS_ETC: args.push_back("ETC"); break;
		default: ERR_FAIL();
	}

	if (EditorSettings::get_singleton()->get("filesystem/import/pvrtc_fast_conversion").operator bool()) {
		args.push_back("-pvrtcfast");
	}
	if (p_image->has_mipmaps())
		args.push_back("-m");

	Ref<ImageTexture> t = memnew(ImageTexture);
	t->create_from_image(Ref<Image>(p_image), 0);
	ResourceSaver::save(src_img, t);

	Error err = OS::get_singleton()->execute(ttpath, args, true);
	ERR_EXPLAIN(TTR("Could not execute PVRTC tool:") + " " + ttpath);
	ERR_FAIL_COND(err != OK);

	t = ResourceLoader::load(dst_img, "Texture");

	ERR_EXPLAIN(TTR("Can't load back converted image using PVRTC tool:") + " " + dst_img);
	ERR_FAIL_COND(t.is_null());

	p_image->copy_internals_from(t->get_data());
}

static void _compress_pvrtc2(Image *p_image) {

	_compress_image(Image::COMPRESS_PVRTC2, p_image);
}

static void _compress_pvrtc4(Image *p_image) {

	_compress_image(Image::COMPRESS_PVRTC4, p_image);
}

void _pvrtc_register_compressors() {

	_base_image_compress_pvrtc2_func = Image::_image_compress_pvrtc2_func;
	_base_image_compress_pvrtc4_func = Image::_image_compress_pvrtc4_func;

	Image::_image_compress_pvrtc2_func = _compress_pvrtc2;
	Image::_image_compress_pvrtc4_func = _compress_pvrtc4;
}
