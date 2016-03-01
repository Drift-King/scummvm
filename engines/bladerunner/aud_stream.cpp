/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#include "bladerunner/aud_stream.h"

#include "bladerunner/audio_player.h"

#include "common/util.h"

namespace BladeRunner {

AudStream::AudStream(byte *data)
	: _cache(nullptr)
{
	init(data);
}

AudStream::AudStream(AudioCache *cache, int32 hash)
	: _cache(cache), _hash(hash)
{
	_cache->incRef(_hash);

	init(_cache->findByHash(_hash));
}

void AudStream::init(byte *data)
{
	_data = data;
	_end = _data + READ_LE_UINT32(_data + 2) + 12;
	assert(_end - _data >= 12);

	_compressionType = *(_data + 11);

	_deafBlockRemain = 0;
	_p = _data + 12;
}

AudStream::~AudStream() {
	if (_cache)
		_cache->decRef(_hash);
}

int AudStream::readBuffer(int16 *buffer, const int numSamples) {
	int samplesRead = 0;

	assert(numSamples % 2 == 0);

	if (_compressionType == 99) {
		while (samplesRead < numSamples) {
			if (_deafBlockRemain == 0) {
				if (_end - _p == 0)
					break;

				assert(_end - _p >= 6);

				uint16 blockSize     = READ_LE_UINT16(_p);
				uint16 blockOutSize  = READ_LE_UINT16(_p + 2);
				uint32 sig           = READ_LE_UINT32(_p + 4);
				_p += 8;

				assert(sig == 0xdeaf);
				assert(_end - _p >= blockSize);
				assert(blockOutSize = 4 * blockSize);

				_deafBlockRemain = blockSize;
			}

			assert(_end - _p >= _deafBlockRemain);

			int bytesConsumed = MIN<int>(_deafBlockRemain, (numSamples - samplesRead) / 2);

			_decoder.decode(_p, bytesConsumed, buffer + samplesRead);
			_p += bytesConsumed;
			_deafBlockRemain -= bytesConsumed;

			samplesRead += 2 * bytesConsumed;
		}
	} else {
		assert(0 && "readBuffer: Unimplemented");
	}

	return samplesRead;
}

bool AudStream::rewind() {
	_p = _data + 12;
	_decoder.setParameters(0, 0);
	return true;
}

} // End of namespace BladeRunner