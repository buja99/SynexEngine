#pragma once
#include "LevelLoader.h"
#include "WorldTransform.h"
#include <unordered_map>
#include <string>

/// <summary>レベルデータの位置・回転・スケールをワールドトランスフォームへ適用するユーティリティ </summary>
class LevelApplier {
public:
    static void ApplyLevelTransforms(
        const LevelData* levelData,
        const std::unordered_map<std::string, WorldTransform*>& transformTable
    );
};