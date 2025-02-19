#include "Object3d.h"
#include "Object3dCommon.h"
#include <fstream>
#ifdef _DEBUG
#include "imgui/imgui.h"
#endif // _DEBUG

void Object3d::Initialize(Object3dCommon* object3dCommon)
{
	this->object3dCommon_ = object3dCommon;
	

	//modelData = LoadobjFile("resources", "plane.obj");

	//TextureManager::GetInstance()->LoadTexture(modelData.material.textureFilePath);
	//modelData.material.textureIndex =
	//	TextureManager::GetInstance()->GetTextureIndexByFilepath(modelData.material.textureFilePath);

	transform = { {1.0f,1.0f,1.0f},{0.0f,3.14f,0.0f},{0.0f,0.0f,10.0f} };
	cameraTransform = { {1.0f,1.0f,1.0f},{0.3f,0.0f,0.0f},{0.0f,4.0f,-10.0f} };

	//CreateVertexBuffer();
	//InitializeMaterial();
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

//MaterialData Object3d::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
//{
//	MaterialData materialData;
//	std::string line;
//	std::ifstream file(directoryPath + "/" + filename);
//	assert(file.is_open());
//
//	while (std::getline(file, line)) {
//		std::string identifier;
//		std::istringstream s(line);
//		s >> identifier;
//
//		if (identifier == "map_Kd")
//		{
//			std::string textureFilename;
//			s >> textureFilename;
//
//			materialData.textureFilePath = directoryPath + "/" + textureFilename;
//
//		}
//
//
//	}
//
//	return materialData;
//
//}
//
//ModelData Object3d::LoadobjFile(const std::string& directoryPath, const std::string& filename)
//{
//	//Declaring variables
//	ModelData modelData;
//	std::vector<Vector4> positions;
//	std::vector<Vector3> normals;
//	std::vector<Vector2> texCoords;
//	std::string line;
//	//Open the file
//	std::ifstream file(directoryPath + "/" + filename);
//	assert(file.is_open());
//	//Read the file and build ModelData
//	while (std::getline(file, line)) {
//		std::string identifier;
//		std::istringstream s(line);
//		s >> identifier;
//		if (identifier == "v") {
//			Vector4 position;
//			s >> position.x >> position.y >> position.z;
//			position.w = 1.0f;
//			position.x *= -1.0f;
//			//position.z *= -1.0f;
//			positions.push_back(position);
//		} else if (identifier == "vt") {
//			Vector2 texCoord;
//			s >> texCoord.x >> texCoord.y;
//			texCoord.y = -1.0f - texCoord.y;
//			texCoords.push_back(texCoord);
//		} else if (identifier == "vn") {
//			Vector3 normal;
//			s >> normal.x >> normal.y >> normal.z;
//			normal.x *= -1.0f;
//			//normal.z *= -1.0f;
//			normals.push_back(normal);
//		} else if (identifier == "f") {
//			VertexData triangle[3];
//			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
//				std::string vertexDefinition;
//				s >> vertexDefinition;
//
//				std::istringstream v(vertexDefinition);
//				uint32_t elementIndices[3];
//				for (int32_t element = 0; element < 3; ++element)
//				{
//					std::string index;
//					std::getline(v, index, '/');
//					elementIndices[element] = std::stoi(index);
//				}
//				Vector4 position = positions[elementIndices[0] - 1];
//				Vector2 texCoord = texCoords[elementIndices[1] - 1];
//				Vector3 normal = normals[elementIndices[2] - 1];
//				//VertexData vertex = { position,texCoord,normal };
//				//modelDate.vertices.push_back(vertex);
//				triangle[faceVertex] = { position, texCoord, normal };
//			}
//			modelData.vertices.push_back(triangle[2]);
//			modelData.vertices.push_back(triangle[1]);
//			modelData.vertices.push_back(triangle[0]);
//		} else if (identifier == "mtllib") {
//			std::string materialFilename;
//			s >> materialFilename;
//
//			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
//		}
//	}
//
//	//Return ModelData
//
//	return modelData;
//}
//
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
