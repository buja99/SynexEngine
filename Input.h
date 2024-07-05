#pragma once
#include <Windows.h>
#include<wrl.h>
#define DIRECTINPUT_VERSION     0x0800
#include <dinput.h>

class Input
{

public:

	template <class T> using ComPtr= Microsoft::WRL::ComPtr<T>;

	void Initialize(HINSTANCE hInstance, HWND hwnd);

	void Update();

	bool PushKey(BYTE keyNumber);

	bool TriggerKey(BYTE keyNumber);

private:

	ComPtr<IDirectInputDevice8> keyboard;
	BYTE key[256] = {};
	BYTE keyPre[256] = {};
	ComPtr<IDirectInput8> directInput = nullptr;
};

