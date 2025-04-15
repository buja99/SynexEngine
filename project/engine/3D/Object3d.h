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

class Object3d
{
	public:
		~Object3d();

	void Initialize(Object3dCommon* object3dCommon, WorldTransform* worldTransform);
	
	void Update();
	void Draw();
	void Cleanup();

	void InitializeMaterial();

	ComPtr<ID3D12Resource> CreateBufferResource(ComPtr <ID3D12Device> device, size_t sizeInBytes);

	void SetModel(Model* model) { this->model_ = model; }

	void SetModel(const std::string& filePath);

	void SetReflectModel(int model) {
		if (materialData_) {
			materialData_->reflectModel = model;
		}
	}

	// setter
	void SetScale(const Vector3& scale) { transform.scale = scale; }
	void SetRotate(const Vector3& rotate) { transform.rotate = rotate; }
	void SetTranslate(const Vector3& translate) { transform.translate = translate; }
	void SetCamera(Camera* camera) { this->camera = camera; }
	void SetDefaultCamera(Camera* camera) { this->defaultCamera = camera; }
	void SetPointLight(const Vector3& position, float intensity, float radius, float decay);
	// getter

	const Vector3& GetScale() const { return transform.scale; }
	const Vector3& GetRotate() const { return transform.rotate; }
	const Vector3& GetTranslate() const { return transform.translate; }
	Camera* GetDefaultCamera() const { return defaultCamera; }
	Model* GetModel() const { return model_; }

private:

	Model* model_ = nullptr;

	Object3dCommon* object3dCommon_ = nullptr;

	ComPtr<ID3D12Resource> transformationMatrixResource;
	TransformationMatrix* transformationMatrixData = nullptr;

	ComPtr<ID3D12Resource> directionalLightResource_;
	DirectionalLight* directionalLightData_ = nullptr;

	ComPtr<ID3D12Resource> pointLightResource_;
	PointLight* pointLightData_ = nullptr;

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

	std::unique_ptr<WorldTransform> worldTransform_;

	ComPtr<ID3D12Resource> materialResource_;
	Material* materialData_ = nullptr;

};

