#include "Model.h"
#include <fstream>
#include "imgui.h"

void Model::Initialize(ModelCommon* modelCommon, Object3dCommon* object3dCommon, const std::string& directorypath, const std::string& filename)
{

	modelCommon_ = modelCommon;

	this->object3dCommon_ = object3dCommon;

	//modelData = LoadobjFile("resources", "plane.obj");
	
	modelData = LoadModelFile(directorypath, filename);

	InitializeVertexBuffer();

	InitializeIndexBuffer(modelData);

	InitializeMaterial();

	TextureManager::GetInstance()->LoadTexture(modelData.material.textureFilePath);
	modelData.material.textureIndex =
		TextureManager::GetInstance()->GetTextureIndexByFilepath(modelData.material.textureFilePath);

}

void Model::Draw()
{

	// 1) VertexBuffer 설정
	object3dCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_);
	// 2) Texture SRV 설정
	auto textureDescriptorHandle = TextureManager::GetInstance()->GetSrvHandleGPU(modelData.material.textureFilePath);
	object3dCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureDescriptorHandle);

	//object3dCommon_->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);

	// 3) IndexBuffer 유무에 따라 Draw 방식 분기
	if (!modelData.indices.empty() && indexBufferView_.SizeInBytes > 0) {
		object3dCommon_->GetCommandList()->IASetIndexBuffer(&indexBufferView_);
		object3dCommon_->GetCommandList()->DrawIndexedInstanced(
			static_cast<UINT>(modelData.indices.size()), // Index 개수
			1,  // 인스턴스 수
			0,  // startIndexLocation
			0,  // baseVertexLocation
			0   // startInstanceLocation
		);
	} else {
		object3dCommon_->GetCommandList()->DrawInstanced(
			static_cast<UINT>(modelData.vertices.size()), 1, 0, 0);
	}

}

void Model::Cleanup()
{
	if (vertexResource_) {
		vertexResource_.Reset();
	}

	if (indexResource_) {
		indexResource_.Reset();
	}

	if (materialResource_) {
		materialResource_.Reset();
		materialData_ = nullptr;
	}
	ZeroMemory(&vertexBufferView_, sizeof(vertexBufferView_));
	ZeroMemory(&indexBufferView_, sizeof(indexBufferView_));
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

	for (uint32_t m = 0; m < scene->mNumMeshes; ++m) {
		aiMesh* mesh = scene->mMeshes[m];

		uint32_t baseVertex = static_cast<uint32_t>(modelData.vertices.size());

		// 정점 추가
		for (uint32_t v = 0; v < mesh->mNumVertices; ++v) {
			aiVector3D pos = mesh->mVertices[v];
			aiVector3D norm = mesh->HasNormals() ? mesh->mNormals[v] : aiVector3D(0, 1, 0);
			aiVector3D tex = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][v] : aiVector3D(0, 0, 0);

			VertexData out{};
			out.position = { pos.x, pos.y, pos.z, 1.0f };
			out.normal = { norm.x, norm.y, norm.z };
			out.texCoord = { tex.x, tex.y };

			modelData.vertices.push_back(out);
		}

		SubMesh sub{};
		sub.indexStart = static_cast<uint32_t>(modelData.indices.size());
		sub.materialIndex = 0;

		// 인덱스 추가 (baseVertex 더해주기)
		for (uint32_t f = 0; f < mesh->mNumFaces; ++f) {
			const aiFace& face = mesh->mFaces[f];
			assert(face.mNumIndices == 3);
			modelData.indices.push_back(baseVertex + face.mIndices[0]);
			modelData.indices.push_back(baseVertex + face.mIndices[1]);
			modelData.indices.push_back(baseVertex + face.mIndices[2]);
		}

		sub.indexCount = static_cast<uint32_t>(modelData.indices.size()) - sub.indexStart;
		modelData.subMeshes.push_back(sub);

		// 머티리얼 처리
		if (scene->HasMaterials()) {
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
				aiString path;
				material->GetTexture(aiTextureType_DIFFUSE, 0, &path);
				modelData.material.textureFilePath = directoryPath + "/" + std::string(path.C_Str());
			}
		}
	}

	modelData.rootNode = ReadNode(scene->mRootNode);

	for (uint32_t m = 0; m < scene->mNumMeshes; ++m) {
		aiMesh* mesh = scene->mMeshes[m];
		for (uint32_t b = 0; b < mesh->mNumBones; ++b) {
			aiBone* aibone = mesh->mBones[b];
			std::string boneName = aibone->mName.C_Str();

			aiMatrix4x4 offset = aibone->mOffsetMatrix; // inverse bind
			offset.Transpose();
			Matrix4x4 converted = MyMath::ConvertMatrix(offset);

			/*auto it = skeleton.boneIndexMap.find(boneName);
			if (it != skeleton.boneIndexMap.end()) {
				skeleton.bones[it->second].offsetMatrix = converted;
			}*/

		}
	}

	// 애니메이션 로딩 추가 
	if (scene->HasAnimations()) {
		for (uint32_t i = 0; i < scene->mNumAnimations; ++i) {
			aiAnimation* aiAnim = scene->mAnimations[i];

			AnimationData animData;
			animData.name = aiAnim->mName.C_Str();
			double tps = (aiAnim->mTicksPerSecond != 0.0) ? aiAnim->mTicksPerSecond : 60.0;
			animData.duration = static_cast<float>(aiAnim->mDuration / tps);

			for (uint32_t j = 0; j < aiAnim->mNumChannels; ++j) {
				aiNodeAnim* channel = aiAnim->mChannels[j];

				AnimationChannel animChannel;
				animChannel.nodeName = channel->mNodeName.C_Str();

				// --- 위치 키 (translation) ---
				for (uint32_t k = 0; k < channel->mNumPositionKeys; ++k) {
					KeyframeVector3 key;
					key.time = static_cast<float>(channel->mPositionKeys[k].mTime / tps);
					aiVector3D p = channel->mPositionKeys[k].mValue;
					key.value = { p.x, p.y, p.z };
					animChannel.translate.keyframes.push_back(key);
				}

				// --- 회전 키 (rotation) ---
				for (uint32_t k = 0; k < channel->mNumRotationKeys; ++k) {
					KeyframeQuaternion key;
					key.time = static_cast<float>(channel->mRotationKeys[k].mTime / tps);
					aiQuaternion r = channel->mRotationKeys[k].mValue;
					key.value = { r.x, r.y, r.z, r.w };
					animChannel.rotate.keyframes.push_back(key);
				}

				// --- 스케일 키 (scale) ---
				for (uint32_t k = 0; k < channel->mNumScalingKeys; ++k) {
					KeyframeVector3 key;
					key.time = static_cast<float>(channel->mScalingKeys[k].mTime / tps);
					aiVector3D s = channel->mScalingKeys[k].mValue;
					key.value = { s.x, s.y, s.z };
					animChannel.scale.keyframes.push_back(key);
				}

				animData.channels.push_back(animChannel);
			}

			modelData.animations.push_back(animData);
		}
	}
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
	Node node;

	node.name = ainode->mName.C_Str();

	// 로컬 행렬
	aiMatrix4x4 aiLocal = ainode->mTransformation;
	aiLocal.Transpose();
	node.localMatrix = MyMath::ConvertMatrix(aiLocal);

	// ★ 이 노드가 참조하는 mesh 인덱스들 저장
	node.meshIndices.resize(ainode->mNumMeshes);
	for (uint32_t i = 0; i < ainode->mNumMeshes; ++i) {
		node.meshIndices[i] = ainode->mMeshes[i]; // aiMesh の index
	}

	// 자식 노드 재귀적으로 생성
	node.children.resize(ainode->mNumChildren);
	for (uint32_t i = 0; i < ainode->mNumChildren; ++i) {
		node.children[i] = ReadNode(ainode->mChildren[i]);
	}

	return node;
}

void Model::DrawRecursive(
	const Node& node,
	const Matrix4x4& parentMatrix,
	const Matrix4x4& viewProj,
	TransformationMatrix* transformData) {

#ifdef _DEBUG
	if (node.name == "Root" || node.name == "Armature" || node.name == "mixamorig:Hips") {
		Vector3 s, r, t;
		MyMath::DecomposeMatrix(node.localMatrix, s, r, t);

		char buf[256];
		sprintf_s(
			buf,
			"Node %s  S(%.3f, %.3f, %.3f)  T(%.3f, %.3f, %.3f)  R(%.3f, %.3f, %.3f)\n",
			node.name.c_str(),
			s.x, s.y, s.z,
			t.x, t.y, t.z,
			r.x, r.y, r.z
		);
		OutputDebugStringA(buf);
	}
#endif

	// ─────────────────────────────
	// 1. 루트 노드 판정
	// ─────────────────────────────
	// modelData.rootNode 를 그대로 넘기고 있으니까
	// 주소 비교로 "지금 재귀가 루트냐?"를 알 수 있어
	const bool isRoot = (&node == &modelData.rootNode);

	// 루트는 localMatrix 를 무시하고, 자식부터 local 을 사용하도록 함
	Matrix4x4 local = node.localMatrix;
	if (isRoot) {
		local = MyMath::MakeIdentity4x4();
	}

	// 부모(World) * 로컬 = 현재 노드의 World
	Matrix4x4 currentMatrix = MyMath::Multiply(local, parentMatrix);

	// ─────────────────────────────
	// 2. VS용 Transform CB 갱신
	// ─────────────────────────────
	transformData->World = currentMatrix;
	transformData->WVP = MyMath::Multiply(currentMatrix, viewProj);
	transformData->WorldInverseTranspose =
		MyMath::Transpose(MyMath::Inverse(currentMatrix));

	// ─────────────────────────────
	// 3. 공통 세팅 (VB / 텍스처)
	// ─────────────────────────────
	auto commandList = object3dCommon_->GetCommandList();

	// 정점 버퍼는 한 번은 반드시 세팅
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);

	// 머티리얼 텍스처
	auto texHandle =
		TextureManager::GetInstance()->GetSrvHandleGPU(
			modelData.material.textureFilePath);
	commandList->SetGraphicsRootDescriptorTable(2, texHandle);

	// ─────────────────────────────
	// 4. 인덱스가 있는 경우: 서브메시 단위로 드로우
	// ─────────────────────────────
	if (!modelData.indices.empty() && indexBufferView_.SizeInBytes > 0) {
		commandList->IASetIndexBuffer(&indexBufferView_);

		for (uint32_t meshIndex : node.meshIndices) {
			if (meshIndex >= modelData.subMeshes.size()) {
				// 잘못된 인덱스 방어
				continue;
			}

			const SubMesh& sub = modelData.subMeshes[meshIndex];
			if (sub.indexCount == 0) {
				continue;
			}

			commandList->DrawIndexedInstanced(
				sub.indexCount,   // index 개수
				1,                // 인스턴스 수
				sub.indexStart,   // startIndexLocation
				0,                // baseVertexLocation (모든 버텍스 하나로 묶였다고 가정)
				0);               // startInstanceLocation
		}
	}
	// ─────────────────────────────
	// 5. 인덱스가 전혀 없는 모델일 때의 fallback
	//    (이 경우는 보통 전체 메쉬를 한 번만 그리면 되니까 루트에서만 호출)
	// ─────────────────────────────
	else {
		if (isRoot) {
			commandList->DrawInstanced(
				static_cast<UINT>(modelData.vertices.size()),
				1, 0, 0);
		}
	}

	// ─────────────────────────────
	// 6. 자식 노드 재귀
	// ─────────────────────────────
	for (const Node& child : node.children) {
		DrawRecursive(child, currentMatrix, viewProj, transformData);
	}
}



void Model::InitializeIndexBuffer(const ModelData& modelData) {
	auto device = modelCommon_->GetDxCommon()->GetDevice();

	// 인덱스가 하나도 없으면 아무 것도 하지 않음
	if (modelData.indices.empty()) {
		indexResource_.Reset();
		ZeroMemory(&indexBufferView_, sizeof(indexBufferView_));
		return;
	}

	const UINT indexCount = static_cast<UINT>(modelData.indices.size());
	const UINT bufferSize = sizeof(uint32_t) * indexCount;

	// 업로드 버퍼 생성 (VertexBuffer 만들 때 쓰는 CreateBufferResource 함수 재사용)
	indexResource_ = CreateBufferResource(
		device.Get(),
		bufferSize);

	// 데이터 복사
	uint32_t* mapped = nullptr;
	HRESULT hr = indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&mapped));
	assert(SUCCEEDED(hr));
	memcpy(mapped, modelData.indices.data(), bufferSize);
	indexResource_->Unmap(0, nullptr);

	// IndexBufferView 설정
	indexBufferView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = bufferSize;
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
}


void Model::InitializeVertexBuffer()
{
	auto device = modelCommon_->GetDxCommon()->GetDevice();

	// 정점이 하나도 없으면 만들지 않음
	if (modelData.vertices.empty()) {
		vertexResource_.Reset();
		ZeroMemory(&vertexBufferView_, sizeof(vertexBufferView_));
		return;
	}

	const size_t vertexCount = modelData.vertices.size();
	const UINT   bufferSize = static_cast<UINT>(sizeof(VertexData) * vertexCount);

	// 업로드 버퍼 생성
	vertexResource_ = CreateBufferResource(
		device.Get(),
		bufferSize);

	// View 설정
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = bufferSize;
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

	// 데이터 복사
	VertexData* mapped = nullptr;
	HRESULT hr = vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&mapped));
	assert(SUCCEEDED(hr));
	memcpy(mapped, modelData.vertices.data(), bufferSize);
	vertexResource_->Unmap(0, nullptr);


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
