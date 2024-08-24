#include<Windows.h>
#include<cstdint>
#include<string>
#include<format>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<cassert>
#include<dxgidebug.h>
#include <dxcapi.h>
//#include"Math.h"
#include "Vector2.h"
#include <cmath>
#include"Matrix3x3.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#include "externals/DirectXTex/DirectXTex.h"
#include<wrl.h>
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <fstream>
#include <sstream>
#include "ResourceObject.h"
#include "Input.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Logger.h"
#include "StringUtility.h"
#include "GameScene.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")

using Microsoft::WRL::ComPtr;

using namespace Logger;
using namespace StringUtility;


//Math* math = new Math();
//
//struct Vector4 final {
//	float x;
//	float y;
//	float z;
//	float w;
//};
//
//struct Transform
//{
//	Vector3 scale;
//	Vector3 rotate;
//	Vector3 translate;
//
//};
//
//struct  VertexData
//{
//	Vector4 position;
//	Vector2 texCoord;
//	Vector3 normal;
//};
//
//struct  Material
//{
//	Vector4 color;
//	int32_t enableLighting;
//	float padding[3];
//	Matrix4x4 uvTransform;
//};
//
//struct TransformationMatrix
//{
//	Matrix4x4 WVP;
//	Matrix4x4 World;
//};

//struct DirectionalLight
//{
//	Vector4 color;
//	Vector3 direction;
//	float intensity;
//};
//struct MaterialData
//{
//	std::string textureFilePath;
//};
//struct ModelData
//{
//	std::vector<VertexData> vertices;
//	MaterialData material;
//};

struct D3DResourceLeakChecker {
	~D3DResourceLeakChecker() {

		ComPtr<IDXGIDebug1> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		}
	}
};

//Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
//Transform cameraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,-10.0f} };
//
//Transform uvTransformSprite
//{
//	{1.0f,1.0f,1.0f},
//	{0.0f,0.0f,0.0f},
//	{0.0f,0.0f,0.0f}
//};







int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	D3DResourceLeakChecker leakCheck;
	
	//COMの初期化
	//CoInitialize(NULL);

	CoInitializeEx(0, COINIT_MULTITHREADED);

	WinApp* winApp = new WinApp();
	DirectXCommon* dxCommon = new DirectXCommon();
	Input* input = new Input();
	GameScene* gameScene = new GameScene();

	winApp->Initialize();
	dxCommon->Initialize(winApp);
	input->Initialize(winApp);
	gameScene->Initialize(dxCommon);
	



	OutputDebugStringA("Hello,DirectX!\n");

	
	//dxCommon->InitializePSO();
	
//ComPtr<IDxcBlob> vertexShaderBlob = dxCommon->CompileShader(L"resources/shaders/Object3D.VS.hlsl", L"vs_6_0");
//ComPtr<IDxcBlob> pixelShaderBlob = dxCommon->CompileShader(L"resources/shaders/Object3D.PS.hlsl", L"ps_6_0");


	while (true) {

		
		if (winApp->ProcessMessage()) {

			break;
		} 
		input->Update();

		gameScene->Update();

		dxCommon->PreDraw();
		dxCommon->GetCommandList()->SetPipelineState(dxCommon->GetGraphicsPipelineState().Get());
		gameScene->Draw();
		dxCommon->PostDraw();
			


		
	}

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	//CloseHandle(fenceEvent);
	delete gameScene;
	dxCommon->Cleanup(); 
	delete dxCommon; 
	winApp->Finalize();
	delete input;
	//delete winApp; 
	//delete myMath;

	CoUninitialize();
	return 0;

}