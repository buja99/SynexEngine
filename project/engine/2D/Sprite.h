#pragma once
#include "MyMath.h"
#include <wrl.h>
#include "WinApp.h"
#include <Windows.h>
#include "externals/DirectXTex/DirectXTex.h"
#include "Transform.h"
#include "Material.h"
#include "VertexData.h"
#include "DirectionalLight.h"
#include "TextureManager.h"

class SpriteCommon;

using Microsoft::WRL::ComPtr;


class Sprite
{

public:

	void Initialize(SpriteCommon* spriteCommon, std::string textureFilePath);

	void Update();
	void ChangeTexture(std::string textureFilePath);
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

	const Vector2& GetAncHorPoint() const { return anchorPoint; }
	void SetAnchorPoint(const Vector2& anchorPont) { this->anchorPoint = anchorPont; }

	bool GetIsFlipX() const { return isFlipX_; }
	void SetIsFlipX(bool isFlipX) { isFlipX_ = isFlipX; }

	bool GetIsFlipY() const { return isFlipY_; }
	void SetIsFlipY(bool isFlipY) { isFlipY_ = isFlipY; }

	const Vector2& GetTextureLeftTop() const { return textureLeftTop; }
	void SetTextureLeftTop(const Vector2& textureLeftTop) { this->textureLeftTop = textureLeftTop; }

	const Vector2& GetTextureSize() const { return textureSize; }
	void SetTextureSize(const Vector2& textureSize) { this->textureSize = textureSize; }

	//std::unordered_map<uint32_t, std::string> textureIndexToFilePathMap;
private:


	uint32_t textureIndex = 0;
	ComPtr<ID3D12Resource> vertexResourceSprite;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
	ComPtr<ID3D12Resource> transformationMatrixResourceSprite;
	TransformationMatrix* transformationMatrixDataSprite = nullptr;
	Transform transformSprite;
	VertexData* vertexDataSprite;
	ComPtr<ID3D12Resource> indexResourceSprite;
	D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};
	ComPtr<ID3D12Resource> materialResourceSprite;
	Material* materialDataSprite;
	uint32_t* indexDataSprite;

	DirectX::ScratchImage mipImages;
	//DirectX::ScratchImage mipImages2;

	Transform uvTransformSprite
	{
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};

	//Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,1.0f} };
	Transform cameraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,-10.0f} };

	Vector2 position = { 0.0f,0.0f };
	float rotation = 0.0f;
	Vector2 size = { 100.0f,100.0f };

	bool isFlipX_ = false;
	bool isFlipY_ = false;

	Vector2 anchorPoint = { 0.0f,0.0f };

	Vector2 textureLeftTop = { 0.0f,0.0f };
	Vector2 textureSize = { 65.0f,65.0f };

	Vector2 textureOriSize = { 100.0f,100.0f };

	void AdjustTextureSize();

	SpriteCommon* spriteCommon_ = nullptr;
	
	D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle;

	MyMath* myMath = new MyMath;
};

