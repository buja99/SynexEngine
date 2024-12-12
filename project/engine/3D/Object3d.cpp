#include "Object3d.h"
#include "Object3dCommon.h"
#include <fstream>
#ifdef _DEBUG
#include "externals/imgui/imgui.h"
#endif // _DEBUG

void Object3d::Initialize(Object3dCommon* object3dCommon)
{
	this->object3dCommon_ = object3dCommon;
	

	//modelData = LoadobjFile("resources", "plane.obj");

	//TextureManager::GetInstance()->LoadTexture(modelData.material.textureFilePath);
	//modelData.material.textureIndex =
	//	TextureManager::GetInstance()->GetTextureIndexByFilepath(modelData.material.textureFilePath);

	transform = { {1.0f,1.0f,1.0f},{0.0f,3.14f,0.0f},{0.0f,0.0f,50.0f} };
	cameraTransform = { {1.0f,1.0f,1.0f},{0.3f,0.0f,0.0f},{0.0f,4.0f,-10.0f} };

	
	InitializeTransformationMatrix();
	InitializeParallelLight();

	this->camera = object3dCommon->GetDefaultCamera();
}

void Object3d::Updata()
{
	Matrix4x4 worldMatrix = myMath->MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 cameraMatrix = myMath->MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
	Matrix4x4 viewMatrix = myMath->Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = myMath->MakePerspectiveFovMatrix(0.45f, 1280.0f / 720.0f, 0.1f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = myMath->Multiply(worldMatrix, myMath->Multiply(viewMatrix, projectionMatrix));
	if (camera) {
		const Matrix4x4& viewProjectionMatrix = camera->GetViewProjectionMatrix();
		worldViewProjectionMatrix = myMath->Multiply(worldMatrix, viewProjectionMatrix);
	} else {
		worldViewProjectionMatrix = worldMatrix;
	}
	transformationMatrixData->WVP = worldViewProjectionMatrix;
	transformationMatrixData->World = worldMatrix;
#ifdef _DEBUG
	ImGui::Begin("payer");
	ImGui::DragFloat3("translate", &transform.translate.x, 0.1f);
	ImGui::DragFloat3("scale", &transform.scale.x, 0.1f);
	ImGui::DragFloat3("rotate", &transform.rotate.x, 0.1f);
	ImGui::End();
#endif // _DEBUG

}

void Object3d::Draw()
{

	//obj3d
	object3dCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResource.Get()->GetGPUVirtualAddress());
	object3dCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, parallelLightResource.Get()->GetGPUVirtualAddress());	

	if (model_) {

		model_->Draw();
	}

}

void Object3d::Cleanup()
{

	delete myMath;
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



void Object3d::InitializeTransformationMatrix()
{

	auto device = object3dCommon_->GetDxCommon()->GetDevice();

	transformationMatrixResource = CreateBufferResource(device.Get(), sizeof(TransformationMatrix));

	transformationMatrixResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData));

	transformationMatrixData->WVP = myMath->MakeIdentity4x4();
	transformationMatrixData->World = myMath->MakeIdentity4x4();
}

void Object3d::InitializeParallelLight()
{
	auto device = object3dCommon_->GetDxCommon()->GetDevice();

	parallelLightResource = CreateBufferResource(device.Get(), sizeof(ParallelLight));

	parallelLightResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&parallelLightData));

	parallelLightData->color = { 1.0f, 1.0f, 1.0f, 1.0f }; 
	parallelLightData->direction = { 0.0f, -1.0f, 0.0f };  
	parallelLightData->intensity = 1.0f;

}
