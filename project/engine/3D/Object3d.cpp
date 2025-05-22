#include "Object3d.h"
#include "Object3dCommon.h"
#include <fstream>
#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG


Object3d::~Object3d() {
	OutputDebugStringA("Object3d Destructor Called\n");
	Cleanup();
}

void Object3d::Initialize(Object3dCommon* object3dCommon, WorldTransform* worldTransform)
{
	assert(object3dCommon != nullptr);
	assert(worldTransform != nullptr);

	this->object3dCommon_ = object3dCommon;
	worldTransform_ = std::make_unique<WorldTransform>();

	//modelData = LoadobjFile("resources", "plane.obj");

	//TextureManager::GetInstance()->LoadTexture(modelData.material.textureFilePath);
	//modelData.material.textureIndex =
	//	TextureManager::GetInstance()->GetTextureIndexByFilepath(modelData.material.textureFilePath);

	transform = { {1.0f,1.0f,1.0f},{0.0f,3.14f,0.0f},{0.0f,0.0f,10.0f} };
	cameraTransform = { {1.0f,1.0f,1.0f},{0.3f,0.0f,0.0f},{0.0f,4.0f,-10.0f} };

	cameraResource_ = CreateBufferResource(object3dCommon_->GetDxCommon()->GetDevice(), sizeof(CameraForGPU));
	cameraResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraData_));

	//CreateVertexBuffer();
	InitializeTransformationMatrix();
	InitializeLights();
	InitializeMaterial();

	this->camera = object3dCommon->GetDefaultCamera();
}

void Object3d::Update()
{
	if (!worldTransform_) {
		return;
	}

	if (worldTransform_) {
		worldTransform_->scale_ = transform.scale;
		worldTransform_->rotate_ = transform.rotate;
		worldTransform_->translate_ = transform.translate;
	}

	// WorldTransform에서 행렬 계산
	worldTransform_->UpdateMatrix();

	// WVP 계산
	
	if (camera) {
		const Matrix4x4& viewProj = camera->GetViewProjectionMatrix();

		if (model_) {
			//  glTF 계층 구조 반영한 최종 모델 행렬
			Matrix4x4 modelMatrix = MyMath::Multiply(model_->GetModelData().rootNode.localMatrix, worldTransform_->matWorld_);
			transformationMatrixData->WVP = MyMath::Multiply(modelMatrix, viewProj);
			transformationMatrixData->World = modelMatrix;
			transformationMatrixData->WorldInverseTranspose = MyMath::Transpose(MyMath::Inverse(modelMatrix));
		} else {
			// fallback
			transformationMatrixData->WVP = MyMath::Multiply(worldTransform_->matWorld_, viewProj);
			transformationMatrixData->World = worldTransform_->matWorld_;
			transformationMatrixData->WorldInverseTranspose = MyMath::Transpose(MyMath::Inverse(worldTransform_->matWorld_));
		}
	}

	if (camera && cameraData_) {
		cameraData_->worldPosition = camera->GetEye();
	}
#ifdef _DEBUG
	
#endif // _DEBUG

}

void Object3d::Draw()
{
	if (!model_ || !object3dCommon_ || !camera || !worldTransform_) return;

	//obj3d
	object3dCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_.Get()->GetGPUVirtualAddress());
	object3dCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource.Get()->GetGPUVirtualAddress());	
	object3dCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	object3dCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(4, cameraResource_->GetGPUVirtualAddress());
	object3dCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(5, pointLightResource_->GetGPUVirtualAddress());
	object3dCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(6, spotLightResource_->GetGPUVirtualAddress());
	object3dCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(7, ambientLightResource_->GetGPUVirtualAddress());
	object3dCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(8, areaLightResource_->GetGPUVirtualAddress());
	
	const Matrix4x4& viewProj = camera->GetViewProjectionMatrix();
	model_->DrawRecursive(model_->GetModelData().rootNode, worldTransform_->matWorld_, viewProj, transformationMatrixData);
}

void Object3d::Cleanup()
{

	if (transformationMatrixResource) {
		transformationMatrixResource.Reset();
		transformationMatrixData = nullptr;
	}

	if (directionalLightResource_) {
		directionalLightResource_.Reset();
		directionalLightData_ = nullptr;
	}

	if (pointLightResource_) {
		pointLightResource_.Reset();
		pointLightData_ = nullptr;
	}
	if (spotLightResource_) {
		spotLightResource_.Reset();
		spotLightData_ = nullptr;
	}
	if (ambientLightResource_) {
		ambientLightResource_.Reset();
		ambientLightData_ = nullptr;
	}
	if (areaLightResource_) {
		areaLightResource_.Reset();
		areaLightData_ = nullptr;
	}
	if (cameraResource_) {
		cameraResource_.Reset();         
		cameraData_ = nullptr;
	}
	if (materialResource_) {
		materialResource_.Reset();             
		materialData_ = nullptr;               
	}
	//model_ = nullptr; // ModelManager가 관리 중이므로 해제하지 않음
	if (worldTransform_) {
		worldTransform_->Cleanup();   // 또는 worldTransform_->Cleanup()
		worldTransform_.reset();     // unique_ptr 해제
	}
	object3dCommon_ = nullptr;
	camera = nullptr;
	defaultCamera = nullptr;
	transformationMatrixData = nullptr;
	directionalLightData_ = nullptr;
	pointLightData_ = nullptr;
	cameraData_ = nullptr;
}

void Object3d::InitializeMaterial() {

	auto device = object3dCommon_->GetDxCommon()->GetDevice();
	materialResource_ = CreateBufferResource(device, sizeof(Material));
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));

	// 조명 및 머티리얼 초기 설정
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialData_->enableLighting = false;
	materialData_->uvTransform = MyMath::MakeIdentity4x4();
	materialData_->shininess = 32.0f;
	materialData_->isBlinnPhong = 0;
	materialData_->usePointLight = 0;
	materialData_->useDirectionalLight = 1;
	materialData_->useSpotLight = 0;
	materialData_->useAmbientLight = 0;
	materialData_->useAreaLight = 0;
}


ComPtr<ID3D12Resource> Object3d::CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeInBytes)
{
	//頂点Heap
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	//頂点Resource
	D3D12_RESOURCE_DESC vertexResourceDesc{};
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeInBytes;

	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;

	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	ComPtr<ID3D12Resource> vertexResource = nullptr;
	HRESULT hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertexResource));
	assert(SUCCEEDED(hr));
	vertexResource->SetName(L"bufferResource");

	return vertexResource;
}

void Object3d::SetModel(const std::string& filePath)
{
	model_ = ModelManager::GetInstance()->FindModel(filePath);
	if (!model_) {
		OutputDebugStringA(("Model not found: " + filePath + "\n").c_str());
	}
}

void Object3d::SetPointLight(const Vector3& position, float intensity, float radius, float decay) {
	if (pointLightData_) {
		pointLightData_->position = position;
		pointLightData_->intensity = intensity;
		pointLightData_->radius = radius;
		pointLightData_->decay = decay;
	}
}

void Object3d::SetSpotLight(const Vector3& position, const Vector3& direction, float intensity, float cutoff, float outerCutoff, float decay, float radius) {
	if (spotLightData_) {
		spotLightData_->position = position;
		spotLightData_->direction = direction;
		spotLightData_->intensity = intensity;
		spotLightData_->cutoff = cutoff;
		spotLightData_->outerCutoff = outerCutoff;
		spotLightData_->decay = decay;
		spotLightData_->radius = radius;
	}
}

void Object3d::SetAreaLight(const Vector3& position, const Vector3& right, float halfWidth, const Vector3& up, float halfHeight, const Vector4& color, float intensity) {
	if (areaLightData_) {
		areaLightData_->position = position;
		areaLightData_->right = right;
		areaLightData_->halfWidth = halfWidth;
		areaLightData_->up = up;
		areaLightData_->halfHeight = halfHeight;
		areaLightData_->color = color;
		areaLightData_->intensity = intensity;
	}
}

void Object3d::SetEnableLighting(bool enable) {
	if (materialData_) materialData_->enableLighting = enable;
}

bool Object3d::GetEnableLighting() const {
	return materialData_ ? materialData_->enableLighting != 0 : false;
}

void Object3d::SetIsBlinnPhong(bool isBlinn) {
	if (materialData_) materialData_->isBlinnPhong = isBlinn;
}

bool Object3d::GetIsBlinnPhong() const {
	return materialData_ ? materialData_->isBlinnPhong != 0 : false;
}

void Object3d::SetUsePointLight(bool use) {
	if (materialData_) materialData_->usePointLight = use;
}

bool Object3d::GetUsePointLight() const {
	return materialData_ ? materialData_->usePointLight != 0 : false;
}

void Object3d::SetUseDirectionalLight(bool use) {
	if (materialData_) materialData_->useDirectionalLight = use;
}

bool Object3d::GetUseDirectionalLight() const {
	return materialData_ ? materialData_->useDirectionalLight != 0 : false;
}

void Object3d::SetUseSpotLight(bool use) {
	if (materialData_) materialData_->useSpotLight = use;
}

bool Object3d::GetUseSpotLight() const {
	return materialData_ ? materialData_->useSpotLight != 0 : false;
}

void Object3d::SetUseAmbientLight(bool use) {
	if (materialData_) materialData_->useAmbientLight = use;
}

bool Object3d::GetUseAmbientLight() const {
	return materialData_ ? materialData_->useAmbientLight != 0 : false;
}

void Object3d::SetUseAreaLight(bool use) {
	if (materialData_) materialData_->useAreaLight = use;
}

bool Object3d::GetUseAreaLight() const {
	return materialData_ ? materialData_->useAreaLight != 0 : false;
}


void Object3d::InitializeTransformationMatrix()
{

	auto device = object3dCommon_->GetDxCommon()->GetDevice();

	transformationMatrixResource = CreateBufferResource(device.Get(), sizeof(TransformationMatrix));

	transformationMatrixResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData));

	transformationMatrixData->WVP = MyMath::MakeIdentity4x4();
	transformationMatrixData->World = MyMath::MakeIdentity4x4();
}

void Object3d::InitializeLights()
{
	auto device = object3dCommon_->GetDxCommon()->GetDevice();

	// Directional
	directionalLightResource_ = CreateBufferResource(device, sizeof(DirectionalLight));
	directionalLightResource_->Map(0, nullptr, (void**)&directionalLightData_);
	directionalLightData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	directionalLightData_->direction = { 0.0f, -1.0f, 0.0f };
	directionalLightData_->intensity = 1.0f;

	// Point
	pointLightResource_ = CreateBufferResource(device, sizeof(PointLight));
	pointLightResource_->Map(0, nullptr, (void**)&pointLightData_);
	pointLightData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	pointLightData_->position = { 0.0f, 5.0f, 0.0f };
	pointLightData_->intensity = 1.0f;
	pointLightData_->radius = 10.0f;
	pointLightData_->decay = 1.0f;

	// Spot
	spotLightResource_ = CreateBufferResource(device, sizeof(SpotLight));
	spotLightResource_->Map(0, nullptr, (void**)&spotLightData_);
	spotLightData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	spotLightData_->position = { 0.0f, 5.0f, 5.0f };     // 빛 위치
	spotLightData_->direction = { 0.0f, -1.0f, -1.0f };  // 빛 방향
	spotLightData_->intensity = 1.0f;
	spotLightData_->cutoff = cosf(MyMath::ToRadian(15.0f));        // 내부 각도 (15도)
	spotLightData_->outerCutoff = cosf(MyMath::ToRadian(30.0f));   // 외부 각도 (30도)
	spotLightData_->radius = 15.0f;
	spotLightData_->decay = 1.0f;

	// Ambient
	ambientLightResource_ = CreateBufferResource(device, sizeof(AmbientLight));
	ambientLightResource_->Map(0, nullptr, (void**)&ambientLightData_);
	ambientLightData_->color = { 0.1f, 0.1f, 0.1f, 1.0f }; // Ambient Light Color

	//Area
	areaLightResource_ = CreateBufferResource(device, sizeof(AreaLight));
	areaLightResource_->Map(0, nullptr, (void**)&areaLightData_);
	areaLightData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	// 중심 위치
	areaLightData_->position = { 0.0f, 5.0f, 0.0f };

	// 가로 방향 벡터 (x축 기준)
	areaLightData_->right = { 1.0f, 0.0f, 0.0f };
	areaLightData_->halfWidth = 2.0f;

	// 세로 방향 벡터 (y축 기준)
	areaLightData_->up = { 0.0f, 1.0f, 0.0f };
	areaLightData_->halfHeight = 2.0f;

	areaLightData_->intensity = 1.0f;
	
}
