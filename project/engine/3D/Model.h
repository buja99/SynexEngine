#pragma once
#include "ModelCommon.h"
#include <wrl.h>
#include "VertexData.h"
#include "Material.h"
#include "TextureManager.h"
#include "MyMath.h"
#include "Object3dCommon.h"

class Object3dCommon;

struct ModelData
{
	std::vector<VertexData> vertices;
	MaterialData material;
};

class Model
{

public:
	void Initialize(ModelCommon* modelCommon, Object3dCommon* object3dCommon ,const std::string& directorypath, const std::string& filename);

	void Draw();

	void Cleanup();

	static ModelData LoadobjFile(const std::string& directoryPath, const std::string& filename);

	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

	ComPtr<ID3D12Resource> CreateBufferResource(ComPtr <ID3D12Device> device, size_t sizeInBytes);

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

	MyMath* myMath = new MyMath;

};

