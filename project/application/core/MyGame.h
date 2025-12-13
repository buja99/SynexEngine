#pragma once
#include "Framework.h"
#include "SceneFactory.h"

/// <summary>
/// ゲームアプリケーションの本体クラス
/// Framework を継承し、シーンファクトリーの登録とゲーム固有の初期化・描画処理を管理
/// </summary>
class MyGame : public Framework {

public:
	/// <summary>シーンファクトリを登録し、初期シーンを設定 </summary>
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	/// <summary>ゲーム固有の描画フロー（オフスクリーン＋ポストエフェクト）を実行 </summary>
	/// <remarks>オフスクリーン描画→エフェクト適用→スワップチェイン出力→PostDraw の順で呼び出す </remarks>
	void Draw() override;

private:

	std::unique_ptr<AbstractSceneFactory> sceneFactory_ = nullptr;

};
