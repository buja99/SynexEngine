#pragma once
#include <string>
#include <map>
#include <memory>


class Model;
class DirectXCommon;
class ModelCommon;
class Object3dCommon;

/// <summary>モデル読み込みとインスタンス管理のシングルトン</summary>
class ModelManager
{


public:
    static ModelManager* GetInstance();



    /// <summary>モデル共通リソース初期化</summary>
    void Initialize(DirectXCommon* dxCommon);

    /// <summary>モデル読み込みと登録</summary>
    void LoadModel(const std::string& directorypath, const std::string& filePath);

    /// <summary>登録済みモデル取得</summary>
    Model* FindModel(const std::string& filePath);

    void Finalize();

private:
    static ModelManager* instance;

    std::map<std::string, std::unique_ptr<Model>> models_;

    std::unique_ptr<ModelCommon> modelCommon;
    Object3dCommon* object3dCommon;

    //constructor
    ModelManager() = default;
    //Destructor
    ~ModelManager() = default;
    //Copy Constructor
    ModelManager(const ModelManager&) = delete;
    //Copy assignment operator
    ModelManager& operator=(const ModelManager&) = delete;




};

