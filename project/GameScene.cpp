#include "GameScene.h"

GameScene::GameScene() : dxCommon_(nullptr), input_(nullptr), camera_(nullptr), player_(nullptr), object3dCommon_(nullptr) {}


GameScene::~GameScene(){
    delete camera_;
    delete player_;
    delete object3dCommon_;
}

void GameScene::Initialize(DirectXCommon* dxCommon, Input* input){
    dxCommon_ = dxCommon;
    input_ = input;

    camera_ = new Camera();
    camera_->SetRotate({ 1.0f, 0.0f, 0.0f });
    camera_->SetTranslate({ 0.0f, 90.0f, 10.0f });

    object3dCommon_ = new Object3dCommon();
    object3dCommon_->SetDefaultCamera(camera_);
    object3dCommon_->Initialize(dxCommon_);

    ModelManager::GetInstance()->Initialize(dxCommon_);

    std::vector<std::string> modelPaths = { "body", "legL", "legR", "head", "armR", "armL", "gun" };
    for (const auto& modelPath : modelPaths) {
        ModelManager::GetInstance()->LoadModel("resources/player/" + modelPath, modelPath + ".obj");
        auto object = std::make_unique<Object3d>();
        object->Initialize(object3dCommon_);
        object->SetModel(modelPath + ".obj");
        object3ds_.emplace_back(std::move(object));
    }

    ModelManager::GetInstance()->LoadModel("resources/enemy", "enemy.obj");
    auto enemyModel = std::make_unique<Object3d>();
    enemyModel->Initialize(object3dCommon_);
    enemyModel->SetModel("enemy.obj");
    enemy_.Initialize(enemyModel.get());

    std::vector<Object3d*> playerParts = {
        object3ds_[0].get(), object3ds_[1].get(), object3ds_[2].get(),
        object3ds_[3].get(), object3ds_[4].get(), object3ds_[5].get(),
        object3ds_[6].get()
    };

    player_ = new Player();
    player_->Initialize(playerParts, object3dCommon_);
}

void GameScene::Update(){
    input_->Update();
    player_->Updata(input_);
    enemy_.Update();
}

void GameScene::Draw(){
    object3dCommon_->CommonDrawSettings();
    player_->Draw();
}
