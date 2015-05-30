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

#include "sherlock/sherlock.h"
#include "sherlock/sound.h"
#include "common/config-manager.h"
#include "audio/audiostream.h"
#include "common/algorithm.h"
#include "audio/mixer.h"
#include "audio/decoders/raw.h"

namespace Sherlock {

static const int8 creativeADPCM_ScaleMap[64] = {
	0,  1,  2,  3,  4,  5,  6,  7,  0,  -1,  -2,  -3,  -4,  -5,  -6,  -7,
	1,  3,  5,  7,  9, 11, 13, 15, -1,  -3,  -5,  -7,  -9, -11, -13, -15,
	2,  6, 10, 14, 18, 22, 26, 30, -2,  -6, -10, -14, -18, -22, -26, -30,
	4, 12, 20, 28, 36, 44, 52, 60, -4, -12, -20, -28, -36, -44, -52, -60
};

static const uint8 creativeADPCM_AdjustMap[64] = {
	0, 0, 0, 0, 0, 16, 16, 16,
	0, 0, 0, 0, 0, 16, 16, 16,
	240, 0, 0, 0, 0, 16, 16, 16,
	240, 0, 0, 0, 0, 16, 16, 16,
	240, 0, 0, 0, 0, 16, 16, 16,
	240, 0, 0, 0, 0, 16, 16, 16,
	240, 0, 0, 0, 0,  0,  0,  0,
	240, 0, 0, 0, 0,  0,  0,  0
};

/*----------------------------------------------------------------*/

Sound::Sound(SherlockEngine *vm, Audio::Mixer *mixer) : _vm(vm), _mixer(mixer) {
	_digitized = false;
	_voices = 0;
	_diskSoundPlaying = false;
	_soundPlaying = false;
	_soundIsOn = &_soundPlaying;
	_curPriority = 0;
	_digiBuf = nullptr;
	_midiDrvLoaded = false;
	_musicVolume = 0;

	_soundOn = true;
	_speechOn = true;

	_vm->_res->addToCache("MUSIC.LIB");
	if (!_vm->_interactiveFl)
		_vm->_res->addToCache("TITLE.SND");
	else {
		_vm->_res->addToCache("MUSIC.LIB");
		
		if (IS_ROSE_TATTOO) {
			_vm->_res->addToCache("SOUND.LIB");
		} else {
			_vm->_res->addToCache("SND.SND");

			if (!_vm->isDemo()) {
				_vm->_res->addToCache("TITLE.SND");
				_vm->_res->addToCache("EPILOGUE.SND");
			}
		}
	}
}

void Sound::syncSoundSettings() {
	_digitized = !ConfMan.getBool("mute");
	_voices = !ConfMan.getBool("mute") && !ConfMan.getBool("speech_mute") ? 1 : 0;
}

void Sound::loadSound(const Common::String &name, int priority) {
	// No implementation required in ScummVM
	warning("loadSound");
}

byte Sound::decodeSample(byte sample, byte &reference, int16 &scale) {
	int16 samp = sample + scale;
	int16 ref = 0;

	// clip bad ADPCM-4 sample
	samp = CLIP<int16>(samp, 0, 63);

	ref = reference + creativeADPCM_ScaleMap[samp];
	if (ref > 0xff) {
		reference = 0xff;
	} else {
		if (ref < 0x00) {
			reference = 0;
		} else {
			reference = (uint8)(ref & 0xff);
		}
	}

	scale = (scale + creativeADPCM_AdjustMap[samp]) & 0xff;
	return reference;
}

bool Sound::playSound(const Common::String &name, WaitType waitType, int priority, const char *libraryFilename) {
	Resources &res = *_vm->_res;
	stopSound();

	Common::String filename = name;
	if (!filename.contains('.'))
		filename += ".SND";

	Common::String libFilename(libraryFilename);
	Common::SeekableReadStream *stream = libFilename.empty() ? res.load(filename) : res.load(filename, libFilename);

	if (!stream)
		error("Unable to find sound file '%s'", filename.c_str());

	stream->skip(2);
	int size = stream->readUint32BE();
	int rate = stream->readUint16BE();
	byte *data = (byte *)malloc(size);
	byte *ptr = data;
	stream->read(ptr, size);

	assert(size > 2);

	byte *decoded = (byte *)malloc((size - 1) * 2);

	// Holmes uses Creative ADPCM 4-bit data
	int counter = 0;
	byte reference = ptr[0];
	int16 scale = 0;

	for(int i = 1; i < size; i++) {
		decoded[counter++] = decodeSample((ptr[i]>>4)&0x0f, reference, scale);
		decoded[counter++] = decodeSample((ptr[i]>>0)&0x0f, reference, scale);
	}

	free(data);

#if 0
	// Debug : used to dump files
	Common::DumpFile outFile;
	outFile.open(filename);
	outFile.write(decoded, (size - 2) * 2);
	outFile.flush();
	outFile.close();
#endif

	Audio::AudioStream *audioStream = Audio::makeRawStream(decoded, (size - 2) * 2, rate, Audio::FLAG_UNSIGNED, DisposeAfterUse::YES);
	_mixer->playStream(Audio::Mixer::kPlainSoundType, &_effectsHandle, audioStream, -1,  Audio::Mixer::kMaxChannelVolume);
	_soundPlaying = true;
	_curPriority = priority;

	if (waitType == WAIT_RETURN_IMMEDIATELY) {
		_diskSoundPlaying = true;
		return true;
	}

	bool retval = true;
	do {
		_vm->_events->pollEvents();
		g_system->delayMillis(10);
		if ((waitType == WAIT_KBD_OR_FINISH) && _vm->_events->kbHit()) {
			retval = false;
			break;
		}
	} while (!_vm->shouldQuit() && _mixer->isSoundHandleActive(_effectsHandle));

	_soundPlaying = false;
	_mixer->stopHandle(_effectsHandle);

	return retval;
}

void Sound::playLoadedSound(int bufNum, WaitType waitType) {
	if (_mixer->isSoundHandleActive(_effectsHandle) && (_curPriority > _vm->_scene->_sounds[bufNum]._priority))
		return;

	stopSound();
	playSound(_vm->_scene->_sounds[bufNum]._name, waitType, _vm->_scene->_sounds[bufNum]._priority);

	return;
}

void Sound::freeLoadedSounds() {
	// As sounds are played with DisposeAfterUse::YES, stopping the sounds also
	// frees them
	stopSound();
}

void Sound::stopSound() {
	_mixer->stopHandle(_effectsHandle);
}

void Sound::stopSndFuncPtr(int v1, int v2) {
	// TODO
	warning("TODO: Sound::stopSndFuncPtr");
}

void Sound::freeDigiSound() {
	delete[] _digiBuf;
	_digiBuf = nullptr;
	_diskSoundPlaying = false;
	_soundPlaying = false;
}

void Sound::setMIDIVolume(int volume) {
	// TODO
}

} // End of namespace Sherlock

