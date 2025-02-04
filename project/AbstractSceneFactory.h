#pragma once
#include "BaseScene.h"
#include "SceneType.h"

class AbstractSceneFactory {
public:
    virtual ~AbstractSceneFactory() = default;
    virtual std::unique_ptr<BaseScene> CreateScene(SceneType sceneType) = 0;
};