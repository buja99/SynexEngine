#include "Model.h"
#include <fstream>


void Model::Initialize(ModelCommon* modelCommon, Object3dCommon* object3dCommon, const std::string& directorypath, const std::string& filename)
{

	modelCommon_ = modelCommon;

	this->object3dCommon_ = object3dCommon;

	//modelData = LoadobjFile("resources", "plane.obj");
	
	modelData = LoadModelFile(directorypath, filename);

	InitializeVertexBuffer();

	InitializeMaterial();

	TextureManager::GetInstance()->LoadTexture(modelData.material.textureFilePath);
	modelData.material.textureIndex =
		TextureManager::GetInstance()->GetTextureIndexByFilepath(modelData.material.textureFilePath);

}

void Model::Draw()
{


	object3dCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);

	auto textureDescriptorHandle = TextureManager::GetInstance()->GetSrvHandleGPU(modelData.material.textureFilePath);
	object3dCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureDescriptorHandle);

	object3dCommon_->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);

}

void Model::Cleanup()
{
	if (vertexResource_) {
		vertexResource_.Reset();
	}

	if (materialResource_) {
		materialResource_.Reset();
		materialData_ = nullptr;
	}

	object3dCommon_ = nullptr;
	modelCommon_ = nullptr;
	
}

ModelData Model::LoadModelFile(const std::string& directoryPath, const std::string& filename)
{
	Assimp::Importer importer;

	std::string filePath = directoryPath + "/" + filename;
	const aiScene* scene = importer.ReadFile(filePath,
		aiProcess_Triangulate |
		aiProcess_FlipUVs |
		aiProcess_FlipWindingOrder |
		aiProcess_ConvertToLeftHanded);

	assert(scene && scene->HasMeshes());

	ModelData modelData;

	aiMesh* mesh = scene->mMeshes[0]; // 첫 번째 메시만 로딩
	for (uint32_t i = 0; i < mesh->mNumFaces; ++i) {
		aiFace face = mesh->mFaces[i];
		assert(face.mNumIndices == 3);

		for (uint32_t j = 0; j < 3; ++j) {
			uint32_t index = face.mIndices[j];
			aiVector3D pos = mesh->mVertices[index];
			aiVector3D norm = mesh->mNormals[index];
			aiVector3D tex = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][index] : aiVector3D();

			VertexData vtx;
			vtx.position = { pos.x, pos.y, pos.z, 1.0f }; // 좌우반전
			vtx.normal = { norm.x, norm.y, norm.z };     // 좌우반전
			vtx.texCoord = { tex.x, tex.y };

			modelData.vertices.push_back(vtx);
		}
	}

	// 머티리얼 처리
	if (scene->HasMaterials()) {
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
			aiString path;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &path);
			modelData.material.textureFilePath = directoryPath + "/" + std::string(path.C_Str());
		}
	}

	modelData.rootNode = ReadNode(scene->mRootNode);

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

Node Model::ReadNode(aiNode* ainode) {
	Node result;

	// Assimp의 행렬은 row-major → DirectX에 맞게 전치
	aiMatrix4x4 aiLocal = ainode->mTransformation;
	aiLocal.Transpose();

	result.localMatrix = MyMath::ConvertMatrix(aiLocal);
	result.name = ainode->mName.C_Str();

	result.children.resize(ainode->mNumChildren);
	for (uint32_t i = 0; i < ainode->mNumChildren; ++i) {
		result.children[i] = ReadNode(ainode->mChildren[i]);
	}

	return result;
}

void Model::DrawRecursive(const Node& node, const Matrix4x4& parentMatrix, const Matrix4x4& viewProj, TransformationMatrix* transformData) {
	Matrix4x4 currentMatrix = MyMath::Multiply(node.localMatrix, parentMatrix);

	transformData->World = currentMatrix;
	transformData->WVP = MyMath::Multiply(currentMatrix, viewProj);
	transformData->WorldInverseTranspose = MyMath::Transpose(MyMath::Inverse(currentMatrix));

	object3dCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	auto textureDescriptorHandle = TextureManager::GetInstance()->GetSrvHandleGPU(modelData.material.textureFilePath);
	object3dCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureDescriptorHandle);
	object3dCommon_->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);

	for (const Node& child : node.children) {
		DrawRecursive(child, currentMatrix, viewProj, transformData); // 재귀 호출 시 transformData도 그대로 전달
	}
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

	auto device = modelCommon_->GetDxCommon()->GetDevice();

	
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
	materialData_->uvTransform = MyMath::MakeIdentity4x4();
	materialData_->shininess = 32.0f;                   
	materialData_->isBlinnPhong = 0;
	materialData_->usePointLight = 0;
	materialData_->useDirectionalLight = 1;
	materialData_->useSpotLight = 0;
	materialData_->useAmbientLight = 0;
	materialData_->useAreaLight = 0;
}
