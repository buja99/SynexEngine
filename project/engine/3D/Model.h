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

struct KeyframeVector3 { Vector3 value;    float time; };
struct KeyframeQuaternion { Quaternion value; float time; };

struct AnimationCurveVector3 { std::vector<KeyframeVector3>    keyframes; };
struct AnimationCurveQuaternion { std::vector<KeyframeQuaternion> keyframes; };

//struct Skeleton;

struct AnimationChannel {
	std::string nodeName;
	AnimationCurveVector3    translate;
	AnimationCurveQuaternion rotate;
	AnimationCurveVector3    scale;
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
	std::vector<uint32_t> meshIndices;
};

struct SubMesh {
	uint32_t indexStart = 0;   // modelData.indices 기준 시작 위치
	uint32_t indexCount = 0;   // 그릴 인덱스 개수
	uint32_t materialIndex = 0; // 지금은 0 고정(머티리얼 1개만 사용)
};

struct ModelData
{
	std::vector<VertexData> vertices;
	std::vector<uint32_t> indices;
	MaterialData material;
	Node rootNode;
	std::vector<AnimationData> animations;
	std::vector<SubMesh>    subMeshes;
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

	
	void InitializeIndexBuffer(const ModelData& modelData);

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

	ComPtr<ID3D12Resource>      indexResource_;    
	D3D12_INDEX_BUFFER_VIEW     indexBufferView_{};

};

