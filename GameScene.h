#pragma once
#include "DirectXCommon.h"
#include "MyMath.h"
#include "ResourceObject.h"
#include "Vector2.h"
#include "WinApp.h"
#include<Windows.h>
#include <wrl.h>


using Microsoft::WRL::ComPtr;


struct Vector4 final {
	float x;
	float y;
	float z;
	float w;
};

struct Transform
{
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;

};

struct  VertexData
{
	Vector4 position;
	Vector2 texCoord;
	Vector3 normal;
};

struct  Material
{
	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
};

struct TransformationMatrix
{
	Matrix4x4 WVP;
	Matrix4x4 World;
};

struct DirectionalLight
{
	Vector4 color;
	Vector3 direction;
	float intensity;
};
struct MaterialData
{
	std::string textureFilePath;
};
struct ModelData
{
	std::vector<VertexData> vertices;
	MaterialData material;
};

class GameScene
{

public:
	//GameScene() = default;
	//~GameScene() = default;

	void Initialize(DirectXCommon* dxCommon);
	void Update();
	void Draw();
	void Cleanup();



	  

private:

    DirectXCommon* dxCommon_ = nullptr;

	ComPtr<ID3D12Resource> vertexResourceSprite;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
	ComPtr<ID3D12Resource> transformationMatrixResourceSprite;
	TransformationMatrix* transformationMatrixDataSprite = nullptr;
	Transform transformSprite;
	VertexData* vertexDataSprite;
	ComPtr<ID3D12Resource> indexResourceSprite;
	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};
	ComPtr<ID3D12Resource>  materialResourceSprite;
	Material* materialDataSprite;
	uint32_t* indexDataSprite;
	DirectX::ScratchImage mipImages;
	DirectX::ScratchImage mipImages2;
	DirectX::TexMetadata metadata;
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2;
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2;
	ComPtr<ID3D12Resource> textureResource;
	ComPtr<ID3D12Resource> textureResource2;
	ComPtr<ID3D12Resource> materialResource;
	ComPtr<ID3D12Resource> wvpResoure;
	Material* materialDate;
	TransformationMatrix* transformationMatrix;
	Transform uvTransformSprite
	{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};

	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	Transform cameraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,-10.0f} };

	WinApp* winApp = nullptr;
	MyMath* myMath = new MyMath;
};

