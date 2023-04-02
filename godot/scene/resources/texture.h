/**************************************************************************/
/*  texture.h                                                             */
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

#ifndef TEXTURE_H
#define TEXTURE_H

#include "core/io/file_access.h"
#include "core/io/resource.h"
#include "core/io/resource_loader.h"
#include "core/math/rect2.h"
#include "core/os/mutex.h"
#include "core/os/rw_lock.h"
#include "core/os/thread_safe.h"
#include "scene/resources/curve.h"
#include "scene/resources/gradient.h"
#include "servers/camera_server.h"
#include "servers/rendering_server.h"

class Texture : public Resource {
	GDCLASS(Texture, Resource);

public:
	Texture() {}
};

class Texture2D : public Texture {
	GDCLASS(Texture2D, Texture);
	OBJ_SAVE_TYPE(Texture2D); // Saves derived classes with common type so they can be interchanged.

protected:
	static void _bind_methods();

	GDVIRTUAL0RC(int, _get_width)
	GDVIRTUAL0RC(int, _get_height)
	GDVIRTUAL2RC(bool, _is_pixel_opaque, int, int)
	GDVIRTUAL0RC(bool, _has_alpha)

	GDVIRTUAL4C(_draw, RID, Point2, Color, bool)
	GDVIRTUAL5C(_draw_rect, RID, Rect2, bool, Color, bool)
	GDVIRTUAL6C(_draw_rect_region, RID, Rect2, Rect2, Color, bool, bool)

public:
	virtual int get_width() const;
	virtual int get_height() const;
	virtual Size2 get_size() const;

	virtual bool is_pixel_opaque(int p_x, int p_y) const;

	virtual bool has_alpha() const;

	virtual void draw(RID p_canvas_item, const Point2 &p_pos, const Color &p_modulate = Color(1, 1, 1), bool p_transpose = false) const;
	virtual void draw_rect(RID p_canvas_item, const Rect2 &p_rect, bool p_tile = false, const Color &p_modulate = Color(1, 1, 1), bool p_transpose = false) const;
	virtual void draw_rect_region(RID p_canvas_item, const Rect2 &p_rect, const Rect2 &p_src_rect, const Color &p_modulate = Color(1, 1, 1), bool p_transpose = false, bool p_clip_uv = true) const;
	virtual bool get_rect_region(const Rect2 &p_rect, const Rect2 &p_src_rect, Rect2 &r_rect, Rect2 &r_src_rect) const;

	virtual Ref<Image> get_image() const { return Ref<Image>(); }

	virtual Ref<Resource> create_placeholder() const;

	Texture2D();
};

class BitMap;

class ImageTexture : public Texture2D {
	GDCLASS(ImageTexture, Texture2D);
	RES_BASE_EXTENSION("tex");

	mutable RID texture;
	Image::Format format = Image::FORMAT_L8;
	bool mipmaps = false;
	int w = 0;
	int h = 0;
	Size2 size_override;
	mutable Ref<BitMap> alpha_cache;
	bool image_stored = false;

protected:
	virtual void reload_from_file() override;

	bool _set(const StringName &p_name, const Variant &p_value);
	bool _get(const StringName &p_name, Variant &r_ret) const;
	void _get_property_list(List<PropertyInfo> *p_list) const;

	static void _bind_methods();

public:
	void set_image(const Ref<Image> &p_image);
	static Ref<ImageTexture> create_from_image(const Ref<Image> &p_image);

	Image::Format get_format() const;

	void update(const Ref<Image> &p_image);
	Ref<Image> get_image() const override;

	int get_width() const override;
	int get_height() const override;

	virtual RID get_rid() const override;

	bool has_alpha() const override;
	virtual void draw(RID p_canvas_item, const Point2 &p_pos, const Color &p_modulate = Color(1, 1, 1), bool p_transpose = false) const override;
	virtual void draw_rect(RID p_canvas_item, const Rect2 &p_rect, bool p_tile = false, const Color &p_modulate = Color(1, 1, 1), bool p_transpose = false) const override;
	virtual void draw_rect_region(RID p_canvas_item, const Rect2 &p_rect, const Rect2 &p_src_rect, const Color &p_modulate = Color(1, 1, 1), bool p_transpose = false, bool p_clip_uv = true) const override;

	bool is_pixel_opaque(int p_x, int p_y) const override;

	void set_size_override(const Size2i &p_size);

	virtual void set_path(const String &p_path, bool p_take_over = false) override;

	ImageTexture();
	~ImageTexture();
};

class PortableCompressedTexture2D : public Texture2D {
	GDCLASS(PortableCompressedTexture2D, Texture2D);

public:
	enum CompressionMode {
		COMPRESSION_MODE_LOSSLESS,
		COMPRESSION_MODE_LOSSY,
		COMPRESSION_MODE_BASIS_UNIVERSAL,
		COMPRESSION_MODE_S3TC,
		COMPRESSION_MODE_ETC2,
		COMPRESSION_MODE_BPTC,
	};

private:
	CompressionMode compression_mode = COMPRESSION_MODE_LOSSLESS;
	static bool keep_all_compressed_buffers;
	bool keep_compressed_buffer = false;
	Vector<uint8_t> compressed_buffer;
	Size2 size;
	Size2 size_override;
	bool mipmaps = false;
	Image::Format format = Image::FORMAT_L8;

	mutable RID texture;
	mutable Ref<BitMap> alpha_cache;

	bool image_stored = false;

protected:
	Vector<uint8_t> _get_data() const;
	void _set_data(const Vector<uint8_t> &p_data);

	static void _bind_methods();

public:
	CompressionMode get_compression_mode() const;
	void create_from_image(const Ref<Image> &p_image, CompressionMode p_compression_mode, bool p_normal_map = false, float p_lossy_quality = 0.8);

	Image::Format get_format() const;

	void update(const Ref<Image> &p_image);
	Ref<Image> get_image() const override;

	int get_width() const override;
	int get_height() const override;

	virtual RID get_rid() const override;

	bool has_alpha() const override;
	virtual void draw(RID p_canvas_item, const Point2 &p_pos, const Color &p_modulate = Color(1, 1, 1), bool p_transpose = false) const override;
	virtual void draw_rect(RID p_canvas_item, const Rect2 &p_rect, bool p_tile = false, const Color &p_modulate = Color(1, 1, 1), bool p_transpose = false) const override;
	virtual void draw_rect_region(RID p_canvas_item, const Rect2 &p_rect, const Rect2 &p_src_rect, const Color &p_modulate = Color(1, 1, 1), bool p_transpose = false, bool p_clip_uv = true) const override;

	bool is_pixel_opaque(int p_x, int p_y) const override;

	virtual void set_path(const String &p_path, bool p_take_over = false) override;

	void set_size_override(const Size2 &p_size);
	Size2 get_size_override() const;

	void set_keep_compressed_buffer(bool p_keep);
	bool is_keeping_compressed_buffer() const;

	static void set_keep_all_compressed_buffers(bool p_keep);
	static bool is_keeping_all_compressed_buffers();

	PortableCompressedTexture2D();
	~PortableCompressedTexture2D();
};

VARIANT_ENUM_CAST(PortableCompressedTexture2D::CompressionMode)

class CompressedTexture2D : public Texture2D {
	GDCLASS(CompressedTexture2D, Texture2D);

public:
	enum DataFormat {
		DATA_FORMAT_IMAGE,
		DATA_FORMAT_PNG,
		DATA_FORMAT_WEBP,
		DATA_FORMAT_BASIS_UNIVERSAL,
	};

	enum {
		FORMAT_VERSION = 1
	};

	enum FormatBits {
		FORMAT_BIT_STREAM = 1 << 22,
		FORMAT_BIT_HAS_MIPMAPS = 1 << 23,
		FORMAT_BIT_DETECT_3D = 1 << 24,
		//FORMAT_BIT_DETECT_SRGB = 1 << 25,
		FORMAT_BIT_DETECT_NORMAL = 1 << 26,
		FORMAT_BIT_DETECT_ROUGNESS = 1 << 27,
	};

private:
	Error _load_data(const String &p_path, int &r_width, int &r_height, Ref<Image> &image, bool &r_request_3d, bool &r_request_normal, bool &r_request_roughness, int &mipmap_limit, int p_size_limit = 0);
	String path_to_file;
	mutable RID texture;
	Image::Format format = Image::FORMAT_L8;
	int w = 0;
	int h = 0;
	mutable Ref<BitMap> alpha_cache;

	virtual void reload_from_file() override;

	static void _requested_3d(void *p_ud);
	static void _requested_roughness(void *p_ud, const String &p_normal_path, RS::TextureDetectRoughnessChannel p_roughness_channel);
	static void _requested_normal(void *p_ud);

protected:
	static void _bind_methods();
	void _validate_property(PropertyInfo &p_property) const;

public:
	static Ref<Image> load_image_from_file(Ref<FileAccess> p_file, int p_size_limit);

	typedef void (*TextureFormatRequestCallback)(const Ref<CompressedTexture2D> &);
	typedef void (*TextureFormatRoughnessRequestCallback)(const Ref<CompressedTexture2D> &, const String &p_normal_path, RS::TextureDetectRoughnessChannel p_roughness_channel);

	static TextureFormatRequestCallback request_3d_callback;
	static TextureFormatRoughnessRequestCallback request_roughness_callback;
	static TextureFormatRequestCallback request_normal_callback;

	Image::Format get_format() const;
	Error load(const String &p_path);
	String get_load_path() const;

	int get_width() const override;
	int get_height() const override;
	virtual RID get_rid() const override;

	virtual void set_path(const String &p_path, bool p_take_over) override;

	virtual void draw(RID p_canvas_item, const Point2 &p_pos, const Color &p_modulate = Color(1, 1, 1), bool p_transpose = false) const override;
	virtual void draw_rect(RID p_canvas_item, const Rect2 &p_rect, bool p_tile = false, const Color &p_modulate = Color(1, 1, 1), bool p_transpose = false) const override;
	virtual void draw_rect_region(RID p_canvas_item, const Rect2 &p_rect, const Rect2 &p_src_rect, const Color &p_modulate = Color(1, 1, 1), bool p_transpose = false, bool p_clip_uv = true) const override;

	virtual bool has_alpha() const override;
	bool is_pixel_opaque(int p_x, int p_y) const override;

	virtual Ref<Image> get_image() const override;

	CompressedTexture2D();
	~CompressedTexture2D();
};

class ResourceFormatLoaderCompressedTexture2D : public ResourceFormatLoader {
public:
	virtual Ref<Resource> load(const String &p_path, const String &p_original_path = "", Error *r_error = nullptr, bool p_use_sub_threads = false, float *r_progress = nullptr, CacheMode p_cache_mode = CACHE_MODE_REUSE);
	virtual void get_recognized_extensions(List<String> *p_extensions) const;
	virtual bool handles_type(const String &p_type) const;
	virtual String get_resource_type(const String &p_path) const;
};

class AtlasTexture : public Texture2D {
	GDCLASS(AtlasTexture, Texture2D);
	RES_BASE_EXTENSION("atlastex");

protected:
	Ref<Texture2D> atlas;
	Rect2 region;
	Rect2 margin;
	bool filter_clip = false;

	static void _bind_methods();

public:
	virtual int get_width() const override;
	virtual int get_height() const override;
	virtual RID get_rid() const override;

	virtual bool has_alpha() const override;

	void set_atlas(const Ref<Texture2D> &p_atlas);
	Ref<Texture2D> get_atlas() const;

	void set_region(const Rect2 &p_region);
	Rect2 get_region() const;

	void set_margin(const Rect2 &p_margin);
	Rect2 get_margin() const;

	void set_filter_clip(const bool p_enable);
	bool has_filter_clip() const;

	virtual void draw(RID p_canvas_item, const Point2 &p_pos, const Color &p_modulate = Color(1, 1, 1), bool p_transpose = false) const override;
	virtual void draw_rect(RID p_canvas_item, const Rect2 &p_rect, bool p_tile = false, const Color &p_modulate = Color(1, 1, 1), bool p_transpose = false) const override;
	virtual void draw_rect_region(RID p_canvas_item, const Rect2 &p_rect, const Rect2 &p_src_rect, const Color &p_modulate = Color(1, 1, 1), bool p_transpose = false, bool p_clip_uv = true) const override;
	virtual bool get_rect_region(const Rect2 &p_rect, const Rect2 &p_src_rect, Rect2 &r_rect, Rect2 &r_src_rect) const override;

	bool is_pixel_opaque(int p_x, int p_y) const override;

	virtual Ref<Image> get_image() const override;

	AtlasTexture();
};

class Mesh;

class MeshTexture : public Texture2D {
	GDCLASS(MeshTexture, Texture2D);
	RES_BASE_EXTENSION("meshtex");

	Ref<Texture2D> base_texture;
	Ref<Mesh> mesh;
	Size2i size;

protected:
	static void _bind_methods();

public:
	virtual int get_width() const override;
	virtual int get_height() const override;
	virtual RID get_rid() const override;

	virtual bool has_alpha() const override;

	void set_mesh(const Ref<Mesh> &p_mesh);
	Ref<Mesh> get_mesh() const;

	void set_image_size(const Size2 &p_size);
	Size2 get_image_size() const;

	void set_base_texture(const Ref<Texture2D> &p_texture);
	Ref<Texture2D> get_base_texture() const;

	virtual void draw(RID p_canvas_item, const Point2 &p_pos, const Color &p_modulate = Color(1, 1, 1), bool p_transpose = false) const override;
	virtual void draw_rect(RID p_canvas_item, const Rect2 &p_rect, bool p_tile = false, const Color &p_modulate = Color(1, 1, 1), bool p_transpose = false) const override;
	virtual void draw_rect_region(RID p_canvas_item, const Rect2 &p_rect, const Rect2 &p_src_rect, const Color &p_modulate = Color(1, 1, 1), bool p_transpose = false, bool p_clip_uv = true) const override;
	virtual bool get_rect_region(const Rect2 &p_rect, const Rect2 &p_src_rect, Rect2 &r_rect, Rect2 &r_src_rect) const override;

	bool is_pixel_opaque(int p_x, int p_y) const override;

	MeshTexture();
};

class TextureLayered : public Texture {
	GDCLASS(TextureLayered, Texture);

protected:
	static void _bind_methods();

	GDVIRTUAL0RC(Image::Format, _get_format)
	GDVIRTUAL0RC(uint32_t, _get_layered_type)
	GDVIRTUAL0RC(int, _get_width)
	GDVIRTUAL0RC(int, _get_height)
	GDVIRTUAL0RC(int, _get_layers)
	GDVIRTUAL0RC(bool, _has_mipmaps)
	GDVIRTUAL1RC(Ref<Image>, _get_layer_data, int)
public:
	enum LayeredType {
		LAYERED_TYPE_2D_ARRAY,
		LAYERED_TYPE_CUBEMAP,
		LAYERED_TYPE_CUBEMAP_ARRAY
	};

	virtual Image::Format get_format() const;
	virtual LayeredType get_layered_type() const;
	virtual int get_width() const;
	virtual int get_height() const;
	virtual int get_layers() const;
	virtual bool has_mipmaps() const;
	virtual Ref<Image> get_layer_data(int p_layer) const;

	TextureLayered() {}
};

VARIANT_ENUM_CAST(TextureLayered::LayeredType)

class ImageTextureLayered : public TextureLayered {
	GDCLASS(ImageTextureLayered, TextureLayered);

	LayeredType layered_type;

	mutable RID texture;
	Image::Format format = Image::FORMAT_L8;

	int width = 0;
	int height = 0;
	int layers = 0;
	bool mipmaps = false;

	Error _create_from_images(const TypedArray<Image> &p_images);

	TypedArray<Image> _get_images() const;

protected:
	static void _bind_methods();

public:
	virtual Image::Format get_format() const override;
	virtual int get_width() const override;
	virtual int get_height() const override;
	virtual int get_layers() const override;
	virtual bool has_mipmaps() const override;
	virtual LayeredType get_layered_type() const override;

	Error create_from_images(Vector<Ref<Image>> p_images);
	void update_layer(const Ref<Image> &p_image, int p_layer);
	virtual Ref<Image> get_layer_data(int p_layer) const override;

	virtual RID get_rid() const override;
	virtual void set_path(const String &p_path, bool p_take_over = false) override;

	ImageTextureLayered(LayeredType p_layered_type);
	~ImageTextureLayered();
};

class Texture2DArray : public ImageTextureLayered {
	GDCLASS(Texture2DArray, ImageTextureLayered)

protected:
	static void _bind_methods();

public:
	Texture2DArray() :
			ImageTextureLayered(LAYERED_TYPE_2D_ARRAY) {}

	virtual Ref<Resource> create_placeholder() const;
};

class Cubemap : public ImageTextureLayered {
	GDCLASS(Cubemap, ImageTextureLayered);

protected:
	static void _bind_methods();

public:
	Cubemap() :
			ImageTextureLayered(LAYERED_TYPE_CUBEMAP) {}

	virtual Ref<Resource> create_placeholder() const;
};

class CubemapArray : public ImageTextureLayered {
	GDCLASS(CubemapArray, ImageTextureLayered);

protected:
	static void _bind_methods();

public:
	CubemapArray() :
			ImageTextureLayered(LAYERED_TYPE_CUBEMAP_ARRAY) {}

	virtual Ref<Resource> create_placeholder() const;
};

class CompressedTextureLayered : public TextureLayered {
	GDCLASS(CompressedTextureLayered, TextureLayered);

public:
	enum DataFormat {
		DATA_FORMAT_IMAGE,
		DATA_FORMAT_PNG,
		DATA_FORMAT_WEBP,
		DATA_FORMAT_BASIS_UNIVERSAL,
	};

	enum {
		FORMAT_VERSION = 1
	};

	enum FormatBits {
		FORMAT_BIT_STREAM = 1 << 22,
		FORMAT_BIT_HAS_MIPMAPS = 1 << 23,
	};

private:
	Error _load_data(const String &p_path, Vector<Ref<Image>> &images, int &mipmap_limit, int p_size_limit = 0);
	String path_to_file;
	mutable RID texture;
	Image::Format format = Image::FORMAT_L8;
	int w = 0;
	int h = 0;
	int layers = 0;
	bool mipmaps = false;
	LayeredType layered_type = LayeredType::LAYERED_TYPE_2D_ARRAY;

	virtual void reload_from_file() override;

protected:
	static void _bind_methods();
	void _validate_property(PropertyInfo &p_property) const;

public:
	Image::Format get_format() const override;
	Error load(const String &p_path);
	String get_load_path() const;
	virtual LayeredType get_layered_type() const override;

	int get_width() const override;
	int get_height() const override;
	int get_layers() const override;
	virtual bool has_mipmaps() const override;
	virtual RID get_rid() const override;

	virtual void set_path(const String &p_path, bool p_take_over) override;

	virtual Ref<Image> get_layer_data(int p_layer) const override;

	CompressedTextureLayered(LayeredType p_layered_type);
	~CompressedTextureLayered();
};

class CompressedTexture2DArray : public CompressedTextureLayered {
	GDCLASS(CompressedTexture2DArray, CompressedTextureLayered)
public:
	CompressedTexture2DArray() :
			CompressedTextureLayered(LAYERED_TYPE_2D_ARRAY) {}
};

class CompressedCubemap : public CompressedTextureLayered {
	GDCLASS(CompressedCubemap, CompressedTextureLayered);

public:
	CompressedCubemap() :
			CompressedTextureLayered(LAYERED_TYPE_CUBEMAP) {}
};

class CompressedCubemapArray : public CompressedTextureLayered {
	GDCLASS(CompressedCubemapArray, CompressedTextureLayered);

public:
	CompressedCubemapArray() :
			CompressedTextureLayered(LAYERED_TYPE_CUBEMAP_ARRAY) {}
};

class ResourceFormatLoaderCompressedTextureLayered : public ResourceFormatLoader {
public:
	virtual Ref<Resource> load(const String &p_path, const String &p_original_path = "", Error *r_error = nullptr, bool p_use_sub_threads = false, float *r_progress = nullptr, CacheMode p_cache_mode = CACHE_MODE_REUSE);
	virtual void get_recognized_extensions(List<String> *p_extensions) const;
	virtual bool handles_type(const String &p_type) const;
	virtual String get_resource_type(const String &p_path) const;
};

class Texture3D : public Texture {
	GDCLASS(Texture3D, Texture);

protected:
	static void _bind_methods();

	TypedArray<Image> _get_datai() const;

	GDVIRTUAL0RC(Image::Format, _get_format)
	GDVIRTUAL0RC(int, _get_width)
	GDVIRTUAL0RC(int, _get_height)
	GDVIRTUAL0RC(int, _get_depth)
	GDVIRTUAL0RC(bool, _has_mipmaps)
	GDVIRTUAL0RC(TypedArray<Image>, _get_data)
public:
	virtual Image::Format get_format() const;
	virtual int get_width() const;
	virtual int get_height() const;
	virtual int get_depth() const;
	virtual bool has_mipmaps() const;
	virtual Vector<Ref<Image>> get_data() const;
	virtual Ref<Resource> create_placeholder() const;
};

class ImageTexture3D : public Texture3D {
	GDCLASS(ImageTexture3D, Texture3D);

	mutable RID texture;

	Image::Format format = Image::FORMAT_L8;
	int width = 1;
	int height = 1;
	int depth = 1;
	bool mipmaps = false;

protected:
	static void _bind_methods();

	Error _create(Image::Format p_format, int p_width, int p_height, int p_depth, bool p_mipmaps, const TypedArray<Image> &p_data);
	void _update(const TypedArray<Image> &p_data);

public:
	virtual Image::Format get_format() const override;
	virtual int get_width() const override;
	virtual int get_height() const override;
	virtual int get_depth() const override;
	virtual bool has_mipmaps() const override;

	Error create(Image::Format p_format, int p_width, int p_height, int p_depth, bool p_mipmaps, const Vector<Ref<Image>> &p_data);
	void update(const Vector<Ref<Image>> &p_data);
	virtual Vector<Ref<Image>> get_data() const override;

	virtual RID get_rid() const override;
	virtual void set_path(const String &p_path, bool p_take_over = false) override;

	ImageTexture3D();
	~ImageTexture3D();
};

class CompressedTexture3D : public Texture3D {
	GDCLASS(CompressedTexture3D, Texture3D);

public:
	enum DataFormat {
		DATA_FORMAT_IMAGE,
		DATA_FORMAT_PNG,
		DATA_FORMAT_WEBP,
		DATA_FORMAT_BASIS_UNIVERSAL,
	};

	enum {
		FORMAT_VERSION = 1
	};

	enum FormatBits {
		FORMAT_BIT_STREAM = 1 << 22,
		FORMAT_BIT_HAS_MIPMAPS = 1 << 23,
	};

private:
	Error _load_data(const String &p_path, Vector<Ref<Image>> &r_data, Image::Format &r_format, int &r_width, int &r_height, int &r_depth, bool &r_mipmaps);
	String path_to_file;
	mutable RID texture;
	Image::Format format = Image::FORMAT_L8;
	int w = 0;
	int h = 0;
	int d = 0;
	bool mipmaps = false;

	virtual void reload_from_file() override;

protected:
	static void _bind_methods();
	void _validate_property(PropertyInfo &p_property) const;

public:
	Image::Format get_format() const override;
	Error load(const String &p_path);
	String get_load_path() const;

	int get_width() const override;
	int get_height() const override;
	int get_depth() const override;
	virtual bool has_mipmaps() const override;
	virtual RID get_rid() const override;

	virtual void set_path(const String &p_path, bool p_take_over) override;

	virtual Vector<Ref<Image>> get_data() const override;

	CompressedTexture3D();
	~CompressedTexture3D();
};

class ResourceFormatLoaderCompressedTexture3D : public ResourceFormatLoader {
public:
	virtual Ref<Resource> load(const String &p_path, const String &p_original_path = "", Error *r_error = nullptr, bool p_use_sub_threads = false, float *r_progress = nullptr, CacheMode p_cache_mode = CACHE_MODE_REUSE);
	virtual void get_recognized_extensions(List<String> *p_extensions) const;
	virtual bool handles_type(const String &p_type) const;
	virtual String get_resource_type(const String &p_path) const;
};

class CurveTexture : public Texture2D {
	GDCLASS(CurveTexture, Texture2D);
	RES_BASE_EXTENSION("curvetex")
public:
	enum TextureMode {
		TEXTURE_MODE_RGB,
		TEXTURE_MODE_RED,
	};

private:
	mutable RID _texture;
	Ref<Curve> _curve;
	int _width = 256;
	int _current_width = 0;
	TextureMode texture_mode = TEXTURE_MODE_RGB;
	TextureMode _current_texture_mode = TEXTURE_MODE_RGB;

	void _update();

protected:
	static void _bind_methods();

public:
	void set_width(int p_width);
	int get_width() const override;

	void set_texture_mode(TextureMode p_mode);
	TextureMode get_texture_mode() const;

	void ensure_default_setup(float p_min = 0, float p_max = 1);

	void set_curve(Ref<Curve> p_curve);
	Ref<Curve> get_curve() const;

	virtual RID get_rid() const override;

	virtual int get_height() const override { return 1; }
	virtual bool has_alpha() const override { return false; }

	CurveTexture();
	~CurveTexture();
};

VARIANT_ENUM_CAST(CurveTexture::TextureMode)

class CurveXYZTexture : public Texture2D {
	GDCLASS(CurveXYZTexture, Texture2D);
	RES_BASE_EXTENSION("curvetex")

private:
	mutable RID _texture;
	Ref<Curve> _curve_x;
	Ref<Curve> _curve_y;
	Ref<Curve> _curve_z;
	int _width = 256;
	int _current_width = 0;

	void _update();

protected:
	static void _bind_methods();

public:
	void set_width(int p_width);
	int get_width() const override;

	void ensure_default_setup(float p_min = 0, float p_max = 1);

	void set_curve_x(Ref<Curve> p_curve);
	Ref<Curve> get_curve_x() const;

	void set_curve_y(Ref<Curve> p_curve);
	Ref<Curve> get_curve_y() const;

	void set_curve_z(Ref<Curve> p_curve);
	Ref<Curve> get_curve_z() const;

	virtual RID get_rid() const override;

	virtual int get_height() const override { return 1; }
	virtual bool has_alpha() const override { return false; }

	CurveXYZTexture();
	~CurveXYZTexture();
};

class GradientTexture1D : public Texture2D {
	GDCLASS(GradientTexture1D, Texture2D);

private:
	Ref<Gradient> gradient;
	bool update_pending = false;
	RID texture;
	int width = 256;
	bool use_hdr = false;

	void _queue_update();
	void _update();

protected:
	static void _bind_methods();

public:
	void set_gradient(Ref<Gradient> p_gradient);
	Ref<Gradient> get_gradient() const;

	void set_width(int p_width);
	int get_width() const override;

	void set_use_hdr(bool p_enabled);
	bool is_using_hdr() const;

	virtual RID get_rid() const override { return texture; }
	virtual int get_height() const override { return 1; }
	virtual bool has_alpha() const override { return true; }

	virtual Ref<Image> get_image() const override;

	GradientTexture1D();
	virtual ~GradientTexture1D();
};

class GradientTexture2D : public Texture2D {
	GDCLASS(GradientTexture2D, Texture2D);

public:
	enum Fill {
		FILL_LINEAR,
		FILL_RADIAL,
	};
	enum Repeat {
		REPEAT_NONE,
		REPEAT,
		REPEAT_MIRROR,
	};

private:
	Ref<Gradient> gradient;
	mutable RID texture;

	int width = 64;
	int height = 64;

	bool use_hdr = false;

	Vector2 fill_from;
	Vector2 fill_to = Vector2(1, 0);

	Fill fill = FILL_LINEAR;
	Repeat repeat = REPEAT_NONE;

	float _get_gradient_offset_at(int x, int y) const;

	bool update_pending = false;
	void _queue_update();
	void _update();

protected:
	static void _bind_methods();

public:
	void set_gradient(Ref<Gradient> p_gradient);
	Ref<Gradient> get_gradient() const;

	void set_width(int p_width);
	virtual int get_width() const override;
	void set_height(int p_height);
	virtual int get_height() const override;

	void set_use_hdr(bool p_enabled);
	bool is_using_hdr() const;

	void set_fill(Fill p_fill);
	Fill get_fill() const;
	void set_fill_from(Vector2 p_fill_from);
	Vector2 get_fill_from() const;
	void set_fill_to(Vector2 p_fill_to);
	Vector2 get_fill_to() const;

	void set_repeat(Repeat p_repeat);
	Repeat get_repeat() const;

	virtual RID get_rid() const override;
	virtual bool has_alpha() const override { return true; }
	virtual Ref<Image> get_image() const override;

	GradientTexture2D();
	virtual ~GradientTexture2D();
};

VARIANT_ENUM_CAST(GradientTexture2D::Fill);
VARIANT_ENUM_CAST(GradientTexture2D::Repeat);

class AnimatedTexture : public Texture2D {
	GDCLASS(AnimatedTexture, Texture2D);

	//use readers writers lock for this, since its far more times read than written to
	RWLock rw_lock;

public:
	enum {
		MAX_FRAMES = 256
	};

private:
	RID proxy_ph;
	RID proxy;

	struct Frame {
		Ref<Texture2D> texture;
		float duration = 1.0;
	};

	Frame frames[MAX_FRAMES];
	int frame_count = 1.0;
	int current_frame = 0;
	bool pause = false;
	bool one_shot = false;
	float speed_scale = 1.0;

	float time = 0.0;

	uint64_t prev_ticks = 0;

	void _update_proxy();

protected:
	static void _bind_methods();
	void _validate_property(PropertyInfo &p_property) const;

public:
	void set_frames(int p_frames);
	int get_frames() const;

	void set_current_frame(int p_frame);
	int get_current_frame() const;

	void set_pause(bool p_pause);
	bool get_pause() const;

	void set_one_shot(bool p_one_shot);
	bool get_one_shot() const;

	void set_frame_texture(int p_frame, const Ref<Texture2D> &p_texture);
	Ref<Texture2D> get_frame_texture(int p_frame) const;

	void set_frame_duration(int p_frame, float p_duration);
	float get_frame_duration(int p_frame) const;

	void set_speed_scale(float p_scale);
	float get_speed_scale() const;

	virtual int get_width() const override;
	virtual int get_height() const override;
	virtual RID get_rid() const override;

	virtual bool has_alpha() const override;

	virtual Ref<Image> get_image() const override;

	bool is_pixel_opaque(int p_x, int p_y) const override;

	AnimatedTexture();
	~AnimatedTexture();
};

class CameraTexture : public Texture2D {
	GDCLASS(CameraTexture, Texture2D);

private:
	mutable RID _texture;
	int camera_feed_id = 0;
	CameraServer::FeedImage which_feed = CameraServer::FEED_RGBA_IMAGE;

protected:
	static void _bind_methods();

public:
	virtual int get_width() const override;
	virtual int get_height() const override;
	virtual RID get_rid() const override;
	virtual bool has_alpha() const override;

	virtual Ref<Image> get_image() const override;

	void set_camera_feed_id(int p_new_id);
	int get_camera_feed_id() const;

	void set_which_feed(CameraServer::FeedImage p_which);
	CameraServer::FeedImage get_which_feed() const;

	void set_camera_active(bool p_active);
	bool get_camera_active() const;

	CameraTexture();
	~CameraTexture();
};

class PlaceholderTexture2D : public Texture2D {
	GDCLASS(PlaceholderTexture2D, Texture2D)

	RID rid;
	Size2 size = Size2(1, 1);

protected:
	static void _bind_methods();

public:
	void set_size(Size2 p_size);

	virtual int get_width() const override;
	virtual int get_height() const override;
	virtual RID get_rid() const override;
	virtual bool has_alpha() const override;

	virtual Ref<Image> get_image() const override;

	PlaceholderTexture2D();
	~PlaceholderTexture2D();
};

class PlaceholderTexture3D : public Texture3D {
	GDCLASS(PlaceholderTexture3D, Texture3D)

	RID rid;
	Vector3i size = Vector3i(1, 1, 1);

protected:
	static void _bind_methods();

public:
	void set_size(const Vector3i &p_size);
	Vector3i get_size() const;
	virtual Image::Format get_format() const override;
	virtual int get_width() const override;
	virtual int get_height() const override;
	virtual int get_depth() const override;
	virtual bool has_mipmaps() const override;
	virtual Vector<Ref<Image>> get_data() const override;

	PlaceholderTexture3D();
	~PlaceholderTexture3D();
};

class PlaceholderTextureLayered : public TextureLayered {
	GDCLASS(PlaceholderTextureLayered, TextureLayered)

	RID rid;
	Size2i size = Size2i(1, 1);
	int layers = 1;
	LayeredType layered_type = LAYERED_TYPE_2D_ARRAY;

protected:
	static void _bind_methods();

public:
	void set_size(const Size2i &p_size);
	Size2i get_size() const;
	void set_layers(int p_layers);
	virtual Image::Format get_format() const override;
	virtual LayeredType get_layered_type() const override;
	virtual int get_width() const override;
	virtual int get_height() const override;
	virtual int get_layers() const override;
	virtual bool has_mipmaps() const override;
	virtual Ref<Image> get_layer_data(int p_layer) const override;

	PlaceholderTextureLayered(LayeredType p_type);
	~PlaceholderTextureLayered();
};

class PlaceholderTexture2DArray : public PlaceholderTextureLayered {
	GDCLASS(PlaceholderTexture2DArray, PlaceholderTextureLayered)
public:
	PlaceholderTexture2DArray() :
			PlaceholderTextureLayered(LAYERED_TYPE_2D_ARRAY) {}
};

class PlaceholderCubemap : public PlaceholderTextureLayered {
	GDCLASS(PlaceholderCubemap, PlaceholderTextureLayered)
public:
	PlaceholderCubemap() :
			PlaceholderTextureLayered(LAYERED_TYPE_CUBEMAP) {}
};

class PlaceholderCubemapArray : public PlaceholderTextureLayered {
	GDCLASS(PlaceholderCubemapArray, PlaceholderTextureLayered)
public:
	PlaceholderCubemapArray() :
			PlaceholderTextureLayered(LAYERED_TYPE_CUBEMAP_ARRAY) {}
};

#endif // TEXTURE_H
