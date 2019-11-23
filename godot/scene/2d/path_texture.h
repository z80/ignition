/*************************************************************************/
/*  path_texture.h                                                       */
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

#ifndef PATH_TEXTURE_H
#define PATH_TEXTURE_H

#include "scene/2d/node_2d.h"

class PathTexture : public Node2D {
	GDCLASS(PathTexture, Node2D);

	Ref<Texture> begin;
	Ref<Texture> repeat;
	Ref<Texture> end;
	int subdivs;
	bool overlap;

public:
	void set_begin_texture(const Ref<Texture> &p_texture);
	Ref<Texture> get_begin_texture() const;

	void set_repeat_texture(const Ref<Texture> &p_texture);
	Ref<Texture> get_repeat_texture() const;

	void set_end_texture(const Ref<Texture> &p_texture);
	Ref<Texture> get_end_texture() const;

	void set_subdivisions(int p_amount);
	int get_subdivisions() const;

	void set_overlap(int p_amount);
	int get_overlap() const;

	PathTexture();
};

#endif // PATH_TEXTURE_H
