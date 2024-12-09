#pragma once
#include <cstdint>
#include<Windows.h>


#pragma comment(lib, "winmm.lib")

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