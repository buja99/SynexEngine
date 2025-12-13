#pragma once
#include "MyMath.h"
#include <wrl.h>
#include "WinApp.h"
#include <Windows.h>
#include "DirectXTex.h"
#include "Transform.h"
#include "Material.h"
#include "VertexData.h"
#include "Lighting.h"
#include "TextureManager.h"
#include "Model.h"
#include "ModelManager.h"
#include "Camera.h"
#include "Matrix4x4.h"
#include "WorldTransform.h"

//struct ParallelLight {
//	Vector4 color;     
//	Vector3 direction; 
//	float intensity;   
//};

class WorldTransform;
class Object3dCommon;

using Microsoft::WRL::ComPtr;

/// <summary>3Dモデルの変換・ライティング・描画管理</summary>
class Object3d
{
	public:
		~Object3d();

	/// <summary>Object3d基本リソース初期化</summary>
	void Initialize(Object3dCommon* object3dCommon, WorldTransform* worldTransform);
	
	/// <summary>Transform・アニメーション・行列更新</summary>
	void Update();
	void Draw();
	void Cleanup();

	void InitializeMaterial();

	ComPtr<ID3D12Resource> CreateBufferResource(ComPtr <ID3D12Device> device, size_t sizeInBytes);

	void SetModel(Model* model) { this->model_ = model; }

	void SetModel(const std::string& filePath);

	void SetEnvironmentMap(const std::string& filePath);

	// setter
	void SetScale(const Vector3& scale) { transform.scale = scale; }
	void SetRotate(const Vector3& rotate) { transform.rotate = rotate; }
	void SetTranslate(const Vector3& translate) { transform.translate = translate; }
	void SetCamera(Camera* camera) { this->camera = camera; }
	void SetDefaultCamera(Camera* camera) { this->defaultCamera = camera; }
	void SetPointLight(const Vector3& position, float intensity, float radius, float decay);
	void SetSpotLight(const Vector3& position, const Vector3& direction, float intensity, float cutoff, float outerCutoff, float decay, float radius);
	void SetAreaLight(const Vector3& position, const Vector3& right, float halfWidth, const Vector3& up, float halfHeight, const Vector4& color, float intensity);
	// getter
	const Vector3& GetScale() const { return transform.scale; }
	const Vector3& GetRotate() const { return transform.rotate; }
	const Vector3& GetTranslate() const { return transform.translate; }
	Camera* GetDefaultCamera() const { return defaultCamera; }
	Model* GetModel() const { return model_; }

	// Lighting setter/getter
    void SetEnableLighting(bool enable);
    bool GetEnableLighting() const;

    void SetIsBlinnPhong(bool isBlinn);
    bool GetIsBlinnPhong() const;

    void SetUsePointLight(bool use);
    bool GetUsePointLight() const;

    void SetUseDirectionalLight(bool use);
    bool GetUseDirectionalLight() const;

	void SetUseSpotLight(bool use);
	bool GetUseSpotLight() const;

	void SetUseAmbientLight(bool use);
	bool GetUseAmbientLight() const;

	void SetUseAreaLight(bool use);
	bool GetUseAreaLight() const;

	void SetUseEnvironmentMap(bool use);
	bool GetUseEnvironmentMap() const;

private:

	std::string modelName_;

	Model* model_ = nullptr;

	Object3dCommon* object3dCommon_ = nullptr;

	ComPtr<ID3D12Resource> transformationMatrixResource;
	TransformationMatrix* transformationMatrixData = nullptr;

	ComPtr<ID3D12Resource> directionalLightResource_;
	DirectionalLight* directionalLightData_ = nullptr;

	ComPtr<ID3D12Resource> pointLightResource_;
	PointLight* pointLightData_ = nullptr;

	ComPtr<ID3D12Resource> spotLightResource_;
	SpotLight* spotLightData_ = nullptr;

	ComPtr<ID3D12Resource> ambientLightResource_;
	AmbientLight* ambientLightData_ = nullptr;

	ComPtr<ID3D12Resource> areaLightResource_;
	AreaLight* areaLightData_ = nullptr;

	ComPtr<ID3D12Resource> cameraResource_;
	CameraForGPU* cameraData_ = nullptr;

	//void CreateVertexBuffer();
	//void InitializeMaterial();
	void InitializeTransformationMatrix();
	void InitializeLights();

	Transform transform;
	Transform cameraTransform;

	Camera* camera = nullptr;
	Camera* defaultCamera = nullptr;

	WorldTransform* worldTransform_ = nullptr;

	ComPtr<ID3D12Resource> materialResource_;
	Material* materialData_ = nullptr;

	D3D12_GPU_DESCRIPTOR_HANDLE envMapSrvHandle_{};

};

