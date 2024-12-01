#pragma once
#include <string>
#include <map>
#include <memory>


class Model;
class DirectXCommon;
class ModelCommon;

class ModelManager
{


public:
    static ModelManager* GetInstance();


    static void Finalize();


    void Initialize(DirectXCommon* dxCommon);

    void LoadModel(const std::string& filePath);

    Model* FindModel(const std::string& filePath);

private:
    static ModelManager* instance;

    std::map<std::string, std::unique_ptr<Model>> models;

    ModelCommon* modelCommon = nullptr;

    //constructor
    ModelManager() = default;
    //Destructor
    ~ModelManager() = default;
    //Copy Constructor
    ModelManager(const ModelManager&) = delete;
    //Copy assignment operator
    ModelManager& operator=(const ModelManager&) = delete;




};

