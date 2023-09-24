/**************************************************************************/
/*  texture.cpp                                                           */
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

#include "texture.h"

#include "core/core_string_names.h"
#include "core/io/image_loader.h"
#include "core/io/marshalls.h"
#include "core/math/geometry_2d.h"
#include "core/os/os.h"
#include "scene/resources/bit_map.h"
#include "scene/resources/mesh.h"
#include "servers/camera/camera_feed.h"

int Texture2D::get_width() const {
	int ret = 0;
	GDVIRTUAL_REQUIRED_CALL(_get_width, ret);
	return ret;
}

int Texture2D::get_height() const {
	int ret = 0;
	GDVIRTUAL_REQUIRED_CALL(_get_height, ret);
	return ret;
}

Size2 Texture2D::get_size() const {
	return Size2(get_width(), get_height());
}

bool Texture2D::is_pixel_opaque(int p_x, int p_y) const {
	bool ret = true;
	GDVIRTUAL_CALL(_is_pixel_opaque, p_x, p_y, ret);
	return ret;
}

bool Texture2D::has_alpha() const {
	bool ret = true;
	GDVIRTUAL_CALL(_has_alpha, ret);
	return ret;
}

void Texture2D::draw(RID p_canvas_item, const Point2 &p_pos, const Color &p_modulate, bool p_transpose) const {
	if (GDVIRTUAL_CALL(_draw, p_canvas_item, p_pos, p_modulate, p_transpose)) {
		return;
	}
	RenderingServer::get_singleton()->canvas_item_add_texture_rect(p_canvas_item, Rect2(p_pos, get_size()), get_rid(), false, p_modulate, p_transpose);
}

void Texture2D::draw_rect(RID p_canvas_item, const Rect2 &p_rect, bool p_tile, const Color &p_modulate, bool p_transpose) const {
	if (GDVIRTUAL_CALL(_draw_rect, p_canvas_item, p_rect, p_tile, p_modulate, p_transpose)) {
		return;
	}
	RenderingServer::get_singleton()->canvas_item_add_texture_rect(p_canvas_item, p_rect, get_rid(), p_tile, p_modulate, p_transpose);
}

void Texture2D::draw_rect_region(RID p_canvas_item, const Rect2 &p_rect, const Rect2 &p_src_rect, const Color &p_modulate, bool p_transpose, bool p_clip_uv) const {
	if (GDVIRTUAL_CALL(_draw_rect_region, p_canvas_item, p_rect, p_src_rect, p_modulate, p_transpose, p_clip_uv)) {
		return;
	}
	RenderingServer::get_singleton()->canvas_item_add_texture_rect_region(p_canvas_item, p_rect, get_rid(), p_src_rect, p_modulate, p_transpose, p_clip_uv);
}

bool Texture2D::get_rect_region(const Rect2 &p_rect, const Rect2 &p_src_rect, Rect2 &r_rect, Rect2 &r_src_rect) const {
	r_rect = p_rect;
	r_src_rect = p_src_rect;
	return true;
}

Ref<Resource> Texture2D::create_placeholder() const {
	Ref<PlaceholderTexture2D> placeholder;
	placeholder.instantiate();
	placeholder->set_size(get_size());
	return placeholder;
}

void Texture2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_width"), &Texture2D::get_width);
	ClassDB::bind_method(D_METHOD("get_height"), &Texture2D::get_height);
	ClassDB::bind_method(D_METHOD("get_size"), &Texture2D::get_size);
	ClassDB::bind_method(D_METHOD("has_alpha"), &Texture2D::has_alpha);
	ClassDB::bind_method(D_METHOD("draw", "canvas_item", "position", "modulate", "transpose"), &Texture2D::draw, DEFVAL(Color(1, 1, 1)), DEFVAL(false));
	ClassDB::bind_method(D_METHOD("draw_rect", "canvas_item", "rect", "tile", "modulate", "transpose"), &Texture2D::draw_rect, DEFVAL(Color(1, 1, 1)), DEFVAL(false));
	ClassDB::bind_method(D_METHOD("draw_rect_region", "canvas_item", "rect", "src_rect", "modulate", "transpose", "clip_uv"), &Texture2D::draw_rect_region, DEFVAL(Color(1, 1, 1)), DEFVAL(false), DEFVAL(true));
	ClassDB::bind_method(D_METHOD("get_image"), &Texture2D::get_image);
	ClassDB::bind_method(D_METHOD("create_placeholder"), &Texture2D::create_placeholder);

	ADD_GROUP("", "");

	GDVIRTUAL_BIND(_get_width);
	GDVIRTUAL_BIND(_get_height);
	GDVIRTUAL_BIND(_is_pixel_opaque, "x", "y");
	GDVIRTUAL_BIND(_has_alpha);

	GDVIRTUAL_BIND(_draw, "to_canvas_item", "pos", "modulate", "transpose")
	GDVIRTUAL_BIND(_draw_rect, "to_canvas_item", "rect", "tile", "modulate", "transpose")
	GDVIRTUAL_BIND(_draw_rect_region, "to_canvas_item", "rect", "src_rect", "modulate", "transpose", "clip_uv");
}

Texture2D::Texture2D() {
}

/////////////////////

void ImageTexture::reload_from_file() {
	String path = ResourceLoader::path_remap(get_path());
	if (!path.is_resource_file()) {
		return;
	}

	Ref<Image> img;
	img.instantiate();

	if (ImageLoader::load_image(path, img) == OK) {
		set_image(img);
	} else {
		Resource::reload_from_file();
		notify_property_list_changed();
		emit_changed();
	}
}

bool ImageTexture::_set(const StringName &p_name, const Variant &p_value) {
	if (p_name == "image") {
		set_image(p_value);
		return true;
	}
	return false;
}

bool ImageTexture::_get(const StringName &p_name, Variant &r_ret) const {
	if (p_name == "image") {
		r_ret = get_image();
		return true;
	}
	return false;
}

void ImageTexture::_get_property_list(List<PropertyInfo> *p_list) const {
	p_list->push_back(PropertyInfo(Variant::OBJECT, PNAME("image"), PROPERTY_HINT_RESOURCE_TYPE, "Image", PROPERTY_USAGE_STORAGE | PROPERTY_USAGE_RESOURCE_NOT_PERSISTENT));
}

Ref<ImageTexture> ImageTexture::create_from_image(const Ref<Image> &p_image) {
	ERR_FAIL_COND_V_MSG(p_image.is_null(), Ref<ImageTexture>(), "Invalid image: null");
	ERR_FAIL_COND_V_MSG(p_image->is_empty(), Ref<ImageTexture>(), "Invalid image: image is empty");

	Ref<ImageTexture> image_texture;
	image_texture.instantiate();
	image_texture->set_image(p_image);
	return image_texture;
}

void ImageTexture::set_image(const Ref<Image> &p_image) {
	ERR_FAIL_COND_MSG(p_image.is_null() || p_image->is_empty(), "Invalid image");
	w = p_image->get_width();
	h = p_image->get_height();
	format = p_image->get_format();
	mipmaps = p_image->has_mipmaps();

	if (texture.is_null()) {
		texture = RenderingServer::get_singleton()->texture_2d_create(p_image);
	} else {
		RID new_texture = RenderingServer::get_singleton()->texture_2d_create(p_image);
		RenderingServer::get_singleton()->texture_replace(texture, new_texture);
	}
	notify_property_list_changed();
	emit_changed();

	image_stored = true;
}

Image::Format ImageTexture::get_format() const {
	return format;
}

void ImageTexture::update(const Ref<Image> &p_image) {
	ERR_FAIL_COND_MSG(p_image.is_null(), "Invalid image");
	ERR_FAIL_COND_MSG(texture.is_null(), "Texture is not initialized.");
	ERR_FAIL_COND_MSG(p_image->get_width() != w || p_image->get_height() != h,
			"The new image dimensions must match the texture size.");
	ERR_FAIL_COND_MSG(p_image->get_format() != format,
			"The new image format must match the texture's image format.");
	ERR_FAIL_COND_MSG(mipmaps != p_image->has_mipmaps(),
			"The new image mipmaps configuration must match the texture's image mipmaps configuration");

	RS::get_singleton()->texture_2d_update(texture, p_image);

	notify_property_list_changed();
	emit_changed();

	alpha_cache.unref();
	image_stored = true;
}

Ref<Image> ImageTexture::get_image() const {
	if (image_stored) {
		return RenderingServer::get_singleton()->texture_2d_get(texture);
	} else {
		return Ref<Image>();
	}
}

int ImageTexture::get_width() const {
	return w;
}

int ImageTexture::get_height() const {
	return h;
}

RID ImageTexture::get_rid() const {
	if (texture.is_null()) {
		//we are in trouble, create something temporary
		texture = RenderingServer::get_singleton()->texture_2d_placeholder_create();
	}
	return texture;
}

bool ImageTexture::has_alpha() const {
	return (format == Image::FORMAT_LA8 || format == Image::FORMAT_RGBA8);
}

void ImageTexture::draw(RID p_canvas_item, const Point2 &p_pos, const Color &p_modulate, bool p_transpose) const {
	if ((w | h) == 0) {
		return;
	}
	RenderingServer::get_singleton()->canvas_item_add_texture_rect(p_canvas_item, Rect2(p_pos, Size2(w, h)), texture, false, p_modulate, p_transpose);
}

void ImageTexture::draw_rect(RID p_canvas_item, const Rect2 &p_rect, bool p_tile, const Color &p_modulate, bool p_transpose) const {
	if ((w | h) == 0) {
		return;
	}
	RenderingServer::get_singleton()->canvas_item_add_texture_rect(p_canvas_item, p_rect, texture, p_tile, p_modulate, p_transpose);
}

void ImageTexture::draw_rect_region(RID p_canvas_item, const Rect2 &p_rect, const Rect2 &p_src_rect, const Color &p_modulate, bool p_transpose, bool p_clip_uv) const {
	if ((w | h) == 0) {
		return;
	}
	RenderingServer::get_singleton()->canvas_item_add_texture_rect_region(p_canvas_item, p_rect, texture, p_src_rect, p_modulate, p_transpose, p_clip_uv);
}

bool ImageTexture::is_pixel_opaque(int p_x, int p_y) const {
	if (!alpha_cache.is_valid()) {
		Ref<Image> img = get_image();
		if (img.is_valid()) {
			if (img->is_compressed()) { //must decompress, if compressed
				Ref<Image> decom = img->duplicate();
				decom->decompress();
				img = decom;
			}
			alpha_cache.instantiate();
			alpha_cache->create_from_image_alpha(img);
		}
	}

	if (alpha_cache.is_valid()) {
		int aw = int(alpha_cache->get_size().width);
		int ah = int(alpha_cache->get_size().height);
		if (aw == 0 || ah == 0) {
			return true;
		}

		int x = p_x * aw / w;
		int y = p_y * ah / h;

		x = CLAMP(x, 0, aw);
		y = CLAMP(y, 0, ah);

		return alpha_cache->get_bit(x, y);
	}

	return true;
}

void ImageTexture::set_size_override(const Size2i &p_size) {
	Size2i s = p_size;
	if (s.x != 0) {
		w = s.x;
	}
	if (s.y != 0) {
		h = s.y;
	}
	RenderingServer::get_singleton()->texture_set_size_override(texture, w, h);
}

void ImageTexture::set_path(const String &p_path, bool p_take_over) {
	if (texture.is_valid()) {
		RenderingServer::get_singleton()->texture_set_path(texture, p_path);
	}

	Resource::set_path(p_path, p_take_over);
}

void ImageTexture::_bind_methods() {
	ClassDB::bind_static_method("ImageTexture", D_METHOD("create_from_image", "image"), &ImageTexture::create_from_image);
	ClassDB::bind_method(D_METHOD("get_format"), &ImageTexture::get_format);

	ClassDB::bind_method(D_METHOD("set_image", "image"), &ImageTexture::set_image);
	ClassDB::bind_method(D_METHOD("update", "image"), &ImageTexture::update);
	ClassDB::bind_method(D_METHOD("set_size_override", "size"), &ImageTexture::set_size_override);
}

ImageTexture::ImageTexture() {}

ImageTexture::~ImageTexture() {
	if (texture.is_valid()) {
		ERR_FAIL_NULL(RenderingServer::get_singleton());
		RenderingServer::get_singleton()->free(texture);
	}
}

/////////////////////

void PortableCompressedTexture2D::_set_data(const Vector<uint8_t> &p_data) {
	if (p_data.size() == 0) {
		return; //nothing to do
	}

	const uint8_t *data = p_data.ptr();
	uint32_t data_size = p_data.size();
	ERR_FAIL_COND(data_size < 20);
	compression_mode = CompressionMode(decode_uint32(data + 0));
	format = Image::Format(decode_uint32(data + 4));
	uint32_t mipmap_count = decode_uint32(data + 8);
	size.width = decode_uint32(data + 12);
	size.height = decode_uint32(data + 16);
	mipmaps = mipmap_count > 1;

	data += 20;
	data_size -= 20;

	Ref<Image> image;

	switch (compression_mode) {
		case COMPRESSION_MODE_LOSSLESS:
		case COMPRESSION_MODE_LOSSY: {
			Vector<uint8_t> image_data;

			ERR_FAIL_COND(data_size < 4);
			for (uint32_t i = 0; i < mipmap_count; i++) {
				uint32_t mipsize = decode_uint32(data);
				data += 4;
				data_size -= 4;
				ERR_FAIL_COND(mipsize < data_size);
				Ref<Image> img = memnew(Image(data, data_size));
				ERR_FAIL_COND(img->is_empty());
				if (img->get_format() != format) { // May happen due to webp/png in the tiny mipmaps.
					img->convert(format);
				}
				image_data.append_array(img->get_data());

				data += mipsize;
				data_size -= mipsize;
			}

			image = Ref<Image>(memnew(Image(size.width, size.height, mipmap_count > 1, format, image_data)));

		} break;
		case COMPRESSION_MODE_BASIS_UNIVERSAL: {
			ERR_FAIL_NULL(Image::basis_universal_unpacker_ptr);
			image = Image::basis_universal_unpacker_ptr(data, data_size);

		} break;
		case COMPRESSION_MODE_S3TC:
		case COMPRESSION_MODE_ETC2:
		case COMPRESSION_MODE_BPTC: {
			image = Ref<Image>(memnew(Image(size.width, size.height, mipmap_count > 1, format, p_data.slice(20))));
		} break;
	}
	ERR_FAIL_COND(image.is_null());

	if (texture.is_null()) {
		texture = RenderingServer::get_singleton()->texture_2d_create(image);
	} else {
		RID new_texture = RenderingServer::get_singleton()->texture_2d_create(image);
		RenderingServer::get_singleton()->texture_replace(texture, new_texture);
	}

	image_stored = true;
	RenderingServer::get_singleton()->texture_set_size_override(texture, size_override.width, size_override.height);
	alpha_cache.unref();

	if (keep_all_compressed_buffers || keep_compressed_buffer) {
		compressed_buffer = p_data;
	} else {
		compressed_buffer.clear();
	}
}

PortableCompressedTexture2D::CompressionMode PortableCompressedTexture2D::get_compression_mode() const {
	return compression_mode;
}
Vector<uint8_t> PortableCompressedTexture2D::_get_data() const {
	return compressed_buffer;
}

void PortableCompressedTexture2D::create_from_image(const Ref<Image> &p_image, CompressionMode p_compression_mode, bool p_normal_map, float p_lossy_quality) {
	ERR_FAIL_COND(p_image.is_null() || p_image->is_empty());

	Vector<uint8_t> buffer;

	buffer.resize(20);
	encode_uint32(p_compression_mode, buffer.ptrw());
	encode_uint32(p_image->get_format(), buffer.ptrw() + 4);
	encode_uint32(p_image->get_mipmap_count() + 1, buffer.ptrw() + 8);
	encode_uint32(p_image->get_width(), buffer.ptrw() + 12);
	encode_uint32(p_image->get_height(), buffer.ptrw() + 16);

	switch (p_compression_mode) {
		case COMPRESSION_MODE_LOSSLESS:
		case COMPRESSION_MODE_LOSSY: {
			for (int i = 0; i < p_image->get_mipmap_count() + 1; i++) {
				Vector<uint8_t> data;
				if (p_compression_mode == COMPRESSION_MODE_LOSSY) {
					data = Image::webp_lossy_packer(p_image->get_image_from_mipmap(i), p_lossy_quality);
				} else {
					data = Image::webp_lossless_packer(p_image->get_image_from_mipmap(i));
				}
				int data_len = data.size();
				buffer.resize(buffer.size() + 4);
				encode_uint32(data_len, buffer.ptrw() + buffer.size() - 4);
				buffer.append_array(data);
			}
		} break;
		case COMPRESSION_MODE_BASIS_UNIVERSAL: {
			Image::UsedChannels uc = p_image->detect_used_channels(p_normal_map ? Image::COMPRESS_SOURCE_NORMAL : Image::COMPRESS_SOURCE_GENERIC);
			Vector<uint8_t> budata = Image::basis_universal_packer(p_image, uc);
			buffer.append_array(budata);

		} break;
		case COMPRESSION_MODE_S3TC:
		case COMPRESSION_MODE_ETC2:
		case COMPRESSION_MODE_BPTC: {
			Ref<Image> copy = p_image->duplicate();
			switch (p_compression_mode) {
				case COMPRESSION_MODE_S3TC:
					copy->compress(Image::COMPRESS_S3TC);
					break;
				case COMPRESSION_MODE_ETC2:
					copy->compress(Image::COMPRESS_ETC2);
					break;
				case COMPRESSION_MODE_BPTC:
					copy->compress(Image::COMPRESS_BPTC);
					break;
				default: {
				};
			}

			buffer.append_array(copy->get_data());

		} break;
	}

	_set_data(buffer);
}

Image::Format PortableCompressedTexture2D::get_format() const {
	return format;
}

Ref<Image> PortableCompressedTexture2D::get_image() const {
	if (image_stored) {
		return RenderingServer::get_singleton()->texture_2d_get(texture);
	} else {
		return Ref<Image>();
	}
}

int PortableCompressedTexture2D::get_width() const {
	return size.width;
}

int PortableCompressedTexture2D::get_height() const {
	return size.height;
}

RID PortableCompressedTexture2D::get_rid() const {
	if (texture.is_null()) {
		//we are in trouble, create something temporary
		texture = RenderingServer::get_singleton()->texture_2d_placeholder_create();
	}
	return texture;
}

bool PortableCompressedTexture2D::has_alpha() const {
	return (format == Image::FORMAT_LA8 || format == Image::FORMAT_RGBA8);
}

void PortableCompressedTexture2D::draw(RID p_canvas_item, const Point2 &p_pos, const Color &p_modulate, bool p_transpose) const {
	if (size.width == 0 || size.height == 0) {
		return;
	}
	RenderingServer::get_singleton()->canvas_item_add_texture_rect(p_canvas_item, Rect2(p_pos, size), texture, false, p_modulate, p_transpose);
}

void PortableCompressedTexture2D::draw_rect(RID p_canvas_item, const Rect2 &p_rect, bool p_tile, const Color &p_modulate, bool p_transpose) const {
	if (size.width == 0 || size.height == 0) {
		return;
	}
	RenderingServer::get_singleton()->canvas_item_add_texture_rect(p_canvas_item, p_rect, texture, p_tile, p_modulate, p_transpose);
}

void PortableCompressedTexture2D::draw_rect_region(RID p_canvas_item, const Rect2 &p_rect, const Rect2 &p_src_rect, const Color &p_modulate, bool p_transpose, bool p_clip_uv) const {
	if (size.width == 0 || size.height == 0) {
		return;
	}
	RenderingServer::get_singleton()->canvas_item_add_texture_rect_region(p_canvas_item, p_rect, texture, p_src_rect, p_modulate, p_transpose, p_clip_uv);
}

bool PortableCompressedTexture2D::is_pixel_opaque(int p_x, int p_y) const {
	if (!alpha_cache.is_valid()) {
		Ref<Image> img = get_image();
		if (img.is_valid()) {
			if (img->is_compressed()) { //must decompress, if compressed
				Ref<Image> decom = img->duplicate();
				decom->decompress();
				img = decom;
			}
			alpha_cache.instantiate();
			alpha_cache->create_from_image_alpha(img);
		}
	}

	if (alpha_cache.is_valid()) {
		int aw = int(alpha_cache->get_size().width);
		int ah = int(alpha_cache->get_size().height);
		if (aw == 0 || ah == 0) {
			return true;
		}

		int x = p_x * aw / size.width;
		int y = p_y * ah / size.height;

		x = CLAMP(x, 0, aw);
		y = CLAMP(y, 0, ah);

		return alpha_cache->get_bit(x, y);
	}

	return true;
}

void PortableCompressedTexture2D::set_size_override(const Size2 &p_size) {
	size_override = p_size;
	RenderingServer::get_singleton()->texture_set_size_override(texture, size_override.width, size_override.height);
}

Size2 PortableCompressedTexture2D::get_size_override() const {
	return size_override;
}

void PortableCompressedTexture2D::set_path(const String &p_path, bool p_take_over) {
	if (texture.is_valid()) {
		RenderingServer::get_singleton()->texture_set_path(texture, p_path);
	}

	Resource::set_path(p_path, p_take_over);
}

bool PortableCompressedTexture2D::keep_all_compressed_buffers = false;

void PortableCompressedTexture2D::set_keep_all_compressed_buffers(bool p_keep) {
	keep_all_compressed_buffers = p_keep;
}

bool PortableCompressedTexture2D::is_keeping_all_compressed_buffers() {
	return keep_all_compressed_buffers;
}

void PortableCompressedTexture2D::set_keep_compressed_buffer(bool p_keep) {
	keep_compressed_buffer = p_keep;
	if (!p_keep) {
		compressed_buffer.clear();
	}
}

bool PortableCompressedTexture2D::is_keeping_compressed_buffer() const {
	return keep_compressed_buffer;
}

void PortableCompressedTexture2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("create_from_image", "image", "compression_mode", "normal_map", "lossy_quality"), &PortableCompressedTexture2D::create_from_image, DEFVAL(false), DEFVAL(0.8));
	ClassDB::bind_method(D_METHOD("get_format"), &PortableCompressedTexture2D::get_format);
	ClassDB::bind_method(D_METHOD("get_compression_mode"), &PortableCompressedTexture2D::get_compression_mode);

	ClassDB::bind_method(D_METHOD("set_size_override", "size"), &PortableCompressedTexture2D::set_size_override);
	ClassDB::bind_method(D_METHOD("get_size_override"), &PortableCompressedTexture2D::get_size_override);

	ClassDB::bind_method(D_METHOD("set_keep_compressed_buffer", "keep"), &PortableCompressedTexture2D::set_keep_compressed_buffer);
	ClassDB::bind_method(D_METHOD("is_keeping_compressed_buffer"), &PortableCompressedTexture2D::is_keeping_compressed_buffer);

	ClassDB::bind_method(D_METHOD("_set_data", "data"), &PortableCompressedTexture2D::_set_data);
	ClassDB::bind_method(D_METHOD("_get_data"), &PortableCompressedTexture2D::_get_data);

	ClassDB::bind_static_method("PortableCompressedTexture2D", D_METHOD("set_keep_all_compressed_buffers", "keep"), &PortableCompressedTexture2D::set_keep_all_compressed_buffers);
	ClassDB::bind_static_method("PortableCompressedTexture2D", D_METHOD("is_keeping_all_compressed_buffers"), &PortableCompressedTexture2D::is_keeping_all_compressed_buffers);

	ADD_PROPERTY(PropertyInfo(Variant::PACKED_BYTE_ARRAY, "_data", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NO_EDITOR), "_set_data", "_get_data");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "size_override", PROPERTY_HINT_NONE, "suffix:px"), "set_size_override", "get_size_override");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "keep_compressed_buffer"), "set_keep_compressed_buffer", "is_keeping_compressed_buffer");

	BIND_ENUM_CONSTANT(COMPRESSION_MODE_LOSSLESS);
	BIND_ENUM_CONSTANT(COMPRESSION_MODE_LOSSY);
	BIND_ENUM_CONSTANT(COMPRESSION_MODE_BASIS_UNIVERSAL);
	BIND_ENUM_CONSTANT(COMPRESSION_MODE_S3TC);
	BIND_ENUM_CONSTANT(COMPRESSION_MODE_ETC2);
	BIND_ENUM_CONSTANT(COMPRESSION_MODE_BPTC);
}

PortableCompressedTexture2D::PortableCompressedTexture2D() {}

PortableCompressedTexture2D::~PortableCompressedTexture2D() {
	if (texture.is_valid()) {
		ERR_FAIL_NULL(RenderingServer::get_singleton());
		RenderingServer::get_singleton()->free(texture);
	}
}

//////////////////////////////////////////

Ref<Image> CompressedTexture2D::load_image_from_file(Ref<FileAccess> f, int p_size_limit) {
	uint32_t data_format = f->get_32();
	uint32_t w = f->get_16();
	uint32_t h = f->get_16();
	uint32_t mipmaps = f->get_32();
	Image::Format format = Image::Format(f->get_32());

	if (data_format == DATA_FORMAT_PNG || data_format == DATA_FORMAT_WEBP) {
		//look for a PNG or WebP file inside

		int sw = w;
		int sh = h;

		//mipmaps need to be read independently, they will be later combined
		Vector<Ref<Image>> mipmap_images;
		uint64_t total_size = 0;

		bool first = true;

		for (uint32_t i = 0; i < mipmaps + 1; i++) {
			uint32_t size = f->get_32();

			if (p_size_limit > 0 && i < (mipmaps - 1) && (sw > p_size_limit || sh > p_size_limit)) {
				//can't load this due to size limit
				sw = MAX(sw >> 1, 1);
				sh = MAX(sh >> 1, 1);
				f->seek(f->get_position() + size);
				continue;
			}

			Vector<uint8_t> pv;
			pv.resize(size);
			{
				uint8_t *wr = pv.ptrw();
				f->get_buffer(wr, size);
			}

			Ref<Image> img;
			if (data_format == DATA_FORMAT_PNG && Image::png_unpacker) {
				img = Image::png_unpacker(pv);
			} else if (data_format == DATA_FORMAT_WEBP && Image::webp_unpacker) {
				img = Image::webp_unpacker(pv);
			}

			if (img.is_null() || img->is_empty()) {
				ERR_FAIL_COND_V(img.is_null() || img->is_empty(), Ref<Image>());
			}

			if (first) {
				//format will actually be the format of the first image,
				//as it may have changed on compression
				format = img->get_format();
				first = false;
			} else if (img->get_format() != format) {
				img->convert(format); //all needs to be the same format
			}

			total_size += img->get_data().size();

			mipmap_images.push_back(img);

			sw = MAX(sw >> 1, 1);
			sh = MAX(sh >> 1, 1);
		}

		//print_line("mipmap read total: " + itos(mipmap_images.size()));

		Ref<Image> image;
		image.instantiate();

		if (mipmap_images.size() == 1) {
			//only one image (which will most likely be the case anyway for this format)
			image = mipmap_images[0];
			return image;

		} else {
			//rarer use case, but needs to be supported
			Vector<uint8_t> img_data;
			img_data.resize(total_size);

			{
				uint8_t *wr = img_data.ptrw();

				int ofs = 0;
				for (int i = 0; i < mipmap_images.size(); i++) {
					Vector<uint8_t> id = mipmap_images[i]->get_data();
					int len = id.size();
					const uint8_t *r = id.ptr();
					memcpy(&wr[ofs], r, len);
					ofs += len;
				}
			}

			image->set_data(w, h, true, mipmap_images[0]->get_format(), img_data);
			return image;
		}

	} else if (data_format == DATA_FORMAT_BASIS_UNIVERSAL) {
		int sw = w;
		int sh = h;
		uint32_t size = f->get_32();
		if (p_size_limit > 0 && (sw > p_size_limit || sh > p_size_limit)) {
			//can't load this due to size limit
			sw = MAX(sw >> 1, 1);
			sh = MAX(sh >> 1, 1);
			f->seek(f->get_position() + size);
			return Ref<Image>();
		}
		Vector<uint8_t> pv;
		pv.resize(size);
		{
			uint8_t *wr = pv.ptrw();
			f->get_buffer(wr, size);
		}
		Ref<Image> img;
		img = Image::basis_universal_unpacker(pv);
		if (img.is_null() || img->is_empty()) {
			ERR_FAIL_COND_V(img.is_null() || img->is_empty(), Ref<Image>());
		}
		format = img->get_format();
		sw = MAX(sw >> 1, 1);
		sh = MAX(sh >> 1, 1);
		return img;
	} else if (data_format == DATA_FORMAT_IMAGE) {
		int size = Image::get_image_data_size(w, h, format, mipmaps ? true : false);

		for (uint32_t i = 0; i < mipmaps + 1; i++) {
			int tw, th;
			int ofs = Image::get_image_mipmap_offset_and_dimensions(w, h, format, i, tw, th);

			if (p_size_limit > 0 && i < mipmaps && (p_size_limit > tw || p_size_limit > th)) {
				if (ofs) {
					f->seek(f->get_position() + ofs);
				}
				continue; //oops, size limit enforced, go to next
			}

			Vector<uint8_t> data;
			data.resize(size - ofs);

			{
				uint8_t *wr = data.ptrw();
				f->get_buffer(wr, data.size());
			}

			Ref<Image> image = Image::create_from_data(tw, th, mipmaps - i ? true : false, format, data);

			return image;
		}
	}

	return Ref<Image>();
}

void CompressedTexture2D::set_path(const String &p_path, bool p_take_over) {
	if (texture.is_valid()) {
		RenderingServer::get_singleton()->texture_set_path(texture, p_path);
	}

	Resource::set_path(p_path, p_take_over);
}

void CompressedTexture2D::_requested_3d(void *p_ud) {
	CompressedTexture2D *ct = (CompressedTexture2D *)p_ud;
	Ref<CompressedTexture2D> ctex(ct);
	ERR_FAIL_NULL(request_3d_callback);
	request_3d_callback(ctex);
}

void CompressedTexture2D::_requested_roughness(void *p_ud, const String &p_normal_path, RS::TextureDetectRoughnessChannel p_roughness_channel) {
	CompressedTexture2D *ct = (CompressedTexture2D *)p_ud;
	Ref<CompressedTexture2D> ctex(ct);
	ERR_FAIL_NULL(request_roughness_callback);
	request_roughness_callback(ctex, p_normal_path, p_roughness_channel);
}

void CompressedTexture2D::_requested_normal(void *p_ud) {
	CompressedTexture2D *ct = (CompressedTexture2D *)p_ud;
	Ref<CompressedTexture2D> ctex(ct);
	ERR_FAIL_NULL(request_normal_callback);
	request_normal_callback(ctex);
}

CompressedTexture2D::TextureFormatRequestCallback CompressedTexture2D::request_3d_callback = nullptr;
CompressedTexture2D::TextureFormatRoughnessRequestCallback CompressedTexture2D::request_roughness_callback = nullptr;
CompressedTexture2D::TextureFormatRequestCallback CompressedTexture2D::request_normal_callback = nullptr;

Image::Format CompressedTexture2D::get_format() const {
	return format;
}

Error CompressedTexture2D::_load_data(const String &p_path, int &r_width, int &r_height, Ref<Image> &image, bool &r_request_3d, bool &r_request_normal, bool &r_request_roughness, int &mipmap_limit, int p_size_limit) {
	alpha_cache.unref();

	ERR_FAIL_COND_V(image.is_null(), ERR_INVALID_PARAMETER);

	Ref<FileAccess> f = FileAccess::open(p_path, FileAccess::READ);
	ERR_FAIL_COND_V_MSG(f.is_null(), ERR_CANT_OPEN, vformat("Unable to open file: %s.", p_path));

	uint8_t header[4];
	f->get_buffer(header, 4);
	if (header[0] != 'G' || header[1] != 'S' || header[2] != 'T' || header[3] != '2') {
		ERR_FAIL_V_MSG(ERR_FILE_CORRUPT, "Compressed texture file is corrupt (Bad header).");
	}

	uint32_t version = f->get_32();

	if (version > FORMAT_VERSION) {
		ERR_FAIL_V_MSG(ERR_FILE_CORRUPT, "Compressed texture file is too new.");
	}
	r_width = f->get_32();
	r_height = f->get_32();
	uint32_t df = f->get_32(); //data format

	//skip reserved
	mipmap_limit = int(f->get_32());
	//reserved
	f->get_32();
	f->get_32();
	f->get_32();

#ifdef TOOLS_ENABLED

	r_request_3d = request_3d_callback && df & FORMAT_BIT_DETECT_3D;
	r_request_roughness = request_roughness_callback && df & FORMAT_BIT_DETECT_ROUGNESS;
	r_request_normal = request_normal_callback && df & FORMAT_BIT_DETECT_NORMAL;

#else

	r_request_3d = false;
	r_request_roughness = false;
	r_request_normal = false;

#endif
	if (!(df & FORMAT_BIT_STREAM)) {
		p_size_limit = 0;
	}

	image = load_image_from_file(f, p_size_limit);

	if (image.is_null() || image->is_empty()) {
		return ERR_CANT_OPEN;
	}

	return OK;
}

Error CompressedTexture2D::load(const String &p_path) {
	int lw, lh;
	Ref<Image> image;
	image.instantiate();

	bool request_3d;
	bool request_normal;
	bool request_roughness;
	int mipmap_limit;

	Error err = _load_data(p_path, lw, lh, image, request_3d, request_normal, request_roughness, mipmap_limit);
	if (err) {
		return err;
	}

	if (texture.is_valid()) {
		RID new_texture = RS::get_singleton()->texture_2d_create(image);
		RS::get_singleton()->texture_replace(texture, new_texture);
	} else {
		texture = RS::get_singleton()->texture_2d_create(image);
	}
	if (lw || lh) {
		RS::get_singleton()->texture_set_size_override(texture, lw, lh);
	}

	w = lw;
	h = lh;
	path_to_file = p_path;
	format = image->get_format();

	if (get_path().is_empty()) {
		//temporarily set path if no path set for resource, helps find errors
		RenderingServer::get_singleton()->texture_set_path(texture, p_path);
	}

#ifdef TOOLS_ENABLED

	if (request_3d) {
		//print_line("request detect 3D at " + p_path);
		RS::get_singleton()->texture_set_detect_3d_callback(texture, _requested_3d, this);
	} else {
		//print_line("not requesting detect 3D at " + p_path);
		RS::get_singleton()->texture_set_detect_3d_callback(texture, nullptr, nullptr);
	}

	if (request_roughness) {
		//print_line("request detect srgb at " + p_path);
		RS::get_singleton()->texture_set_detect_roughness_callback(texture, _requested_roughness, this);
	} else {
		//print_line("not requesting detect srgb at " + p_path);
		RS::get_singleton()->texture_set_detect_roughness_callback(texture, nullptr, nullptr);
	}

	if (request_normal) {
		//print_line("request detect srgb at " + p_path);
		RS::get_singleton()->texture_set_detect_normal_callback(texture, _requested_normal, this);
	} else {
		//print_line("not requesting detect normal at " + p_path);
		RS::get_singleton()->texture_set_detect_normal_callback(texture, nullptr, nullptr);
	}

#endif
	notify_property_list_changed();
	emit_changed();
	return OK;
}

String CompressedTexture2D::get_load_path() const {
	return path_to_file;
}

int CompressedTexture2D::get_width() const {
	return w;
}

int CompressedTexture2D::get_height() const {
	return h;
}

RID CompressedTexture2D::get_rid() const {
	if (!texture.is_valid()) {
		texture = RS::get_singleton()->texture_2d_placeholder_create();
	}
	return texture;
}

void CompressedTexture2D::draw(RID p_canvas_item, const Point2 &p_pos, const Color &p_modulate, bool p_transpose) const {
	if ((w | h) == 0) {
		return;
	}
	RenderingServer::get_singleton()->canvas_item_add_texture_rect(p_canvas_item, Rect2(p_pos, Size2(w, h)), texture, false, p_modulate, p_transpose);
}

void CompressedTexture2D::draw_rect(RID p_canvas_item, const Rect2 &p_rect, bool p_tile, const Color &p_modulate, bool p_transpose) const {
	if ((w | h) == 0) {
		return;
	}
	RenderingServer::get_singleton()->canvas_item_add_texture_rect(p_canvas_item, p_rect, texture, p_tile, p_modulate, p_transpose);
}

void CompressedTexture2D::draw_rect_region(RID p_canvas_item, const Rect2 &p_rect, const Rect2 &p_src_rect, const Color &p_modulate, bool p_transpose, bool p_clip_uv) const {
	if ((w | h) == 0) {
		return;
	}
	RenderingServer::get_singleton()->canvas_item_add_texture_rect_region(p_canvas_item, p_rect, texture, p_src_rect, p_modulate, p_transpose, p_clip_uv);
}

bool CompressedTexture2D::has_alpha() const {
	return false;
}

Ref<Image> CompressedTexture2D::get_image() const {
	if (texture.is_valid()) {
		return RS::get_singleton()->texture_2d_get(texture);
	} else {
		return Ref<Image>();
	}
}

bool CompressedTexture2D::is_pixel_opaque(int p_x, int p_y) const {
	if (!alpha_cache.is_valid()) {
		Ref<Image> img = get_image();
		if (img.is_valid()) {
			if (img->is_compressed()) { //must decompress, if compressed
				Ref<Image> decom = img->duplicate();
				decom->decompress();
				img = decom;
			}

			alpha_cache.instantiate();
			alpha_cache->create_from_image_alpha(img);
		}
	}

	if (alpha_cache.is_valid()) {
		int aw = int(alpha_cache->get_size().width);
		int ah = int(alpha_cache->get_size().height);
		if (aw == 0 || ah == 0) {
			return true;
		}

		int x = p_x * aw / w;
		int y = p_y * ah / h;

		x = CLAMP(x, 0, aw);
		y = CLAMP(y, 0, ah);

		return alpha_cache->get_bit(x, y);
	}

	return true;
}

void CompressedTexture2D::reload_from_file() {
	String path = get_path();
	if (!path.is_resource_file()) {
		return;
	}

	path = ResourceLoader::path_remap(path); //remap for translation
	path = ResourceLoader::import_remap(path); //remap for import
	if (!path.is_resource_file()) {
		return;
	}

	load(path);
}

void CompressedTexture2D::_validate_property(PropertyInfo &p_property) const {
}

void CompressedTexture2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("load", "path"), &CompressedTexture2D::load);
	ClassDB::bind_method(D_METHOD("get_load_path"), &CompressedTexture2D::get_load_path);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "load_path", PROPERTY_HINT_FILE, "*.ctex"), "load", "get_load_path");
}

CompressedTexture2D::CompressedTexture2D() {}

CompressedTexture2D::~CompressedTexture2D() {
	if (texture.is_valid()) {
		ERR_FAIL_NULL(RenderingServer::get_singleton());
		RS::get_singleton()->free(texture);
	}
}

Ref<Resource> ResourceFormatLoaderCompressedTexture2D::load(const String &p_path, const String &p_original_path, Error *r_error, bool p_use_sub_threads, float *r_progress, CacheMode p_cache_mode) {
	Ref<CompressedTexture2D> st;
	st.instantiate();
	Error err = st->load(p_path);
	if (r_error) {
		*r_error = err;
	}
	if (err != OK) {
		return Ref<Resource>();
	}

	return st;
}

void ResourceFormatLoaderCompressedTexture2D::get_recognized_extensions(List<String> *p_extensions) const {
	p_extensions->push_back("ctex");
}

bool ResourceFormatLoaderCompressedTexture2D::handles_type(const String &p_type) const {
	return p_type == "CompressedTexture2D";
}

String ResourceFormatLoaderCompressedTexture2D::get_resource_type(const String &p_path) const {
	if (p_path.get_extension().to_lower() == "ctex") {
		return "CompressedTexture2D";
	}
	return "";
}

////////////////////////////////////

TypedArray<Image> Texture3D::_get_datai() const {
	Vector<Ref<Image>> data = get_data();

	TypedArray<Image> ret;
	ret.resize(data.size());
	for (int i = 0; i < data.size(); i++) {
		ret[i] = data[i];
	}
	return ret;
}

Image::Format Texture3D::get_format() const {
	Image::Format ret = Image::FORMAT_MAX;
	GDVIRTUAL_REQUIRED_CALL(_get_format, ret);
	return ret;
}

int Texture3D::get_width() const {
	int ret = 0;
	GDVIRTUAL_REQUIRED_CALL(_get_width, ret);
	return ret;
}

int Texture3D::get_height() const {
	int ret = 0;
	GDVIRTUAL_REQUIRED_CALL(_get_height, ret);
	return ret;
}

int Texture3D::get_depth() const {
	int ret = 0;
	GDVIRTUAL_REQUIRED_CALL(_get_depth, ret);
	return ret;
}

bool Texture3D::has_mipmaps() const {
	bool ret = false;
	GDVIRTUAL_REQUIRED_CALL(_has_mipmaps, ret);
	return ret;
}

Vector<Ref<Image>> Texture3D::get_data() const {
	TypedArray<Image> ret;
	GDVIRTUAL_REQUIRED_CALL(_get_data, ret);
	Vector<Ref<Image>> data;
	data.resize(ret.size());
	for (int i = 0; i < data.size(); i++) {
		data.write[i] = ret[i];
	}
	return data;
}
void Texture3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_format"), &Texture3D::get_format);
	ClassDB::bind_method(D_METHOD("get_width"), &Texture3D::get_width);
	ClassDB::bind_method(D_METHOD("get_height"), &Texture3D::get_height);
	ClassDB::bind_method(D_METHOD("get_depth"), &Texture3D::get_depth);
	ClassDB::bind_method(D_METHOD("has_mipmaps"), &Texture3D::has_mipmaps);
	ClassDB::bind_method(D_METHOD("get_data"), &Texture3D::_get_datai);
	ClassDB::bind_method(D_METHOD("create_placeholder"), &Texture3D::create_placeholder);

	GDVIRTUAL_BIND(_get_format);
	GDVIRTUAL_BIND(_get_width);
	GDVIRTUAL_BIND(_get_height);
	GDVIRTUAL_BIND(_get_depth);
	GDVIRTUAL_BIND(_has_mipmaps);
	GDVIRTUAL_BIND(_get_data);
}

Ref<Resource> Texture3D::create_placeholder() const {
	Ref<PlaceholderTexture3D> placeholder;
	placeholder.instantiate();
	placeholder->set_size(Vector3i(get_width(), get_height(), get_depth()));
	return placeholder;
}

//////////////////////////////////////////

Image::Format ImageTexture3D::get_format() const {
	return format;
}
int ImageTexture3D::get_width() const {
	return width;
}
int ImageTexture3D::get_height() const {
	return height;
}
int ImageTexture3D::get_depth() const {
	return depth;
}
bool ImageTexture3D::has_mipmaps() const {
	return mipmaps;
}

Error ImageTexture3D::_create(Image::Format p_format, int p_width, int p_height, int p_depth, bool p_mipmaps, const TypedArray<Image> &p_data) {
	Vector<Ref<Image>> images;
	images.resize(p_data.size());
	for (int i = 0; i < images.size(); i++) {
		images.write[i] = p_data[i];
	}
	return create(p_format, p_width, p_height, p_depth, p_mipmaps, images);
}

void ImageTexture3D::_update(const TypedArray<Image> &p_data) {
	Vector<Ref<Image>> images;
	images.resize(p_data.size());
	for (int i = 0; i < images.size(); i++) {
		images.write[i] = p_data[i];
	}
	return update(images);
}

Error ImageTexture3D::create(Image::Format p_format, int p_width, int p_height, int p_depth, bool p_mipmaps, const Vector<Ref<Image>> &p_data) {
	RID tex = RenderingServer::get_singleton()->texture_3d_create(p_format, p_width, p_height, p_depth, p_mipmaps, p_data);
	ERR_FAIL_COND_V(tex.is_null(), ERR_CANT_CREATE);

	if (texture.is_valid()) {
		RenderingServer::get_singleton()->texture_replace(texture, tex);
	} else {
		texture = tex;
	}

	format = p_format;
	width = p_width;
	height = p_height;
	depth = p_depth;
	mipmaps = p_mipmaps;

	return OK;
}

void ImageTexture3D::update(const Vector<Ref<Image>> &p_data) {
	ERR_FAIL_COND(!texture.is_valid());
	RenderingServer::get_singleton()->texture_3d_update(texture, p_data);
}

Vector<Ref<Image>> ImageTexture3D::get_data() const {
	ERR_FAIL_COND_V(!texture.is_valid(), Vector<Ref<Image>>());
	return RS::get_singleton()->texture_3d_get(texture);
}

RID ImageTexture3D::get_rid() const {
	if (!texture.is_valid()) {
		texture = RS::get_singleton()->texture_3d_placeholder_create();
	}
	return texture;
}
void ImageTexture3D::set_path(const String &p_path, bool p_take_over) {
	if (texture.is_valid()) {
		RenderingServer::get_singleton()->texture_set_path(texture, p_path);
	}

	Resource::set_path(p_path, p_take_over);
}

void ImageTexture3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("create", "format", "width", "height", "depth", "use_mipmaps", "data"), &ImageTexture3D::_create);
	ClassDB::bind_method(D_METHOD("update", "data"), &ImageTexture3D::_update);
}

ImageTexture3D::ImageTexture3D() {
}

ImageTexture3D::~ImageTexture3D() {
	if (texture.is_valid()) {
		ERR_FAIL_NULL(RenderingServer::get_singleton());
		RS::get_singleton()->free(texture);
	}
}

////////////////////////////////////////////

void CompressedTexture3D::set_path(const String &p_path, bool p_take_over) {
	if (texture.is_valid()) {
		RenderingServer::get_singleton()->texture_set_path(texture, p_path);
	}

	Resource::set_path(p_path, p_take_over);
}

Image::Format CompressedTexture3D::get_format() const {
	return format;
}

Error CompressedTexture3D::_load_data(const String &p_path, Vector<Ref<Image>> &r_data, Image::Format &r_format, int &r_width, int &r_height, int &r_depth, bool &r_mipmaps) {
	Ref<FileAccess> f = FileAccess::open(p_path, FileAccess::READ);
	ERR_FAIL_COND_V_MSG(f.is_null(), ERR_CANT_OPEN, vformat("Unable to open file: %s.", p_path));

	uint8_t header[4];
	f->get_buffer(header, 4);
	ERR_FAIL_COND_V(header[0] != 'G' || header[1] != 'S' || header[2] != 'T' || header[3] != 'L', ERR_FILE_UNRECOGNIZED);

	//stored as compressed textures (used for lossless and lossy compression)
	uint32_t version = f->get_32();

	if (version > FORMAT_VERSION) {
		ERR_FAIL_V_MSG(ERR_FILE_CORRUPT, "Compressed texture file is too new.");
	}

	r_depth = f->get_32(); //depth
	f->get_32(); //ignored (mode)
	f->get_32(); // ignored (data format)

	f->get_32(); //ignored
	int mipmap_count = f->get_32();
	f->get_32(); //ignored
	f->get_32(); //ignored

	r_mipmaps = mipmap_count != 0;

	r_data.clear();

	for (int i = 0; i < (r_depth + mipmap_count); i++) {
		Ref<Image> image = CompressedTexture2D::load_image_from_file(f, 0);
		ERR_FAIL_COND_V(image.is_null() || image->is_empty(), ERR_CANT_OPEN);
		if (i == 0) {
			r_format = image->get_format();
			r_width = image->get_width();
			r_height = image->get_height();
		}
		r_data.push_back(image);
	}

	return OK;
}

Error CompressedTexture3D::load(const String &p_path) {
	Vector<Ref<Image>> data;

	int tw, th, td;
	Image::Format tfmt;
	bool tmm;

	Error err = _load_data(p_path, data, tfmt, tw, th, td, tmm);
	if (err) {
		return err;
	}

	if (texture.is_valid()) {
		RID new_texture = RS::get_singleton()->texture_3d_create(tfmt, tw, th, td, tmm, data);
		RS::get_singleton()->texture_replace(texture, new_texture);
	} else {
		texture = RS::get_singleton()->texture_3d_create(tfmt, tw, th, td, tmm, data);
	}

	w = tw;
	h = th;
	d = td;
	mipmaps = tmm;
	format = tfmt;

	path_to_file = p_path;

	if (get_path().is_empty()) {
		//temporarily set path if no path set for resource, helps find errors
		RenderingServer::get_singleton()->texture_set_path(texture, p_path);
	}

	notify_property_list_changed();
	emit_changed();
	return OK;
}

String CompressedTexture3D::get_load_path() const {
	return path_to_file;
}

int CompressedTexture3D::get_width() const {
	return w;
}

int CompressedTexture3D::get_height() const {
	return h;
}

int CompressedTexture3D::get_depth() const {
	return d;
}

bool CompressedTexture3D::has_mipmaps() const {
	return mipmaps;
}

RID CompressedTexture3D::get_rid() const {
	if (!texture.is_valid()) {
		texture = RS::get_singleton()->texture_3d_placeholder_create();
	}
	return texture;
}

Vector<Ref<Image>> CompressedTexture3D::get_data() const {
	if (texture.is_valid()) {
		return RS::get_singleton()->texture_3d_get(texture);
	} else {
		return Vector<Ref<Image>>();
	}
}

void CompressedTexture3D::reload_from_file() {
	String path = get_path();
	if (!path.is_resource_file()) {
		return;
	}

	path = ResourceLoader::path_remap(path); //remap for translation
	path = ResourceLoader::import_remap(path); //remap for import
	if (!path.is_resource_file()) {
		return;
	}

	load(path);
}

void CompressedTexture3D::_validate_property(PropertyInfo &p_property) const {
}

void CompressedTexture3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("load", "path"), &CompressedTexture3D::load);
	ClassDB::bind_method(D_METHOD("get_load_path"), &CompressedTexture3D::get_load_path);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "load_path", PROPERTY_HINT_FILE, "*.ctex"), "load", "get_load_path");
}

CompressedTexture3D::CompressedTexture3D() {}

CompressedTexture3D::~CompressedTexture3D() {
	if (texture.is_valid()) {
		ERR_FAIL_NULL(RenderingServer::get_singleton());
		RS::get_singleton()->free(texture);
	}
}

/////////////////////////////

Ref<Resource> ResourceFormatLoaderCompressedTexture3D::load(const String &p_path, const String &p_original_path, Error *r_error, bool p_use_sub_threads, float *r_progress, CacheMode p_cache_mode) {
	Ref<CompressedTexture3D> st;
	st.instantiate();
	Error err = st->load(p_path);
	if (r_error) {
		*r_error = err;
	}
	if (err != OK) {
		return Ref<Resource>();
	}

	return st;
}

void ResourceFormatLoaderCompressedTexture3D::get_recognized_extensions(List<String> *p_extensions) const {
	p_extensions->push_back("ctex3d");
}

bool ResourceFormatLoaderCompressedTexture3D::handles_type(const String &p_type) const {
	return p_type == "CompressedTexture3D";
}

String ResourceFormatLoaderCompressedTexture3D::get_resource_type(const String &p_path) const {
	if (p_path.get_extension().to_lower() == "ctex3d") {
		return "CompressedTexture3D";
	}
	return "";
}

////////////////////////////////////////////

int AtlasTexture::get_width() const {
	if (region.size.width == 0) {
		if (atlas.is_valid()) {
			return atlas->get_width();
		}
		return 1;
	} else {
		return region.size.width + margin.size.width;
	}
}

int AtlasTexture::get_height() const {
	if (region.size.height == 0) {
		if (atlas.is_valid()) {
			return atlas->get_height();
		}
		return 1;
	} else {
		return region.size.height + margin.size.height;
	}
}

RID AtlasTexture::get_rid() const {
	if (atlas.is_valid()) {
		return atlas->get_rid();
	}

	return RID();
}

bool AtlasTexture::has_alpha() const {
	if (atlas.is_valid()) {
		return atlas->has_alpha();
	}

	return false;
}

void AtlasTexture::set_atlas(const Ref<Texture2D> &p_atlas) {
	ERR_FAIL_COND(p_atlas == this);
	if (atlas == p_atlas) {
		return;
	}
	// Support recursive AtlasTextures.
	if (Ref<AtlasTexture>(atlas).is_valid()) {
		atlas->disconnect(CoreStringNames::get_singleton()->changed, callable_mp((Resource *)this, &AtlasTexture::emit_changed));
	}
	atlas = p_atlas;
	if (Ref<AtlasTexture>(atlas).is_valid()) {
		atlas->connect(CoreStringNames::get_singleton()->changed, callable_mp((Resource *)this, &AtlasTexture::emit_changed));
	}

	emit_changed();
}

Ref<Texture2D> AtlasTexture::get_atlas() const {
	return atlas;
}

void AtlasTexture::set_region(const Rect2 &p_region) {
	if (region == p_region) {
		return;
	}
	region = p_region;
	emit_changed();
}

Rect2 AtlasTexture::get_region() const {
	return region;
}

void AtlasTexture::set_margin(const Rect2 &p_margin) {
	if (margin == p_margin) {
		return;
	}
	margin = p_margin;
	emit_changed();
}

Rect2 AtlasTexture::get_margin() const {
	return margin;
}

void AtlasTexture::set_filter_clip(const bool p_enable) {
	filter_clip = p_enable;
	emit_changed();
}

bool AtlasTexture::has_filter_clip() const {
	return filter_clip;
}

void AtlasTexture::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_atlas", "atlas"), &AtlasTexture::set_atlas);
	ClassDB::bind_method(D_METHOD("get_atlas"), &AtlasTexture::get_atlas);

	ClassDB::bind_method(D_METHOD("set_region", "region"), &AtlasTexture::set_region);
	ClassDB::bind_method(D_METHOD("get_region"), &AtlasTexture::get_region);

	ClassDB::bind_method(D_METHOD("set_margin", "margin"), &AtlasTexture::set_margin);
	ClassDB::bind_method(D_METHOD("get_margin"), &AtlasTexture::get_margin);

	ClassDB::bind_method(D_METHOD("set_filter_clip", "enable"), &AtlasTexture::set_filter_clip);
	ClassDB::bind_method(D_METHOD("has_filter_clip"), &AtlasTexture::has_filter_clip);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "atlas", PROPERTY_HINT_RESOURCE_TYPE, "Texture2D"), "set_atlas", "get_atlas");
	ADD_PROPERTY(PropertyInfo(Variant::RECT2, "region", PROPERTY_HINT_NONE, "suffix:px"), "set_region", "get_region");
	ADD_PROPERTY(PropertyInfo(Variant::RECT2, "margin", PROPERTY_HINT_NONE, "suffix:px"), "set_margin", "get_margin");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "filter_clip"), "set_filter_clip", "has_filter_clip");
}

void AtlasTexture::draw(RID p_canvas_item, const Point2 &p_pos, const Color &p_modulate, bool p_transpose) const {
	if (!atlas.is_valid()) {
		return;
	}

	Rect2 rc = region;

	if (rc.size.width == 0) {
		rc.size.width = atlas->get_width();
	}

	if (rc.size.height == 0) {
		rc.size.height = atlas->get_height();
	}

	atlas->draw_rect_region(p_canvas_item, Rect2(p_pos + margin.position, rc.size), rc, p_modulate, p_transpose, filter_clip);
}

void AtlasTexture::draw_rect(RID p_canvas_item, const Rect2 &p_rect, bool p_tile, const Color &p_modulate, bool p_transpose) const {
	if (!atlas.is_valid()) {
		return;
	}

	Rect2 rc = region;

	if (rc.size.width == 0) {
		rc.size.width = atlas->get_width();
	}

	if (rc.size.height == 0) {
		rc.size.height = atlas->get_height();
	}

	Vector2 scale = p_rect.size / (region.size + margin.size);
	Rect2 dr(p_rect.position + margin.position * scale, rc.size * scale);

	atlas->draw_rect_region(p_canvas_item, dr, rc, p_modulate, p_transpose, filter_clip);
}

void AtlasTexture::draw_rect_region(RID p_canvas_item, const Rect2 &p_rect, const Rect2 &p_src_rect, const Color &p_modulate, bool p_transpose, bool p_clip_uv) const {
	//this might not necessarily work well if using a rect, needs to be fixed properly
	if (!atlas.is_valid()) {
		return;
	}

	Rect2 dr;
	Rect2 src_c;
	get_rect_region(p_rect, p_src_rect, dr, src_c);

	atlas->draw_rect_region(p_canvas_item, dr, src_c, p_modulate, p_transpose, filter_clip);
}

bool AtlasTexture::get_rect_region(const Rect2 &p_rect, const Rect2 &p_src_rect, Rect2 &r_rect, Rect2 &r_src_rect) const {
	if (!atlas.is_valid()) {
		return false;
	}

	Rect2 src = p_src_rect;
	if (src.size == Size2()) {
		src.size = region.size;
	}
	Vector2 scale = p_rect.size / src.size;

	src.position += (region.position - margin.position);
	Rect2 src_clipped = region.intersection(src);
	if (src_clipped.size == Size2()) {
		return false;
	}

	Vector2 ofs = (src_clipped.position - src.position);
	if (scale.x < 0) {
		ofs.x += (src_clipped.size.x - src.size.x);
	}
	if (scale.y < 0) {
		ofs.y += (src_clipped.size.y - src.size.y);
	}

	r_rect = Rect2(p_rect.position + ofs * scale, src_clipped.size * scale);
	r_src_rect = src_clipped;
	return true;
}

bool AtlasTexture::is_pixel_opaque(int p_x, int p_y) const {
	if (!atlas.is_valid()) {
		return true;
	}

	int x = p_x + region.position.x - margin.position.x;
	int y = p_y + region.position.y - margin.position.y;

	// margin edge may outside of atlas
	if (x < 0 || x >= atlas->get_width()) {
		return false;
	}
	if (y < 0 || y >= atlas->get_height()) {
		return false;
	}

	return atlas->is_pixel_opaque(x, y);
}

Ref<Image> AtlasTexture::get_image() const {
	if (!atlas.is_valid() || !atlas->get_image().is_valid()) {
		return Ref<Image>();
	}

	return atlas->get_image()->get_region(region);
}

AtlasTexture::AtlasTexture() {}

/////////////////////////////////////////

int MeshTexture::get_width() const {
	return size.width;
}

int MeshTexture::get_height() const {
	return size.height;
}

RID MeshTexture::get_rid() const {
	return RID();
}

bool MeshTexture::has_alpha() const {
	return false;
}

void MeshTexture::set_mesh(const Ref<Mesh> &p_mesh) {
	mesh = p_mesh;
}

Ref<Mesh> MeshTexture::get_mesh() const {
	return mesh;
}

void MeshTexture::set_image_size(const Size2 &p_size) {
	size = p_size;
}

Size2 MeshTexture::get_image_size() const {
	return size;
}

void MeshTexture::set_base_texture(const Ref<Texture2D> &p_texture) {
	base_texture = p_texture;
}

Ref<Texture2D> MeshTexture::get_base_texture() const {
	return base_texture;
}

void MeshTexture::draw(RID p_canvas_item, const Point2 &p_pos, const Color &p_modulate, bool p_transpose) const {
	if (mesh.is_null() || base_texture.is_null()) {
		return;
	}
	Transform2D xform;
	xform.set_origin(p_pos);
	if (p_transpose) {
		SWAP(xform.columns[0][1], xform.columns[1][0]);
		SWAP(xform.columns[0][0], xform.columns[1][1]);
	}
	RenderingServer::get_singleton()->canvas_item_add_mesh(p_canvas_item, mesh->get_rid(), xform, p_modulate, base_texture->get_rid());
}

void MeshTexture::draw_rect(RID p_canvas_item, const Rect2 &p_rect, bool p_tile, const Color &p_modulate, bool p_transpose) const {
	if (mesh.is_null() || base_texture.is_null()) {
		return;
	}
	Transform2D xform;
	Vector2 origin = p_rect.position;
	if (p_rect.size.x < 0) {
		origin.x += size.x;
	}
	if (p_rect.size.y < 0) {
		origin.y += size.y;
	}
	xform.set_origin(origin);
	xform.set_scale(p_rect.size / size);

	if (p_transpose) {
		SWAP(xform.columns[0][1], xform.columns[1][0]);
		SWAP(xform.columns[0][0], xform.columns[1][1]);
	}
	RenderingServer::get_singleton()->canvas_item_add_mesh(p_canvas_item, mesh->get_rid(), xform, p_modulate, base_texture->get_rid());
}

void MeshTexture::draw_rect_region(RID p_canvas_item, const Rect2 &p_rect, const Rect2 &p_src_rect, const Color &p_modulate, bool p_transpose, bool p_clip_uv) const {
	if (mesh.is_null() || base_texture.is_null()) {
		return;
	}
	Transform2D xform;
	Vector2 origin = p_rect.position;
	if (p_rect.size.x < 0) {
		origin.x += size.x;
	}
	if (p_rect.size.y < 0) {
		origin.y += size.y;
	}
	xform.set_origin(origin);
	xform.set_scale(p_rect.size / size);

	if (p_transpose) {
		SWAP(xform.columns[0][1], xform.columns[1][0]);
		SWAP(xform.columns[0][0], xform.columns[1][1]);
	}
	RenderingServer::get_singleton()->canvas_item_add_mesh(p_canvas_item, mesh->get_rid(), xform, p_modulate, base_texture->get_rid());
}

bool MeshTexture::get_rect_region(const Rect2 &p_rect, const Rect2 &p_src_rect, Rect2 &r_rect, Rect2 &r_src_rect) const {
	r_rect = p_rect;
	r_src_rect = p_src_rect;
	return true;
}

bool MeshTexture::is_pixel_opaque(int p_x, int p_y) const {
	return true;
}

void MeshTexture::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_mesh", "mesh"), &MeshTexture::set_mesh);
	ClassDB::bind_method(D_METHOD("get_mesh"), &MeshTexture::get_mesh);
	ClassDB::bind_method(D_METHOD("set_image_size", "size"), &MeshTexture::set_image_size);
	ClassDB::bind_method(D_METHOD("get_image_size"), &MeshTexture::get_image_size);
	ClassDB::bind_method(D_METHOD("set_base_texture", "texture"), &MeshTexture::set_base_texture);
	ClassDB::bind_method(D_METHOD("get_base_texture"), &MeshTexture::get_base_texture);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "mesh", PROPERTY_HINT_RESOURCE_TYPE, "Mesh"), "set_mesh", "get_mesh");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "base_texture", PROPERTY_HINT_RESOURCE_TYPE, "Texture2D"), "set_base_texture", "get_base_texture");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "image_size", PROPERTY_HINT_RANGE, "0,16384,1,suffix:px"), "set_image_size", "get_image_size");
}

MeshTexture::MeshTexture() {
}

//////////////////////////////////////////

void CurveTexture::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_width", "width"), &CurveTexture::set_width);

	ClassDB::bind_method(D_METHOD("set_curve", "curve"), &CurveTexture::set_curve);
	ClassDB::bind_method(D_METHOD("get_curve"), &CurveTexture::get_curve);

	ClassDB::bind_method(D_METHOD("set_texture_mode", "texture_mode"), &CurveTexture::set_texture_mode);
	ClassDB::bind_method(D_METHOD("get_texture_mode"), &CurveTexture::get_texture_mode);

	ClassDB::bind_method(D_METHOD("_update"), &CurveTexture::_update);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "width", PROPERTY_HINT_RANGE, "1,4096,suffix:px"), "set_width", "get_width");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "texture_mode", PROPERTY_HINT_ENUM, "RGB,Red"), "set_texture_mode", "get_texture_mode");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "curve", PROPERTY_HINT_RESOURCE_TYPE, "Curve"), "set_curve", "get_curve");

	BIND_ENUM_CONSTANT(TEXTURE_MODE_RGB);
	BIND_ENUM_CONSTANT(TEXTURE_MODE_RED);
}

void CurveTexture::set_width(int p_width) {
	ERR_FAIL_COND(p_width < 32 || p_width > 4096);

	if (_width == p_width) {
		return;
	}

	_width = p_width;
	_update();
}

int CurveTexture::get_width() const {
	return _width;
}

void CurveTexture::ensure_default_setup(float p_min, float p_max) {
	if (_curve.is_null()) {
		Ref<Curve> curve = Ref<Curve>(memnew(Curve));
		curve->add_point(Vector2(0, 1));
		curve->add_point(Vector2(1, 1));
		curve->set_min_value(p_min);
		curve->set_max_value(p_max);
		set_curve(curve);
		// Min and max is 0..1 by default
	}
}

void CurveTexture::set_curve(Ref<Curve> p_curve) {
	if (_curve != p_curve) {
		if (_curve.is_valid()) {
			_curve->disconnect(CoreStringNames::get_singleton()->changed, callable_mp(this, &CurveTexture::_update));
		}
		_curve = p_curve;
		if (_curve.is_valid()) {
			_curve->connect(CoreStringNames::get_singleton()->changed, callable_mp(this, &CurveTexture::_update));
		}
		_update();
	}
}

void CurveTexture::_update() {
	Vector<uint8_t> data;
	data.resize(_width * sizeof(float) * (texture_mode == TEXTURE_MODE_RGB ? 3 : 1));

	// The array is locked in that scope
	{
		uint8_t *wd8 = data.ptrw();
		float *wd = (float *)wd8;

		if (_curve.is_valid()) {
			Curve &curve = **_curve;
			for (int i = 0; i < _width; ++i) {
				float t = i / static_cast<float>(_width);
				if (texture_mode == TEXTURE_MODE_RGB) {
					wd[i * 3 + 0] = curve.sample_baked(t);
					wd[i * 3 + 1] = wd[i * 3 + 0];
					wd[i * 3 + 2] = wd[i * 3 + 0];
				} else {
					wd[i] = curve.sample_baked(t);
				}
			}

		} else {
			for (int i = 0; i < _width; ++i) {
				if (texture_mode == TEXTURE_MODE_RGB) {
					wd[i * 3 + 0] = 0;
					wd[i * 3 + 1] = 0;
					wd[i * 3 + 2] = 0;
				} else {
					wd[i] = 0;
				}
			}
		}
	}

	Ref<Image> image = memnew(Image(_width, 1, false, texture_mode == TEXTURE_MODE_RGB ? Image::FORMAT_RGBF : Image::FORMAT_RF, data));

	if (_texture.is_valid()) {
		if (_current_texture_mode != texture_mode || _current_width != _width) {
			RID new_texture = RS::get_singleton()->texture_2d_create(image);
			RS::get_singleton()->texture_replace(_texture, new_texture);
		} else {
			RS::get_singleton()->texture_2d_update(_texture, image);
		}
	} else {
		_texture = RS::get_singleton()->texture_2d_create(image);
	}
	_current_texture_mode = texture_mode;
	_current_width = _width;

	emit_changed();
}

Ref<Curve> CurveTexture::get_curve() const {
	return _curve;
}

void CurveTexture::set_texture_mode(TextureMode p_mode) {
	ERR_FAIL_COND(p_mode < TEXTURE_MODE_RGB || p_mode > TEXTURE_MODE_RED);
	if (texture_mode == p_mode) {
		return;
	}
	texture_mode = p_mode;
	_update();
}
CurveTexture::TextureMode CurveTexture::get_texture_mode() const {
	return texture_mode;
}

RID CurveTexture::get_rid() const {
	if (!_texture.is_valid()) {
		_texture = RS::get_singleton()->texture_2d_placeholder_create();
	}
	return _texture;
}

CurveTexture::CurveTexture() {}

CurveTexture::~CurveTexture() {
	if (_texture.is_valid()) {
		ERR_FAIL_NULL(RenderingServer::get_singleton());
		RS::get_singleton()->free(_texture);
	}
}

//////////////////

void CurveXYZTexture::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_width", "width"), &CurveXYZTexture::set_width);

	ClassDB::bind_method(D_METHOD("set_curve_x", "curve"), &CurveXYZTexture::set_curve_x);
	ClassDB::bind_method(D_METHOD("get_curve_x"), &CurveXYZTexture::get_curve_x);

	ClassDB::bind_method(D_METHOD("set_curve_y", "curve"), &CurveXYZTexture::set_curve_y);
	ClassDB::bind_method(D_METHOD("get_curve_y"), &CurveXYZTexture::get_curve_y);

	ClassDB::bind_method(D_METHOD("set_curve_z", "curve"), &CurveXYZTexture::set_curve_z);
	ClassDB::bind_method(D_METHOD("get_curve_z"), &CurveXYZTexture::get_curve_z);

	ClassDB::bind_method(D_METHOD("_update"), &CurveXYZTexture::_update);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "width", PROPERTY_HINT_RANGE, "1,4096,suffix:px"), "set_width", "get_width");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "curve_x", PROPERTY_HINT_RESOURCE_TYPE, "Curve"), "set_curve_x", "get_curve_x");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "curve_y", PROPERTY_HINT_RESOURCE_TYPE, "Curve"), "set_curve_y", "get_curve_y");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "curve_z", PROPERTY_HINT_RESOURCE_TYPE, "Curve"), "set_curve_z", "get_curve_z");
}

void CurveXYZTexture::set_width(int p_width) {
	ERR_FAIL_COND(p_width < 32 || p_width > 4096);

	if (_width == p_width) {
		return;
	}

	_width = p_width;
	_update();
}

int CurveXYZTexture::get_width() const {
	return _width;
}

void CurveXYZTexture::ensure_default_setup(float p_min, float p_max) {
	if (_curve_x.is_null()) {
		Ref<Curve> curve = Ref<Curve>(memnew(Curve));
		curve->add_point(Vector2(0, 1));
		curve->add_point(Vector2(1, 1));
		curve->set_min_value(p_min);
		curve->set_max_value(p_max);
		set_curve_x(curve);
	}

	if (_curve_y.is_null()) {
		Ref<Curve> curve = Ref<Curve>(memnew(Curve));
		curve->add_point(Vector2(0, 1));
		curve->add_point(Vector2(1, 1));
		curve->set_min_value(p_min);
		curve->set_max_value(p_max);
		set_curve_y(curve);
	}

	if (_curve_z.is_null()) {
		Ref<Curve> curve = Ref<Curve>(memnew(Curve));
		curve->add_point(Vector2(0, 1));
		curve->add_point(Vector2(1, 1));
		curve->set_min_value(p_min);
		curve->set_max_value(p_max);
		set_curve_z(curve);
	}
}

void CurveXYZTexture::set_curve_x(Ref<Curve> p_curve) {
	if (_curve_x != p_curve) {
		if (_curve_x.is_valid()) {
			_curve_x->disconnect(CoreStringNames::get_singleton()->changed, callable_mp(this, &CurveXYZTexture::_update));
		}
		_curve_x = p_curve;
		if (_curve_x.is_valid()) {
			_curve_x->connect(CoreStringNames::get_singleton()->changed, callable_mp(this, &CurveXYZTexture::_update), CONNECT_REFERENCE_COUNTED);
		}
		_update();
	}
}

void CurveXYZTexture::set_curve_y(Ref<Curve> p_curve) {
	if (_curve_y != p_curve) {
		if (_curve_y.is_valid()) {
			_curve_y->disconnect(CoreStringNames::get_singleton()->changed, callable_mp(this, &CurveXYZTexture::_update));
		}
		_curve_y = p_curve;
		if (_curve_y.is_valid()) {
			_curve_y->connect(CoreStringNames::get_singleton()->changed, callable_mp(this, &CurveXYZTexture::_update), CONNECT_REFERENCE_COUNTED);
		}
		_update();
	}
}

void CurveXYZTexture::set_curve_z(Ref<Curve> p_curve) {
	if (_curve_z != p_curve) {
		if (_curve_z.is_valid()) {
			_curve_z->disconnect(CoreStringNames::get_singleton()->changed, callable_mp(this, &CurveXYZTexture::_update));
		}
		_curve_z = p_curve;
		if (_curve_z.is_valid()) {
			_curve_z->connect(CoreStringNames::get_singleton()->changed, callable_mp(this, &CurveXYZTexture::_update), CONNECT_REFERENCE_COUNTED);
		}
		_update();
	}
}

void CurveXYZTexture::_update() {
	Vector<uint8_t> data;
	data.resize(_width * sizeof(float) * 3);

	// The array is locked in that scope
	{
		uint8_t *wd8 = data.ptrw();
		float *wd = (float *)wd8;

		if (_curve_x.is_valid()) {
			Curve &curve_x = **_curve_x;
			for (int i = 0; i < _width; ++i) {
				float t = i / static_cast<float>(_width);
				wd[i * 3 + 0] = curve_x.sample_baked(t);
			}

		} else {
			for (int i = 0; i < _width; ++i) {
				wd[i * 3 + 0] = 0;
			}
		}

		if (_curve_y.is_valid()) {
			Curve &curve_y = **_curve_y;
			for (int i = 0; i < _width; ++i) {
				float t = i / static_cast<float>(_width);
				wd[i * 3 + 1] = curve_y.sample_baked(t);
			}

		} else {
			for (int i = 0; i < _width; ++i) {
				wd[i * 3 + 1] = 0;
			}
		}

		if (_curve_z.is_valid()) {
			Curve &curve_z = **_curve_z;
			for (int i = 0; i < _width; ++i) {
				float t = i / static_cast<float>(_width);
				wd[i * 3 + 2] = curve_z.sample_baked(t);
			}

		} else {
			for (int i = 0; i < _width; ++i) {
				wd[i * 3 + 2] = 0;
			}
		}
	}

	Ref<Image> image = memnew(Image(_width, 1, false, Image::FORMAT_RGBF, data));

	if (_texture.is_valid()) {
		if (_current_width != _width) {
			RID new_texture = RS::get_singleton()->texture_2d_create(image);
			RS::get_singleton()->texture_replace(_texture, new_texture);
		} else {
			RS::get_singleton()->texture_2d_update(_texture, image);
		}
	} else {
		_texture = RS::get_singleton()->texture_2d_create(image);
	}
	_current_width = _width;

	emit_changed();
}

Ref<Curve> CurveXYZTexture::get_curve_x() const {
	return _curve_x;
}

Ref<Curve> CurveXYZTexture::get_curve_y() const {
	return _curve_y;
}

Ref<Curve> CurveXYZTexture::get_curve_z() const {
	return _curve_z;
}

RID CurveXYZTexture::get_rid() const {
	if (!_texture.is_valid()) {
		_texture = RS::get_singleton()->texture_2d_placeholder_create();
	}
	return _texture;
}

CurveXYZTexture::CurveXYZTexture() {}

CurveXYZTexture::~CurveXYZTexture() {
	if (_texture.is_valid()) {
		ERR_FAIL_NULL(RenderingServer::get_singleton());
		RS::get_singleton()->free(_texture);
	}
}

//////////////////

GradientTexture1D::GradientTexture1D() {
	_queue_update();
}

GradientTexture1D::~GradientTexture1D() {
	if (texture.is_valid()) {
		ERR_FAIL_NULL(RenderingServer::get_singleton());
		RS::get_singleton()->free(texture);
	}
}

void GradientTexture1D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_gradient", "gradient"), &GradientTexture1D::set_gradient);
	ClassDB::bind_method(D_METHOD("get_gradient"), &GradientTexture1D::get_gradient);

	ClassDB::bind_method(D_METHOD("set_width", "width"), &GradientTexture1D::set_width);
	// The `get_width()` method is already exposed by the parent class Texture2D.

	ClassDB::bind_method(D_METHOD("set_use_hdr", "enabled"), &GradientTexture1D::set_use_hdr);
	ClassDB::bind_method(D_METHOD("is_using_hdr"), &GradientTexture1D::is_using_hdr);

	ClassDB::bind_method(D_METHOD("_update"), &GradientTexture1D::_update);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "gradient", PROPERTY_HINT_RESOURCE_TYPE, "Gradient", PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_EDITOR_INSTANTIATE_OBJECT), "set_gradient", "get_gradient");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "width", PROPERTY_HINT_RANGE, "1,16384,suffix:px"), "set_width", "get_width");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "use_hdr"), "set_use_hdr", "is_using_hdr");
}

void GradientTexture1D::set_gradient(Ref<Gradient> p_gradient) {
	if (p_gradient == gradient) {
		return;
	}
	if (gradient.is_valid()) {
		gradient->disconnect(CoreStringNames::get_singleton()->changed, callable_mp(this, &GradientTexture1D::_update));
	}
	gradient = p_gradient;
	if (gradient.is_valid()) {
		gradient->connect(CoreStringNames::get_singleton()->changed, callable_mp(this, &GradientTexture1D::_update));
	}
	_update();
	emit_changed();
}

Ref<Gradient> GradientTexture1D::get_gradient() const {
	return gradient;
}

void GradientTexture1D::_queue_update() {
	if (update_pending) {
		return;
	}

	update_pending = true;
	call_deferred(SNAME("_update"));
}

void GradientTexture1D::_update() {
	update_pending = false;

	if (gradient.is_null()) {
		return;
	}

	if (use_hdr) {
		// High dynamic range.
		Ref<Image> image = memnew(Image(width, 1, false, Image::FORMAT_RGBAF));
		Gradient &g = **gradient;
		// `create()` isn't available for non-uint8_t data, so fill in the data manually.
		for (int i = 0; i < width; i++) {
			float ofs = float(i) / (width - 1);
			image->set_pixel(i, 0, g.get_color_at_offset(ofs));
		}

		if (texture.is_valid()) {
			RID new_texture = RS::get_singleton()->texture_2d_create(image);
			RS::get_singleton()->texture_replace(texture, new_texture);
		} else {
			texture = RS::get_singleton()->texture_2d_create(image);
		}
	} else {
		// Low dynamic range. "Overbright" colors will be clamped.
		Vector<uint8_t> data;
		data.resize(width * 4);
		{
			uint8_t *wd8 = data.ptrw();
			Gradient &g = **gradient;

			for (int i = 0; i < width; i++) {
				float ofs = float(i) / (width - 1);
				Color color = g.get_color_at_offset(ofs);

				wd8[i * 4 + 0] = uint8_t(CLAMP(color.r * 255.0, 0, 255));
				wd8[i * 4 + 1] = uint8_t(CLAMP(color.g * 255.0, 0, 255));
				wd8[i * 4 + 2] = uint8_t(CLAMP(color.b * 255.0, 0, 255));
				wd8[i * 4 + 3] = uint8_t(CLAMP(color.a * 255.0, 0, 255));
			}
		}

		Ref<Image> image = memnew(Image(width, 1, false, Image::FORMAT_RGBA8, data));

		if (texture.is_valid()) {
			RID new_texture = RS::get_singleton()->texture_2d_create(image);
			RS::get_singleton()->texture_replace(texture, new_texture);
		} else {
			texture = RS::get_singleton()->texture_2d_create(image);
		}
	}

	emit_changed();
}

void GradientTexture1D::set_width(int p_width) {
	ERR_FAIL_COND_MSG(p_width <= 0 || p_width > 16384, "Texture dimensions have to be within 1 to 16384 range.");
	width = p_width;
	_queue_update();
}

int GradientTexture1D::get_width() const {
	return width;
}

void GradientTexture1D::set_use_hdr(bool p_enabled) {
	if (p_enabled == use_hdr) {
		return;
	}

	use_hdr = p_enabled;
	_queue_update();
}

bool GradientTexture1D::is_using_hdr() const {
	return use_hdr;
}

Ref<Image> GradientTexture1D::get_image() const {
	if (!texture.is_valid()) {
		return Ref<Image>();
	}
	return RenderingServer::get_singleton()->texture_2d_get(texture);
}

//////////////////

GradientTexture2D::GradientTexture2D() {
	_queue_update();
}

GradientTexture2D::~GradientTexture2D() {
	if (texture.is_valid()) {
		ERR_FAIL_NULL(RenderingServer::get_singleton());
		RS::get_singleton()->free(texture);
	}
}

void GradientTexture2D::set_gradient(Ref<Gradient> p_gradient) {
	if (gradient == p_gradient) {
		return;
	}
	if (gradient.is_valid()) {
		gradient->disconnect(CoreStringNames::get_singleton()->changed, callable_mp(this, &GradientTexture2D::_queue_update));
	}
	gradient = p_gradient;
	if (gradient.is_valid()) {
		gradient->connect(CoreStringNames::get_singleton()->changed, callable_mp(this, &GradientTexture2D::_queue_update));
	}
	_update();
	emit_changed();
}

Ref<Gradient> GradientTexture2D::get_gradient() const {
	return gradient;
}

void GradientTexture2D::_queue_update() {
	if (update_pending) {
		return;
	}
	update_pending = true;
	call_deferred(SNAME("_update"));
}

void GradientTexture2D::_update() {
	update_pending = false;

	if (gradient.is_null()) {
		return;
	}
	Ref<Image> image;
	image.instantiate();

	if (gradient->get_point_count() <= 1) { // No need to interpolate.
		image->initialize_data(width, height, false, (use_hdr) ? Image::FORMAT_RGBAF : Image::FORMAT_RGBA8);
		image->fill((gradient->get_point_count() == 1) ? gradient->get_color(0) : Color(0, 0, 0, 1));
	} else {
		if (use_hdr) {
			image->initialize_data(width, height, false, Image::FORMAT_RGBAF);
			Gradient &g = **gradient;
			// `create()` isn't available for non-uint8_t data, so fill in the data manually.
			for (int y = 0; y < height; y++) {
				for (int x = 0; x < width; x++) {
					float ofs = _get_gradient_offset_at(x, y);
					image->set_pixel(x, y, g.get_color_at_offset(ofs));
				}
			}
		} else {
			Vector<uint8_t> data;
			data.resize(width * height * 4);
			{
				uint8_t *wd8 = data.ptrw();
				Gradient &g = **gradient;
				for (int y = 0; y < height; y++) {
					for (int x = 0; x < width; x++) {
						float ofs = _get_gradient_offset_at(x, y);
						const Color &c = g.get_color_at_offset(ofs);

						wd8[(x + (y * width)) * 4 + 0] = uint8_t(CLAMP(c.r * 255.0, 0, 255));
						wd8[(x + (y * width)) * 4 + 1] = uint8_t(CLAMP(c.g * 255.0, 0, 255));
						wd8[(x + (y * width)) * 4 + 2] = uint8_t(CLAMP(c.b * 255.0, 0, 255));
						wd8[(x + (y * width)) * 4 + 3] = uint8_t(CLAMP(c.a * 255.0, 0, 255));
					}
				}
			}
			image->set_data(width, height, false, Image::FORMAT_RGBA8, data);
		}
	}

	if (texture.is_valid()) {
		RID new_texture = RS::get_singleton()->texture_2d_create(image);
		RS::get_singleton()->texture_replace(texture, new_texture);
	} else {
		texture = RS::get_singleton()->texture_2d_create(image);
	}
	emit_changed();
}

float GradientTexture2D::_get_gradient_offset_at(int x, int y) const {
	if (fill_to == fill_from) {
		return 0;
	}
	float ofs = 0;
	Vector2 pos;
	if (width > 1) {
		pos.x = static_cast<float>(x) / (width - 1);
	}
	if (height > 1) {
		pos.y = static_cast<float>(y) / (height - 1);
	}
	if (fill == Fill::FILL_LINEAR) {
		Vector2 segment[2];
		segment[0] = fill_from;
		segment[1] = fill_to;
		Vector2 closest = Geometry2D::get_closest_point_to_segment_uncapped(pos, &segment[0]);
		ofs = (closest - fill_from).length() / (fill_to - fill_from).length();
		if ((closest - fill_from).dot(fill_to - fill_from) < 0) {
			ofs *= -1;
		}
	} else if (fill == Fill::FILL_RADIAL) {
		ofs = (pos - fill_from).length() / (fill_to - fill_from).length();
	} else if (fill == Fill::FILL_SQUARE) {
		ofs = MAX(Math::abs(pos.x - fill_from.x), Math::abs(pos.y - fill_from.y)) / MAX(Math::abs(fill_to.x - fill_from.x), Math::abs(fill_to.y - fill_from.y));
	}
	if (repeat == Repeat::REPEAT_NONE) {
		ofs = CLAMP(ofs, 0.0, 1.0);
	} else if (repeat == Repeat::REPEAT) {
		ofs = Math::fmod(ofs, 1.0f);
		if (ofs < 0) {
			ofs = 1 + ofs;
		}
	} else if (repeat == Repeat::REPEAT_MIRROR) {
		ofs = Math::abs(ofs);
		ofs = Math::fmod(ofs, 2.0f);
		if (ofs > 1.0) {
			ofs = 2.0 - ofs;
		}
	}
	return ofs;
}

void GradientTexture2D::set_width(int p_width) {
	ERR_FAIL_COND_MSG(p_width <= 0 || p_width > 16384, "Texture dimensions have to be within 1 to 16384 range.");
	width = p_width;
	_queue_update();
}

int GradientTexture2D::get_width() const {
	return width;
}

void GradientTexture2D::set_height(int p_height) {
	ERR_FAIL_COND_MSG(p_height <= 0 || p_height > 16384, "Texture dimensions have to be within 1 to 16384 range.");
	height = p_height;
	_queue_update();
}
int GradientTexture2D::get_height() const {
	return height;
}

void GradientTexture2D::set_use_hdr(bool p_enabled) {
	if (p_enabled == use_hdr) {
		return;
	}

	use_hdr = p_enabled;
	_queue_update();
}

bool GradientTexture2D::is_using_hdr() const {
	return use_hdr;
}

void GradientTexture2D::set_fill_from(Vector2 p_fill_from) {
	fill_from = p_fill_from;
	_queue_update();
}

Vector2 GradientTexture2D::get_fill_from() const {
	return fill_from;
}

void GradientTexture2D::set_fill_to(Vector2 p_fill_to) {
	fill_to = p_fill_to;
	_queue_update();
}

Vector2 GradientTexture2D::get_fill_to() const {
	return fill_to;
}

void GradientTexture2D::set_fill(Fill p_fill) {
	fill = p_fill;
	_queue_update();
}

GradientTexture2D::Fill GradientTexture2D::get_fill() const {
	return fill;
}

void GradientTexture2D::set_repeat(Repeat p_repeat) {
	repeat = p_repeat;
	_queue_update();
}

GradientTexture2D::Repeat GradientTexture2D::get_repeat() const {
	return repeat;
}

RID GradientTexture2D::get_rid() const {
	if (!texture.is_valid()) {
		texture = RS::get_singleton()->texture_2d_placeholder_create();
	}
	return texture;
}

Ref<Image> GradientTexture2D::get_image() const {
	if (!texture.is_valid()) {
		return Ref<Image>();
	}
	return RenderingServer::get_singleton()->texture_2d_get(texture);
}

void GradientTexture2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_gradient", "gradient"), &GradientTexture2D::set_gradient);
	ClassDB::bind_method(D_METHOD("get_gradient"), &GradientTexture2D::get_gradient);

	ClassDB::bind_method(D_METHOD("set_width", "width"), &GradientTexture2D::set_width);
	ClassDB::bind_method(D_METHOD("set_height", "height"), &GradientTexture2D::set_height);

	ClassDB::bind_method(D_METHOD("set_use_hdr", "enabled"), &GradientTexture2D::set_use_hdr);
	ClassDB::bind_method(D_METHOD("is_using_hdr"), &GradientTexture2D::is_using_hdr);

	ClassDB::bind_method(D_METHOD("set_fill", "fill"), &GradientTexture2D::set_fill);
	ClassDB::bind_method(D_METHOD("get_fill"), &GradientTexture2D::get_fill);
	ClassDB::bind_method(D_METHOD("set_fill_from", "fill_from"), &GradientTexture2D::set_fill_from);
	ClassDB::bind_method(D_METHOD("get_fill_from"), &GradientTexture2D::get_fill_from);
	ClassDB::bind_method(D_METHOD("set_fill_to", "fill_to"), &GradientTexture2D::set_fill_to);
	ClassDB::bind_method(D_METHOD("get_fill_to"), &GradientTexture2D::get_fill_to);

	ClassDB::bind_method(D_METHOD("set_repeat", "repeat"), &GradientTexture2D::set_repeat);
	ClassDB::bind_method(D_METHOD("get_repeat"), &GradientTexture2D::get_repeat);

	ClassDB::bind_method(D_METHOD("_update"), &GradientTexture2D::_update);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "gradient", PROPERTY_HINT_RESOURCE_TYPE, "Gradient", PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_EDITOR_INSTANTIATE_OBJECT), "set_gradient", "get_gradient");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "width", PROPERTY_HINT_RANGE, "1,2048,or_greater,suffix:px"), "set_width", "get_width");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "height", PROPERTY_HINT_RANGE, "1,2048,or_greater,suffix:px"), "set_height", "get_height");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "use_hdr"), "set_use_hdr", "is_using_hdr");

	ADD_GROUP("Fill", "fill_");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "fill", PROPERTY_HINT_ENUM, "Linear,Radial,Square"), "set_fill", "get_fill");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "fill_from"), "set_fill_from", "get_fill_from");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "fill_to"), "set_fill_to", "get_fill_to");

	ADD_GROUP("Repeat", "repeat_");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "repeat", PROPERTY_HINT_ENUM, "No Repeat,Repeat,Mirror Repeat"), "set_repeat", "get_repeat");

	BIND_ENUM_CONSTANT(FILL_LINEAR);
	BIND_ENUM_CONSTANT(FILL_RADIAL);
	BIND_ENUM_CONSTANT(FILL_SQUARE);

	BIND_ENUM_CONSTANT(REPEAT_NONE);
	BIND_ENUM_CONSTANT(REPEAT);
	BIND_ENUM_CONSTANT(REPEAT_MIRROR);
}

//////////////////////////////////////

void AnimatedTexture::_update_proxy() {
	RWLockRead r(rw_lock);

	float delta;
	if (prev_ticks == 0) {
		delta = 0;
		prev_ticks = OS::get_singleton()->get_ticks_usec();
	} else {
		uint64_t ticks = OS::get_singleton()->get_ticks_usec();
		delta = float(double(ticks - prev_ticks) / 1000000.0);
		prev_ticks = ticks;
	}

	time += delta;

	float speed = speed_scale == 0 ? 0 : abs(1.0 / speed_scale);

	int iter_max = frame_count;
	while (iter_max && !pause) {
		float frame_limit = frames[current_frame].duration * speed;

		if (time > frame_limit) {
			if (speed_scale > 0.0) {
				current_frame++;
			} else {
				current_frame--;
			}
			if (current_frame >= frame_count) {
				if (one_shot) {
					current_frame = frame_count - 1;
				} else {
					current_frame = 0;
				}
			} else if (current_frame < 0) {
				if (one_shot) {
					current_frame = 0;
				} else {
					current_frame = frame_count - 1;
				}
			}
			time -= frame_limit;

		} else {
			break;
		}
		iter_max--;
	}

	if (frames[current_frame].texture.is_valid()) {
		RenderingServer::get_singleton()->texture_proxy_update(proxy, frames[current_frame].texture->get_rid());
	}
}

void AnimatedTexture::set_frames(int p_frames) {
	ERR_FAIL_COND(p_frames < 1 || p_frames > MAX_FRAMES);

	RWLockWrite r(rw_lock);

	frame_count = p_frames;
}

int AnimatedTexture::get_frames() const {
	return frame_count;
}

void AnimatedTexture::set_current_frame(int p_frame) {
	ERR_FAIL_COND(p_frame < 0 || p_frame >= frame_count);

	RWLockWrite r(rw_lock);

	current_frame = p_frame;
	time = 0;
}

int AnimatedTexture::get_current_frame() const {
	return current_frame;
}

void AnimatedTexture::set_pause(bool p_pause) {
	RWLockWrite r(rw_lock);
	pause = p_pause;
}

bool AnimatedTexture::get_pause() const {
	return pause;
}

void AnimatedTexture::set_one_shot(bool p_one_shot) {
	RWLockWrite r(rw_lock);
	one_shot = p_one_shot;
}

bool AnimatedTexture::get_one_shot() const {
	return one_shot;
}

void AnimatedTexture::set_frame_texture(int p_frame, const Ref<Texture2D> &p_texture) {
	ERR_FAIL_COND(p_texture == this);
	ERR_FAIL_INDEX(p_frame, MAX_FRAMES);

	RWLockWrite w(rw_lock);

	frames[p_frame].texture = p_texture;
}

Ref<Texture2D> AnimatedTexture::get_frame_texture(int p_frame) const {
	ERR_FAIL_INDEX_V(p_frame, MAX_FRAMES, Ref<Texture2D>());

	RWLockRead r(rw_lock);

	return frames[p_frame].texture;
}

void AnimatedTexture::set_frame_duration(int p_frame, float p_duration) {
	ERR_FAIL_INDEX(p_frame, MAX_FRAMES);

	RWLockWrite r(rw_lock);

	frames[p_frame].duration = p_duration;
}

float AnimatedTexture::get_frame_duration(int p_frame) const {
	ERR_FAIL_INDEX_V(p_frame, MAX_FRAMES, 0);

	RWLockRead r(rw_lock);

	return frames[p_frame].duration;
}

void AnimatedTexture::set_speed_scale(float p_scale) {
	ERR_FAIL_COND(p_scale < -1000 || p_scale >= 1000);

	RWLockWrite r(rw_lock);

	speed_scale = p_scale;
}

float AnimatedTexture::get_speed_scale() const {
	return speed_scale;
}

int AnimatedTexture::get_width() const {
	RWLockRead r(rw_lock);

	if (!frames[current_frame].texture.is_valid()) {
		return 1;
	}

	return frames[current_frame].texture->get_width();
}

int AnimatedTexture::get_height() const {
	RWLockRead r(rw_lock);

	if (!frames[current_frame].texture.is_valid()) {
		return 1;
	}

	return frames[current_frame].texture->get_height();
}

RID AnimatedTexture::get_rid() const {
	return proxy;
}

bool AnimatedTexture::has_alpha() const {
	RWLockRead r(rw_lock);

	if (!frames[current_frame].texture.is_valid()) {
		return false;
	}

	return frames[current_frame].texture->has_alpha();
}

Ref<Image> AnimatedTexture::get_image() const {
	RWLockRead r(rw_lock);

	if (!frames[current_frame].texture.is_valid()) {
		return Ref<Image>();
	}

	return frames[current_frame].texture->get_image();
}

bool AnimatedTexture::is_pixel_opaque(int p_x, int p_y) const {
	RWLockRead r(rw_lock);

	if (frames[current_frame].texture.is_valid()) {
		return frames[current_frame].texture->is_pixel_opaque(p_x, p_y);
	}
	return true;
}

void AnimatedTexture::_validate_property(PropertyInfo &p_property) const {
	String prop = p_property.name;
	if (prop.begins_with("frame_")) {
		int frame = prop.get_slicec('/', 0).get_slicec('_', 1).to_int();
		if (frame >= frame_count) {
			p_property.usage = PROPERTY_USAGE_NONE;
		}
	}
}

void AnimatedTexture::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_frames", "frames"), &AnimatedTexture::set_frames);
	ClassDB::bind_method(D_METHOD("get_frames"), &AnimatedTexture::get_frames);

	ClassDB::bind_method(D_METHOD("set_current_frame", "frame"), &AnimatedTexture::set_current_frame);
	ClassDB::bind_method(D_METHOD("get_current_frame"), &AnimatedTexture::get_current_frame);

	ClassDB::bind_method(D_METHOD("set_pause", "pause"), &AnimatedTexture::set_pause);
	ClassDB::bind_method(D_METHOD("get_pause"), &AnimatedTexture::get_pause);

	ClassDB::bind_method(D_METHOD("set_one_shot", "one_shot"), &AnimatedTexture::set_one_shot);
	ClassDB::bind_method(D_METHOD("get_one_shot"), &AnimatedTexture::get_one_shot);

	ClassDB::bind_method(D_METHOD("set_speed_scale", "scale"), &AnimatedTexture::set_speed_scale);
	ClassDB::bind_method(D_METHOD("get_speed_scale"), &AnimatedTexture::get_speed_scale);

	ClassDB::bind_method(D_METHOD("set_frame_texture", "frame", "texture"), &AnimatedTexture::set_frame_texture);
	ClassDB::bind_method(D_METHOD("get_frame_texture", "frame"), &AnimatedTexture::get_frame_texture);

	ClassDB::bind_method(D_METHOD("set_frame_duration", "frame", "duration"), &AnimatedTexture::set_frame_duration);
	ClassDB::bind_method(D_METHOD("get_frame_duration", "frame"), &AnimatedTexture::get_frame_duration);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "frames", PROPERTY_HINT_RANGE, "1," + itos(MAX_FRAMES), PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_UPDATE_ALL_IF_MODIFIED), "set_frames", "get_frames");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "current_frame", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "set_current_frame", "get_current_frame");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "pause"), "set_pause", "get_pause");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "one_shot"), "set_one_shot", "get_one_shot");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "speed_scale", PROPERTY_HINT_RANGE, "-60,60,0.1,or_less,or_greater"), "set_speed_scale", "get_speed_scale");

	for (int i = 0; i < MAX_FRAMES; i++) {
		ADD_PROPERTYI(PropertyInfo(Variant::OBJECT, "frame_" + itos(i) + "/texture", PROPERTY_HINT_RESOURCE_TYPE, "Texture2D", PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_INTERNAL), "set_frame_texture", "get_frame_texture", i);
		ADD_PROPERTYI(PropertyInfo(Variant::FLOAT, "frame_" + itos(i) + "/duration", PROPERTY_HINT_RANGE, "0.0,16.0,0.01,or_greater,suffix:s", PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_INTERNAL), "set_frame_duration", "get_frame_duration", i);
	}

	BIND_CONSTANT(MAX_FRAMES);
}

AnimatedTexture::AnimatedTexture() {
	//proxy = RS::get_singleton()->texture_create();
	proxy_ph = RS::get_singleton()->texture_2d_placeholder_create();
	proxy = RS::get_singleton()->texture_proxy_create(proxy_ph);

	RenderingServer::get_singleton()->texture_set_force_redraw_if_visible(proxy, true);
	RenderingServer::get_singleton()->connect("frame_pre_draw", callable_mp(this, &AnimatedTexture::_update_proxy));
}

AnimatedTexture::~AnimatedTexture() {
	ERR_FAIL_NULL(RenderingServer::get_singleton());
	RS::get_singleton()->free(proxy);
	RS::get_singleton()->free(proxy_ph);
}

///////////////////////////////

Image::Format TextureLayered::get_format() const {
	Image::Format ret = Image::FORMAT_MAX;
	GDVIRTUAL_REQUIRED_CALL(_get_format, ret);
	return ret;
}

TextureLayered::LayeredType TextureLayered::get_layered_type() const {
	uint32_t ret = LAYERED_TYPE_2D_ARRAY;
	GDVIRTUAL_REQUIRED_CALL(_get_layered_type, ret);
	return (LayeredType)ret;
}

int TextureLayered::get_width() const {
	int ret = 0;
	GDVIRTUAL_REQUIRED_CALL(_get_width, ret);
	return ret;
}

int TextureLayered::get_height() const {
	int ret = 0;
	GDVIRTUAL_REQUIRED_CALL(_get_height, ret);
	return ret;
}

int TextureLayered::get_layers() const {
	int ret = 0;
	GDVIRTUAL_REQUIRED_CALL(_get_layers, ret);
	return ret;
}

bool TextureLayered::has_mipmaps() const {
	bool ret = false;
	GDVIRTUAL_REQUIRED_CALL(_has_mipmaps, ret);
	return ret;
}

Ref<Image> TextureLayered::get_layer_data(int p_layer) const {
	Ref<Image> ret;
	GDVIRTUAL_REQUIRED_CALL(_get_layer_data, p_layer, ret);
	return ret;
}

void TextureLayered::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_format"), &TextureLayered::get_format);
	ClassDB::bind_method(D_METHOD("get_layered_type"), &TextureLayered::get_layered_type);
	ClassDB::bind_method(D_METHOD("get_width"), &TextureLayered::get_width);
	ClassDB::bind_method(D_METHOD("get_height"), &TextureLayered::get_height);
	ClassDB::bind_method(D_METHOD("get_layers"), &TextureLayered::get_layers);
	ClassDB::bind_method(D_METHOD("has_mipmaps"), &TextureLayered::has_mipmaps);
	ClassDB::bind_method(D_METHOD("get_layer_data", "layer"), &TextureLayered::get_layer_data);

	BIND_ENUM_CONSTANT(LAYERED_TYPE_2D_ARRAY);
	BIND_ENUM_CONSTANT(LAYERED_TYPE_CUBEMAP);
	BIND_ENUM_CONSTANT(LAYERED_TYPE_CUBEMAP_ARRAY);

	GDVIRTUAL_BIND(_get_format);
	GDVIRTUAL_BIND(_get_layered_type);
	GDVIRTUAL_BIND(_get_width);
	GDVIRTUAL_BIND(_get_height);
	GDVIRTUAL_BIND(_get_layers);
	GDVIRTUAL_BIND(_has_mipmaps);
	GDVIRTUAL_BIND(_get_layer_data, "layer_index");
}

///////////////////////////////
Image::Format ImageTextureLayered::get_format() const {
	return format;
}

int ImageTextureLayered::get_width() const {
	return width;
}

int ImageTextureLayered::get_height() const {
	return height;
}

int ImageTextureLayered::get_layers() const {
	return layers;
}

bool ImageTextureLayered::has_mipmaps() const {
	return mipmaps;
}

ImageTextureLayered::LayeredType ImageTextureLayered::get_layered_type() const {
	return layered_type;
}

Error ImageTextureLayered::_create_from_images(const TypedArray<Image> &p_images) {
	Vector<Ref<Image>> images;
	for (int i = 0; i < p_images.size(); i++) {
		Ref<Image> img = p_images[i];
		ERR_FAIL_COND_V(img.is_null(), ERR_INVALID_PARAMETER);
		images.push_back(img);
	}

	return create_from_images(images);
}

TypedArray<Image> ImageTextureLayered::_get_images() const {
	TypedArray<Image> images;
	for (int i = 0; i < layers; i++) {
		images.push_back(get_layer_data(i));
	}
	return images;
}

void ImageTextureLayered::_set_images(const TypedArray<Image> &p_images) {
	ERR_FAIL_COND(_create_from_images(p_images) != OK);
}

Error ImageTextureLayered::create_from_images(Vector<Ref<Image>> p_images) {
	int new_layers = p_images.size();
	ERR_FAIL_COND_V(new_layers == 0, ERR_INVALID_PARAMETER);
	if (layered_type == LAYERED_TYPE_CUBEMAP) {
		ERR_FAIL_COND_V_MSG(new_layers != 6, ERR_INVALID_PARAMETER,
				"Cubemaps require exactly 6 layers");
	} else if (layered_type == LAYERED_TYPE_CUBEMAP_ARRAY) {
		ERR_FAIL_COND_V_MSG((new_layers % 6) != 0, ERR_INVALID_PARAMETER,
				"Cubemap array layers must be a multiple of 6");
	}

	ERR_FAIL_COND_V(p_images[0].is_null() || p_images[0]->is_empty(), ERR_INVALID_PARAMETER);

	Image::Format new_format = p_images[0]->get_format();
	int new_width = p_images[0]->get_width();
	int new_height = p_images[0]->get_height();
	bool new_mipmaps = p_images[0]->has_mipmaps();

	for (int i = 1; i < p_images.size(); i++) {
		ERR_FAIL_COND_V_MSG(p_images[i]->get_format() != new_format, ERR_INVALID_PARAMETER,
				"All images must share the same format");
		ERR_FAIL_COND_V_MSG(p_images[i]->get_width() != new_width || p_images[i]->get_height() != new_height, ERR_INVALID_PARAMETER,
				"All images must share the same dimensions");
		ERR_FAIL_COND_V_MSG(p_images[i]->has_mipmaps() != new_mipmaps, ERR_INVALID_PARAMETER,
				"All images must share the usage of mipmaps");
	}

	if (texture.is_valid()) {
		RID new_texture = RS::get_singleton()->texture_2d_layered_create(p_images, RS::TextureLayeredType(layered_type));
		ERR_FAIL_COND_V(!new_texture.is_valid(), ERR_CANT_CREATE);
		RS::get_singleton()->texture_replace(texture, new_texture);
	} else {
		texture = RS::get_singleton()->texture_2d_layered_create(p_images, RS::TextureLayeredType(layered_type));
		ERR_FAIL_COND_V(!texture.is_valid(), ERR_CANT_CREATE);
	}

	format = new_format;
	width = new_width;
	height = new_height;
	layers = new_layers;
	mipmaps = new_mipmaps;
	return OK;
}

void ImageTextureLayered::update_layer(const Ref<Image> &p_image, int p_layer) {
	ERR_FAIL_COND_MSG(texture.is_null(), "Texture is not initialized.");
	ERR_FAIL_COND_MSG(p_image.is_null(), "Invalid image.");
	ERR_FAIL_COND_MSG(p_image->get_format() != format, "Image format must match texture's image format.");
	ERR_FAIL_COND_MSG(p_image->get_width() != width || p_image->get_height() != height, "Image size must match texture's image size.");
	ERR_FAIL_COND_MSG(p_image->has_mipmaps() != mipmaps, "Image mipmap configuration must match texture's image mipmap configuration.");
	ERR_FAIL_INDEX_MSG(p_layer, layers, "Layer index is out of bounds.");
	RS::get_singleton()->texture_2d_update(texture, p_image, p_layer);
}

Ref<Image> ImageTextureLayered::get_layer_data(int p_layer) const {
	ERR_FAIL_INDEX_V(p_layer, layers, Ref<Image>());
	return RS::get_singleton()->texture_2d_layer_get(texture, p_layer);
}

RID ImageTextureLayered::get_rid() const {
	if (texture.is_null()) {
		texture = RS::get_singleton()->texture_2d_layered_placeholder_create(RS::TextureLayeredType(layered_type));
	}
	return texture;
}

void ImageTextureLayered::set_path(const String &p_path, bool p_take_over) {
	if (texture.is_valid()) {
		RS::get_singleton()->texture_set_path(texture, p_path);
	}

	Resource::set_path(p_path, p_take_over);
}

void ImageTextureLayered::_bind_methods() {
	ClassDB::bind_method(D_METHOD("create_from_images", "images"), &ImageTextureLayered::_create_from_images);
	ClassDB::bind_method(D_METHOD("update_layer", "image", "layer"), &ImageTextureLayered::update_layer);

	ClassDB::bind_method(D_METHOD("_get_images"), &ImageTextureLayered::_get_images);
	ClassDB::bind_method(D_METHOD("_set_images", "images"), &ImageTextureLayered::_set_images);

	ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "_images", PROPERTY_HINT_ARRAY_TYPE, "Image", PROPERTY_USAGE_INTERNAL), "_set_images", "_get_images");
}

ImageTextureLayered::ImageTextureLayered(LayeredType p_layered_type) {
	layered_type = p_layered_type;
}

ImageTextureLayered::~ImageTextureLayered() {
	if (texture.is_valid()) {
		ERR_FAIL_NULL(RenderingServer::get_singleton());
		RS::get_singleton()->free(texture);
	}
}

void Texture2DArray::_bind_methods() {
	ClassDB::bind_method(D_METHOD("create_placeholder"), &Texture2DArray::create_placeholder);
}

Ref<Resource> Texture2DArray::create_placeholder() const {
	Ref<PlaceholderTexture2DArray> placeholder;
	placeholder.instantiate();
	placeholder->set_size(Size2i(get_width(), get_height()));
	placeholder->set_layers(get_layers());
	return placeholder;
}

void Cubemap::_bind_methods() {
	ClassDB::bind_method(D_METHOD("create_placeholder"), &Cubemap::create_placeholder);
}

Ref<Resource> Cubemap::create_placeholder() const {
	Ref<PlaceholderCubemap> placeholder;
	placeholder.instantiate();
	placeholder->set_size(Size2i(get_width(), get_height()));
	placeholder->set_layers(get_layers());
	return placeholder;
}

void CubemapArray::_bind_methods() {
	ClassDB::bind_method(D_METHOD("create_placeholder"), &CubemapArray::create_placeholder);
}

Ref<Resource> CubemapArray::create_placeholder() const {
	Ref<PlaceholderCubemapArray> placeholder;
	placeholder.instantiate();
	placeholder->set_size(Size2i(get_width(), get_height()));
	placeholder->set_layers(get_layers());
	return placeholder;
}

///////////////////////////////////////////

void CompressedTextureLayered::set_path(const String &p_path, bool p_take_over) {
	if (texture.is_valid()) {
		RenderingServer::get_singleton()->texture_set_path(texture, p_path);
	}

	Resource::set_path(p_path, p_take_over);
}

Image::Format CompressedTextureLayered::get_format() const {
	return format;
}

Error CompressedTextureLayered::_load_data(const String &p_path, Vector<Ref<Image>> &images, int &mipmap_limit, int p_size_limit) {
	ERR_FAIL_COND_V(images.size() != 0, ERR_INVALID_PARAMETER);

	Ref<FileAccess> f = FileAccess::open(p_path, FileAccess::READ);
	ERR_FAIL_COND_V_MSG(f.is_null(), ERR_CANT_OPEN, vformat("Unable to open file: %s.", p_path));

	uint8_t header[4];
	f->get_buffer(header, 4);
	if (header[0] != 'G' || header[1] != 'S' || header[2] != 'T' || header[3] != 'L') {
		ERR_FAIL_V_MSG(ERR_FILE_CORRUPT, "Compressed texture layered file is corrupt (Bad header).");
	}

	uint32_t version = f->get_32();

	if (version > FORMAT_VERSION) {
		ERR_FAIL_V_MSG(ERR_FILE_CORRUPT, "Compressed texture file is too new.");
	}

	uint32_t layer_count = f->get_32(); //layer count
	uint32_t type = f->get_32(); //layer count
	ERR_FAIL_COND_V((int)type != layered_type, ERR_INVALID_DATA);

	uint32_t df = f->get_32(); //data format
	mipmap_limit = int(f->get_32());
	//reserved
	f->get_32();
	f->get_32();
	f->get_32();

	if (!(df & FORMAT_BIT_STREAM)) {
		p_size_limit = 0;
	}

	images.resize(layer_count);

	for (uint32_t i = 0; i < layer_count; i++) {
		Ref<Image> image = CompressedTexture2D::load_image_from_file(f, p_size_limit);
		ERR_FAIL_COND_V(image.is_null() || image->is_empty(), ERR_CANT_OPEN);
		images.write[i] = image;
	}

	return OK;
}

Error CompressedTextureLayered::load(const String &p_path) {
	Vector<Ref<Image>> images;

	int mipmap_limit;

	Error err = _load_data(p_path, images, mipmap_limit);
	if (err) {
		return err;
	}

	if (texture.is_valid()) {
		RID new_texture = RS::get_singleton()->texture_2d_layered_create(images, RS::TextureLayeredType(layered_type));
		RS::get_singleton()->texture_replace(texture, new_texture);
	} else {
		texture = RS::get_singleton()->texture_2d_layered_create(images, RS::TextureLayeredType(layered_type));
	}

	w = images[0]->get_width();
	h = images[0]->get_height();
	mipmaps = images[0]->has_mipmaps();
	format = images[0]->get_format();
	layers = images.size();

	path_to_file = p_path;

	if (get_path().is_empty()) {
		//temporarily set path if no path set for resource, helps find errors
		RenderingServer::get_singleton()->texture_set_path(texture, p_path);
	}

	notify_property_list_changed();
	emit_changed();
	return OK;
}

String CompressedTextureLayered::get_load_path() const {
	return path_to_file;
}

int CompressedTextureLayered::get_width() const {
	return w;
}

int CompressedTextureLayered::get_height() const {
	return h;
}

int CompressedTextureLayered::get_layers() const {
	return layers;
}

bool CompressedTextureLayered::has_mipmaps() const {
	return mipmaps;
}

TextureLayered::LayeredType CompressedTextureLayered::get_layered_type() const {
	return layered_type;
}

RID CompressedTextureLayered::get_rid() const {
	if (!texture.is_valid()) {
		texture = RS::get_singleton()->texture_2d_layered_placeholder_create(RS::TextureLayeredType(layered_type));
	}
	return texture;
}

Ref<Image> CompressedTextureLayered::get_layer_data(int p_layer) const {
	if (texture.is_valid()) {
		return RS::get_singleton()->texture_2d_layer_get(texture, p_layer);
	} else {
		return Ref<Image>();
	}
}

void CompressedTextureLayered::reload_from_file() {
	String path = get_path();
	if (!path.is_resource_file()) {
		return;
	}

	path = ResourceLoader::path_remap(path); //remap for translation
	path = ResourceLoader::import_remap(path); //remap for import
	if (!path.is_resource_file()) {
		return;
	}

	load(path);
}

void CompressedTextureLayered::_validate_property(PropertyInfo &p_property) const {
}

void CompressedTextureLayered::_bind_methods() {
	ClassDB::bind_method(D_METHOD("load", "path"), &CompressedTextureLayered::load);
	ClassDB::bind_method(D_METHOD("get_load_path"), &CompressedTextureLayered::get_load_path);

	ADD_PROPERTY(PropertyInfo(Variant::STRING, "load_path", PROPERTY_HINT_FILE, "*.ctex"), "load", "get_load_path");
}

CompressedTextureLayered::CompressedTextureLayered(LayeredType p_type) {
	layered_type = p_type;
}

CompressedTextureLayered::~CompressedTextureLayered() {
	if (texture.is_valid()) {
		ERR_FAIL_NULL(RenderingServer::get_singleton());
		RS::get_singleton()->free(texture);
	}
}

/////////////////////////////////////////////////

Ref<Resource> ResourceFormatLoaderCompressedTextureLayered::load(const String &p_path, const String &p_original_path, Error *r_error, bool p_use_sub_threads, float *r_progress, CacheMode p_cache_mode) {
	Ref<CompressedTextureLayered> ct;
	if (p_path.get_extension().to_lower() == "ctexarray") {
		Ref<CompressedTexture2DArray> c;
		c.instantiate();
		ct = c;
	} else if (p_path.get_extension().to_lower() == "ccube") {
		Ref<CompressedCubemap> c;
		c.instantiate();
		ct = c;
	} else if (p_path.get_extension().to_lower() == "ccubearray") {
		Ref<CompressedCubemapArray> c;
		c.instantiate();
		ct = c;
	} else {
		if (r_error) {
			*r_error = ERR_FILE_UNRECOGNIZED;
		}
		return Ref<Resource>();
	}
	Error err = ct->load(p_path);
	if (r_error) {
		*r_error = err;
	}
	if (err != OK) {
		return Ref<Resource>();
	}

	return ct;
}

void ResourceFormatLoaderCompressedTextureLayered::get_recognized_extensions(List<String> *p_extensions) const {
	p_extensions->push_back("ctexarray");
	p_extensions->push_back("ccube");
	p_extensions->push_back("ccubearray");
}

bool ResourceFormatLoaderCompressedTextureLayered::handles_type(const String &p_type) const {
	return p_type == "CompressedTexture2DArray" || p_type == "CompressedCubemap" || p_type == "CompressedCubemapArray";
}

String ResourceFormatLoaderCompressedTextureLayered::get_resource_type(const String &p_path) const {
	if (p_path.get_extension().to_lower() == "ctexarray") {
		return "CompressedTexture2DArray";
	}
	if (p_path.get_extension().to_lower() == "ccube") {
		return "CompressedCubemap";
	}
	if (p_path.get_extension().to_lower() == "ccubearray") {
		return "CompressedCubemapArray";
	}
	return "";
}

///////////////////////////////

void CameraTexture::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_camera_feed_id", "feed_id"), &CameraTexture::set_camera_feed_id);
	ClassDB::bind_method(D_METHOD("get_camera_feed_id"), &CameraTexture::get_camera_feed_id);

	ClassDB::bind_method(D_METHOD("set_which_feed", "which_feed"), &CameraTexture::set_which_feed);
	ClassDB::bind_method(D_METHOD("get_which_feed"), &CameraTexture::get_which_feed);

	ClassDB::bind_method(D_METHOD("set_camera_active", "active"), &CameraTexture::set_camera_active);
	ClassDB::bind_method(D_METHOD("get_camera_active"), &CameraTexture::get_camera_active);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "camera_feed_id"), "set_camera_feed_id", "get_camera_feed_id");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "which_feed"), "set_which_feed", "get_which_feed");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "camera_is_active"), "set_camera_active", "get_camera_active");
}

int CameraTexture::get_width() const {
	Ref<CameraFeed> feed = CameraServer::get_singleton()->get_feed_by_id(camera_feed_id);
	if (feed.is_valid()) {
		return feed->get_base_width();
	} else {
		return 0;
	}
}

int CameraTexture::get_height() const {
	Ref<CameraFeed> feed = CameraServer::get_singleton()->get_feed_by_id(camera_feed_id);
	if (feed.is_valid()) {
		return feed->get_base_height();
	} else {
		return 0;
	}
}

bool CameraTexture::has_alpha() const {
	return false;
}

RID CameraTexture::get_rid() const {
	Ref<CameraFeed> feed = CameraServer::get_singleton()->get_feed_by_id(camera_feed_id);
	if (feed.is_valid()) {
		return feed->get_texture(which_feed);
	} else {
		if (_texture.is_null()) {
			_texture = RenderingServer::get_singleton()->texture_2d_placeholder_create();
		}
		return _texture;
	}
}

Ref<Image> CameraTexture::get_image() const {
	// not (yet) supported
	return Ref<Image>();
}

void CameraTexture::set_camera_feed_id(int p_new_id) {
	camera_feed_id = p_new_id;
	notify_property_list_changed();
}

int CameraTexture::get_camera_feed_id() const {
	return camera_feed_id;
}

void CameraTexture::set_which_feed(CameraServer::FeedImage p_which) {
	which_feed = p_which;
	notify_property_list_changed();
}

CameraServer::FeedImage CameraTexture::get_which_feed() const {
	return which_feed;
}

void CameraTexture::set_camera_active(bool p_active) {
	Ref<CameraFeed> feed = CameraServer::get_singleton()->get_feed_by_id(camera_feed_id);
	if (feed.is_valid()) {
		feed->set_active(p_active);
		notify_property_list_changed();
	}
}

bool CameraTexture::get_camera_active() const {
	Ref<CameraFeed> feed = CameraServer::get_singleton()->get_feed_by_id(camera_feed_id);
	if (feed.is_valid()) {
		return feed->is_active();
	} else {
		return false;
	}
}

CameraTexture::CameraTexture() {}

CameraTexture::~CameraTexture() {
	if (_texture.is_valid()) {
		ERR_FAIL_NULL(RenderingServer::get_singleton());
		RenderingServer::get_singleton()->free(_texture);
	}
}

///////////////////////////

void PlaceholderTexture2D::set_size(Size2 p_size) {
	size = p_size;
}

int PlaceholderTexture2D::get_width() const {
	return size.width;
}

int PlaceholderTexture2D::get_height() const {
	return size.height;
}

bool PlaceholderTexture2D::has_alpha() const {
	return false;
}

Ref<Image> PlaceholderTexture2D::get_image() const {
	return Ref<Image>();
}

RID PlaceholderTexture2D::get_rid() const {
	return rid;
}

void PlaceholderTexture2D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_size", "size"), &PlaceholderTexture2D::set_size);

	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "size", PROPERTY_HINT_NONE, "suffix:px"), "set_size", "get_size");
}

PlaceholderTexture2D::PlaceholderTexture2D() {
	rid = RS::get_singleton()->texture_2d_placeholder_create();
}

PlaceholderTexture2D::~PlaceholderTexture2D() {
	ERR_FAIL_NULL(RenderingServer::get_singleton());
	RS::get_singleton()->free(rid);
}

///////////////////////////////////////////////

void PlaceholderTexture3D::set_size(const Vector3i &p_size) {
	size = p_size;
}

Vector3i PlaceholderTexture3D::get_size() const {
	return size;
}

Image::Format PlaceholderTexture3D::get_format() const {
	return Image::FORMAT_RGB8;
}

int PlaceholderTexture3D::get_width() const {
	return size.x;
}

int PlaceholderTexture3D::get_height() const {
	return size.y;
}

int PlaceholderTexture3D::get_depth() const {
	return size.z;
}

bool PlaceholderTexture3D::has_mipmaps() const {
	return false;
}

Vector<Ref<Image>> PlaceholderTexture3D::get_data() const {
	return Vector<Ref<Image>>();
}

void PlaceholderTexture3D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_size", "size"), &PlaceholderTexture3D::set_size);
	ClassDB::bind_method(D_METHOD("get_size"), &PlaceholderTexture3D::get_size);
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3I, "size", PROPERTY_HINT_NONE, "suffix:px"), "set_size", "get_size");
}

PlaceholderTexture3D::PlaceholderTexture3D() {
	rid = RS::get_singleton()->texture_3d_placeholder_create();
}
PlaceholderTexture3D::~PlaceholderTexture3D() {
	ERR_FAIL_NULL(RenderingServer::get_singleton());
	RS::get_singleton()->free(rid);
}

/////////////////////////////////////////////////

void PlaceholderTextureLayered::set_size(const Size2i &p_size) {
	size = p_size;
}

Size2i PlaceholderTextureLayered::get_size() const {
	return size;
}

void PlaceholderTextureLayered::set_layers(int p_layers) {
	layers = p_layers;
}

Image::Format PlaceholderTextureLayered::get_format() const {
	return Image::FORMAT_RGB8;
}

TextureLayered::LayeredType PlaceholderTextureLayered::get_layered_type() const {
	return layered_type;
}

int PlaceholderTextureLayered::get_width() const {
	return size.x;
}

int PlaceholderTextureLayered::get_height() const {
	return size.y;
}

int PlaceholderTextureLayered::get_layers() const {
	return layers;
}

bool PlaceholderTextureLayered::has_mipmaps() const {
	return false;
}

Ref<Image> PlaceholderTextureLayered::get_layer_data(int p_layer) const {
	return Ref<Image>();
}

void PlaceholderTextureLayered::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_size", "size"), &PlaceholderTextureLayered::set_size);
	ClassDB::bind_method(D_METHOD("get_size"), &PlaceholderTextureLayered::get_size);
	ClassDB::bind_method(D_METHOD("set_layers", "layers"), &PlaceholderTextureLayered::set_layers);
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2I, "size", PROPERTY_HINT_NONE, "suffix:px"), "set_size", "get_size");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "layers", PROPERTY_HINT_RANGE, "1,4096"), "set_layers", "get_layers");
}

PlaceholderTextureLayered::PlaceholderTextureLayered(LayeredType p_type) {
	layered_type = p_type;
	rid = RS::get_singleton()->texture_2d_layered_placeholder_create(RS::TextureLayeredType(layered_type));
}
PlaceholderTextureLayered::~PlaceholderTextureLayered() {
	ERR_FAIL_NULL(RenderingServer::get_singleton());
	RS::get_singleton()->free(rid);
}
