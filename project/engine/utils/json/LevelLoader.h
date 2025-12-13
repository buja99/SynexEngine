#pragma once
#include <string>
#include <fstream>
#include "json.hpp"
#include "Vector3.h"

/// <summary>レベル内オブジェクト定義</summary>
struct ObjectData {
    std::string name;
    std::string fileName;
    Vector3 translation;
    Vector3 rotation;
    Vector3 scaling;
    bool disabled = false;
};
/// <summary>敵スポーン定義</summary>
struct EnemySpawnData {
    std::string fileName;
    Vector3 translation;
    Vector3 rotation;
};
/// <summary>プレイヤースポーン定義</summary>
struct PlayerSpawnData {
    Vector3 translation;
    Vector3 rotation;
};
/// <summary>レベル構成データ一式</summary>
struct LevelData {
    std::vector<ObjectData> objects;
    std::vector<EnemySpawnData> enemies;
    std::vector<PlayerSpawnData> players;
};

/// <summary>レベルJSONを読み込み、オブジェクトやスポーン情報をLevelDataに変換するローダー</summary>
class LevelLoader {
public:
    static LevelData* LoadLevelData(const std::string& filePath);
    static std::unique_ptr<LevelData> LoadJsonFile(const std::string& directoryPath, const std::string& filename);
private:
    static void DeserializeObjectRecursive(nlohmann::json& object, LevelData* levelData);

   
};