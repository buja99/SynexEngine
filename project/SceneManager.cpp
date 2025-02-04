#include "SceneManager.h"

SceneManager* SceneManager::GetInstance()
{
    static SceneManager instance;
    return &instance;
}

void SceneManager::ChangeScene(SceneType sceneType) {
    currentScene_ = sceneType;
}

SceneType SceneManager::GetCurrentScene() const {
    return currentScene_;
}

void SceneManager::Initialize(DirectXCommon* dxCommon, Input* input) {
    if (currentScene_ == SceneType::Game) {
        gameScene_ = new GameScene();
        gameScene_->Initialize(dxCommon, input);
    }
}

void SceneManager::Update() {
    if (currentScene_ == SceneType::Game) {
        gameScene_->Update();
    }
}

void SceneManager::Draw() {
    if (currentScene_ == SceneType::Game) {
        gameScene_->Draw();
    }
}