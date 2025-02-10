#include "Sound.h"
#include <fstream>
#include <cassert>

Sound* Sound::GetInstance() {
	static Sound instance;
	return &instance;
}

void Sound::Finalize() {
}

void Sound::Initialize() {
	HRESULT result = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	result = xAudio2_->CreateMasteringVoice(&masterVoice_);
}

SoundData Sound::LoadWaveFile(const std::string& fileName) {
	
	std::ifstream file;
	file.open(fileName, std::ios_base::binary);
	assert(file.is_open());

	
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		assert(0);
	}
	
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}

	FormatChunk format = {};

	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}

	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);

	ChunkHeader data;
	file.read((char*)&data, sizeof(data));

	if (strncmp(data.id, "LIST", 4) == 0) {
		file.seekg(data.size, std::ios_base::cur);
		file.read((char*)&data, sizeof(data));
	}

	if (strncmp(data.id, "JUNK", 4) == 0) {
		file.seekg(data.size, std::ios_base::cur);
		file.read((char*)&data, sizeof(data));
	}

	if (strncmp(data.id, "data", 4) != 0) {
		assert(0);
	}

	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	file.close();

	SoundData soundData = {};

	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;

	return soundData;
}

LoopSoundData Sound::LoadLoopingWaveFile(const std::string& fileName) {
	LoopSoundData loopSoundData;

	loopSoundData.soundData = LoadWaveFile(fileName);

	HRESULT result;
	result = xAudio2_->CreateSourceVoice(&loopSoundData.pSourceVoice, &loopSoundData.soundData.wfex);

	return loopSoundData;
}

void Sound::playSoundWave(const SoundData& soundData, float volume) {
	HRESULT result;

	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2_->CreateSourceVoice(&pSourceVoice, &soundData.wfex);

	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.pBuffer;
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;

	result = pSourceVoice->SubmitSourceBuffer(&buf);
	result = pSourceVoice->SetVolume(volume);
	result = pSourceVoice->Start();
}

void Sound::playLoopingSoundWave(const LoopSoundData& loopSoundData, float volume) {

	HRESULT result;

	XAUDIO2_BUFFER buf{};
	buf.pAudioData = loopSoundData.soundData.pBuffer;
	buf.AudioBytes = loopSoundData.soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;

	buf.LoopCount = XAUDIO2_LOOP_INFINITE;

	result = loopSoundData.pSourceVoice->SubmitSourceBuffer(&buf);
	result = loopSoundData.pSourceVoice->SetVolume(volume);
	result = loopSoundData.pSourceVoice->Start();

}

void Sound::StopSoundWave(const LoopSoundData& loopSoundData) {
	if (loopSoundData.pSourceVoice) {
		
		loopSoundData.pSourceVoice->Stop(0);

		loopSoundData.pSourceVoice->FlushSourceBuffers();
	}
}

void Sound::UnloadSoundData(SoundData* soundData) {
	delete[] soundData->pBuffer;

	soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

void Sound::UnloadSoundData(LoopSoundData* loopSoundData) {
	delete loopSoundData->pSourceVoice;

	UnloadSoundData(&loopSoundData->soundData);
}
