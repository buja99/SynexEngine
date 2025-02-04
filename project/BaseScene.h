#pragma once
#include "DirectXCommon.h"
#include "Input.h"

class SceneManager;

class BaseScene {
public:
    virtual ~BaseScene() = default;

    virtual void Initialize(DirectXCommon* dxCommon, Input* input) = 0;
    virtual void Update() = 0;
    virtual void Draw() = 0;
    virtual void Finalize() = 0;

    void SetSceneManager(SceneManager* sceneManager) { sceneManager_ = sceneManager; }

protected:
    SceneManager* sceneManager_ = nullptr;
};
