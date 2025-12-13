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
#include "imgui.h"
#include "imgui_impl_dx12.h"
#include "imgui_impl_win32.h"
#endif // _DEBUG
#include <crtdbg.h>

/// <summary>
/// ゲームアプリケーション全体の初期化・更新・描画・終了処理を管理する基盤クラス
/// 派生クラスが「Draw()」を実装することで、統一されたゲームループを提供
/// </summary>
class Framework {
public:
	virtual ~Framework() = default;

	void CheckHeap();

	/// <summary> DirectX・入力・シーン管理など、ゲーム実行に必要な基盤を初期化 </summary>
	virtual void Initialize();

	virtual void Finalize();

	virtual void Update();

	virtual void Draw() = 0;

	/// <summary> 初期化から終了までの統一されたゲームループを実行 </summary>
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
	
	SpriteCommon* spriteCommon_ = nullptr;
	Object3dCommon* object3dCommon_ = nullptr;
#ifdef _DEBUG
	std::unique_ptr<ImGuiManager> imGuiManager_ = nullptr;
#endif // _DEBUG
	std::unique_ptr<ModelCommon> modelCommon_ = nullptr;
	//std::unique_ptr<Object3d> object3d_ = nullptr;
	std::unique_ptr<Camera> camera_ = nullptr;
	std::unique_ptr<ParticleManager> particleManager_ = nullptr;

};