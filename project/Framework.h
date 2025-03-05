#pragma once
#include <Windows.h>
#include "WinApp.h"
#include "Input.h"
#include "DirectXCommon.h"
#include "Sprite.h"
#include "SpriteCommon.h"
#include "TextureManager.h"
#include "Object3d.h"
#include "Object3dCommon.h"
#include "ModelCommon.h"
#include "SrvManager.h"
#include "ParticleManager.h"
#include "Sound.h"
#include "SceneManager.h"
#include "AbstractSceneFactory.h"
#ifdef _DEBUG
#include "ImGuiManager.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx12.h"
#include "imgui/imgui_impl_win32.h"
#endif // _DEBUG
#include <crtdbg.h>
class Framework {
public:
	virtual ~Framework() = default;

	void CheckHeap();

	virtual void Initialize();

	virtual void Finalize();

	virtual void Update();

	virtual void Draw() = 0;

	void Run();

	virtual bool GetIsEndReqest() { return isEndReqest_; }


protected:
	bool isEndReqest_ = false;
	std::unique_ptr<WinApp> winApp_;
	DirectXCommon* dxCommon_ = nullptr;
	Sound* audio_ = nullptr;
	SrvManager* srvManager_ = nullptr;
	Input* input_ = nullptr;
	SceneManager* sceneManager_ = nullptr;
	
	std::unique_ptr<SpriteCommon> spriteCommon_ = nullptr;
	std::unique_ptr<Object3dCommon> object3dCommon_ = nullptr;
	std::unique_ptr<ImGuiManager> imGuiManager_ = nullptr;
	std::unique_ptr<ModelCommon> modelCommon_ = nullptr;
	//std::unique_ptr<Object3d> object3d_ = nullptr;
	std::unique_ptr<Camera> camera_ = nullptr;
	std::unique_ptr<ParticleManager> particleManager_ = nullptr;

};