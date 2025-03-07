#pragma once
#include "MyMath.h"
#include <wrl.h>
#include "WinApp.h"
#include <Windows.h>
#include "DirectXTex.h"
#include "Transform.h"
#include "Material.h"
#include "VertexData.h"
#include "DirectionalLight.h"
#include "TextureManager.h"
#include "Model.h"
#include "ModelManager.h"
#include "Camera.h"
#include "Matrix4x4.h"

struct ParallelLight {
	Vector4 color;     
	Vector3 direction; 
	float intensity;   
};


class Object3dCommon;

using Microsoft::WRL::ComPtr;

class Object3d
{
	public:


	void Initialize(Object3dCommon* object3dCommon);
	void Updata();
	void Draw();
	void Cleanup();

	ComPtr<ID3D12Resource> CreateBufferResource(ComPtr <ID3D12Device> device, size_t sizeInBytes);

	void SetModel(Model* model) { this->model_ = model; }

	void SetModel(const std::string& filePath);

	// setter
	void SetScale(const Vector3& scale) { transform.scale = scale; }
	void SetRotate(const Vector3& rotate) { transform.rotate = rotate; }
	void SetTranslate(const Vector3& translate) { transform.translate = translate; }
	void SetCamera(Camera* camera) { this->camera = camera; }
	void SetDefaultCamera(Camera* camera) { this->defaultCamera = camera; }

	// getter

	const Vector3& GetScale() const { return transform.scale; }
	const Vector3& GetRotate() const { return transform.rotate; }
	const Vector3& GetTranslate() const { return transform.translate; }
	Camera* GetDefaultCamera() const { return defaultCamera; }

private:

	Model* model_ = nullptr;

	Object3dCommon* object3dCommon_ = nullptr;

	//ModelData modelData;

	//ComPtr<ID3D12Resource> vertexResource;
	//D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	//
	//ComPtr<ID3D12Resource> materialResource;
	//Material* materialData = nullptr;

	ComPtr<ID3D12Resource> transformationMatrixResource;
	TransformationMatrix* transformationMatrixData = nullptr;

	ComPtr<ID3D12Resource> parallelLightResource;
	ParallelLight* parallelLightData = nullptr;

	//void CreateVertexBuffer();
	//void InitializeMaterial();
	void InitializeTransformationMatrix();
	void InitializeParallelLight();

	Transform transform;
	Transform cameraTransform;

	Camera* camera = nullptr;
	Camera* defaultCamera = nullptr;

	MyMath* myMath = new MyMath;

};

