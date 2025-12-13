#pragma once
#include <xaudio2.h>
#include <wrl.h>
#include <string>

#pragma comment(lib,"xaudio2.lib")

/// <summary>WAV音声データ保持構造</summary>
struct SoundData {
	WAVEFORMATEX wfex;
	BYTE* pBuffer;
	unsigned int bufferSize;
};

/// <summary>RIFFチャンクヘッダ</summary>
struct ChunkHeader {
	char id[4];
	int32_t size;

};

/// <summary>RIFFファイルヘッダ</summary>
struct RiffHeader {
	ChunkHeader chunk;
	char type[4];
};

/// <summary>WAVフォーマットチャンク</summary>
struct FormatChunk {
	ChunkHeader chunk;
	WAVEFORMATEX fmt;

};

/// <summary>ループ再生用音声データ</summary>
struct LoopSoundData {
	SoundData soundData;

	IXAudio2SourceVoice* pSourceVoice = nullptr;
};

/// <summary>XAudio2による音声再生管理</summary>
class Sound {

public:
	//シングルトンインスタンスの取得
	static Sound* GetInstance();

	//終了
	void Finalize();

	/// <summary>XAudio2デバイス初期化</summary>
	void Initialize();

	/// <summary>WAVファイル読み込み</summary>
	SoundData LoadWaveFile(const std::string& fileName);

	/// <summary>ループ再生用WAV読み込み</summary>
	LoopSoundData LoadLoopingWaveFile(const std::string& fileName);

	/// <summary>単発音声再生</summary>
	//音声再生。1.0f = 100%の音量
	void playSoundWave(const SoundData& soundData, float volume = 1.0f);

	/// <summary>ループ音声再生</summary>
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