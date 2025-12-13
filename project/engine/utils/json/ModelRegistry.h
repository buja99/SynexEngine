#pragma once
#include "Object3d.h"
#include "WorldTransform.h"
#include <unordered_map>
#include <string>

/// <summary>名前をキーにObject3dとWorldTransformを登録・参照するレジストリ</summary>
class ModelRegistry {
public:
    void Register(const std::string& name, Object3d* model, WorldTransform* transform);

    Object3d* GetModel(const std::string& name) const;
    WorldTransform* GetTransform(const std::string& name) const;

    const std::unordered_map<std::string, Object3d*>& GetModelMap() const { return modelTable_; }
    const std::unordered_map<std::string, WorldTransform*>& GetTransformMap() const { return transformTable_; }

private:
    std::unordered_map<std::string, Object3d*> modelTable_;
    std::unordered_map<std::string, WorldTransform*> transformTable_;
};

