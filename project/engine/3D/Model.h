#pragma once
#include "ModelCommon.h"
#include <wrl.h>
#include "VertexData.h"
#include "Material.h"
#include "TextureManager.h"
#include "MyMath.h"
#include "Object3dCommon.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

class Object3dCommon;

struct Keyframe {
	float time;
	Vector3 position;
	Quaternion rotation;
	Vector3 scale;
};

struct AnimationChannel {
	std::string nodeName; // 대상 본 이름
	std::vector<Keyframe> keyframes;
};

struct AnimationData {
	std::string name;
	float duration = 0.0f;
	std::vector<AnimationChannel> channels;
};

struct Node {
	Matrix4x4 localMatrix;
	std::string name;
	std::vector<Node> children;
};

struct ModelData
{
	std::vector<VertexData> vertices;
	MaterialData material;
	Node rootNode;
	std::vector<AnimationData> animations;
};


class Model
{

public:
	void Initialize(ModelCommon* modelCommon, Object3dCommon* object3dCommon ,const std::string& directorypath, const std::string& filename);

	void Draw();

	void Cleanup();

	static ModelData LoadModelFile(const std::string& directoryPath, const std::string& filename);

	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

	ComPtr<ID3D12Resource> CreateBufferResource(ComPtr <ID3D12Device> device, size_t sizeInBytes);

	Material GetMaterialData() const { return *materialData_; }

	static Node ReadNode(aiNode* ainode);

	const ModelData& GetModelData() const { return modelData; }

	void DrawRecursive(const Node& node, const Matrix4x4& parentMatrix, const Matrix4x4& viewProj, TransformationMatrix* transformData);

private:
	ModelCommon* modelCommon_ = nullptr;
	// Obj file
	std::vector<VertexData> vertices_;
	MaterialData material_;
	//
	ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	ComPtr<ID3D12Resource> materialResource_;
	Material* materialData_ = nullptr;


	ModelData modelData;

	void InitializeVertexBuffer();
	void InitializeMaterial();

	Object3dCommon* object3dCommon_ = nullptr;

};

