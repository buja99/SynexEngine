#pragma once
#include "MyMath.h"
#include <wrl.h>
#include "ResourceObject.h"
#include "WinApp.h"
#include <Windows.h>
#include "externals/DirectXTex/DirectXTex.h"
#include "Transform.h"
#include "Material.h"
#include "VertexData.h"
#include "DirectionalLight.h"

class SpriteCommon;

using Microsoft::WRL::ComPtr;


class Sprite
{

public:

	void Initialize(SpriteCommon* spriteCommon, std::string textureFilePath);

	void Update();
	void Draw();
	void Cleanup();


	//position getter
	const Vector2& GetPosition() const { return position; }
	//position setter
	void SetPosition(const Vector2& position) { this->position = position; }
	//rotation getter
	float GetRotation() const { return rotation; }
	//rotation setter
	void SetRotation(float rotation) { this->rotation = rotation; }
	//color getter
	const Vector4& GetColor() const { return materialDataSprite->color; }
	//color setter
	void SetColor(const Vector4& color) { materialDataSprite->color = color; }
	//size getter
	const Vector2& GetSize() const { return size; }
	//size setter
	void SetSize(const Vector2& size) { this->size = size; }

private:

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

	//Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	Transform cameraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,-10.0f} };

	Vector2 position = { 0.0f,0.0f };
	float rotation = 0.0f;
	Vector2 size = { 100.0f,100.0f };

	uint32_t textureIndex = 0;


	SpriteCommon* spriteCommon = nullptr;
	
	MyMath* myMath = new MyMath;
};

