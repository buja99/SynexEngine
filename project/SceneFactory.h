#pragma once

#include "AbstractSceneFactory.h"
#include "TitleScene.h"
#include "GameScene.h"

class SceneFactory : public AbstractSceneFactory {
public:
	

	std::unique_ptr<BaseScene> CreateScene(const std::string& sceneName) override;
};


