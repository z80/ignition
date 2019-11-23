/*************************************************************************/
/*  visual_shader_nodes.cpp                                              */
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

#include "visual_shader_nodes.h"
////////////// Scalar

String VisualShaderNodeScalarConstant::get_caption() const {
	return "Scalar";
}

int VisualShaderNodeScalarConstant::get_input_port_count() const {
	return 0;
}
VisualShaderNodeScalarConstant::PortType VisualShaderNodeScalarConstant::get_input_port_type(int p_port) const {
	return PORT_TYPE_SCALAR;
}
String VisualShaderNodeScalarConstant::get_input_port_name(int p_port) const {
	return String();
}

int VisualShaderNodeScalarConstant::get_output_port_count() const {
	return 1;
}
VisualShaderNodeScalarConstant::PortType VisualShaderNodeScalarConstant::get_output_port_type(int p_port) const {
	return PORT_TYPE_SCALAR;
}
String VisualShaderNodeScalarConstant::get_output_port_name(int p_port) const {
	return ""; //no output port means the editor will be used as port
}

String VisualShaderNodeScalarConstant::generate_code(Shader::Mode p_mode, VisualShader::Type p_type, int p_id, const String *p_input_vars, const String *p_output_vars, bool p_for_preview) const {
	return "\t" + p_output_vars[0] + " = " + vformat("%.6f", constant) + ";\n";
}

void VisualShaderNodeScalarConstant::set_constant(float p_value) {

	constant = p_value;
	emit_changed();
}

float VisualShaderNodeScalarConstant::get_constant() const {

	return constant;
}

Vector<StringName> VisualShaderNodeScalarConstant::get_editable_properties() const {
	Vector<StringName> props;
	props.push_back("constant");
	return props;
}

void VisualShaderNodeScalarConstant::_bind_methods() {

	ClassDB::bind_method(D_METHOD("set_constant", "value"), &VisualShaderNodeScalarConstant::set_constant);
	ClassDB::bind_method(D_METHOD("get_constant"), &VisualShaderNodeScalarConstant::get_constant);

	ADD_PROPERTY(PropertyInfo(Variant::REAL, "constant"), "set_constant", "get_constant");
}

VisualShaderNodeScalarConstant::VisualShaderNodeScalarConstant() {
	constant = 0;
}

////////////// Color

String VisualShaderNodeColorConstant::get_caption() const {
	return "Color";
}

int VisualShaderNodeColorConstant::get_input_port_count() const {
	return 0;
}
VisualShaderNodeColorConstant::PortType VisualShaderNodeColorConstant::get_input_port_type(int p_port) const {
	return PORT_TYPE_VECTOR;
}
String VisualShaderNodeColorConstant::get_input_port_name(int p_port) const {
	return String();
}

int VisualShaderNodeColorConstant::get_output_port_count() const {
	return 2;
}
VisualShaderNodeColorConstant::PortType VisualShaderNodeColorConstant::get_output_port_type(int p_port) const {
	return p_port == 0 ? PORT_TYPE_VECTOR : PORT_TYPE_SCALAR;
}
String VisualShaderNodeColorConstant::get_output_port_name(int p_port) const {
	return p_port == 0 ? "" : "alpha"; //no output port means the editor will be used as port
}

String VisualShaderNodeColorConstant::generate_code(Shader::Mode p_mode, VisualShader::Type p_type, int p_id, const String *p_input_vars, const String *p_output_vars, bool p_for_preview) const {

	String code;
	code += "\t" + p_output_vars[0] + " = " + vformat("vec3(%.6f,%.6f,%.6f)", constant.r, constant.g, constant.b) + ";\n";
	code += "\t" + p_output_vars[1] + " = " + vformat("%.6f", constant.a) + ";\n";

	return code;
}

void VisualShaderNodeColorConstant::set_constant(Color p_value) {

	constant = p_value;
	emit_changed();
}

Color VisualShaderNodeColorConstant::get_constant() const {

	return constant;
}

Vector<StringName> VisualShaderNodeColorConstant::get_editable_properties() const {
	Vector<StringName> props;
	props.push_back("constant");
	return props;
}

void VisualShaderNodeColorConstant::_bind_methods() {

	ClassDB::bind_method(D_METHOD("set_constant", "value"), &VisualShaderNodeColorConstant::set_constant);
	ClassDB::bind_method(D_METHOD("get_constant"), &VisualShaderNodeColorConstant::get_constant);

	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "constant"), "set_constant", "get_constant");
}

VisualShaderNodeColorConstant::VisualShaderNodeColorConstant() {
	constant = Color(1, 1, 1, 1);
}

////////////// Vector

String VisualShaderNodeVec3Constant::get_caption() const {
	return "Vector";
}

int VisualShaderNodeVec3Constant::get_input_port_count() const {
	return 0;
}
VisualShaderNodeVec3Constant::PortType VisualShaderNodeVec3Constant::get_input_port_type(int p_port) const {
	return PORT_TYPE_VECTOR;
}
String VisualShaderNodeVec3Constant::get_input_port_name(int p_port) const {
	return String();
}

int VisualShaderNodeVec3Constant::get_output_port_count() const {
	return 1;
}
VisualShaderNodeVec3Constant::PortType VisualShaderNodeVec3Constant::get_output_port_type(int p_port) const {
	return PORT_TYPE_VECTOR;
}
String VisualShaderNodeVec3Constant::get_output_port_name(int p_port) const {
	return ""; //no output port means the editor will be used as port
}

String VisualShaderNodeVec3Constant::generate_code(Shader::Mode p_mode, VisualShader::Type p_type, int p_id, const String *p_input_vars, const String *p_output_vars, bool p_for_preview) const {
	return "\t" + p_output_vars[0] + " = " + vformat("vec3(%.6f,%.6f,%.6f)", constant.x, constant.y, constant.z) + ";\n";
}

void VisualShaderNodeVec3Constant::set_constant(Vector3 p_value) {

	constant = p_value;
	emit_changed();
}

Vector3 VisualShaderNodeVec3Constant::get_constant() const {

	return constant;
}

Vector<StringName> VisualShaderNodeVec3Constant::get_editable_properties() const {
	Vector<StringName> props;
	props.push_back("constant");
	return props;
}

void VisualShaderNodeVec3Constant::_bind_methods() {

	ClassDB::bind_method(D_METHOD("set_constant", "value"), &VisualShaderNodeVec3Constant::set_constant);
	ClassDB::bind_method(D_METHOD("get_constant"), &VisualShaderNodeVec3Constant::get_constant);

	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "constant"), "set_constant", "get_constant");
}

VisualShaderNodeVec3Constant::VisualShaderNodeVec3Constant() {
}

////////////// Transform

String VisualShaderNodeTransformConstant::get_caption() const {
	return "Transform";
}

int VisualShaderNodeTransformConstant::get_input_port_count() const {
	return 0;
}
VisualShaderNodeTransformConstant::PortType VisualShaderNodeTransformConstant::get_input_port_type(int p_port) const {
	return PORT_TYPE_VECTOR;
}
String VisualShaderNodeTransformConstant::get_input_port_name(int p_port) const {
	return String();
}

int VisualShaderNodeTransformConstant::get_output_port_count() const {
	return 1;
}
VisualShaderNodeTransformConstant::PortType VisualShaderNodeTransformConstant::get_output_port_type(int p_port) const {
	return PORT_TYPE_TRANSFORM;
}
String VisualShaderNodeTransformConstant::get_output_port_name(int p_port) const {
	return ""; //no output port means the editor will be used as port
}

String VisualShaderNodeTransformConstant::generate_code(Shader::Mode p_mode, VisualShader::Type p_type, int p_id, const String *p_input_vars, const String *p_output_vars, bool p_for_preview) const {
	Transform t = constant;
	t.basis.transpose();

	String code = "\t" + p_output_vars[0] + " = mat4(";
	code += vformat("vec4(%.6f,%.6f,%.6f,0.0),", t.basis[0].x, t.basis[0].y, t.basis[0].z);
	code += vformat("vec4(%.6f,%.6f,%.6f,0.0),", t.basis[1].x, t.basis[1].y, t.basis[1].z);
	code += vformat("vec4(%.6f,%.6f,%.6f,0.0),", t.basis[2].x, t.basis[2].y, t.basis[2].z);
	code += vformat("vec4(%.6f,%.6f,%.6f,1.0) );\n", t.origin.x, t.origin.y, t.origin.z);
	return code;
}

void VisualShaderNodeTransformConstant::set_constant(Transform p_value) {

	constant = p_value;
	emit_changed();
}

Transform VisualShaderNodeTransformConstant::get_constant() const {

	return constant;
}

Vector<StringName> VisualShaderNodeTransformConstant::get_editable_properties() const {
	Vector<StringName> props;
	props.push_back("constant");
	return props;
}

void VisualShaderNodeTransformConstant::_bind_methods() {

	ClassDB::bind_method(D_METHOD("set_constant", "value"), &VisualShaderNodeTransformConstant::set_constant);
	ClassDB::bind_method(D_METHOD("get_constant"), &VisualShaderNodeTransformConstant::get_constant);

	ADD_PROPERTY(PropertyInfo(Variant::TRANSFORM, "constant"), "set_constant", "get_constant");
}

VisualShaderNodeTransformConstant::VisualShaderNodeTransformConstant() {
}

////////////// Texture

String VisualShaderNodeTexture::get_caption() const {
	return "Texture";
}

int VisualShaderNodeTexture::get_input_port_count() const {
	return 2;
}
VisualShaderNodeTexture::PortType VisualShaderNodeTexture::get_input_port_type(int p_port) const {
	return p_port == 0 ? PORT_TYPE_VECTOR : PORT_TYPE_SCALAR;
}
String VisualShaderNodeTexture::get_input_port_name(int p_port) const {
	return p_port == 0 ? "uv" : "lod";
}

int VisualShaderNodeTexture::get_output_port_count() const {
	return 2;
}
VisualShaderNodeTexture::PortType VisualShaderNodeTexture::get_output_port_type(int p_port) const {
	return p_port == 0 ? PORT_TYPE_VECTOR : PORT_TYPE_SCALAR;
}
String VisualShaderNodeTexture::get_output_port_name(int p_port) const {
	return p_port == 0 ? "rgb" : "alpha";
}

static String make_unique_id(VisualShader::Type p_type, int p_id, const String &p_name) {

	static const char *typepf[VisualShader::TYPE_MAX] = { "vtx", "frg", "lgt" };
	return p_name + "_" + String(typepf[p_type]) + "_" + itos(p_id);
}

Vector<VisualShader::DefaultTextureParam> VisualShaderNodeTexture::get_default_texture_parameters(VisualShader::Type p_type, int p_id) const {
	VisualShader::DefaultTextureParam dtp;
	dtp.name = make_unique_id(p_type, p_id, "tex");
	dtp.param = texture;
	Vector<VisualShader::DefaultTextureParam> ret;
	ret.push_back(dtp);
	return ret;
}

String VisualShaderNodeTexture::generate_global(Shader::Mode p_mode, VisualShader::Type p_type, int p_id) const {

	if (source == SOURCE_TEXTURE) {

		String u = "uniform sampler2D " + make_unique_id(p_type, p_id, "tex");
		switch (texture_type) {
			case TYPE_DATA: break;
			case TYPE_COLOR: u += " : hint_albedo"; break;
			case TYPE_NORMALMAP: u += " : hint_normal"; break;
		}
		return u + ";";
	}

	return String();
}

String VisualShaderNodeTexture::generate_code(Shader::Mode p_mode, VisualShader::Type p_type, int p_id, const String *p_input_vars, const String *p_output_vars, bool p_for_preview) const {

	if (source == SOURCE_TEXTURE) {
		String id = make_unique_id(p_type, p_id, "tex");
		String code;
		if (p_input_vars[0] == String()) { //none bound, do nothing

			code += "\tvec4 " + id + "_read = vec4(0.0);\n";

		} else if (p_input_vars[1] == String()) {
			//no lod
			code += "\tvec4 " + id + "_read = texture( " + id + " , " + p_input_vars[0] + ".xy );\n";
		} else {
			code += "\tvec4 " + id + "_read = textureLod( " + id + " , " + p_input_vars[0] + ".xy , " + p_input_vars[1] + " );\n";
		}

		code += "\t" + p_output_vars[0] + " = " + id + "_read.rgb;\n";
		code += "\t" + p_output_vars[1] + " = " + id + "_read.a;\n";
		return code;
	}

	if (source == SOURCE_SCREEN && (p_mode == Shader::MODE_SPATIAL || p_mode == Shader::MODE_CANVAS_ITEM) && p_type == VisualShader::TYPE_FRAGMENT) {

		String code = "\t{\n";
		if (p_input_vars[0] == String() || p_for_preview) { //none bound, do nothing

			code += "\t\tvec4 _tex_read = vec4(0.0);\n";

		} else if (p_input_vars[1] == String()) {
			//no lod
			code += "\t\tvec4 _tex_read = textureLod( SCREEN_TEXTURE , " + p_input_vars[0] + ".xy, 0.0 );\n";
		} else {
			code += "\t\tvec4 _tex_read = textureLod( SCREEN_TEXTURE , " + p_input_vars[0] + ".xy , " + p_input_vars[1] + " );\n";
		}

		code += "\t\t" + p_output_vars[0] + " = _tex_read.rgb;\n";
		code += "\t\t" + p_output_vars[1] + " = _tex_read.a;\n";
		code += "\t}\n";
		return code;
	}

	if (source == SOURCE_2D_TEXTURE && p_mode == Shader::MODE_CANVAS_ITEM && p_type == VisualShader::TYPE_FRAGMENT) {

		String code = "\t{\n";
		if (p_input_vars[0] == String()) { //none bound, do nothing

			code += "\t\tvec4 _tex_read = vec4(0.0);\n";

		} else if (p_input_vars[1] == String()) {
			//no lod
			code += "\t\tvec4 _tex_read = texture( TEXTURE , " + p_input_vars[0] + ".xy );\n";
		} else {
			code += "\t\tvec4 _tex_read = textureLod( TEXTURE , " + p_input_vars[0] + ".xy , " + p_input_vars[1] + " );\n";
		}

		code += "\t\t" + p_output_vars[0] + " = _tex_read.rgb;\n";
		code += "\t\t" + p_output_vars[1] + " = _tex_read.a;\n";
		code += "\t}\n";
		return code;
	}

	if (source == SOURCE_2D_NORMAL && p_mode == Shader::MODE_CANVAS_ITEM && p_type == VisualShader::TYPE_FRAGMENT) {

		String code = "\t{\n";
		if (p_input_vars[0] == String()) { //none bound, do nothing

			code += "\t\tvec4 _tex_read = vec4(0.0);\n";

		} else if (p_input_vars[1] == String()) {
			//no lod
			code += "\t\tvec4 _tex_read = texture( NORMAL_TEXTURE , " + p_input_vars[0] + ".xy );\n";
		} else {
			code += "\t\tvec4 _tex_read = textureLod( NORMAL_TEXTURE , " + p_input_vars[0] + ".xy , " + p_input_vars[1] + " );\n";
		}

		code += "\t\t" + p_output_vars[0] + " = _tex_read.rgb;\n";
		code += "\t\t" + p_output_vars[1] + " = _tex_read.a;\n";
		code += "\t}\n";
		return code;
	}

	//none
	String code;
	code += "\t" + p_output_vars[0] + " = vec3(0.0);\n";
	code += "\t" + p_output_vars[1] + " = 1.0;\n";
	return code;
}

void VisualShaderNodeTexture::set_source(Source p_source) {
	source = p_source;
	emit_changed();
	emit_signal("editor_refresh_request");
}

VisualShaderNodeTexture::Source VisualShaderNodeTexture::get_source() const {
	return source;
}

void VisualShaderNodeTexture::set_texture(Ref<Texture> p_value) {

	texture = p_value;
	emit_changed();
}

Ref<Texture> VisualShaderNodeTexture::get_texture() const {

	return texture;
}

void VisualShaderNodeTexture::set_texture_type(TextureType p_type) {
	texture_type = p_type;
	emit_changed();
}

VisualShaderNodeTexture::TextureType VisualShaderNodeTexture::get_texture_type() const {
	return texture_type;
}

Vector<StringName> VisualShaderNodeTexture::get_editable_properties() const {
	Vector<StringName> props;
	props.push_back("source");
	if (source == SOURCE_TEXTURE) {
		props.push_back("texture");
		props.push_back("texture_type");
	}
	return props;
}

String VisualShaderNodeTexture::get_warning(Shader::Mode p_mode, VisualShader::Type p_type) const {

	if (source == SOURCE_TEXTURE) {
		return String(); // all good
	}

	if (source == SOURCE_SCREEN && (p_mode == Shader::MODE_SPATIAL || p_mode == Shader::MODE_CANVAS_ITEM) && p_type == VisualShader::TYPE_FRAGMENT) {

		return String(); // all good
	}

	if (source == SOURCE_2D_TEXTURE && p_mode == Shader::MODE_CANVAS_ITEM && p_type == VisualShader::TYPE_FRAGMENT) {

		return String(); // all good
	}

	if (source == SOURCE_2D_NORMAL && p_mode == Shader::MODE_CANVAS_ITEM) {

		return String(); // all good
	}

	return TTR("Invalid source for shader.");
}

void VisualShaderNodeTexture::_bind_methods() {

	ClassDB::bind_method(D_METHOD("set_source", "value"), &VisualShaderNodeTexture::set_source);
	ClassDB::bind_method(D_METHOD("get_source"), &VisualShaderNodeTexture::get_source);

	ClassDB::bind_method(D_METHOD("set_texture", "value"), &VisualShaderNodeTexture::set_texture);
	ClassDB::bind_method(D_METHOD("get_texture"), &VisualShaderNodeTexture::get_texture);

	ClassDB::bind_method(D_METHOD("set_texture_type", "value"), &VisualShaderNodeTexture::set_texture_type);
	ClassDB::bind_method(D_METHOD("get_texture_type"), &VisualShaderNodeTexture::get_texture_type);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "source", PROPERTY_HINT_ENUM, "Texture,Screen,Texture2D,NormalMap2D"), "set_source", "get_source");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture", PROPERTY_HINT_RESOURCE_TYPE, "Texture"), "set_texture", "get_texture");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "texture_type", PROPERTY_HINT_ENUM, "Data,Color,Normalmap"), "set_texture_type", "get_texture_type");

	BIND_ENUM_CONSTANT(SOURCE_TEXTURE);
	BIND_ENUM_CONSTANT(SOURCE_SCREEN);
	BIND_ENUM_CONSTANT(SOURCE_2D_TEXTURE);
	BIND_ENUM_CONSTANT(SOURCE_2D_NORMAL);
	BIND_ENUM_CONSTANT(TYPE_DATA);
	BIND_ENUM_CONSTANT(TYPE_COLOR);
	BIND_ENUM_CONSTANT(TYPE_NORMALMAP);
}

VisualShaderNodeTexture::VisualShaderNodeTexture() {
	texture_type = TYPE_DATA;
	source = SOURCE_TEXTURE;
}

////////////// CubeMap

String VisualShaderNodeCubeMap::get_caption() const {
	return "CubeMap";
}

int VisualShaderNodeCubeMap::get_input_port_count() const {
	return 2;
}
VisualShaderNodeCubeMap::PortType VisualShaderNodeCubeMap::get_input_port_type(int p_port) const {
	return p_port == 0 ? PORT_TYPE_VECTOR : PORT_TYPE_SCALAR;
}
String VisualShaderNodeCubeMap::get_input_port_name(int p_port) const {
	return p_port == 0 ? "uv" : "lod";
}

int VisualShaderNodeCubeMap::get_output_port_count() const {
	return 2;
}
VisualShaderNodeCubeMap::PortType VisualShaderNodeCubeMap::get_output_port_type(int p_port) const {
	return p_port == 0 ? PORT_TYPE_VECTOR : PORT_TYPE_SCALAR;
}
String VisualShaderNodeCubeMap::get_output_port_name(int p_port) const {
	return p_port == 0 ? "rgb" : "alpha";
}

Vector<VisualShader::DefaultTextureParam> VisualShaderNodeCubeMap::get_default_texture_parameters(VisualShader::Type p_type, int p_id) const {
	VisualShader::DefaultTextureParam dtp;
	dtp.name = make_unique_id(p_type, p_id, "cube");
	dtp.param = cube_map;
	Vector<VisualShader::DefaultTextureParam> ret;
	ret.push_back(dtp);
	return ret;
}

String VisualShaderNodeCubeMap::generate_global(Shader::Mode p_mode, VisualShader::Type p_type, int p_id) const {

	String u = "uniform sampler2DCube " + make_unique_id(p_type, p_id, "cube");
	switch (texture_type) {
		case TYPE_DATA: break;
		case TYPE_COLOR: u += " : hint_albedo"; break;
		case TYPE_NORMALMAP: u += " : hint_normal"; break;
	}
	return u + ";";
}

String VisualShaderNodeCubeMap::generate_code(Shader::Mode p_mode, VisualShader::Type p_type, int p_id, const String *p_input_vars, const String *p_output_vars, bool p_for_preview) const {

	String id = make_unique_id(p_type, p_id, "cube");
	String code;
	if (p_input_vars[0] == String()) { //none bound, do nothing

		code += "\tvec4 " + id + "_read = vec4(0.0);\n";

	} else if (p_input_vars[1] == String()) {
		//no lod
		code += "\tvec4 " + id + "_read = texture( " + id + " , " + p_input_vars[0] + " );\n";
	} else {
		code += "\tvec4 " + id + "_read = textureLod( " + id + " , " + p_input_vars[0] + " , " + p_input_vars[1] + " );\n";
	}

	code += "\t" + p_output_vars[0] + " = " + id + "_read.rgb;\n";
	code += "\t" + p_output_vars[1] + " = " + id + "_read.a;\n";
	return code;
}

void VisualShaderNodeCubeMap::set_cube_map(Ref<CubeMap> p_value) {

	cube_map = p_value;
	emit_changed();
}

Ref<CubeMap> VisualShaderNodeCubeMap::get_cube_map() const {

	return cube_map;
}

void VisualShaderNodeCubeMap::set_texture_type(TextureType p_type) {
	texture_type = p_type;
	emit_changed();
}

VisualShaderNodeCubeMap::TextureType VisualShaderNodeCubeMap::get_texture_type() const {
	return texture_type;
}

Vector<StringName> VisualShaderNodeCubeMap::get_editable_properties() const {
	Vector<StringName> props;
	props.push_back("cube_map");
	props.push_back("texture_type");
	return props;
}

void VisualShaderNodeCubeMap::_bind_methods() {

	ClassDB::bind_method(D_METHOD("set_cube_map", "value"), &VisualShaderNodeCubeMap::set_cube_map);
	ClassDB::bind_method(D_METHOD("get_cube_map"), &VisualShaderNodeCubeMap::get_cube_map);

	ClassDB::bind_method(D_METHOD("set_texture_type", "value"), &VisualShaderNodeCubeMap::set_texture_type);
	ClassDB::bind_method(D_METHOD("get_texture_type"), &VisualShaderNodeCubeMap::get_texture_type);

	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "cube_map", PROPERTY_HINT_RESOURCE_TYPE, "CubeMap"), "set_cube_map", "get_cube_map");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "texture_type", PROPERTY_HINT_ENUM, "Data,Color,Normalmap"), "set_texture_type", "get_texture_type");

	BIND_ENUM_CONSTANT(TYPE_DATA);
	BIND_ENUM_CONSTANT(TYPE_COLOR);
	BIND_ENUM_CONSTANT(TYPE_NORMALMAP);
}

VisualShaderNodeCubeMap::VisualShaderNodeCubeMap() {
	texture_type = TYPE_DATA;
}
////////////// Scalar Op

String VisualShaderNodeScalarOp::get_caption() const {
	return "ScalarOp";
}

int VisualShaderNodeScalarOp::get_input_port_count() const {
	return 2;
}
VisualShaderNodeScalarOp::PortType VisualShaderNodeScalarOp::get_input_port_type(int p_port) const {
	return PORT_TYPE_SCALAR;
}
String VisualShaderNodeScalarOp::get_input_port_name(int p_port) const {
	return p_port == 0 ? "a" : "b";
}

int VisualShaderNodeScalarOp::get_output_port_count() const {
	return 1;
}
VisualShaderNodeScalarOp::PortType VisualShaderNodeScalarOp::get_output_port_type(int p_port) const {
	return PORT_TYPE_SCALAR;
}
String VisualShaderNodeScalarOp::get_output_port_name(int p_port) const {
	return "op"; //no output port means the editor will be used as port
}

String VisualShaderNodeScalarOp::generate_code(Shader::Mode p_mode, VisualShader::Type p_type, int p_id, const String *p_input_vars, const String *p_output_vars, bool p_for_preview) const {

	String code = "\t" + p_output_vars[0] + " = ";
	switch (op) {

		case OP_ADD: code += p_input_vars[0] + " + " + p_input_vars[1] + ";\n"; break;
		case OP_SUB: code += p_input_vars[0] + " - " + p_input_vars[1] + ";\n"; break;
		case OP_MUL: code += p_input_vars[0] + " * " + p_input_vars[1] + ";\n"; break;
		case OP_DIV: code += p_input_vars[0] + " / " + p_input_vars[1] + ";\n"; break;
		case OP_MOD: code += "mod( " + p_input_vars[0] + " , " + p_input_vars[1] + " );\n"; break;
		case OP_POW: code += "pow( " + p_input_vars[0] + " , " + p_input_vars[1] + " );\n"; break;
		case OP_MAX: code += "max( " + p_input_vars[0] + " , " + p_input_vars[1] + " );\n"; break;
		case OP_MIN: code += "min( " + p_input_vars[0] + " , " + p_input_vars[1] + " );\n"; break;
		case OP_ATAN2: code += "atan( " + p_input_vars[0] + " , " + p_input_vars[1] + " );\n"; break;
	}

	return code;
}

void VisualShaderNodeScalarOp::set_operator(Operator p_op) {

	op = p_op;
	emit_changed();
}

VisualShaderNodeScalarOp::Operator VisualShaderNodeScalarOp::get_operator() const {

	return op;
}

Vector<StringName> VisualShaderNodeScalarOp::get_editable_properties() const {
	Vector<StringName> props;
	props.push_back("operator");
	return props;
}

void VisualShaderNodeScalarOp::_bind_methods() {

	ClassDB::bind_method(D_METHOD("set_operator", "op"), &VisualShaderNodeScalarOp::set_operator);
	ClassDB::bind_method(D_METHOD("get_operator"), &VisualShaderNodeScalarOp::get_operator);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "operator", PROPERTY_HINT_ENUM, "Add,Sub,Multiply,Divide,Remainder,Power,Max,Min,Atan2"), "set_operator", "get_operator");

	BIND_ENUM_CONSTANT(OP_ADD);
	BIND_ENUM_CONSTANT(OP_SUB);
	BIND_ENUM_CONSTANT(OP_MUL);
	BIND_ENUM_CONSTANT(OP_DIV);
	BIND_ENUM_CONSTANT(OP_MOD);
	BIND_ENUM_CONSTANT(OP_POW);
	BIND_ENUM_CONSTANT(OP_MAX);
	BIND_ENUM_CONSTANT(OP_MIN);
	BIND_ENUM_CONSTANT(OP_ATAN2);
}

VisualShaderNodeScalarOp::VisualShaderNodeScalarOp() {
	op = OP_ADD;
	set_input_port_default_value(0, 0.0);
	set_input_port_default_value(1, 0.0);
}

////////////// Vector Op

String VisualShaderNodeVectorOp::get_caption() const {
	return "VectorOp";
}

int VisualShaderNodeVectorOp::get_input_port_count() const {
	return 2;
}
VisualShaderNodeVectorOp::PortType VisualShaderNodeVectorOp::get_input_port_type(int p_port) const {
	return PORT_TYPE_VECTOR;
}
String VisualShaderNodeVectorOp::get_input_port_name(int p_port) const {
	return p_port == 0 ? "a" : "b";
}

int VisualShaderNodeVectorOp::get_output_port_count() const {
	return 1;
}
VisualShaderNodeVectorOp::PortType VisualShaderNodeVectorOp::get_output_port_type(int p_port) const {
	return PORT_TYPE_VECTOR;
}
String VisualShaderNodeVectorOp::get_output_port_name(int p_port) const {
	return "op"; //no output port means the editor will be used as port
}

String VisualShaderNodeVectorOp::generate_code(Shader::Mode p_mode, VisualShader::Type p_type, int p_id, const String *p_input_vars, const String *p_output_vars, bool p_for_preview) const {

	String code = "\t" + p_output_vars[0] + " = ";
	switch (op) {

		case OP_ADD: code += p_input_vars[0] + " + " + p_input_vars[1] + ";\n"; break;
		case OP_SUB: code += p_input_vars[0] + " - " + p_input_vars[1] + ";\n"; break;
		case OP_MUL: code += p_input_vars[0] + " * " + p_input_vars[1] + ";\n"; break;
		case OP_DIV: code += p_input_vars[0] + " / " + p_input_vars[1] + ";\n"; break;
		case OP_MOD: code += "mod( " + p_input_vars[0] + " , " + p_input_vars[1] + " );\n"; break;
		case OP_POW: code += "pow( " + p_input_vars[0] + " , " + p_input_vars[1] + " );\n"; break;
		case OP_MAX: code += "max( " + p_input_vars[0] + " , " + p_input_vars[1] + " );\n"; break;
		case OP_MIN: code += "min( " + p_input_vars[0] + " , " + p_input_vars[1] + " );\n"; break;
		case OP_CROSS: code += "cross( " + p_input_vars[0] + " , " + p_input_vars[1] + " );\n"; break;
	}

	return code;
}

void VisualShaderNodeVectorOp::set_operator(Operator p_op) {

	op = p_op;
	emit_changed();
}

VisualShaderNodeVectorOp::Operator VisualShaderNodeVectorOp::get_operator() const {

	return op;
}

Vector<StringName> VisualShaderNodeVectorOp::get_editable_properties() const {
	Vector<StringName> props;
	props.push_back("operator");
	return props;
}

void VisualShaderNodeVectorOp::_bind_methods() {

	ClassDB::bind_method(D_METHOD("set_operator", "op"), &VisualShaderNodeVectorOp::set_operator);
	ClassDB::bind_method(D_METHOD("get_operator"), &VisualShaderNodeVectorOp::get_operator);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "operator", PROPERTY_HINT_ENUM, "Add,Sub,Multiply,Divide,Remainder,Power,Max,Min,Cross"), "set_operator", "get_operator");

	BIND_ENUM_CONSTANT(OP_ADD);
	BIND_ENUM_CONSTANT(OP_SUB);
	BIND_ENUM_CONSTANT(OP_MUL);
	BIND_ENUM_CONSTANT(OP_DIV);
	BIND_ENUM_CONSTANT(OP_MOD);
	BIND_ENUM_CONSTANT(OP_POW);
	BIND_ENUM_CONSTANT(OP_MAX);
	BIND_ENUM_CONSTANT(OP_MIN);
	BIND_ENUM_CONSTANT(OP_CROSS);
}

VisualShaderNodeVectorOp::VisualShaderNodeVectorOp() {
	op = OP_ADD;
	set_input_port_default_value(0, Vector3());
	set_input_port_default_value(1, Vector3());
}

////////////// Color Op

String VisualShaderNodeColorOp::get_caption() const {
	return "ColorOp";
}

int VisualShaderNodeColorOp::get_input_port_count() const {
	return 2;
}
VisualShaderNodeColorOp::PortType VisualShaderNodeColorOp::get_input_port_type(int p_port) const {
	return PORT_TYPE_VECTOR;
}
String VisualShaderNodeColorOp::get_input_port_name(int p_port) const {
	return p_port == 0 ? "a" : "b";
}

int VisualShaderNodeColorOp::get_output_port_count() const {
	return 1;
}
VisualShaderNodeColorOp::PortType VisualShaderNodeColorOp::get_output_port_type(int p_port) const {
	return PORT_TYPE_VECTOR;
}
String VisualShaderNodeColorOp::get_output_port_name(int p_port) const {
	return "op"; //no output port means the editor will be used as port
}

String VisualShaderNodeColorOp::generate_code(Shader::Mode p_mode, VisualShader::Type p_type, int p_id, const String *p_input_vars, const String *p_output_vars, bool p_for_preview) const {

	String code;
	static const char *axisn[3] = { "x", "y", "z" };
	switch (op) {
		case OP_SCREEN: {

			code += "\t" + p_output_vars[0] + "=vec3(1.0)-(vec3(1.0)-" + p_input_vars[0] + ")*(vec3(1.0)-" + p_input_vars[1] + ");\n";
		} break;
		case OP_DIFFERENCE: {

			code += "\t" + p_output_vars[0] + "=abs(" + p_input_vars[0] + "-" + p_input_vars[1] + ");\n";
		} break;
		case OP_DARKEN: {

			code += "\t" + p_output_vars[0] + "=min(" + p_input_vars[0] + "," + p_input_vars[1] + ");\n";
		} break;
		case OP_LIGHTEN: {

			code += "\t" + p_output_vars[0] + "=max(" + p_input_vars[0] + "," + p_input_vars[1] + ");\n";

		} break;
		case OP_OVERLAY: {

			for (int i = 0; i < 3; i++) {
				code += "\t{\n";
				code += "\t\tfloat base=" + p_input_vars[0] + "." + axisn[i] + ";\n";
				code += "\t\tfloat blend=" + p_input_vars[1] + "." + axisn[i] + ";\n";
				code += "\t\tif (base < 0.5) {\n";
				code += "\t\t\t" + p_output_vars[0] + "." + axisn[i] + " = 2.0 * base * blend;\n";
				code += "\t\t} else {\n";
				code += "\t\t\t" + p_output_vars[0] + "." + axisn[i] + " = 1.0 - 2.0 * (1.0 - blend) * (1.0 - base);\n";
				code += "\t\t}\n";
				code += "\t}\n";
			}

		} break;
		case OP_DODGE: {

			code += "\t" + p_output_vars[0] + "=(" + p_input_vars[0] + ")/(vec3(1.0)-" + p_input_vars[1] + ");\n";

		} break;
		case OP_BURN: {

			code += "\t" + p_output_vars[0] + "=vec3(1.0)-(vec3(1.0)-" + p_input_vars[0] + ")/(" + p_input_vars[1] + ");\n";
		} break;
		case OP_SOFT_LIGHT: {

			for (int i = 0; i < 3; i++) {
				code += "\t{\n";
				code += "\t\tfloat base=" + p_input_vars[0] + "." + axisn[i] + ";\n";
				code += "\t\tfloat blend=" + p_input_vars[1] + "." + axisn[i] + ";\n";
				code += "\t\tif (base < 0.5) {\n";
				code += "\t\t\t" + p_output_vars[0] + "." + axisn[i] + " = (base * (blend+0.5));\n";
				code += "\t\t} else {\n";
				code += "\t\t\t" + p_output_vars[0] + "." + axisn[i] + " = (1.0 - (1.0-base) * (1.0-(blend-0.5)));\n";
				code += "\t\t}\n";
				code += "\t}\n";
			}

		} break;
		case OP_HARD_LIGHT: {

			for (int i = 0; i < 3; i++) {
				code += "\t{\n";
				code += "\t\tfloat base=" + p_input_vars[0] + "." + axisn[i] + ";\n";
				code += "\t\tfloat blend=" + p_input_vars[1] + "." + axisn[i] + ";\n";
				code += "\t\tif (base < 0.5) {\n";
				code += "\t\t\t" + p_output_vars[0] + "." + axisn[i] + " = (base * (2.0*blend));\n";
				code += "\t\t} else {\n";
				code += "\t\t\t" + p_output_vars[0] + "." + axisn[i] + " = (1.0 - (1.0-base) * (1.0-2.0*(blend-0.5)));\n";
				code += "\t\t}\n";
				code += "\t}\n";
			}

		} break;
	}

	return code;
}

void VisualShaderNodeColorOp::set_operator(Operator p_op) {

	op = p_op;
	emit_changed();
}

VisualShaderNodeColorOp::Operator VisualShaderNodeColorOp::get_operator() const {

	return op;
}

Vector<StringName> VisualShaderNodeColorOp::get_editable_properties() const {
	Vector<StringName> props;
	props.push_back("operator");
	return props;
}

void VisualShaderNodeColorOp::_bind_methods() {

	ClassDB::bind_method(D_METHOD("set_operator", "op"), &VisualShaderNodeColorOp::set_operator);
	ClassDB::bind_method(D_METHOD("get_operator"), &VisualShaderNodeColorOp::get_operator);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "operator", PROPERTY_HINT_ENUM, "Screen,Difference,Darken,Lighten,Overlay,Dodge,Burn,SoftLight,HardLight"), "set_operator", "get_operator");

	BIND_ENUM_CONSTANT(OP_SCREEN);
	BIND_ENUM_CONSTANT(OP_DIFFERENCE);
	BIND_ENUM_CONSTANT(OP_DARKEN);
	BIND_ENUM_CONSTANT(OP_LIGHTEN);
	BIND_ENUM_CONSTANT(OP_OVERLAY);
	BIND_ENUM_CONSTANT(OP_DODGE);
	BIND_ENUM_CONSTANT(OP_BURN);
	BIND_ENUM_CONSTANT(OP_SOFT_LIGHT);
	BIND_ENUM_CONSTANT(OP_HARD_LIGHT);
}

VisualShaderNodeColorOp::VisualShaderNodeColorOp() {
	op = OP_SCREEN;
	set_input_port_default_value(0, Vector3());
	set_input_port_default_value(1, Vector3());
}

////////////// Transform Mult

String VisualShaderNodeTransformMult::get_caption() const {
	return "TransformMult";
}

int VisualShaderNodeTransformMult::get_input_port_count() const {
	return 2;
}
VisualShaderNodeTransformMult::PortType VisualShaderNodeTransformMult::get_input_port_type(int p_port) const {
	return PORT_TYPE_TRANSFORM;
}
String VisualShaderNodeTransformMult::get_input_port_name(int p_port) const {
	return p_port == 0 ? "a" : "b";
}

int VisualShaderNodeTransformMult::get_output_port_count() const {
	return 1;
}
VisualShaderNodeTransformMult::PortType VisualShaderNodeTransformMult::get_output_port_type(int p_port) const {
	return PORT_TYPE_TRANSFORM;
}
String VisualShaderNodeTransformMult::get_output_port_name(int p_port) const {
	return "mult"; //no output port means the editor will be used as port
}

String VisualShaderNodeTransformMult::generate_code(Shader::Mode p_mode, VisualShader::Type p_type, int p_id, const String *p_input_vars, const String *p_output_vars, bool p_for_preview) const {

	if (op == OP_AxB) {
		return "\t" + p_output_vars[0] + " = " + p_input_vars[0] + " * " + p_input_vars[1] + ";\n";
	} else {
		return "\t" + p_output_vars[0] + " = " + p_input_vars[1] + " * " + p_input_vars[0] + ";\n";
	}
}

void VisualShaderNodeTransformMult::set_operator(Operator p_op) {

	op = p_op;
	emit_changed();
}

VisualShaderNodeTransformMult::Operator VisualShaderNodeTransformMult::get_operator() const {

	return op;
}

Vector<StringName> VisualShaderNodeTransformMult::get_editable_properties() const {
	Vector<StringName> props;
	props.push_back("operator");
	return props;
}

void VisualShaderNodeTransformMult::_bind_methods() {

	ClassDB::bind_method(D_METHOD("set_operator", "op"), &VisualShaderNodeTransformMult::set_operator);
	ClassDB::bind_method(D_METHOD("get_operator"), &VisualShaderNodeTransformMult::get_operator);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "operator", PROPERTY_HINT_ENUM, "A x B,B x A"), "set_operator", "get_operator");

	BIND_ENUM_CONSTANT(OP_AxB);
	BIND_ENUM_CONSTANT(OP_BxA);
}

VisualShaderNodeTransformMult::VisualShaderNodeTransformMult() {
	op = OP_AxB;
	set_input_port_default_value(0, Transform());
	set_input_port_default_value(1, Transform());
}

////////////// TransformVec Mult

String VisualShaderNodeTransformVecMult::get_caption() const {
	return "TransformVectorMult";
}

int VisualShaderNodeTransformVecMult::get_input_port_count() const {
	return 2;
}
VisualShaderNodeTransformVecMult::PortType VisualShaderNodeTransformVecMult::get_input_port_type(int p_port) const {
	return p_port == 0 ? PORT_TYPE_TRANSFORM : PORT_TYPE_VECTOR;
}
String VisualShaderNodeTransformVecMult::get_input_port_name(int p_port) const {
	return p_port == 0 ? "a" : "b";
}

int VisualShaderNodeTransformVecMult::get_output_port_count() const {
	return 1;
}
VisualShaderNodeTransformVecMult::PortType VisualShaderNodeTransformVecMult::get_output_port_type(int p_port) const {
	return PORT_TYPE_VECTOR;
}
String VisualShaderNodeTransformVecMult::get_output_port_name(int p_port) const {
	return ""; //no output port means the editor will be used as port
}

String VisualShaderNodeTransformVecMult::generate_code(Shader::Mode p_mode, VisualShader::Type p_type, int p_id, const String *p_input_vars, const String *p_output_vars, bool p_for_preview) const {
	if (op == OP_AxB) {
		return "\t" + p_output_vars[0] + " = ( " + p_input_vars[0] + " * vec4(" + p_input_vars[1] + ", 1.0) ).xyz;\n";
	} else if (op == OP_BxA) {
		return "\t" + p_output_vars[0] + " = ( vec4(" + p_input_vars[1] + ", 1.0) * " + p_input_vars[0] + " ).xyz;\n";
	} else if (op == OP_3x3_AxB) {
		return "\t" + p_output_vars[0] + " = ( " + p_input_vars[0] + " * vec4(" + p_input_vars[1] + ", 0.0) ).xyz;\n";
	} else {
		return "\t" + p_output_vars[0] + " = ( vec4(" + p_input_vars[1] + ", 0.0) * " + p_input_vars[0] + " ).xyz;\n";
	}
}

void VisualShaderNodeTransformVecMult::set_operator(Operator p_op) {

	op = p_op;
	emit_changed();
}

VisualShaderNodeTransformVecMult::Operator VisualShaderNodeTransformVecMult::get_operator() const {

	return op;
}

Vector<StringName> VisualShaderNodeTransformVecMult::get_editable_properties() const {
	Vector<StringName> props;
	props.push_back("operator");
	return props;
}

void VisualShaderNodeTransformVecMult::_bind_methods() {

	ClassDB::bind_method(D_METHOD("set_operator", "op"), &VisualShaderNodeTransformVecMult::set_operator);
	ClassDB::bind_method(D_METHOD("get_operator"), &VisualShaderNodeTransformVecMult::get_operator);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "operator", PROPERTY_HINT_ENUM, "A x B,B x A,A x B (3x3),B x A (3x3)"), "set_operator", "get_operator");

	BIND_ENUM_CONSTANT(OP_AxB);
	BIND_ENUM_CONSTANT(OP_BxA);
	BIND_ENUM_CONSTANT(OP_3x3_AxB);
	BIND_ENUM_CONSTANT(OP_3x3_BxA);
}

VisualShaderNodeTransformVecMult::VisualShaderNodeTransformVecMult() {
	op = OP_AxB;
	set_input_port_default_value(0, Transform());
	set_input_port_default_value(1, Vector3());
}

////////////// Scalar Func

String VisualShaderNodeScalarFunc::get_caption() const {
	return "ScalarFunc";
}

int VisualShaderNodeScalarFunc::get_input_port_count() const {
	return 1;
}
VisualShaderNodeScalarFunc::PortType VisualShaderNodeScalarFunc::get_input_port_type(int p_port) const {
	return PORT_TYPE_SCALAR;
}
String VisualShaderNodeScalarFunc::get_input_port_name(int p_port) const {
	return "";
}

int VisualShaderNodeScalarFunc::get_output_port_count() const {
	return 1;
}
VisualShaderNodeScalarFunc::PortType VisualShaderNodeScalarFunc::get_output_port_type(int p_port) const {
	return PORT_TYPE_SCALAR;
}
String VisualShaderNodeScalarFunc::get_output_port_name(int p_port) const {
	return ""; //no output port means the editor will be used as port
}

String VisualShaderNodeScalarFunc::generate_code(Shader::Mode p_mode, VisualShader::Type p_type, int p_id, const String *p_input_vars, const String *p_output_vars, bool p_for_preview) const {

	static const char *scalar_func_id[FUNC_NEGATE + 1] = {
		"sin($)",
		"cos($)",
		"tan($)",
		"asin($)",
		"acos($)",
		"atan($)",
		"sinh($)",
		"cosh($)",
		"tanh($)",
		"log($)",
		"exp($)",
		"sqrt($)",
		"abs($)",
		"sign($)",
		"floor($)",
		"round($)",
		"ceil($)",
		"fract($)",
		"min(max($,0.0),1.0)",
		"-($)",
	};

	return "\t" + p_output_vars[0] + " = " + String(scalar_func_id[func]).replace("$", p_input_vars[0]) + ";\n";
}

void VisualShaderNodeScalarFunc::set_function(Function p_func) {

	func = p_func;
	emit_changed();
}

VisualShaderNodeScalarFunc::Function VisualShaderNodeScalarFunc::get_function() const {

	return func;
}

Vector<StringName> VisualShaderNodeScalarFunc::get_editable_properties() const {
	Vector<StringName> props;
	props.push_back("function");
	return props;
}

void VisualShaderNodeScalarFunc::_bind_methods() {

	ClassDB::bind_method(D_METHOD("set_function", "func"), &VisualShaderNodeScalarFunc::set_function);
	ClassDB::bind_method(D_METHOD("get_function"), &VisualShaderNodeScalarFunc::get_function);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "function", PROPERTY_HINT_ENUM, "Sin,Cos,Tan,ASin,ACos,ATan,SinH,CosH,TanH,Log,Exp,Sqrt,Abs,Sign,Floor,Round,Ceil,Frac,Saturate,Negate"), "set_function", "get_function");

	BIND_ENUM_CONSTANT(FUNC_SIN);
	BIND_ENUM_CONSTANT(FUNC_COS);
	BIND_ENUM_CONSTANT(FUNC_TAN);
	BIND_ENUM_CONSTANT(FUNC_ASIN);
	BIND_ENUM_CONSTANT(FUNC_ACOS);
	BIND_ENUM_CONSTANT(FUNC_ATAN);
	BIND_ENUM_CONSTANT(FUNC_SINH);
	BIND_ENUM_CONSTANT(FUNC_COSH);
	BIND_ENUM_CONSTANT(FUNC_TANH);
	BIND_ENUM_CONSTANT(FUNC_LOG);
	BIND_ENUM_CONSTANT(FUNC_EXP);
	BIND_ENUM_CONSTANT(FUNC_SQRT);
	BIND_ENUM_CONSTANT(FUNC_ABS);
	BIND_ENUM_CONSTANT(FUNC_SIGN);
	BIND_ENUM_CONSTANT(FUNC_FLOOR);
	BIND_ENUM_CONSTANT(FUNC_ROUND);
	BIND_ENUM_CONSTANT(FUNC_CEIL);
	BIND_ENUM_CONSTANT(FUNC_FRAC);
	BIND_ENUM_CONSTANT(FUNC_SATURATE);
	BIND_ENUM_CONSTANT(FUNC_NEGATE);
}

VisualShaderNodeScalarFunc::VisualShaderNodeScalarFunc() {
	func = FUNC_SIGN;
	set_input_port_default_value(0, 0.0);
}

////////////// Vector Func

String VisualShaderNodeVectorFunc::get_caption() const {
	return "VectorFunc";
}

int VisualShaderNodeVectorFunc::get_input_port_count() const {
	return 1;
}
VisualShaderNodeVectorFunc::PortType VisualShaderNodeVectorFunc::get_input_port_type(int p_port) const {
	return PORT_TYPE_VECTOR;
}
String VisualShaderNodeVectorFunc::get_input_port_name(int p_port) const {
	return "";
}

int VisualShaderNodeVectorFunc::get_output_port_count() const {
	return 1;
}
VisualShaderNodeVectorFunc::PortType VisualShaderNodeVectorFunc::get_output_port_type(int p_port) const {
	return PORT_TYPE_VECTOR;
}
String VisualShaderNodeVectorFunc::get_output_port_name(int p_port) const {
	return ""; //no output port means the editor will be used as port
}

String VisualShaderNodeVectorFunc::generate_code(Shader::Mode p_mode, VisualShader::Type p_type, int p_id, const String *p_input_vars, const String *p_output_vars, bool p_for_preview) const {

	static const char *vec_func_id[FUNC_HSV2RGB + 1] = {
		"normalize($)",
		"max(min($,vec3(1.0)),vec3(0.0))",
		"-($)",
		"1.0/($)",
		"",
		"",
	};

	String code;

	if (func == FUNC_RGB2HSV) {
		code += "\t{\n";
		code += "\t\tvec3 c = " + p_input_vars[0] + ";\n";
		code += "\t\tvec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);\n";
		code += "\t\tvec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));\n";
		code += "\t\tvec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));\n";
		code += "\t\tfloat d = q.x - min(q.w, q.y);\n";
		code += "\t\tfloat e = 1.0e-10;\n";
		code += "\t\t" + p_output_vars[0] + "=vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);\n";
		code += "\t}\n";
	} else if (func == FUNC_HSV2RGB) {
		code += "\t{\n";
		code += "\t\tvec3 c = " + p_input_vars[0] + ";\n";
		code += "\t\tvec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);\n";
		code += "\t\tvec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);\n";
		code += "\t\t" + p_output_vars[0] + "=c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);\n";
		code += "\t}\n";

	} else {
		code += "\t" + p_output_vars[0] + "=" + String(vec_func_id[func]).replace("$", p_input_vars[0]) + ";\n";
	}

	return code;
}

void VisualShaderNodeVectorFunc::set_function(Function p_func) {

	func = p_func;
	emit_changed();
}

VisualShaderNodeVectorFunc::Function VisualShaderNodeVectorFunc::get_function() const {

	return func;
}

Vector<StringName> VisualShaderNodeVectorFunc::get_editable_properties() const {
	Vector<StringName> props;
	props.push_back("function");
	return props;
}

void VisualShaderNodeVectorFunc::_bind_methods() {

	ClassDB::bind_method(D_METHOD("set_function", "func"), &VisualShaderNodeVectorFunc::set_function);
	ClassDB::bind_method(D_METHOD("get_function"), &VisualShaderNodeVectorFunc::get_function);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "function", PROPERTY_HINT_ENUM, "Normalize,Saturate,Negate,Reciprocal,RGB2HSV,HSV2RGB"), "set_function", "get_function");

	BIND_ENUM_CONSTANT(FUNC_NORMALIZE);
	BIND_ENUM_CONSTANT(FUNC_SATURATE);
	BIND_ENUM_CONSTANT(FUNC_NEGATE);
	BIND_ENUM_CONSTANT(FUNC_RECIPROCAL);
	BIND_ENUM_CONSTANT(FUNC_RGB2HSV);
	BIND_ENUM_CONSTANT(FUNC_HSV2RGB);
}

VisualShaderNodeVectorFunc::VisualShaderNodeVectorFunc() {
	func = FUNC_NORMALIZE;
	set_input_port_default_value(0, Vector3());
}

////////////// Dot Product

String VisualShaderNodeDotProduct::get_caption() const {
	return "DotProduct";
}

int VisualShaderNodeDotProduct::get_input_port_count() const {
	return 2;
}
VisualShaderNodeDotProduct::PortType VisualShaderNodeDotProduct::get_input_port_type(int p_port) const {
	return PORT_TYPE_VECTOR;
}
String VisualShaderNodeDotProduct::get_input_port_name(int p_port) const {
	return p_port == 0 ? "a" : "b";
}

int VisualShaderNodeDotProduct::get_output_port_count() const {
	return 1;
}
VisualShaderNodeDotProduct::PortType VisualShaderNodeDotProduct::get_output_port_type(int p_port) const {
	return PORT_TYPE_SCALAR;
}
String VisualShaderNodeDotProduct::get_output_port_name(int p_port) const {
	return "dot";
}

String VisualShaderNodeDotProduct::generate_code(Shader::Mode p_mode, VisualShader::Type p_type, int p_id, const String *p_input_vars, const String *p_output_vars, bool p_for_preview) const {
	return "\t" + p_output_vars[0] + " = dot( " + p_input_vars[0] + " , " + p_input_vars[1] + " );\n";
}

VisualShaderNodeDotProduct::VisualShaderNodeDotProduct() {
	set_input_port_default_value(0, Vector3());
	set_input_port_default_value(1, Vector3());
}

////////////// Vector Len

String VisualShaderNodeVectorLen::get_caption() const {
	return "VectorLen";
}

int VisualShaderNodeVectorLen::get_input_port_count() const {
	return 1;
}
VisualShaderNodeVectorLen::PortType VisualShaderNodeVectorLen::get_input_port_type(int p_port) const {
	return PORT_TYPE_VECTOR;
}
String VisualShaderNodeVectorLen::get_input_port_name(int p_port) const {
	return "";
}

int VisualShaderNodeVectorLen::get_output_port_count() const {
	return 1;
}
VisualShaderNodeVectorLen::PortType VisualShaderNodeVectorLen::get_output_port_type(int p_port) const {
	return PORT_TYPE_SCALAR;
}
String VisualShaderNodeVectorLen::get_output_port_name(int p_port) const {
	return "length";
}

String VisualShaderNodeVectorLen::generate_code(Shader::Mode p_mode, VisualShader::Type p_type, int p_id, const String *p_input_vars, const String *p_output_vars, bool p_for_preview) const {
	return "\t" + p_output_vars[0] + " = length( " + p_input_vars[0] + " );\n";
}

VisualShaderNodeVectorLen::VisualShaderNodeVectorLen() {
	set_input_port_default_value(0, Vector3());
}

////////////// Scalar Interp

String VisualShaderNodeScalarInterp::get_caption() const {
	return "ScalarInterp";
}

int VisualShaderNodeScalarInterp::get_input_port_count() const {
	return 3;
}
VisualShaderNodeScalarInterp::PortType VisualShaderNodeScalarInterp::get_input_port_type(int p_port) const {
	return PORT_TYPE_SCALAR;
}
String VisualShaderNodeScalarInterp::get_input_port_name(int p_port) const {
	if (p_port == 0) {
		return "a";
	} else if (p_port == 1) {
		return "b";
	} else {
		return "c";
	}
}

int VisualShaderNodeScalarInterp::get_output_port_count() const {
	return 1;
}
VisualShaderNodeScalarInterp::PortType VisualShaderNodeScalarInterp::get_output_port_type(int p_port) const {
	return PORT_TYPE_SCALAR;
}
String VisualShaderNodeScalarInterp::get_output_port_name(int p_port) const {
	return "mix";
}

String VisualShaderNodeScalarInterp::generate_code(Shader::Mode p_mode, VisualShader::Type p_type, int p_id, const String *p_input_vars, const String *p_output_vars, bool p_for_preview) const {
	return "\t" + p_output_vars[0] + " = mix( " + p_input_vars[0] + " , " + p_input_vars[1] + " , " + p_input_vars[2] + " );\n";
}

VisualShaderNodeScalarInterp::VisualShaderNodeScalarInterp() {
	set_input_port_default_value(0, 0.0);
	set_input_port_default_value(1, 0.0);
	set_input_port_default_value(2, 0.0);
}

////////////// Vector Interp

String VisualShaderNodeVectorInterp::get_caption() const {
	return "VectorInterp";
}

int VisualShaderNodeVectorInterp::get_input_port_count() const {
	return 3;
}
VisualShaderNodeVectorInterp::PortType VisualShaderNodeVectorInterp::get_input_port_type(int p_port) const {
	return PORT_TYPE_VECTOR;
}
String VisualShaderNodeVectorInterp::get_input_port_name(int p_port) const {
	if (p_port == 0) {
		return "a";
	} else if (p_port == 1) {
		return "b";
	} else {
		return "c";
	}
}

int VisualShaderNodeVectorInterp::get_output_port_count() const {
	return 1;
}
VisualShaderNodeVectorInterp::PortType VisualShaderNodeVectorInterp::get_output_port_type(int p_port) const {
	return PORT_TYPE_VECTOR;
}
String VisualShaderNodeVectorInterp::get_output_port_name(int p_port) const {
	return "mix";
}

String VisualShaderNodeVectorInterp::generate_code(Shader::Mode p_mode, VisualShader::Type p_type, int p_id, const String *p_input_vars, const String *p_output_vars, bool p_for_preview) const {
	return "\t" + p_output_vars[0] + " = mix( " + p_input_vars[0] + " , " + p_input_vars[1] + " , " + p_input_vars[2] + " );\n";
}

VisualShaderNodeVectorInterp::VisualShaderNodeVectorInterp() {
	set_input_port_default_value(0, Vector3());
	set_input_port_default_value(1, Vector3());
	set_input_port_default_value(2, Vector3());
}

////////////// Vector Compose
String VisualShaderNodeVectorCompose::get_caption() const {
	return "VectorCompose";
}

int VisualShaderNodeVectorCompose::get_input_port_count() const {
	return 3;
}
VisualShaderNodeVectorCompose::PortType VisualShaderNodeVectorCompose::get_input_port_type(int p_port) const {
	return PORT_TYPE_SCALAR;
}
String VisualShaderNodeVectorCompose::get_input_port_name(int p_port) const {
	if (p_port == 0) {
		return "x";
	} else if (p_port == 1) {
		return "y";
	} else {
		return "z";
	}
}

int VisualShaderNodeVectorCompose::get_output_port_count() const {
	return 1;
}
VisualShaderNodeVectorCompose::PortType VisualShaderNodeVectorCompose::get_output_port_type(int p_port) const {
	return PORT_TYPE_VECTOR;
}
String VisualShaderNodeVectorCompose::get_output_port_name(int p_port) const {
	return "vec";
}

String VisualShaderNodeVectorCompose::generate_code(Shader::Mode p_mode, VisualShader::Type p_type, int p_id, const String *p_input_vars, const String *p_output_vars, bool p_for_preview) const {
	return "\t" + p_output_vars[0] + " = vec3( " + p_input_vars[0] + " , " + p_input_vars[1] + " , " + p_input_vars[2] + " );\n";
}

VisualShaderNodeVectorCompose::VisualShaderNodeVectorCompose() {

	set_input_port_default_value(0, 0.0);
	set_input_port_default_value(1, 0.0);
	set_input_port_default_value(2, 0.0);
}

////////////// Transform Compose

String VisualShaderNodeTransformCompose::get_caption() const {
	return "TransformCompose";
}

int VisualShaderNodeTransformCompose::get_input_port_count() const {
	return 4;
}
VisualShaderNodeTransformCompose::PortType VisualShaderNodeTransformCompose::get_input_port_type(int p_port) const {
	return PORT_TYPE_VECTOR;
}
String VisualShaderNodeTransformCompose::get_input_port_name(int p_port) const {
	if (p_port == 0) {
		return "x";
	} else if (p_port == 1) {
		return "y";
	} else if (p_port == 2) {
		return "z";
	} else {
		return "origin";
	}
}

int VisualShaderNodeTransformCompose::get_output_port_count() const {
	return 1;
}
VisualShaderNodeTransformCompose::PortType VisualShaderNodeTransformCompose::get_output_port_type(int p_port) const {
	return PORT_TYPE_TRANSFORM;
}
String VisualShaderNodeTransformCompose::get_output_port_name(int p_port) const {
	return "xform";
}

String VisualShaderNodeTransformCompose::generate_code(Shader::Mode p_mode, VisualShader::Type p_type, int p_id, const String *p_input_vars, const String *p_output_vars, bool p_for_preview) const {
	return "\t" + p_output_vars[0] + " = mat4( vec4(" + p_input_vars[0] + ", 0.0) , vec4(" + p_input_vars[1] + ", 0.0) , vec4(" + p_input_vars[2] + ",0.0), vec4(" + p_input_vars[3] + ",1.0) );\n";
}

VisualShaderNodeTransformCompose::VisualShaderNodeTransformCompose() {

	set_input_port_default_value(0, Vector3());
	set_input_port_default_value(1, Vector3());
	set_input_port_default_value(2, Vector3());
	set_input_port_default_value(3, Vector3());
}

////////////// Vector Decompose
String VisualShaderNodeVectorDecompose::get_caption() const {
	return "VectorDecompose";
}

int VisualShaderNodeVectorDecompose::get_input_port_count() const {
	return 1;
}
VisualShaderNodeVectorDecompose::PortType VisualShaderNodeVectorDecompose::get_input_port_type(int p_port) const {
	return PORT_TYPE_VECTOR;
}
String VisualShaderNodeVectorDecompose::get_input_port_name(int p_port) const {
	return "vec";
}

int VisualShaderNodeVectorDecompose::get_output_port_count() const {
	return 3;
}
VisualShaderNodeVectorDecompose::PortType VisualShaderNodeVectorDecompose::get_output_port_type(int p_port) const {
	return PORT_TYPE_SCALAR;
}
String VisualShaderNodeVectorDecompose::get_output_port_name(int p_port) const {
	if (p_port == 0) {
		return "x";
	} else if (p_port == 1) {
		return "y";
	} else {
		return "z";
	}
}

String VisualShaderNodeVectorDecompose::generate_code(Shader::Mode p_mode, VisualShader::Type p_type, int p_id, const String *p_input_vars, const String *p_output_vars, bool p_for_preview) const {
	String code;
	code += "\t" + p_output_vars[0] + " = " + p_input_vars[0] + ".x;\n";
	code += "\t" + p_output_vars[1] + " = " + p_input_vars[0] + ".y;\n";
	code += "\t" + p_output_vars[2] + " = " + p_input_vars[0] + ".z;\n";
	return code;
}

VisualShaderNodeVectorDecompose::VisualShaderNodeVectorDecompose() {
	set_input_port_default_value(0, Vector3());
}

////////////// Transform Decompose

String VisualShaderNodeTransformDecompose::get_caption() const {
	return "TransformDecompose";
}

int VisualShaderNodeTransformDecompose::get_input_port_count() const {
	return 1;
}
VisualShaderNodeTransformDecompose::PortType VisualShaderNodeTransformDecompose::get_input_port_type(int p_port) const {
	return PORT_TYPE_TRANSFORM;
}
String VisualShaderNodeTransformDecompose::get_input_port_name(int p_port) const {
	return "xform";
}

int VisualShaderNodeTransformDecompose::get_output_port_count() const {
	return 4;
}
VisualShaderNodeTransformDecompose::PortType VisualShaderNodeTransformDecompose::get_output_port_type(int p_port) const {
	return PORT_TYPE_VECTOR;
}
String VisualShaderNodeTransformDecompose::get_output_port_name(int p_port) const {
	if (p_port == 0) {
		return "x";
	} else if (p_port == 1) {
		return "y";
	} else if (p_port == 2) {
		return "z";
	} else {
		return "origin";
	}
}

String VisualShaderNodeTransformDecompose::generate_code(Shader::Mode p_mode, VisualShader::Type p_type, int p_id, const String *p_input_vars, const String *p_output_vars, bool p_for_preview) const {
	String code;
	code += "\t" + p_output_vars[0] + " = " + p_input_vars[0] + "[0].xyz;\n";
	code += "\t" + p_output_vars[1] + " = " + p_input_vars[0] + "[1].xyz;\n";
	code += "\t" + p_output_vars[2] + " = " + p_input_vars[0] + "[2].xyz;\n";
	code += "\t" + p_output_vars[3] + " = " + p_input_vars[0] + "[3].xyz;\n";
	return code;
}

VisualShaderNodeTransformDecompose::VisualShaderNodeTransformDecompose() {
	set_input_port_default_value(0, Transform());
}

////////////// Scalar Uniform

String VisualShaderNodeScalarUniform::get_caption() const {
	return "ScalarUniform";
}

int VisualShaderNodeScalarUniform::get_input_port_count() const {
	return 0;
}
VisualShaderNodeScalarUniform::PortType VisualShaderNodeScalarUniform::get_input_port_type(int p_port) const {
	return PORT_TYPE_SCALAR;
}
String VisualShaderNodeScalarUniform::get_input_port_name(int p_port) const {
	return String();
}

int VisualShaderNodeScalarUniform::get_output_port_count() const {
	return 1;
}
VisualShaderNodeScalarUniform::PortType VisualShaderNodeScalarUniform::get_output_port_type(int p_port) const {
	return PORT_TYPE_SCALAR;
}
String VisualShaderNodeScalarUniform::get_output_port_name(int p_port) const {
	return ""; //no output port means the editor will be used as port
}

String VisualShaderNodeScalarUniform::generate_global(Shader::Mode p_mode, VisualShader::Type p_type, int p_id) const {
	return "uniform float " + get_uniform_name() + ";\n";
}
String VisualShaderNodeScalarUniform::generate_code(Shader::Mode p_mode, VisualShader::Type p_type, int p_id, const String *p_input_vars, const String *p_output_vars, bool p_for_preview) const {
	return "\t" + p_output_vars[0] + " = " + get_uniform_name() + ";\n";
}

VisualShaderNodeScalarUniform::VisualShaderNodeScalarUniform() {
}

////////////// Color Uniform

String VisualShaderNodeColorUniform::get_caption() const {
	return "ColorUniform";
}

int VisualShaderNodeColorUniform::get_input_port_count() const {
	return 0;
}
VisualShaderNodeColorUniform::PortType VisualShaderNodeColorUniform::get_input_port_type(int p_port) const {
	return PORT_TYPE_VECTOR;
}
String VisualShaderNodeColorUniform::get_input_port_name(int p_port) const {
	return String();
}

int VisualShaderNodeColorUniform::get_output_port_count() const {
	return 2;
}
VisualShaderNodeColorUniform::PortType VisualShaderNodeColorUniform::get_output_port_type(int p_port) const {
	return p_port == 0 ? PORT_TYPE_VECTOR : PORT_TYPE_SCALAR;
}
String VisualShaderNodeColorUniform::get_output_port_name(int p_port) const {
	return p_port == 0 ? "color" : "alpha"; //no output port means the editor will be used as port
}

String VisualShaderNodeColorUniform::generate_global(Shader::Mode p_mode, VisualShader::Type p_type, int p_id) const {

	return "uniform vec4 " + get_uniform_name() + " : hint_color;\n";
}

String VisualShaderNodeColorUniform::generate_code(Shader::Mode p_mode, VisualShader::Type p_type, int p_id, const String *p_input_vars, const String *p_output_vars, bool p_for_preview) const {
	String code = "\t" + p_output_vars[0] + " = " + get_uniform_name() + ".rgb;\n";
	code += "\t" + p_output_vars[1] + " = " + get_uniform_name() + ".a;\n";
	return code;
}

VisualShaderNodeColorUniform::VisualShaderNodeColorUniform() {
}

////////////// Vector Uniform

String VisualShaderNodeVec3Uniform::get_caption() const {
	return "VectorUniform";
}

int VisualShaderNodeVec3Uniform::get_input_port_count() const {
	return 0;
}
VisualShaderNodeVec3Uniform::PortType VisualShaderNodeVec3Uniform::get_input_port_type(int p_port) const {
	return PORT_TYPE_VECTOR;
}
String VisualShaderNodeVec3Uniform::get_input_port_name(int p_port) const {
	return String();
}

int VisualShaderNodeVec3Uniform::get_output_port_count() const {
	return 1;
}
VisualShaderNodeVec3Uniform::PortType VisualShaderNodeVec3Uniform::get_output_port_type(int p_port) const {
	return PORT_TYPE_VECTOR;
}
String VisualShaderNodeVec3Uniform::get_output_port_name(int p_port) const {
	return ""; //no output port means the editor will be used as port
}
String VisualShaderNodeVec3Uniform::generate_global(Shader::Mode p_mode, VisualShader::Type p_type, int p_id) const {
	return "uniform vec3 " + get_uniform_name() + ";\n";
}

String VisualShaderNodeVec3Uniform::generate_code(Shader::Mode p_mode, VisualShader::Type p_type, int p_id, const String *p_input_vars, const String *p_output_vars, bool p_for_preview) const {
	return "\t" + p_output_vars[0] + " = " + get_uniform_name() + ";\n";
}

VisualShaderNodeVec3Uniform::VisualShaderNodeVec3Uniform() {
}

////////////// Transform Uniform

String VisualShaderNodeTransformUniform::get_caption() const {
	return "TransformUniform";
}

int VisualShaderNodeTransformUniform::get_input_port_count() const {
	return 0;
}
VisualShaderNodeTransformUniform::PortType VisualShaderNodeTransformUniform::get_input_port_type(int p_port) const {
	return PORT_TYPE_VECTOR;
}
String VisualShaderNodeTransformUniform::get_input_port_name(int p_port) const {
	return String();
}

int VisualShaderNodeTransformUniform::get_output_port_count() const {
	return 1;
}
VisualShaderNodeTransformUniform::PortType VisualShaderNodeTransformUniform::get_output_port_type(int p_port) const {
	return PORT_TYPE_TRANSFORM;
}
String VisualShaderNodeTransformUniform::get_output_port_name(int p_port) const {
	return ""; //no output port means the editor will be used as port
}
String VisualShaderNodeTransformUniform::generate_global(Shader::Mode p_mode, VisualShader::Type p_type, int p_id) const {
	return "uniform mat4 " + get_uniform_name() + ";\n";
}

String VisualShaderNodeTransformUniform::generate_code(Shader::Mode p_mode, VisualShader::Type p_type, int p_id, const String *p_input_vars, const String *p_output_vars, bool p_for_preview) const {
	return "\t" + p_output_vars[0] + " = " + get_uniform_name() + ";\n";
}

VisualShaderNodeTransformUniform::VisualShaderNodeTransformUniform() {
}

////////////// Texture Uniform

String VisualShaderNodeTextureUniform::get_caption() const {
	return "TextureUniform";
}

int VisualShaderNodeTextureUniform::get_input_port_count() const {
	return 2;
}
VisualShaderNodeTextureUniform::PortType VisualShaderNodeTextureUniform::get_input_port_type(int p_port) const {
	return p_port == 0 ? PORT_TYPE_VECTOR : PORT_TYPE_SCALAR;
}
String VisualShaderNodeTextureUniform::get_input_port_name(int p_port) const {
	return p_port == 0 ? "uv" : "lod";
}

int VisualShaderNodeTextureUniform::get_output_port_count() const {
	return 2;
}
VisualShaderNodeTextureUniform::PortType VisualShaderNodeTextureUniform::get_output_port_type(int p_port) const {
	return p_port == 0 ? PORT_TYPE_VECTOR : PORT_TYPE_SCALAR;
}
String VisualShaderNodeTextureUniform::get_output_port_name(int p_port) const {
	return p_port == 0 ? "rgb" : "alpha";
}

String VisualShaderNodeTextureUniform::generate_global(Shader::Mode p_mode, VisualShader::Type p_type, int p_id) const {
	String code = "uniform sampler2D " + get_uniform_name();

	switch (texture_type) {
		case TYPE_DATA:
			if (color_default == COLOR_DEFAULT_BLACK)
				code += " : hint_black;\n";
			else
				code += ";\n";
			break;
		case TYPE_COLOR:
			if (color_default == COLOR_DEFAULT_BLACK)
				code += " : hint_black_albedo;\n";
			else
				code += " : hint_albedo;\n";
			break;
		case TYPE_NORMALMAP: code += " : hint_normal;\n"; break;
		case TYPE_ANISO: code += " : hint_aniso;\n"; break;
	}

	return code;
}

String VisualShaderNodeTextureUniform::generate_code(Shader::Mode p_mode, VisualShader::Type p_type, int p_id, const String *p_input_vars, const String *p_output_vars, bool p_for_preview) const {

	String id = get_uniform_name();
	String code = "\t{\n";
	if (p_input_vars[0] == String()) { //none bound, do nothing

		code += "\t\tvec4 n_tex_read = vec4(0.0);\n";
	} else if (p_input_vars[1] == String()) {
		//no lod
		code += "\t\tvec4 n_tex_read = texture( " + id + " , " + p_input_vars[0] + ".xy );\n";
	} else {
		code += "\t\tvec4 n_tex_read = textureLod( " + id + " , " + p_input_vars[0] + ".xy , " + p_input_vars[1] + " );\n";
	}

	code += "\t\t" + p_output_vars[0] + " = n_tex_read.rgb;\n";
	code += "\t\t" + p_output_vars[1] + " = n_tex_read.a;\n";
	code += "\t}\n";
	return code;
}

void VisualShaderNodeTextureUniform::set_texture_type(TextureType p_type) {

	texture_type = p_type;
	emit_changed();
}

VisualShaderNodeTextureUniform::TextureType VisualShaderNodeTextureUniform::get_texture_type() const {
	return texture_type;
}

void VisualShaderNodeTextureUniform::set_color_default(ColorDefault p_default) {
	color_default = p_default;
	emit_changed();
}
VisualShaderNodeTextureUniform::ColorDefault VisualShaderNodeTextureUniform::get_color_default() const {
	return color_default;
}

Vector<StringName> VisualShaderNodeTextureUniform::get_editable_properties() const {
	Vector<StringName> props;
	props.push_back("texture_type");
	props.push_back("color_default");
	return props;
}

void VisualShaderNodeTextureUniform::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_texture_type", "type"), &VisualShaderNodeTextureUniform::set_texture_type);
	ClassDB::bind_method(D_METHOD("get_texture_type"), &VisualShaderNodeTextureUniform::get_texture_type);

	ClassDB::bind_method(D_METHOD("set_color_default", "type"), &VisualShaderNodeTextureUniform::set_color_default);
	ClassDB::bind_method(D_METHOD("get_color_default"), &VisualShaderNodeTextureUniform::get_color_default);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "texture_type", PROPERTY_HINT_ENUM, "Data,Color,Normalmap,Aniso"), "set_texture_type", "get_texture_type");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "color_default", PROPERTY_HINT_ENUM, "White Default,Black Default"), "set_color_default", "get_color_default");

	BIND_ENUM_CONSTANT(TYPE_DATA);
	BIND_ENUM_CONSTANT(TYPE_COLOR);
	BIND_ENUM_CONSTANT(TYPE_NORMALMAP);
	BIND_ENUM_CONSTANT(TYPE_ANISO);

	BIND_ENUM_CONSTANT(COLOR_DEFAULT_WHITE);
	BIND_ENUM_CONSTANT(COLOR_DEFAULT_BLACK);
}

VisualShaderNodeTextureUniform::VisualShaderNodeTextureUniform() {
	texture_type = TYPE_DATA;
	color_default = COLOR_DEFAULT_WHITE;
}

////////////// CubeMap Uniform

String VisualShaderNodeCubeMapUniform::get_caption() const {
	return "CubeMapUniform";
}

int VisualShaderNodeCubeMapUniform::get_input_port_count() const {
	return 2;
}
VisualShaderNodeCubeMapUniform::PortType VisualShaderNodeCubeMapUniform::get_input_port_type(int p_port) const {
	return p_port == 0 ? PORT_TYPE_VECTOR : PORT_TYPE_SCALAR;
}
String VisualShaderNodeCubeMapUniform::get_input_port_name(int p_port) const {
	return p_port == 0 ? "normal" : "lod";
}

int VisualShaderNodeCubeMapUniform::get_output_port_count() const {
	return 2;
}
VisualShaderNodeCubeMapUniform::PortType VisualShaderNodeCubeMapUniform::get_output_port_type(int p_port) const {
	return p_port == 0 ? PORT_TYPE_VECTOR : PORT_TYPE_SCALAR;
}
String VisualShaderNodeCubeMapUniform::get_output_port_name(int p_port) const {
	return p_port == 0 ? "rgb" : "alpha";
}

String VisualShaderNodeCubeMapUniform::generate_code(Shader::Mode p_mode, VisualShader::Type p_type, int p_id, const String *p_input_vars, const String *p_output_vars, bool p_for_preview) const {
	return String();
}

VisualShaderNodeCubeMapUniform::VisualShaderNodeCubeMapUniform() {
}
