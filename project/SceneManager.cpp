#include "SceneManager.h"

SceneManager* SceneManager::GetInstance()
{
    static SceneManager instance;
    return &instance;
}

void SceneManager::ChangeScene(SceneType sceneType)
{
    currentScene_ = sceneType;
}

SceneType SceneManager::GetCurrentScene() const
{
    return currentScene_;
}

void SceneManager::Initialize()
{
    
}

Sprite* SceneManager::GetCurrentSprite() const
{
    return nullptr;
}
