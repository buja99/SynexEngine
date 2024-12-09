#include "Input.h"
#include <cassert>




#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")



void Input::Initialize(WinApp* winApp)
{
	
	this->winApp_ = winApp;


	HRESULT result;
	//ComPtr<IDirectInput8> directInput = nullptr;
	result = DirectInput8Create(winApp->GetHInstance(), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, nullptr);
	assert(SUCCEEDED(result));

	//ComPtr<IDirectInputDevice8> keyboard;
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	result = keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));

	result = keyboard->SetCooperativeLevel(winApp->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));


}

void Input::Update()
{

	keyboard->Acquire();

	//BYTE key[256] = {};
	memcpy(keyPre, key, sizeof(key));
	keyboard->GetDeviceState(sizeof(key), key);
}

bool Input::PushKey(BYTE keyNumber)
{
	if (key[keyNumber]) {
		return true;
	}
	
	return false;
}

bool Input::TriggerKey(BYTE keyNumber)
{
	return false;
}
