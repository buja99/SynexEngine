#pragma once
#include "Framework.h"
#include "SceneFactory.h"

/// <summary>
/// ゲームアプリケーションの本体クラス。
/// Framework を継承し、シーンファクトリーの登録とゲーム固有の初期化・描画処理を管理する。
/// </summary>
class MyGame : public Framework {

public:
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

private:

	std::unique_ptr<AbstractSceneFactory> sceneFactory_ = nullptr;

};
