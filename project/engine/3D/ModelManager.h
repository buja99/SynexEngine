#pragma once
#include <string>
#include <map>
#include <memory>


class Model;
class DirectXCommon;
class ModelCommon;
class Object3dCommon;

class ModelManager
{


public:
    static ModelManager* GetInstance();




    void Initialize(DirectXCommon* dxCommon);

    void LoadModel(const std::string& directorypath, const std::string& filePath);

    Model* FindModel(const std::string& filePath);

    static void Finalize();

private:
    static ModelManager* instance;

    std::map<std::string, std::unique_ptr<Model>> models;

    std::unique_ptr<ModelCommon> modelCommon;
    std::unique_ptr<Object3dCommon> object3dCommon;

    //constructor
    ModelManager() = default;
    //Destructor
    ~ModelManager() = default;
    //Copy Constructor
    ModelManager(const ModelManager&) = delete;
    //Copy assignment operator
    ModelManager& operator=(const ModelManager&) = delete;




};

