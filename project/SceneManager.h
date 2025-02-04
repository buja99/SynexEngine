#pragma once
#include "BaseScene.h"
#include "GameScene.h"
#include "TitleScene.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "AbstractSceneFactory.h"
#include "SceneType.h" 
#include <memory>


class SceneManager {
public:
    static SceneManager* GetInstance();
    void ChangeScene(SceneType sceneType);
    SceneType GetCurrentScene() const;
    void Initialize(DirectXCommon* dxCommon, Input* input);
    void Update();
    void Draw();

    void SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; }

private:
    SceneManager() = default;
    ~SceneManager() = default;
    SceneManager(const SceneManager&) = delete;
    SceneManager& operator=(const SceneManager&) = delete;

    SceneType currentScene_ = SceneType::Title;
    std::unique_ptr<BaseScene> currentScenePtr_;
    AbstractSceneFactory* sceneFactory_ = nullptr;

    DirectXCommon* dxCommon_ = nullptr;
    Input* input_ = nullptr;
};