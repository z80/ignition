// Copyright © 2019 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#include "LZ4Format.h"
#include "lz4/lz4frame.h"
#include "profiler/Profiler.h"
#include <SDL_endian.h>
#include <functional>
#include <memory>

bool lz4::IsLZ4Format(const char *data, size_t length)
{
	const uint32_t magic = *reinterpret_cast<const uint32_t *>(data);

	return magic == SDL_SwapLE32(0x184D2204);
}

template <typename T>
static void checkError(std::size_t errorCode)
{
	if (LZ4F_isError(errorCode)) {
		throw T(LZ4F_getErrorName(errorCode));
	}
}

std::string lz4::DecompressLZ4(const char *data, size_t length)
{
	PROFILE_SCOPED()
	LZ4F_dctx *_tmp;
	LZ4F_errorCode_t err = LZ4F_createDecompressionContext(&_tmp, LZ4F_VERSION);
	checkError<lz4::DecompressionFailedException>(err);

	std::unique_ptr<LZ4F_dctx, std::function<size_t(LZ4F_dctx *)>> dctx(_tmp, LZ4F_freeDecompressionContext);

	const char *read_ptr = data;
	std::size_t read_len = length;
	std::size_t write_len = 0;

	LZ4F_frameInfo_t frame = LZ4F_INIT_FRAMEINFO;
	std::size_t nextLen = LZ4F_getFrameInfo(dctx.get(), &frame, read_ptr, &read_len);
	checkError<lz4::DecompressionFailedException>(nextLen);

	const std::size_t buffer_len = 1 << 16;
	std::unique_ptr<char[]> decompress_buffer(new char[buffer_len]);

	std::string out;

	while (nextLen != 0) {
		read_ptr += read_len;
		read_len = length - (read_ptr - data);
		write_len = buffer_len;

		nextLen = LZ4F_decompress(dctx.get(), decompress_buffer.get(), &write_len, read_ptr, &read_len, NULL);
		checkError<lz4::DecompressionFailedException>(nextLen);

		out.append(decompress_buffer.get(), write_len);
	}

	decompress_buffer.reset();
	dctx.reset();

	return out;
}

std::unique_ptr<char[]> lz4::CompressLZ4(const std::string &data, const int lz4_preset, std::size_t &outSize)
{
	PROFILE_SCOPED()
	LZ4F_preferences_t pref = LZ4F_INIT_PREFERENCES;
	pref.compressionLevel = lz4_preset;

	std::size_t compressBound = LZ4F_compressFrameBound(data.size(), &pref);
	std::unique_ptr<char[]> out(new char[compressBound]);

	std::size_t _size = LZ4F_compressFrame(out.get(), compressBound, data.data(), data.size(), &pref);
	checkError<lz4::CompressionFailedException>(_size);

	outSize = _size;
	return std::move(out);
}
