#pragma once
#include "GameScene.h"
#include "TitleScene.h"

enum class SceneType {
    Title,
    Game,
    GameClear
};

class SceneManager {
public:
    static SceneManager* GetInstance();
    void ChangeScene(SceneType sceneType);
    SceneType GetCurrentScene() const;
    void Initialize(DirectXCommon* dxCommon, Input* input);
    void Update();
    void Draw();

private:
    SceneType currentScene_ = SceneType::Title;
    GameScene* gameScene_ = nullptr;
};