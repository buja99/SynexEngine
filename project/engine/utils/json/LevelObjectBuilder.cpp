#include "LevelObjectBuilder.h"
#include "ModelManager.h"
#include <iostream>

void LevelObjectBuilder::BuildFromJson(
    const LevelData* levelData,
    std::vector<std::unique_ptr<Object3d>>& objects,
    std::vector<std::unique_ptr<WorldTransform>>& transforms,
    Camera* camera)
{
    assert(levelData);

    // === 일반 오브젝트 처리 ===
    for (const auto& obj : levelData->objects) {
        if (obj.disabled) {
            continue;
        }

        auto transform = std::make_unique<WorldTransform>();
        transform->Initialize();
        transform->translate_ = obj.translation;
        transform->rotate_ = obj.rotation;
        transform->scale_ = obj.scaling;

        auto model = std::make_unique<Object3d>();
        model->Initialize(Object3dCommon::GetInstance(), transform.get());
        model->SetModel(obj.fileName);
        if (camera) model->SetCamera(camera);

        transforms.push_back(std::move(transform));
        objects.push_back(std::move(model));
    }

    // === 적 스폰 포인트 처리 ===
    for (const auto& enemy : levelData->enemies) {
        auto transform = std::make_unique<WorldTransform>();
        transform->Initialize();
        transform->translate_ = enemy.translation;
        transform->rotate_ = enemy.rotation;
        transform->scale_ = { 1.0f, 1.0f, 1.0f };  // 기본 스케일

        auto model = std::make_unique<Object3d>();
        model->Initialize(Object3dCommon::GetInstance(), transform.get());
        model->SetModel(enemy.fileName); // 예: "enemy.obj"
        if (camera) model->SetCamera(camera);

        transforms.push_back(std::move(transform));
        objects.push_back(std::move(model));
    }

    // === 플레이어 스폰 포인트 처리 ===
    for (const auto& player : levelData->players) {
        auto transform = std::make_unique<WorldTransform>();
        transform->Initialize();
        transform->translate_ = player.translation;
        transform->rotate_ = player.rotation;
        transform->scale_ = { 1.0f, 1.0f, 1.0f };

        auto model = std::make_unique<Object3d>();
        model->Initialize(Object3dCommon::GetInstance(), transform.get());
        model->SetModel("player"); // 고정된 player 모델 이름
        if (camera) model->SetCamera(camera);

        transforms.push_back(std::move(transform));
        objects.push_back(std::move(model));
    }
}
