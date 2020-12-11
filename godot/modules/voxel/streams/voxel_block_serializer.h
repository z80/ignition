#ifndef VOXEL_BLOCK_SERIALIZER_H
#define VOXEL_BLOCK_SERIALIZER_H

#include <core/io/file_access_memory.h>
#include <core/reference.h>
#include <vector>

class VoxelBuffer;
class StreamPeer;

class VoxelBlockSerializerInternal {
	// Had to be named differently to not conflict with the wrapper for Godot script API
public:
	const std::vector<uint8_t> &serialize(VoxelBuffer &voxel_buffer);
	bool deserialize(const std::vector<uint8_t> &p_data, VoxelBuffer &out_voxel_buffer);

	const std::vector<uint8_t> &serialize_and_compress(VoxelBuffer &voxel_buffer);
	bool decompress_and_deserialize(const std::vector<uint8_t> &p_data, VoxelBuffer &out_voxel_buffer);
	bool decompress_and_deserialize(FileAccess *f, unsigned int size_to_read, VoxelBuffer &out_voxel_buffer);

	int serialize(Ref<StreamPeer> peer, Ref<VoxelBuffer> voxel_buffer, bool compress);
	void deserialize(Ref<StreamPeer> peer, Ref<VoxelBuffer> voxel_buffer, int size, bool decompress);

private:
	std::vector<uint8_t> _data;
	std::vector<uint8_t> _compressed_data;
	std::vector<uint8_t> _metadata_tmp;
	FileAccessMemory _file_access_memory;
};

class VoxelBlockSerializer : public Reference {
	GDCLASS(VoxelBlockSerializer, Reference)
public:
	int serialize(Ref<StreamPeer> peer, Ref<VoxelBuffer> voxel_buffer, bool compress);
	void deserialize(Ref<StreamPeer> peer, Ref<VoxelBuffer> voxel_buffer, int size, bool decompress);

private:
	static void _bind_methods();

	VoxelBlockSerializerInternal _serializer;
};

#endif // VOXEL_BLOCK_SERIALIZER_H
