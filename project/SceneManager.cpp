#include "SceneManager.h"

SceneManager* SceneManager::GetInstance() {
    static SceneManager instance;
    return &instance;
}

void SceneManager::Initialize(DirectXCommon* dxCommon, Input* input) {
    dxCommon_ = dxCommon;
    input_ = input;

    // 처음 시작할 때 TitleScene으로 초기화
    ChangeScene(SceneType::Title);
}

void SceneManager::ChangeScene(SceneType sceneType) {
    currentScene_ = sceneType;

    // 기존 씬 해제 (unique_ptr이 자동으로 관리)
    currentScenePtr_.reset();

    // 새로운 씬 생성
    if (sceneFactory_) {
        currentScenePtr_ = sceneFactory_->CreateScene(sceneType);
    } else {
        if (currentScene_ == SceneType::Title) {
            currentScenePtr_ = std::unique_ptr<BaseScene>(std::make_unique<TitleScene>());
        } else if (currentScene_ == SceneType::Game) {
            currentScenePtr_ = std::make_unique<GameScene>();
        }
    }

    if (currentScenePtr_) {
        currentScenePtr_->Initialize(dxCommon_, input_);
    }
}

SceneType SceneManager::GetCurrentScene() const {
    return currentScene_;
}

void SceneManager::Update() {
    if (currentScenePtr_) {
        currentScenePtr_->Update();
    }
}

void SceneManager::Draw() {
    if (currentScenePtr_) {
        currentScenePtr_->Draw();
    }
}