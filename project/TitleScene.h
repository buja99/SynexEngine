#pragma once
#include "SceneManager.h"
#include "Input.h"
#include "Sprite.h"
#include "SpriteCommon.h"

class TitleScene {
public:
    TitleScene();
    ~TitleScene();

    void Initialize(SpriteCommon* spriteCommon, Input* input);
    void Update();
    void Draw();

private:
    Input* input_;
    Sprite* titleSprite_;
};