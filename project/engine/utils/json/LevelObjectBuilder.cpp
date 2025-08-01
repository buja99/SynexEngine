#include "LevelObjectBuilder.h"
#include "ModelManager.h"
#include <iostream>

void LevelObjectBuilder::BuildFromJson(
    const std::string& jsonPath,
    ModelRegistry& registry,
    std::vector<std::unique_ptr<Object3d>>& objects,
    std::vector<std::unique_ptr<WorldTransform>>& transforms,
    Camera* camera
) {
    LevelData* levelData = LevelLoader::LoadLevelData(jsonPath);

    for (const auto& obj : levelData->objects) {

        // 💥 문제 추적용 로그 출력
        std::cout << "[DEBUG] checking name: " << obj.name << std::endl;

        // 💥 진단용 복사 검사
        std::unordered_map<std::string, WorldTransform*> mapCopy = registry.GetTransformMap();
        if (mapCopy.find(obj.name) != mapCopy.end()) {
            throw std::runtime_error("Duplicate object name detected: " + obj.name);
        }

        // 트랜스폼 생성
        auto transform = std::make_unique<WorldTransform>();
        transform->Initialize();

        // 모델 생성
        auto model = std::make_unique<Object3d>();
        model->Initialize(Object3dCommon::GetInstance(), transform.get());
        std::string modelPath;
        if (obj.fileName == "tree") {
            modelPath = "tree.obj";
        } else if (obj.fileName == "weeds") {
            modelPath = "weeds.obj";
        } else {
            modelPath = obj.fileName; // 그 외는 그대로 사용
        }
        model->SetModel(modelPath);
        if (camera) model->SetCamera(camera);

        if (obj.name.empty()) {
            throw std::runtime_error("Object name is empty. Each object must have a unique 'name'.");
        }

        if (registry.GetTransformMap().find(obj.name) != registry.GetTransformMap().end()) {
            throw std::runtime_error("Duplicate object name detected: " + obj.name);
        }

        // 등록
        registry.Register(obj.name, model.get(), transform.get());

        // 리스트에 추가
        transforms.push_back(std::move(transform));
        objects.push_back(std::move(model));
    }

    // 트랜스폼 적용
    LevelApplier::ApplyLevelTransforms(levelData, registry.GetTransformMap());
}
