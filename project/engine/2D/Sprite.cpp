#include "Sprite.h"
#include "SpriteCommon.h"
#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG
#include "SrvManager.h"
void Sprite::Initialize(SpriteCommon* spriteCommon, std::string textureFilePath)
{
	this->spriteCommon_ = spriteCommon;

	textureIndex = TextureManager::GetInstance()->GetTextureIndexByFilepath(textureFilePath);

	const DirectX::TexMetadata& metadata =
			TextureManager::GetInstance()->GetMetaData(textureFilePath);
	
	textureOriSize = { static_cast<float>(metadata.width),static_cast<float>(metadata.height) };

	vertexResourceSprite = spriteCommon->CreateBufferResource(spriteCommon->GetDevice(), sizeof(VertexData) * 6);
	vertexBufferViewSprite.BufferLocation = vertexResourceSprite.Get()->GetGPUVirtualAddress();
	vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 6;
	vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);
	
	
	vertexResourceSprite.Get()->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));
	vertexDataSprite[0].position = { 0.0f, 360.0f, 0.0f, 1.0f };
	vertexDataSprite[0].texCoord = { 0.0f, 1.0f };
	vertexDataSprite[1].position = { 0.0f, 0.0f, 0.0f, 1.0f };
	vertexDataSprite[1].texCoord = { 0.0f, 0.0f };
	vertexDataSprite[2].position = { 640.0f, 360.0f, 0.0f, 1.0f };
	vertexDataSprite[2].texCoord = { 1.0f, 1.0f };
	vertexDataSprite[3].position = { 640.0f, 0.0f, 0.0f, 1.0f };
	vertexDataSprite[3].texCoord = { 1.0f, 0.0f };
	
	indexResourceSprite = spriteCommon->CreateBufferResource(spriteCommon->GetDevice(), sizeof(uint32_t) * 6);
	indexBufferViewSprite.BufferLocation = indexResourceSprite.Get()->GetGPUVirtualAddress();
	indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6;
	indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;
	
	
	indexResourceSprite.Get()->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
	indexDataSprite[0] = 0; indexDataSprite[1] = 1; indexDataSprite[2] = 2;
	indexDataSprite[3] = 1; indexDataSprite[4] = 3; indexDataSprite[5] = 2;
	
	materialResourceSprite = spriteCommon->CreateBufferResource(spriteCommon->GetDevice(), sizeof(Material));
	materialResourceSprite.Get()->Map(0, nullptr, reinterpret_cast<void**>(&materialDataSprite));
	materialDataSprite->color = { 1.0f,1.0f,1.0f,1.0f };
	materialDataSprite->enableLighting = false;
	
	transformationMatrixResourceSprite = spriteCommon->CreateBufferResource(spriteCommon->GetDevice(), sizeof(TransformationMatrix));
	transformationMatrixResourceSprite.Get()->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite));
	transformationMatrixDataSprite->World = MyMath::MakeIdentity4x4();
	transformationMatrixDataSprite->WVP = MyMath::MakeIdentity4x4();

	transformSprite = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	GPUHandle = TextureManager::GetInstance()->GetSrvHandleGPU(textureFilePath);

	AdjustTextureSize();

}

void Sprite::Update()
{
	float left = 0.0f - anchorPoint.x;
	float right = 1.0f - anchorPoint.x;
	float top = 0.0f - anchorPoint.y;
	float bottom = 1.0f - anchorPoint.y;

	if (isFlipX_) {
		left = -left;
		right = -right;
	}
	if (isFlipY_) {
		top = -top;
		bottom = -bottom;
	}

	
	float tex_left = uvTransformSprite.translate.x / textureOriSize.x;
	float tex_right = (uvTransformSprite.translate.x+ textureSize.x) / textureOriSize.x;
	float tex_top = uvTransformSprite.translate.y / textureOriSize.y;
	float tex_bottom = (uvTransformSprite.translate.y+ textureSize.y) / textureOriSize.y;

	vertexDataSprite[0].position = { left,bottom,0.0f,1.0f };//lower left
	vertexDataSprite[0].texCoord = { tex_left,tex_bottom };
	vertexDataSprite[0].normal = { 0.0f,0.0f, 1.0f };
	
	vertexDataSprite[1].position = { left,top,0.0f,1.0f };//upper left
	vertexDataSprite[1].texCoord = { tex_left,tex_top };
	vertexDataSprite[1].normal = { 0.0f,0.0f, 1.0f };
	
	vertexDataSprite[2].position = { right,bottom,0.0f,1.0f };//lower right
	vertexDataSprite[2].texCoord = { tex_right,tex_bottom };
	vertexDataSprite[2].normal = { 0.0f,0.0f, 1.0f };
	
	vertexDataSprite[3].position = { right,top,0.0f,1.0f };//upper left
	vertexDataSprite[3].texCoord = { tex_right,tex_top };
	vertexDataSprite[3].normal = { 0.0f,0.0f, 1.0f };

	transformSprite.translate = { position.x,position.y,0.0f };
	transformSprite.rotate = { 0.0f,0.0f,rotation };
	transformSprite.scale = { size.x,size.y,1.0f };


	Matrix4x4 uvTransformMatrix = MyMath::MakeScaleMatrix(uvTransformSprite.scale);
	uvTransformMatrix = MyMath::Multiply(uvTransformMatrix, MyMath::MakeRotateZMatrix(uvTransformSprite.rotate.z));
	uvTransformMatrix = MyMath::Multiply(uvTransformMatrix, MyMath::MakeTranslateMatrix(uvTransformSprite.translate));
	materialDataSprite->uvTransform = uvTransformMatrix;


	Matrix4x4 worldMatrixSprite = MyMath::MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
	Matrix4x4 viewMatrixSprite = MyMath::MakeIdentity4x4();
	Matrix4x4 projectionMatrixSprite = MyMath::MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kClientWidth), float(WinApp::kClientHeight), 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrixSprite = MyMath::Multiply(worldMatrixSprite, MyMath::Multiply(viewMatrixSprite, projectionMatrixSprite));
	transformationMatrixDataSprite->WVP = worldViewProjectionMatrixSprite;
	transformationMatrixDataSprite->World = worldViewProjectionMatrixSprite;

#ifdef _DEBUG
	ImGui::Begin("Sprite");
	ImGui::DragFloat("rotate", &rotation, 0.1f);
	ImGui::DragFloat2("position", &position.x, 1.0f);
	ImGui::DragFloat2("anchorPoint", &anchorPoint.x, 0.1f);
	ImGui::DragFloat3("scl", &transformSprite.scale.x, 0.1f);
	ImGui::DragFloat2("size", &size.x, 1.0f);
	ImGui::Checkbox("isFlipX", &isFlipX_);
	ImGui::Checkbox("isFlipY", &isFlipY_);
	ImGui::End();
#endif // _DEBUG


}


void Sprite::ChangeTexture(std::string textureFilePath)
{

	textureIndex = TextureManager::GetInstance()->GetTextureIndexByFilepath(textureFilePath);

}

void Sprite::Draw()
{
	

	//ID3D12DescriptorHeap* descriptorHeaps[] = { spriteCommon->GetSrvDescriptorHeap().Get() };
	//spriteCommon->GetCommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	spriteCommon_->GetCommandList()->SetPipelineState(spriteCommon_->GetGraphicsPipelineState().Get());

	spriteCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	spriteCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);
	spriteCommon_->GetCommandList()->IASetIndexBuffer(&indexBufferViewSprite);
	spriteCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResourceSprite->GetGPUVirtualAddress()); // Material (b0)
	spriteCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, spriteCommon_->GetDirectionLightBuffer()->GetGPUVirtualAddress()); // DirectionLight (b1) 추가!
	spriteCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(2, transformationMatrixResourceSprite->GetGPUVirtualAddress()); // TransformMatrix (b2)
	spriteCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(3, GPUHandle);
	
	//commandList->DrawInstanced(6, 1, 0, 0);
	spriteCommon_->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);

}

void Sprite::Cleanup()
{

	vertexResourceSprite.Reset();
	indexResourceSprite.Reset();
	materialResourceSprite.Reset();
	transformationMatrixResourceSprite.Reset();
	spriteCommon_ = nullptr;
	
	

}

void Sprite::AdjustTextureSize()
{
	//const DirectX::TexMetadata& metadata = TextureManager::GetInstance()->GetMetaData(textureOriSize);
	textureSize.x = (textureOriSize.x);
	textureSize.y = (textureOriSize.y);

	size = textureSize;

}




