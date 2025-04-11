#include "Object3d.h"
#include "Object3dCommon.h"
#include <fstream>
#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG


void Object3d::Initialize(Object3dCommon* object3dCommon, WorldTransform* worldTransform)
{
	assert(object3dCommon != nullptr);
	assert(worldTransform != nullptr);

	this->object3dCommon_ = object3dCommon;
	this->worldTransform_ = worldTransform;

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
		transformationMatrixData->WVP = MyMath::Multiply(worldTransform_->matWorld_, viewProj);
	} else {
		transformationMatrixData->WVP = worldTransform_->matWorld_;
	}
	transformationMatrixData->World = worldTransform_->matWorld_;
	transformationMatrixData->WorldInverseTranspose =
		MyMath::Transpose(MyMath::Inverse(worldTransform_->matWorld_));

	if (camera && cameraData_) {
		cameraData_->worldPosition = camera->GetEye();
	}
#ifdef _DEBUG
	ImGui::Begin("plane");
	ImGui::DragFloat3("translate", &transform.translate.x, 0.1f);
	ImGui::DragFloat3("scale", &transform.scale.x, 0.1f);
	ImGui::DragFloat3("rotate", &transform.rotate.x, 0.1f);
	ImGui::End();

	ImGui::Begin("camera");
	ImGui::DragFloat3("translate", &cameraTransform.translate.x, 0.1f);
	ImGui::DragFloat3("rotate", &cameraTransform.rotate.x, 0.1f);
	ImGui::End();
#endif // _DEBUG

}

void Object3d::Draw()
{
	if (model_) {
		if (materialData_) {
			*materialData_ = model_->GetMaterialData();  // ← 이걸 추가!
		}
	}
	//obj3d
	object3dCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_.Get()->GetGPUVirtualAddress());
	object3dCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource.Get()->GetGPUVirtualAddress());	
	object3dCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource_->GetGPUVirtualAddress());
	object3dCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(4, cameraResource_->GetGPUVirtualAddress());
	object3dCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(5, pointLightResource_->GetGPUVirtualAddress());

	if (model_) {

		model_->Draw();
	}

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

	//model_ = nullptr; // ModelManager가 관리 중이므로 해제하지 않음
	worldTransform_ = nullptr;
	object3dCommon_ = nullptr;
	camera = nullptr;
	defaultCamera = nullptr;
}

void Object3d::InitializeMaterial() {

	auto device = object3dCommon_->GetDxCommon()->GetDevice();
	materialResource_ = CreateBufferResource(device, sizeof(Material));
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));

	// 조명 및 머티리얼 초기 설정
	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialData_->enableLighting = true;
	materialData_->uvTransform = MyMath::MakeIdentity4x4();
	materialData_->shininess = 32.0f;
	materialData_->reflectModel = 2; // 2 이상이면 포인트 라이트 사용
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

	//if (model) {
	//	this->model = model; 
	//} else {
	//	
	//	ModelManager::GetInstance()->LoadModel(filePath);
	//	this->model = ModelManager::GetInstance()->FindModel(filePath);
	//}
}

void Object3d::SetPointLight(const Vector3& position, float intensity, float radius, float decay) {
	if (pointLightData_) {
		pointLightData_->position = position;
		pointLightData_->intensity = intensity;
		pointLightData_->radius = radius;
		pointLightData_->decay = decay;
	}
}

//void Object3d::CreateVertexBuffer()
//{
//
//	auto device = object3dCommon->GetDxCommon()->GetDevice();
//
//	
//	vertexResource = CreateBufferResource(device.Get(), sizeof(VertexData) * modelData.vertices.size());
//	vertexBufferView.BufferLocation = vertexResource.Get()->GetGPUVirtualAddress();
//	vertexBufferView.SizeInBytes = static_cast<UINT>(sizeof(VertexData) * modelData.vertices.size());
//	vertexBufferView.StrideInBytes = sizeof(VertexData);
//
//	
//	VertexData* vertexData = nullptr;
//	vertexResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
//	memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());
//	vertexResource.Get()->Unmap(0, nullptr);
//
//}
//
//void Object3d::InitializeMaterial()
//{
//
//	auto device = object3dCommon->GetDxCommon()->GetDevice();
//
//	
//	materialResource = CreateBufferResource(device.Get(), sizeof(Material));
//
//	
//	materialResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
//
//	materialData->color = { 1.0f, 1.0f, 1.0f, 1.0f }; 
//	materialData->enableLighting = false;          
//	materialData->uvTransform = myMath->MakeIdentity4x4(); 
//
//}

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

}
