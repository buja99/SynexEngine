#pragma once
#include "Sprite.h"
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

    Sprite* titleSprite_ = nullptr;
    Sprite* gameClearSprite_ = nullptr;

};

