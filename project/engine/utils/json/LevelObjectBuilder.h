#pragma once
#include "LevelLoader.h"
#include "LevelApplier.h"
#include "ModelRegistry.h"
#include "Object3d.h"
#include "WorldTransform.h"
#include "Camera.h"
#include <vector>
#include <memory>
#include <string>

class LevelObjectBuilder {
public:
    static void BuildFromJson(
        const LevelData* levelData,
        std::vector<std::unique_ptr<Object3d>>& objects,
        std::vector<std::unique_ptr<WorldTransform>>& transforms,
        Camera* camera
    );
};