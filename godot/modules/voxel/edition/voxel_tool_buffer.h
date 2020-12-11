#ifndef VOXEL_TOOL_BUFFER_H
#define VOXEL_TOOL_BUFFER_H

#include "voxel_tool.h"

class VoxelBuffer;

class VoxelToolBuffer : public VoxelTool {
	GDCLASS(VoxelToolBuffer, VoxelTool)
public:
	VoxelToolBuffer(Ref<VoxelBuffer> vb);

	bool is_area_editable(const Rect3i &box) const override;
	void paste(Vector3i p_pos, Ref<VoxelBuffer> p_voxels, uint64_t mask_value) override;

	void set_voxel_metadata(Vector3i pos, Variant meta) override;
	Variant get_voxel_metadata(Vector3i pos) override;

protected:
	uint64_t _get_voxel(Vector3i pos) override;
	float _get_voxel_f(Vector3i pos) override;
	void _set_voxel(Vector3i pos, uint64_t v) override;
	void _set_voxel_f(Vector3i pos, float v) override;
	void _post_edit(const Rect3i &box) override;

private:
	Ref<VoxelBuffer> _buffer;
};

#endif // VOXEL_TOOL_BUFFER_H
