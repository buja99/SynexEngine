#pragma once
#include "Sprite.h"
#include "SpriteCommon.h"
enum class SceneType {
    Title,
    Game,
    GameClear
};
class SceneManager
{

public:
    static SceneManager* GetInstance();        
    void ChangeScene(SceneType sceneType);     
    SceneType GetCurrentScene() const; 
    void Initialize(); 
    Sprite* GetCurrentSprite() const;

private:
    SceneType currentScene_ = SceneType::Title; 
    SpriteCommon* title = nullptr;
    SpriteCommon* clear = nullptr;
    Sprite* titleSprite_ = nullptr;
    Sprite* gameClearSprite_ = nullptr;

};

