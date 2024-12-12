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

    object3dCommon = std::make_unique<Object3dCommon>();
    object3dCommon->Initialize(dxCommon);
}



void ModelManager::LoadModel(const std::string& directorypath,const std::string& filePath)
{
    if (models.contains(filePath)) {
        return;
    }
    std::unique_ptr<Model> model = std::make_unique<Model>();
    model->Initialize(modelCommon.get(), object3dCommon.get(), directorypath, filePath);

    models.insert(std::make_pair(filePath, std::move(model)));
}

Model* ModelManager::FindModel(const std::string& filePath)
{
    if (models.contains(filePath)) {
        return models.at(filePath).get();
    }
    return nullptr;
}

void ModelManager::Finalize()
{


    if (instance) {
        delete instance;
        instance = nullptr;
    }
   
}
