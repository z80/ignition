#include "voxel_block_serializer.h"
#include "../math/vector3i.h"
#include "../thirdparty/lz4/lz4.h"
#include "../util/macros.h"
#include "../voxel_buffer.h"
#include "../voxel_memory_pool.h"

#include <core/io/marshalls.h>
#include <core/io/stream_peer.h>
//#include <core/map.h>
#include <core/os/file_access.h>

namespace {
// TODO Introduce versionning
//const uint16_t BLOCK_VERSION = 0;
//const unsigned int BLOCK_VERSION_HEADER_SIZE = sizeof(uint16_t);
const unsigned int BLOCK_TRAILING_MAGIC = 0x900df00d;
const unsigned int BLOCK_TRAILING_MAGIC_SIZE = 4;
const unsigned int BLOCK_METADATA_HEADER_SIZE = sizeof(uint32_t);
} // namespace

size_t get_metadata_size_in_bytes(const VoxelBuffer &buffer) {
	size_t size = 0;

	const Map<Vector3i, Variant>::Element *elem = buffer.get_voxel_metadata().front();
	while (elem != nullptr) {
		const Vector3i pos = elem->key();

		ERR_FAIL_COND_V_MSG(pos.x < 0 || static_cast<uint32_t>(pos.x) >= VoxelBuffer::MAX_SIZE, 0,
				"Invalid voxel metadata X position");
		ERR_FAIL_COND_V_MSG(pos.y < 0 || static_cast<uint32_t>(pos.y) >= VoxelBuffer::MAX_SIZE, 0,
				"Invalid voxel metadata Y position");
		ERR_FAIL_COND_V_MSG(pos.z < 0 || static_cast<uint32_t>(pos.z) >= VoxelBuffer::MAX_SIZE, 0,
				"Invalid voxel metadata Z position");

		size += 3 * sizeof(uint16_t); // Positions are stored as 3 unsigned shorts

		int len;
		const Error err = encode_variant(elem->value(), nullptr, len, false);
		ERR_FAIL_COND_V_MSG(err != OK, 0, "Error when trying to encode voxel metadata.");
		size += len;

		elem = elem->next();
	}

	// If no metadata is found at all, nothing is serialized, not even null.
	// It spares 24 bytes (40 if real_t == double),
	// and is backward compatible with saves made before introduction of metadata.

	if (size != 0 || buffer.get_block_metadata() != Variant()) {
		int len;
		// Get size first by invoking the function is "length mode"
		const Error err = encode_variant(buffer.get_block_metadata(), nullptr, len, false);
		ERR_FAIL_COND_V_MSG(err != OK, 0, "Error when trying to encode block metadata.");
		size += len;
	}

	return size;
}

template <typename T>
inline void write(uint8_t *&dst, T d) {
	*(T *)dst = d;
	dst += sizeof(T);
}

template <typename T>
inline T read(uint8_t *&src) {
	T d = *(T *)src;
	src += sizeof(T);
	return d;
}

// The target buffer MUST have correct size. Recoverable errors must have been checked before.
void serialize_metadata(uint8_t *p_dst, const VoxelBuffer &buffer, const size_t metadata_size) {
	uint8_t *dst = p_dst;

	{
		int written_length;
		encode_variant(buffer.get_block_metadata(), dst, written_length, false);
		dst += written_length;

		// I chose to cast this way to fix a GCC warning.
		// If dst - p_dst is negative (which is wrong), it will wrap and cause a justified assertion failure
		CRASH_COND_MSG(static_cast<size_t>(dst - p_dst) > metadata_size, "Wrote block metadata out of expected bounds");
	}

	const Map<Vector3i, Variant>::Element *elem = buffer.get_voxel_metadata().front();
	while (elem != nullptr) {
		// Serializing key as ushort because it's more than enough for a 3D dense array
		static_assert(VoxelBuffer::MAX_SIZE <= 65535, "Maximum size exceeds serialization support");
		const Vector3i pos = elem->key();
		write<uint16_t>(dst, pos.x);
		write<uint16_t>(dst, pos.y);
		write<uint16_t>(dst, pos.z);

		int written_length;
		const Error err = encode_variant(elem->value(), dst, written_length, false);
		CRASH_COND_MSG(err != OK, "Error when trying to encode voxel metadata.");
		dst += written_length;

		CRASH_COND_MSG(static_cast<size_t>(dst - p_dst) > metadata_size, "Wrote voxel metadata out of expected bounds");

		elem = elem->next();
	}

	CRASH_COND_MSG(static_cast<size_t>(dst - p_dst) != metadata_size,
			String("Written metadata doesn't match expected count (expected {0}, got {1})")
					.format(varray(SIZE_T_TO_VARIANT(metadata_size), (int)(dst - p_dst))));
}

bool deserialize_metadata(uint8_t *p_src, VoxelBuffer &buffer, const size_t metadata_size) {
	uint8_t *src = p_src;
	size_t remaining_length = metadata_size;

	{
		Variant block_metadata;
		int read_length;
		const Error err = decode_variant(block_metadata, src, remaining_length, &read_length, false);
		ERR_FAIL_COND_V_MSG(err != OK, false, "Failed to deserialize block metadata");
		remaining_length -= read_length;
		src += read_length;
		CRASH_COND_MSG(remaining_length > metadata_size, "Block metadata size underflow");
		buffer.set_block_metadata(block_metadata);
	}

	while (remaining_length > 0) {
		Vector3i pos;
		pos.x = read<uint16_t>(src);
		pos.y = read<uint16_t>(src);
		pos.z = read<uint16_t>(src);
		remaining_length -= 3 * sizeof(uint16_t);

		Variant metadata;
		int read_length;
		const Error err = decode_variant(metadata, src, remaining_length, &read_length, false);
		ERR_FAIL_COND_V_MSG(err != OK, false, "Failed to deserialize block metadata");
		remaining_length -= read_length;
		src += read_length;
		CRASH_COND_MSG(remaining_length > metadata_size, "Block metadata size underflow");

		buffer.set_voxel_metadata(pos, metadata);
	}

	CRASH_COND_MSG(remaining_length != 0, "Did not read expected size");
	return true;
}

size_t get_size_in_bytes(const VoxelBuffer &buffer, size_t &metadata_size) {
	size_t size = 0;
	const Vector3i size_in_voxels = buffer.get_size();

	for (unsigned int channel_index = 0; channel_index < VoxelBuffer::MAX_CHANNELS; ++channel_index) {
		const VoxelBuffer::Compression compression = buffer.get_channel_compression(channel_index);
		size += 1;

		switch (compression) {
			case VoxelBuffer::COMPRESSION_NONE: {
				size += size_in_voxels.volume() * sizeof(uint8_t);
			} break;

			case VoxelBuffer::COMPRESSION_UNIFORM: {
				size += 1;
			} break;

			default:
				ERR_PRINT("Unhandled compression mode");
				CRASH_NOW();
		}
	}

	metadata_size = get_metadata_size_in_bytes(buffer);

	size_t metadata_size_with_header = 0;
	if (metadata_size > 0) {
		metadata_size_with_header = metadata_size + BLOCK_METADATA_HEADER_SIZE;
	}

	return size + metadata_size_with_header + BLOCK_TRAILING_MAGIC_SIZE;
}

const std::vector<uint8_t> &VoxelBlockSerializerInternal::serialize(VoxelBuffer &voxel_buffer) {
	size_t metadata_size = 0;
	const size_t data_size = get_size_in_bytes(voxel_buffer, metadata_size);
	_data.resize(data_size);

	CRASH_COND(_file_access_memory.open_custom(_data.data(), _data.size()) != OK);
	FileAccessMemory *f = &_file_access_memory;

	for (unsigned int channel_index = 0; channel_index < VoxelBuffer::MAX_CHANNELS; ++channel_index) {
		VoxelBuffer::Compression compression = voxel_buffer.get_channel_compression(channel_index);
		f->store_8(static_cast<uint8_t>(compression));

		switch (compression) {
			case VoxelBuffer::COMPRESSION_NONE: {
				ArraySlice<uint8_t> data;
				CRASH_COND(!voxel_buffer.get_channel_raw(channel_index, data));
				f->store_buffer(data.data(), data.size());
			} break;

			case VoxelBuffer::COMPRESSION_UNIFORM: {
				uint64_t v = voxel_buffer.get_voxel(Vector3i(), channel_index);
				switch (voxel_buffer.get_channel_depth(channel_index)) {
					case VoxelBuffer::DEPTH_8_BIT:
						f->store_8(v);
						break;
					case VoxelBuffer::DEPTH_16_BIT:
						f->store_16(v);
						break;
					case VoxelBuffer::DEPTH_32_BIT:
						f->store_32(v);
						break;
					case VoxelBuffer::DEPTH_64_BIT:
						f->store_64(v);
						break;
					default:
						CRASH_NOW();
				}
			} break;

			default:
				CRASH_COND("Unhandled compression mode");
		}
	}

	// Metadata has more reasons to fail. If a recoverable error occurs prior to serializing,
	// we just discard all metadata as if it was empty.
	if (metadata_size > 0) {
		f->store_32(metadata_size);
		_metadata_tmp.resize(metadata_size);
		// This function brings me joy. </irony>
		serialize_metadata(_metadata_tmp.data(), voxel_buffer, metadata_size);
		f->store_buffer(_metadata_tmp.data(), _metadata_tmp.size());
	}

	f->store_32(BLOCK_TRAILING_MAGIC);

	return _data;
}

bool VoxelBlockSerializerInternal::deserialize(const std::vector<uint8_t> &p_data, VoxelBuffer &out_voxel_buffer) {
	CRASH_COND(_file_access_memory.open_custom(p_data.data(), p_data.size()) != OK);
	FileAccessMemory *f = &_file_access_memory;

	for (unsigned int channel_index = 0; channel_index < VoxelBuffer::MAX_CHANNELS; ++channel_index) {
		uint8_t compression_value = f->get_8();
		ERR_FAIL_COND_V_MSG(compression_value >= VoxelBuffer::COMPRESSION_COUNT, false,
				"At offset 0x" + String::num_int64(f->get_position() - 1, 16));
		VoxelBuffer::Compression compression = (VoxelBuffer::Compression)compression_value;

		switch (compression) {
			case VoxelBuffer::COMPRESSION_NONE: {
				out_voxel_buffer.decompress_channel(channel_index);

				ArraySlice<uint8_t> buffer;
				CRASH_COND(!out_voxel_buffer.get_channel_raw(channel_index, buffer));

				uint32_t read_len = f->get_buffer(buffer.data(), buffer.size());
				if (read_len != buffer.size()) {
					ERR_PRINT("Unexpected end of file");
					return false;
				}

			} break;

			case VoxelBuffer::COMPRESSION_UNIFORM: {
				uint64_t v;
				switch (out_voxel_buffer.get_channel_depth(channel_index)) {
					case VoxelBuffer::DEPTH_8_BIT:
						v = f->get_8();
						break;
					case VoxelBuffer::DEPTH_16_BIT:
						v = f->get_16();
						break;
					case VoxelBuffer::DEPTH_32_BIT:
						v = f->get_32();
						break;
					case VoxelBuffer::DEPTH_64_BIT:
						v = f->get_64();
						break;
					default:
						CRASH_NOW();
				}
				out_voxel_buffer.clear_channel(channel_index, v);
			} break;

			default:
				ERR_PRINT("Unhandled compression mode");
				return false;
		}
	}

	if (p_data.size() - f->get_position() > BLOCK_TRAILING_MAGIC_SIZE) {
		size_t metadata_size = f->get_32();
		_metadata_tmp.resize(metadata_size);
		f->get_buffer(_metadata_tmp.data(), _metadata_tmp.size());
		deserialize_metadata(_metadata_tmp.data(), out_voxel_buffer, _metadata_tmp.size());
	}

	// Failure at this indicates file corruption
	ERR_FAIL_COND_V_MSG(f->get_32() != BLOCK_TRAILING_MAGIC, false,
			"At offset 0x" + String::num_int64(f->get_position() - 4, 16));
	return true;
}

const std::vector<uint8_t> &VoxelBlockSerializerInternal::serialize_and_compress(VoxelBuffer &voxel_buffer) {
	const std::vector<uint8_t> &data = serialize(voxel_buffer);

	unsigned int header_size = sizeof(unsigned int);
	_compressed_data.resize(header_size + LZ4_compressBound(data.size()));

	// Write header
	CRASH_COND(_file_access_memory.open_custom(_compressed_data.data(), _compressed_data.size()) != OK);
	_file_access_memory.store_32(data.size());
	_file_access_memory.close();

	int compressed_size = LZ4_compress_default(
			(const char *)data.data(),
			(char *)_compressed_data.data() + header_size,
			data.size(),
			_compressed_data.size() - header_size);

	CRASH_COND(compressed_size < 0);
	CRASH_COND(compressed_size == 0);

	_compressed_data.resize(header_size + compressed_size);
	return _compressed_data;
}

bool VoxelBlockSerializerInternal::decompress_and_deserialize(const std::vector<uint8_t> &p_data, VoxelBuffer &out_voxel_buffer) {
	// Read header
	unsigned int header_size = sizeof(unsigned int);
	ERR_FAIL_COND_V(_file_access_memory.open_custom(p_data.data(), p_data.size()) != OK, false);
	unsigned int decompressed_size = _file_access_memory.get_32();
	_file_access_memory.close();

	_data.resize(decompressed_size);

	unsigned int actually_decompressed_size = LZ4_decompress_safe(
			(const char *)p_data.data() + header_size,
			(char *)_data.data(),
			p_data.size() - header_size,
			_data.size());

	ERR_FAIL_COND_V_MSG(actually_decompressed_size < 0, false,
			String("LZ4 decompression error {0}").format(varray(actually_decompressed_size)));

	ERR_FAIL_COND_V_MSG(actually_decompressed_size != decompressed_size, false,
			String("Expected {0} bytes, obtained {1}").format(varray(decompressed_size, actually_decompressed_size)));

	return deserialize(_data, out_voxel_buffer);
}

bool VoxelBlockSerializerInternal::decompress_and_deserialize(FileAccess *f, unsigned int size_to_read, VoxelBuffer &out_voxel_buffer) {
	ERR_FAIL_COND_V(f == nullptr, false);

	_compressed_data.resize(size_to_read);
	unsigned int read_size = f->get_buffer(_compressed_data.data(), size_to_read);
	ERR_FAIL_COND_V(read_size != size_to_read, false);

	return decompress_and_deserialize(_compressed_data, out_voxel_buffer);
}

int VoxelBlockSerializerInternal::serialize(Ref<StreamPeer> peer, Ref<VoxelBuffer> voxel_buffer, bool compress) {
	if (compress) {
		const std::vector<uint8_t> &data = serialize_and_compress(**voxel_buffer);
		peer->put_data(data.data(), data.size());
		return data.size();

	} else {
		const std::vector<uint8_t> &data = serialize(**voxel_buffer);
		peer->put_data(data.data(), data.size());
		return data.size();
	}
}

void VoxelBlockSerializerInternal::deserialize(Ref<StreamPeer> peer, Ref<VoxelBuffer> voxel_buffer, int size, bool decompress) {
	if (decompress) {
		_compressed_data.resize(size);
		const Error err = peer->get_data(_compressed_data.data(), _compressed_data.size());
		ERR_FAIL_COND(err != OK);
		bool success = decompress_and_deserialize(_compressed_data, **voxel_buffer);
		ERR_FAIL_COND(!success);

	} else {
		_data.resize(size);
		const Error err = peer->get_data(_data.data(), _data.size());
		ERR_FAIL_COND(err != OK);
		deserialize(_data, **voxel_buffer);
	}
}

int VoxelBlockSerializer::serialize(Ref<StreamPeer> peer, Ref<VoxelBuffer> voxel_buffer, bool compress) {
	ERR_FAIL_COND_V(voxel_buffer.is_null(), 0);
	ERR_FAIL_COND_V(peer.is_null(), 0);
	return _serializer.serialize(peer, voxel_buffer, compress);
}

void VoxelBlockSerializer::deserialize(Ref<StreamPeer> peer, Ref<VoxelBuffer> voxel_buffer, int size, bool decompress) {
	ERR_FAIL_COND(voxel_buffer.is_null());
	ERR_FAIL_COND(peer.is_null());
	ERR_FAIL_COND(size <= 0);
	_serializer.deserialize(peer, voxel_buffer, size, decompress);
}

void VoxelBlockSerializer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("serialize", "peer", "voxel_buffer", "compress"), &VoxelBlockSerializer::serialize);
	ClassDB::bind_method(D_METHOD("deserialize", "peer", "voxel_buffer", "size", "decompress"), &VoxelBlockSerializer::deserialize);
}
