#pragma once
#include "LevelLoader.h"
#include "WorldTransform.h"
#include <unordered_map>
#include <string>

class LevelApplier {
public:
    static void ApplyLevelTransforms(
        const LevelData* levelData,
        const std::unordered_map<std::string, WorldTransform*>& transformTable
    );
};