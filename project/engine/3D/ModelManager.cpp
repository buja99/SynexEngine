#include "ModelManager.h"
#include "DirectXCommon.h"
#include "Model.h"
#include "ModelCommon.h"

ModelManager* ModelManager::instance = nullptr;

ModelManager* ModelManager::GetInstance()
{
    if (!instance) {
        instance = new ModelManager();
    }
    return instance;
}



void ModelManager::Initialize(DirectXCommon* dxCommon)
{
	

    modelCommon = std::make_unique<ModelCommon>();
    modelCommon->Initialize(dxCommon);

    object3dCommon = Object3dCommon::GetInstance();
    //object3dCommon->Initialize(dxCommon);
}



void ModelManager::LoadModel(const std::string& directorypath, const std::string& filePath)
{

    std::string fullPath = directorypath + "/" + filePath;

    if (models_.contains(fullPath)) {
        return;
    }
    std::unique_ptr<Model> model = std::make_unique<Model>();
    model->Initialize(modelCommon.get(), object3dCommon, directorypath, filePath);

    models_.insert(std::make_pair(filePath, std::move(model)));

}

Model* ModelManager::FindModel(const std::string& filePath)
{
    if (models_.contains(filePath)) {
        return models_.at(filePath).get();
    }
    return nullptr;
}

void ModelManager::Finalize()
{
    for (auto& [_, model] : models_) {
        model->Cleanup(); 
    }
    models_.clear();
    
    if (instance) {
        delete instance;
        instance = nullptr;
    }

}
