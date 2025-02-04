#pragma once
#include "AbstractSceneFactory.h"
#include "GameScene.h"
#include "TitleScene.h"
#include <memory>

class SceneFactory : public AbstractSceneFactory {
public:
    std::unique_ptr<BaseScene> CreateScene(SceneType sceneType) override { 
        switch (sceneType) {
        case SceneType::Title:
            return std::make_unique<TitleScene>();
        case SceneType::Game:
            return std::unique_ptr<BaseScene>(std::make_unique<GameScene>());
        default:
            return nullptr;
        }
    }
};