#include "LevelApplier.h"

void LevelApplier::ApplyLevelTransforms(
    const LevelData* levelData,
    const std::unordered_map<std::string, WorldTransform*>& transformTable
) {
    for (const auto& objectData : levelData->objects) {
        const std::string& key = objectData.name;

        auto it = transformTable.find(key);
        if (it != transformTable.end()) {
            WorldTransform* wt = it->second;
            wt->translate_ = objectData.translation;
            wt->rotate_ = objectData.rotation;
            wt->scale_ = objectData.scaling;
        }
    }
}