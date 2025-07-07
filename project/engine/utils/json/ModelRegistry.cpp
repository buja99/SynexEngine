#include "ModelRegistry.h"

void ModelRegistry::Register(const std::string& name, Object3d* model, WorldTransform* transform) {
    modelTable_[name] = model;
    transformTable_[name] = transform;
}

Object3d* ModelRegistry::GetModel(const std::string& name) const {
    auto it = modelTable_.find(name);
    return (it != modelTable_.end()) ? it->second : nullptr;
}

WorldTransform* ModelRegistry::GetTransform(const std::string& name) const {
    auto it = transformTable_.find(name);
    return (it != transformTable_.end()) ? it->second : nullptr;
}