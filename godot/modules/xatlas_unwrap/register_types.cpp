/*************************************************************************/
/*  register_types.cpp                                                   */
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

#include "register_types.h"
#include "core/error_macros.h"
#include "thirdparty/xatlas/xatlas.h"

#include <stdio.h>
#include <stdlib.h>
extern bool (*array_mesh_lightmap_unwrap_callback)(float p_texel_size, const float *p_vertices, const float *p_normals, int p_vertex_count, const int *p_indices, const int *p_face_materials, int p_index_count, float **r_uv, int **r_vertex, int *r_vertex_count, int **r_index, int *r_index_count, int *r_size_hint_x, int *r_size_hint_y);

bool xatlas_mesh_lightmap_unwrap_callback(float p_texel_size, const float *p_vertices, const float *p_normals, int p_vertex_count, const int *p_indices, const int *p_face_materials, int p_index_count, float **r_uv, int **r_vertex, int *r_vertex_count, int **r_index, int *r_index_count, int *r_size_hint_x, int *r_size_hint_y) {

	//set up input mesh
	xatlas::InputMesh input_mesh;
	input_mesh.indexData = malloc(sizeof(int) * p_index_count);
	input_mesh.indexCount = p_index_count;
	input_mesh.indexFormat = xatlas::IndexFormat::Float; //really xatlas?
	input_mesh.faceMaterialData = (uint16_t *)malloc(sizeof(uint16_t) * p_index_count);

	for (int i = 0; i < p_index_count; i++) {
		int *index = (int *)input_mesh.indexData;
		index[i] = p_indices[i];
	}
	for (int i = 0; i < p_index_count / 3; i++) {
		uint16_t *mat_index = (uint16_t *)input_mesh.faceMaterialData;
		mat_index[i] = p_face_materials[i];
	}

	input_mesh.vertexCount = p_vertex_count;
	input_mesh.vertexPositionData = malloc(sizeof(float) * p_vertex_count * 3);
	input_mesh.vertexPositionStride = sizeof(float) * 3;
	input_mesh.vertexNormalData = malloc(sizeof(float) * p_vertex_count * 3);
	input_mesh.vertexNormalStride = sizeof(float) * 3;

	//material is a better hint than this i guess?
	input_mesh.vertexUvData = NULL;
	input_mesh.vertexUvStride = 0;

	for (int i = 0; i < p_vertex_count * 3; i++) {
		float *vertex_ptr = (float *)input_mesh.vertexPositionData;
		float *normal_ptr = (float *)input_mesh.vertexNormalData;

		vertex_ptr[i] = p_vertices[i];
		normal_ptr[i] = p_normals[i];
	}

	xatlas::CharterOptions chart_options;
	xatlas::PackerOptions pack_options;

	pack_options.method = xatlas::PackMethod::TexelArea;
	pack_options.texelArea = 1.0 / p_texel_size;
	pack_options.quality = 3;

	xatlas::Atlas *atlas = xatlas::Create();
	printf("adding mesh..\n");
	xatlas::AddMeshError err = xatlas::AddMesh(atlas, input_mesh);
	ERR_EXPLAINC(xatlas::StringForEnum(err.code));
	ERR_FAIL_COND_V(err.code != xatlas::AddMeshErrorCode::Success, false);

	printf("generate..\n");
	xatlas::Generate(atlas, chart_options, pack_options);

	*r_size_hint_x = xatlas::GetWidth(atlas);
	*r_size_hint_y = xatlas::GetHeight(atlas);

	float w = *r_size_hint_x;
	float h = *r_size_hint_y;

	if (w == 0 || h == 0) {
		return false; //could not bake
	}

	const xatlas::OutputMesh *const *output_meshes = xatlas::GetOutputMeshes(atlas);

	const xatlas::OutputMesh *output = output_meshes[0];

	*r_vertex = (int *)malloc(sizeof(int) * output->vertexCount);
	*r_uv = (float *)malloc(sizeof(float) * output->vertexCount * 2);
	*r_index = (int *)malloc(sizeof(int) * output->indexCount);

	float max_x = 0;
	float max_y = 0;
	for (uint32_t i = 0; i < output->vertexCount; i++) {
		(*r_vertex)[i] = output->vertexArray[i].xref;
		(*r_uv)[i * 2 + 0] = output->vertexArray[i].uv[0] / w;
		(*r_uv)[i * 2 + 1] = output->vertexArray[i].uv[1] / h;
		max_x = MAX(max_x, output->vertexArray[i].uv[0]);
		max_y = MAX(max_y, output->vertexArray[i].uv[1]);
	}

	printf("final texsize: %f,%f - max %f,%f\n", w, h, max_x, max_y);
	*r_vertex_count = output->vertexCount;

	for (uint32_t i = 0; i < output->indexCount; i++) {
		(*r_index)[i] = output->indexArray[i];
	}

	*r_index_count = output->indexCount;

	//xatlas::Destroy(atlas);
	free((void *)input_mesh.indexData);
	free((void *)input_mesh.vertexPositionData);
	free((void *)input_mesh.vertexNormalData);
	free((void *)input_mesh.faceMaterialData);
	printf("done");
	return true;
}

void register_xatlas_unwrap_types() {

	array_mesh_lightmap_unwrap_callback = xatlas_mesh_lightmap_unwrap_callback;
}

void unregister_xatlas_unwrap_types() {
}
