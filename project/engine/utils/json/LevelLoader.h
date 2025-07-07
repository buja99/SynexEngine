#pragma once
#include <string>
#include <fstream>
#include "json.hpp"
#include "Vector3.h"

struct LevelData {
    struct ObjectData {
        std::string fileName;
        Vector3 translation;
        Vector3 rotation;
        Vector3 scaling;
    };
    std::vector<ObjectData> objects;
};

class LevelLoader {
public:
    static LevelData* LoadLevelData(const std::string& filePath);
private:
    static void DeserializeObjectRecursive(nlohmann::json& object, LevelData* levelData);
};