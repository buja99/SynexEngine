#pragma once

#include "AbstractSceneFactory.h"
#include "TitleScene.h"
#include "GameScene.h"

/// <summary>シーン名に応じて具体的なシーンインスタンスを生成するファクトリクラス。</summary>
class SceneFactory : public AbstractSceneFactory {
public:
	

	std::unique_ptr<BaseScene> CreateScene(const std::string& sceneName) override;
};


