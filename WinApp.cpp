#include "WinApp.h"
#include "externals/imgui/imgui.h"


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void WinApp::Initialize()
{

	if (this == nullptr) {
		OutputDebugStringA("WinApp::Initialize: this pointer is null\n");
		return;
	}

	hr = CoInitializeEx(0, COINIT_MULTITHREADED);

	//WNDCLASS wc{};
	//Window Procedure
	wc.lpfnWndProc = WindowProc;
	//Window Class Name(何でもいい)
	wc.lpszClassName = L"CG2WindowClass";
	//Instance Handle
	wc.hInstance = GetModuleHandle(nullptr);
	//Cursor
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	//ウィンドウクラスを登録する
	RegisterClass(&wc);



	

	RECT wrc = { 0,0,kClientWidth,kClientHeight };

	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);




 hwnd = CreateWindow(

		//利用するクラス名
		wc.lpszClassName,
		//タイトルバーの文字列
		L"CG2",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc.right - wrc.left,
		wrc.bottom - wrc.top,
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr);

	ShowWindow(hwnd, SW_SHOW);
}

void WinApp::Finalize()
{
	if (this == nullptr) {
		OutputDebugStringA("WinApp::Finalize: this pointer is null\n");
		return;
	}
	CloseWindow(hwnd);
	CoUninitialize();

}

LRESULT WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}
	//メッセージに応じてゲーム固有の処理を行う
	switch (msg) {
		//ウィンドウが破棄された
	case WM_DESTROY:
		//osに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}
	//標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

bool WinApp::ProcessMessage()
{
	MSG msg{}; // メッセージ

	if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) // メッセージがある？
	{
		TranslateMessage(&msg); // キー入力メッセージの処理
		DispatchMessage(&msg);  // ウィンドウプロシージャにメッセージを送る
	}

	if (msg.message == WM_QUIT) // 終了メッセージが来たらループを抜ける
	{
		return true;
	}

	return false;
}


