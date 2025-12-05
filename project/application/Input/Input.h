#pragma once
#include <Windows.h>
#include<wrl.h>
#define DIRECTINPUT_VERSION     0x0800
#include <dinput.h>
#include "WinApp.h"

/// <summary>
/// DirectInput を用いてキーボード入力を取得し、押下状態・トリガー状態を管理するシングルトンクラス。
/// 毎フレーム現在のキー状態と前フレームの状態を更新し、入力判定を提供する。
/// </summary>
class Input
{

public:
	//

	static Input* GetInstance();

	template <class T> using ComPtr= Microsoft::WRL::ComPtr<T>;

	void Finalize();

	void Initialize(WinApp* winApp);

	void Update();

	bool PushKey(BYTE keyNumber);

	bool TriggerKey(BYTE keyNumber);



private:

	Input() = default;
	~Input() = default;
	Input(const Input&) = delete;
	Input& operator=(const Input&) = delete;

	ComPtr<IDirectInputDevice8> keyboard;
	BYTE key[256] = {};
	BYTE keyPre[256] = {};
	ComPtr<IDirectInput8> directInput = nullptr;

	WinApp* winApp_ = nullptr;

};

