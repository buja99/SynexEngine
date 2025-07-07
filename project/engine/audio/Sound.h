#pragma once
#include <xaudio2.h>
#include <wrl.h>
#include <string>

#pragma comment(lib,"xaudio2.lib")

struct SoundData {
	WAVEFORMATEX wfex;
	BYTE* pBuffer;
	unsigned int bufferSize;
};
struct ChunkHeader {
	char id[4];
	int32_t size;

};
struct RiffHeader {
	ChunkHeader chunk;
	char type[4];
};
struct FormatChunk {
	ChunkHeader chunk;
	WAVEFORMATEX fmt;

};
struct LoopSoundData {
	SoundData soundData;

	IXAudio2SourceVoice* pSourceVoice = nullptr;
};
class Sound {
public:
	//シングルトンインスタンスの取得
	static Sound* GetInstance();

	//終了
	void Finalize();

	//初期化
	void Initialize();

	SoundData LoadWaveFile(const std::string& fileName);

	LoopSoundData LoadLoopingWaveFile(const std::string& fileName);

	//音声再生。1.0f = 100%の音量
	void playSoundWave(const SoundData& soundData, float volume = 1.0f);

	//ループ再生。1.0f = 100%の音量
	void playLoopingSoundWave(const LoopSoundData& loopSoundData, float volume = 1.0f);

	//音楽を止める
	void StopSoundWave(const LoopSoundData& loopSoundData);

	//音声データ解放
	void UnloadSoundData(SoundData* soundData);
	void UnloadSoundData(LoopSoundData* loopSoundData);

private:

	Sound() = default;
	~Sound() = default;
	Sound(Sound&) = delete;
	Sound& operator=(Sound&) = delete;

	Microsoft::WRL::ComPtr<IXAudio2> xAudio2_;
	IXAudio2MasteringVoice* masterVoice_;
};