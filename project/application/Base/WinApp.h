#pragma once
#include <cstdint>
#include<Windows.h>
#pragma comment(lib, "winmm.lib")

/// <summary>
/// Windows アプリケーションのウィンドウ生成とメッセージ処理を行う基盤クラス。
/// HWND / HINSTANCE の管理および Win32 メッセージループを提供する。
/// </summary>

class WinApp {
public:
	WinApp();
	~WinApp();

	//size
	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHeight = 720;

	void Initialize();

	void Finalize();

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	HWND GetHwnd() const { return hwnd; }

	HINSTANCE GetHInstance() const { return wc.hInstance; }

	HRESULT GetHRESULT() const { return hr; }


	bool ProcessMessage();

private:
   
	
	
	

	

	HWND hwnd = nullptr;

	WNDCLASS wc{};

	HRESULT hr;
};