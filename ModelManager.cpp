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

void ModelManager::Finalize()
{

    if (instance) {
        delete instance;
        instance = nullptr;
    }

}

void ModelManager::Initialize(DirectXCommon* dxCommon)
{
    modelCommon = new ModelCommon();
    modelCommon->Initialize(dxCommon);
}

void ModelManager::LoadModel(const std::string& filePath)
{
    if (models.contains(filePath)) {
        return;
    }
    std::unique_ptr<Model> model = std::make_unique<Model>();
    model->Initialize(modelCommon, "resources", filePath);

    models.insert(std::make_pair(filePath, std::move(model)));
}

Model* ModelManager::FindModel(const std::string& filePath)
{
    if (models.contains(filePath)) {
        return models.at(filePath).get();
    }
    return nullptr;
}
