#ifndef VOXEL_MESHER_BLOCKY_H
#define VOXEL_MESHER_BLOCKY_H

#include "../../util/zprofiling.h"
#include "../voxel_mesher.h"
#include "voxel_library.h"
#include <core/reference.h>
#include <scene/resources/mesh.h>
#include <vector>

class VoxelMesherBlocky : public VoxelMesher {
	GDCLASS(VoxelMesherBlocky, VoxelMesher)

public:
	static const unsigned int MAX_MATERIALS = 8; // Arbitrary. Tweak if needed.
	static const int PADDING = 1;

	VoxelMesherBlocky();

	void set_library(Ref<VoxelLibrary> library);
	Ref<VoxelLibrary> get_library() const { return _library; }

	void set_occlusion_darkness(float darkness);
	float get_occlusion_darkness() const { return _baked_occlusion_darkness; }

	void set_occlusion_enabled(bool enable);
	bool get_occlusion_enabled() const { return _bake_occlusion; }

	void build(VoxelMesher::Output &output, const VoxelMesher::Input &input) override;

	VoxelMesher *clone() override;

	// Using std::vector because they make this mesher twice as fast than Godot Vectors.
	// See why: https://github.com/godotengine/godot/issues/24731
	struct Arrays {
		std::vector<Vector3> positions;
		std::vector<Vector3> normals;
		std::vector<Vector2> uvs;
		std::vector<Color> colors;
		std::vector<int> indices;
	};

protected:
	static void _bind_methods();

private:
	Ref<VoxelLibrary> _library;
	FixedArray<Arrays, MAX_MATERIALS> _arrays_per_material;
	float _baked_occlusion_darkness;
	bool _bake_occlusion;
};

#endif // VOXEL_MESHER_BLOCKY_H
