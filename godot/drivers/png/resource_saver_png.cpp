/*************************************************************************/
/*  resource_saver_png.cpp                                               */
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

#include "resource_saver_png.h"

#include "core/image.h"
#include "core/os/file_access.h"
#include "core/project_settings.h"
#include "scene/resources/texture.h"

#include <png.h>

static void _write_png_data(png_structp png_ptr, png_bytep data, png_size_t p_length) {

	FileAccess *f = (FileAccess *)png_get_io_ptr(png_ptr);
	f->store_buffer((const uint8_t *)data, p_length);
}

Error ResourceSaverPNG::save(const String &p_path, const RES &p_resource, uint32_t p_flags) {

	Ref<ImageTexture> texture = p_resource;

	ERR_FAIL_COND_V(!texture.is_valid(), ERR_INVALID_PARAMETER);
	ERR_EXPLAIN("Can't save empty texture as PNG");
	ERR_FAIL_COND_V(!texture->get_width() || !texture->get_height(), ERR_INVALID_PARAMETER);

	Ref<Image> img = texture->get_data();

	Error err = save_image(p_path, img);

	if (err == OK) {
	}

	return err;
};

Error ResourceSaverPNG::save_image(const String &p_path, const Ref<Image> &p_img) {

	Ref<Image> img = p_img->duplicate();

	if (img->is_compressed())
		img->decompress();

	ERR_FAIL_COND_V(img->is_compressed(), ERR_INVALID_PARAMETER);

	png_structp png_ptr;
	png_infop info_ptr;
	png_bytep *row_pointers;

	/* initialize stuff */
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	ERR_FAIL_COND_V(!png_ptr, ERR_CANT_CREATE);

	info_ptr = png_create_info_struct(png_ptr);

	ERR_FAIL_COND_V(!info_ptr, ERR_CANT_CREATE);

	if (setjmp(png_jmpbuf(png_ptr))) {
		ERR_FAIL_V(ERR_CANT_OPEN);
	}
	//change this
	Error err;
	FileAccess *f = FileAccess::open(p_path, FileAccess::WRITE, &err);
	if (err) {
		ERR_FAIL_V(err);
	}

	png_set_write_fn(png_ptr, f, _write_png_data, NULL);

	/* write header */
	if (setjmp(png_jmpbuf(png_ptr))) {
		ERR_FAIL_V(ERR_CANT_OPEN);
	}

	int pngf = 0;
	int cs = 0;

	switch (img->get_format()) {

		case Image::FORMAT_L8: {

			pngf = PNG_COLOR_TYPE_GRAY;
			cs = 1;
		} break;
		case Image::FORMAT_LA8: {

			pngf = PNG_COLOR_TYPE_GRAY_ALPHA;
			cs = 2;
		} break;
		case Image::FORMAT_RGB8: {

			pngf = PNG_COLOR_TYPE_RGB;
			cs = 3;
		} break;
		case Image::FORMAT_RGBA8: {

			pngf = PNG_COLOR_TYPE_RGB_ALPHA;
			cs = 4;
		} break;
		default: {

			if (img->detect_alpha()) {

				img->convert(Image::FORMAT_RGBA8);
				pngf = PNG_COLOR_TYPE_RGB_ALPHA;
				cs = 4;
			} else {

				img->convert(Image::FORMAT_RGB8);
				pngf = PNG_COLOR_TYPE_RGB;
				cs = 3;
			}
		}
	}

	int w = img->get_width();
	int h = img->get_height();
	png_set_IHDR(png_ptr, info_ptr, w, h,
			8, pngf, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);

	/* write bytes */
	if (setjmp(png_jmpbuf(png_ptr))) {
		memdelete(f);
		ERR_FAIL_V(ERR_CANT_OPEN);
	}

	PoolVector<uint8_t>::Read r = img->get_data().read();

	row_pointers = (png_bytep *)memalloc(sizeof(png_bytep) * h);
	for (int i = 0; i < h; i++) {

		row_pointers[i] = (png_bytep)&r[i * w * cs];
	}
	png_write_image(png_ptr, row_pointers);

	memfree(row_pointers);

	/* end write */
	if (setjmp(png_jmpbuf(png_ptr))) {

		memdelete(f);
		ERR_FAIL_V(ERR_CANT_OPEN);
	}

	png_write_end(png_ptr, NULL);
	memdelete(f);

	/* cleanup heap allocation */
	png_destroy_write_struct(&png_ptr, &info_ptr);

	return OK;
}

bool ResourceSaverPNG::recognize(const RES &p_resource) const {

	return (p_resource.is_valid() && p_resource->is_class("ImageTexture"));
}
void ResourceSaverPNG::get_recognized_extensions(const RES &p_resource, List<String> *p_extensions) const {

	if (Object::cast_to<Texture>(*p_resource)) {
		p_extensions->push_back("png");
	}
}

ResourceSaverPNG::ResourceSaverPNG() {

	Image::save_png_func = &save_image;
};
