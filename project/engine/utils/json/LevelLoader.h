#pragma once
#include <string>
#include <fstream>
#include "json.hpp"
#include "Vector3.h"

struct ObjectData {
    std::string name;
    std::string fileName;
    Vector3 translation;
    Vector3 rotation;
    Vector3 scaling;
    bool disabled = false;
};
struct EnemySpawnData {
    std::string fileName;
    Vector3 translation;
    Vector3 rotation;
};

struct PlayerSpawnData {
    Vector3 translation;
    Vector3 rotation;
};
struct LevelData {
    std::vector<ObjectData> objects;
    std::vector<EnemySpawnData> enemies;
    std::vector<PlayerSpawnData> players;
};

class LevelLoader {
public:
    static LevelData* LoadLevelData(const std::string& filePath);
    static std::unique_ptr<LevelData> LoadJsonFile(const std::string& directoryPath, const std::string& filename);
private:
    static void DeserializeObjectRecursive(nlohmann::json& object, LevelData* levelData);

   
};