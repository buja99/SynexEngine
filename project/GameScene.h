#pragma once
#include "SceneManager.h"
#include "DirectXCommon.h"
#include "Input.h"
#include "Camera.h"
#include "Player.h"
#include "Enemy.h"
#include "ModelManager.h"
#include "Object3dCommon.h"

class GameScene {
public:
    GameScene();
    ~GameScene();

    void Initialize(DirectXCommon* dxCommon, Input* input);
    void Update();
    void Draw();

private:
    DirectXCommon* dxCommon_;
    Input* input_;
    Camera* camera_;
    Player* player_;
    Enemy enemy_;
    Object3dCommon* object3dCommon_;
    std::vector<std::unique_ptr<Object3d>> object3ds_;
};

