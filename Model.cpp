#include "Model.h"
#include <fstream>


void Model::Initialize(ModelCommon* modelCommon, const std::string& directorypath, const std::string& filename)
{

	modelCommon_ = modelCommon;

	this->object3dCommon = object3dCommon;

	
	modelData = LoadobjFile(directorypath, filename);

	InitializeVertexBuffer();

	InitializeMaterial();

	TextureManager::GetInstance()->LoadTexture(modelData.material.textureFilePath);
	modelData.material.textureIndex =
		TextureManager::GetInstance()->GetTextureIndexByFilepath(modelData.material.textureFilePath);


}

void Model::Draw()
{
	object3dCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);

	object3dCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_.Get()->GetGPUVirtualAddress());

	auto textureDescriptorHandle = TextureManager::GetInstance()->GetSrvHandleGPU(modelData.material.textureIndex);
	object3dCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureDescriptorHandle);

	object3dCommon->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);

}

void Model::Cleanup()
{

	delete myMath;
}

ModelData Model::LoadobjFile(const std::string& directoryPath, const std::string& filename)
{
	//Declaring variables
	ModelData modelData;
	std::vector<Vector4> positions;
	std::vector<Vector3> normals;
	std::vector<Vector2> texCoords;
	std::string line;
	//Open the file
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());
	//Read the file and build ModelData
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;
		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.w = 1.0f;
			position.x *= -1.0f;
			//position.z *= -1.0f;
			positions.push_back(position);
		} else if (identifier == "vt") {
			Vector2 texCoord;
			s >> texCoord.x >> texCoord.y;
			texCoord.y = -1.0f - texCoord.y;
			texCoords.push_back(texCoord);
		} else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f;
			//normal.z *= -1.0f;
			normals.push_back(normal);
		} else if (identifier == "f") {
			VertexData triangle[3];
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;

				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element)
				{
					std::string index;
					std::getline(v, index, '/');
					elementIndices[element] = std::stoi(index);
				}
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texCoord = texCoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				//VertexData vertex = { position,texCoord,normal };
				//modelDate.vertices.push_back(vertex);
				triangle[faceVertex] = { position, texCoord, normal };
			}
			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);
		} else if (identifier == "mtllib") {
			std::string materialFilename;
			s >> materialFilename;

			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}
	}

	//Return ModelData

	return modelData;
}

MaterialData Model::LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename)
{
	MaterialData materialData;
	std::string line;
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		if (identifier == "map_Kd")
		{
			std::string textureFilename;
			s >> textureFilename;

			materialData.textureFilePath = directoryPath + "/" + textureFilename;

		}


	}

	return materialData;
}

ComPtr<ID3D12Resource> Model::CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeInBytes)
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


void Model::InitializeVertexBuffer()
{
	//if (!object3dCommon || !object3dCommon->GetDxCommon()) {
	//	OutputDebugStringA("Error: object3dCommon or dxCommon is nullptr.\n");
	//	return;
	//}
	//
	//if (modelData.vertices.empty()) {
	//	OutputDebugStringA("Error: modelData.vertices is empty in InitializeVertexBuffer.\n");
	//	return;
	//}

	auto device = object3dCommon->GetDxCommon()->GetDevice();

	
	vertexResource_ = CreateBufferResource(device.Get(), sizeof(VertexData) * modelData.vertices.size());
	vertexBufferView_.BufferLocation = vertexResource_.Get()->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = static_cast<UINT>(sizeof(VertexData) * modelData.vertices.size());
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	
	VertexData* vertexData = nullptr;
	vertexResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());
	vertexResource_.Get()->Unmap(0, nullptr);


}

void Model::InitializeMaterial()
{
	auto device = modelCommon_->GetDxCommon()->GetDevice();


	materialResource_ = CreateBufferResource(device.Get(), sizeof(Material));


	materialResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));

	materialData_->color = { 1.0f, 1.0f, 1.0f, 1.0f };
	materialData_->enableLighting = false;
	materialData_->uvTransform = myMath->MakeIdentity4x4();

}
