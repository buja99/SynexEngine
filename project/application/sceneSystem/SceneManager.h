#pragma once
#include <memory>
#include "BaseScene.h"
#include "AbstractSceneFactory.h"

/// <summary>シーン切り替えと現在シーンの更新・描画を管理するシングルトンクラス </summary>
class SceneManager {

public:

	static SceneManager* GetInstance();

	void Finalize();
	/// <summary> シーン切り替え処理を含む現在シーンの更新を行う </summary>
	void Update();

	void Draw();

	/// <summary> 次の Update タイミングで指定したシーンへ切り替える </summary>
	void ChangeScene(const std::string& sceneName);

    void SetSceneFactory(std::unique_ptr<AbstractSceneFactory> sceneFactory) { sceneFactory_ = std::move(sceneFactory); }

private:
	~SceneManager() = default;
	SceneManager() = default;
	SceneManager(SceneManager&) = delete;
	SceneManager& operator=(SceneManager&) = delete;

	std::unique_ptr<BaseScene> scene_ = nullptr;

	//std::unique_ptr<BaseScene> nextScene_ = nullptr;

	std::unique_ptr<AbstractSceneFactory> sceneFactory_ = nullptr;

	std::string nextSceneName_;
};

