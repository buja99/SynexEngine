#include "LevelLoader.h"
#include <cassert>

LevelData* LevelLoader::LoadLevelData(const std::string& filePath) {
    std::ifstream file(filePath);
    if (file.fail()) {
        assert(0 && "Failed to open level JSON file.");
    }

    nlohmann::json deserialized;
    file >> deserialized;

    assert(deserialized.is_object());
    assert(deserialized.contains("name"));
    assert(deserialized["name"].get<std::string>() == "scene");

    LevelData* levelData = new LevelData();
    for (nlohmann::json& object : deserialized["objects"]) {
        DeserializeObjectRecursive(object, levelData);
    }

    return levelData;
}

std::unique_ptr<LevelData> LevelLoader::LoadJsonFile(const std::string& directoryPath, const std::string& filename) {
    std::unique_ptr<LevelData> levelData = std::make_unique<LevelData>();

    std::ifstream file(directoryPath + "/" + filename);
    assert(file.is_open());

    nlohmann::json root;
    file >> root;

    for (const auto& object : root["objects"]) {
        std::string type = object.value("type", "");

        if (type == "EnemySpawn") {
            EnemySpawnData enemy;
            enemy.fileName = object.value("file_name", "enemy");

            auto trans = object["transform"]["translation"];
            enemy.translation = { trans[0], trans[1], trans[2] };

            auto rot = object["transform"]["rotation"];
            enemy.rotation = { rot[0], rot[1], rot[2] };

            levelData->enemies.push_back(enemy);
        } else if (type == "PlayerSpawn") {
            PlayerSpawnData player;

            auto trans = object["transform"]["translation"];
            player.translation = { trans[0], trans[1], trans[2] };

            auto rot = object["transform"]["rotation"];
            player.rotation = { rot[0], rot[1], rot[2] };

            levelData->players.push_back(player);
        } else {
            ObjectData obj;
            obj.name = object.value("name", "");
            obj.fileName = object.value("file_name", "");
            auto trans = object["transform"]["translation"];
            auto rot = object["transform"]["rotation"];
            auto scale = object["transform"]["scaling"];
            obj.translation = { trans[0], trans[1], trans[2] };
            obj.rotation = { rot[0], rot[1], rot[2] };
            obj.scaling = { scale[0], scale[1], scale[2] };
            levelData->objects.push_back(obj);
        }
    }

    return levelData;
}

void LevelLoader::DeserializeObjectRecursive(nlohmann::json& object, LevelData* levelData) {
    if (!object.contains("type")) return;

    std::string type = object["type"].get<std::string>();

    if (type == "MESH") {
        levelData->objects.emplace_back(ObjectData{});
        ObjectData& objectData = levelData->objects.back();

        if (object.contains("name")) {
            objectData.name = object["name"];
        } else {
            assert(0 && "Missing 'name' field in JSON object.");
        }
        if (object.contains("file_name")) {
            objectData.fileName = object["file_name"];
        }

        if (object.contains("Disabled option")) {
            objectData.disabled = object["Disabled option"].get<bool>();
        } else {
            objectData.disabled = false;
        }

        auto& transform = object["transform"];

        constexpr float Deg2Rad = 3.1415926535f / 360.0f;

        objectData.translation = {
            (float)transform["translation"][0],
            (float)transform["translation"][1],
            (float)transform["translation"][2],
        };
        objectData.rotation = {
            (float)transform["rotation"][0] * Deg2Rad,
            (float)transform["rotation"][1] * Deg2Rad,
            (float)transform["rotation"][2] * Deg2Rad,
        };
        objectData.scaling = {
            (float)transform["scaling"][0],
            (float)transform["scaling"][1],
            (float)transform["scaling"][2],
        };
    }

    if (object.contains("children")) {
        for (auto& child : object["children"]) {
            DeserializeObjectRecursive(child, levelData);
        }
    }
}