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

	GPUHandle = TextureManager::GetInstance()->GetSrvHandleGPU(textureFilePath);

	textureOriSize.x = static_cast<float>(metadata.width);
	textureOriSize.y = static_cast<float>(metadata.height);

	CreateVertexData();

	CreateIndexData();

	CreateMaterialData();

	CreateTransformationMatrixData();

	AdjustTextureSize();

}

void Sprite::Update()
{
	float left = -anchorPoint.x * size.x;
	float right = (1.0f - anchorPoint.x) * size.x;
	float top = -anchorPoint.y * size.y;
	float bottom = (1.0f - anchorPoint.y) * size.y;

	if (isFlipX_) std::swap(left, right);
	if (isFlipY_) std::swap(top, bottom);

	float tex_left = uvTransformSprite.translate.x / textureOriSize.x;
	float tex_right = (uvTransformSprite.translate.x+ textureSize.x) / textureOriSize.x;
	float tex_top = uvTransformSprite.translate.y / textureOriSize.y;
	float tex_bottom = (uvTransformSprite.translate.y+ textureSize.y) / textureOriSize.y;

	vertexDataSprite[0] = { { left,  bottom, 0.0f, 1.0f }, { tex_left,  tex_bottom }, {0, 0, 1} }; 
	vertexDataSprite[1] = { { left,  top,    0.0f, 1.0f }, { tex_left,  tex_top },    {0, 0, 1} }; 
	vertexDataSprite[2] = { { right, bottom, 0.0f, 1.0f }, { tex_right, tex_bottom }, {0, 0, 1} }; 
	vertexDataSprite[3] = { { right, top,    0.0f, 1.0f }, { tex_right, tex_top },    {0, 0, 1} }; 

	indexDataSprite[0] = 0; indexDataSprite[1] = 1; indexDataSprite[2] = 2;
	indexDataSprite[3] = 1; indexDataSprite[4] = 3; indexDataSprite[5] = 2;

	transformSprite.translate = { position.x,position.y,0.1f };
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
	GPUHandle = TextureManager::GetInstance()->GetSrvHandleGPU(textureFilePath);
}

void Sprite::Draw()
{
	

	ID3D12DescriptorHeap* descriptorHeaps[] = { SrvManager::GetInstance()->GetSrvDescriptorHeap() };
	spriteCommon_->GetCommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
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

void Sprite::CreateVertexData() {
	auto device = spriteCommon_->GetDevice();
	vertexResourceSprite = spriteCommon_->CreateBufferResource(device,sizeof(VertexData) * 4);
	vertexBufferViewSprite = {
		vertexResourceSprite->GetGPUVirtualAddress(),
		UINT(sizeof(VertexData) * 4),
		UINT(sizeof(VertexData))
	};
	vertexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));
}

void Sprite::CreateIndexData() {
	auto device = spriteCommon_->GetDevice();
	indexResourceSprite = spriteCommon_->CreateBufferResource(device, sizeof(uint32_t) * 6);
	indexBufferViewSprite = {
		indexResourceSprite->GetGPUVirtualAddress(),
		UINT(sizeof(uint32_t) * 6),
		DXGI_FORMAT_R32_UINT
	};
	indexResourceSprite->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
}

void Sprite::CreateMaterialData() {
	auto device = spriteCommon_->GetDevice();
	materialResourceSprite = spriteCommon_->CreateBufferResource(device,sizeof(Material));

	materialResourceSprite.Get()->Map(0, nullptr, reinterpret_cast<void**>(&materialDataSprite));

	materialDataSprite->color = { 1.0f,1.0f,1.0f,1.0f };

	materialDataSprite->enableLighting = false;

	materialDataSprite->uvTransform = MyMath::MakeIdentity4x4();
}

void Sprite::CreateTransformationMatrixData() {
	auto device = spriteCommon_->GetDevice();
	transformationMatrixResourceSprite = this->spriteCommon_->CreateBufferResource(device,sizeof(TransformationMatrix));

	// データを書き込むためのアドレスを取得
	transformationMatrixResourceSprite.Get()->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite));

	//　単位行列を書き込んでおく
	transformationMatrixDataSprite->WVP = MyMath::MakeIdentity4x4();
	transformationMatrixDataSprite->World = MyMath::MakeIdentity4x4();
}

void Sprite::AdjustTextureSize()
{
	//const DirectX::TexMetadata& metadata = TextureManager::GetInstance()->GetMetaData(textureOriSize);
	textureSize.x = (textureOriSize.x);
	textureSize.y = (textureOriSize.y);

	size = textureSize;

}




