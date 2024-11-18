#include "Sprite.h"
#include "SpriteCommon.h"


void Sprite::Initialize(SpriteCommon* spriteCommon, std::string textureFilePath)
{
	this->spriteCommon = spriteCommon;

	

	//textureIndex = TextureManager::GetInstance()->GetOrLoadTextureIndex(textureFilePath);



	//mipImages = spriteCommon->LoadTexture("resources/uvChecker.png");
	//metadata = mipImages.GetMetadata();
	//textureResource = spriteCommon->CreateTextureResource(spriteCommon->GetDevice(), metadata);
	//spriteCommon->UploadTextureDate(textureResource, mipImages);
	//
	//mipImages2 = spriteCommon->LoadTexture("resources/monsterBall.png");
	//const DirectX::TexMetadata& metadata2 = mipImages2.GetMetadata();
	//textureResource2 = spriteCommon->CreateTextureResource(spriteCommon->GetDevice(), metadata2);
	//spriteCommon->UploadTextureDate(textureResource2, mipImages2);
	//
	//D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	//srvDesc.Format = metadata.format;
	//srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	//srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);
	//
	//D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
	//srvDesc2.Format = metadata2.format;
	//srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	//srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	//srvDesc2.Texture2D.MipLevels = UINT(metadata2.mipLevels);
	//
	//textureSrvHandleCPU = spriteCommon->GetSrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	//textureSrvHandleGPU = spriteCommon->GetSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
	//
	//textureSrvHandleCPU2 = spriteCommon->GetCPUDescriptorHandle(spriteCommon->GetSrvDescriptorHeap(), spriteCommon->GetDescriptorSizeSRV(), 2);
	//textureSrvHandleGPU2 = spriteCommon->GetGPUDescriptorHandle(spriteCommon->GetSrvDescriptorHeap(), spriteCommon->GetDescriptorSizeSRV(), 2);
	//
	//textureSrvHandleCPU.ptr += spriteCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//textureSrvHandleGPU.ptr += spriteCommon->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//
	//spriteCommon->GetDevice()->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);
	//
	//spriteCommon->GetDevice()->CreateShaderResourceView(textureResource2.Get(), &srvDesc2, textureSrvHandleCPU2);
	//
	////Material Resource
	//materialResource = spriteCommon->CreateBufferResource(spriteCommon->GetDevice(), sizeof(Material));
	//
	//materialResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&materialDate));
	//materialDate->color = { 1.0f,1.0f,1.0f,1.0f };
	//materialDate->enableLighting = true;
	//
	////TransformationMatrix Resource
	//wvpResoure = spriteCommon->CreateBufferResource(spriteCommon->GetDevice(), sizeof(TransformationMatrix));
	//
	//wvpResoure.Get()->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrix));
	//transformationMatrix->WVP = myMath->MakeIdentity4x4();
	//transformationMatrix->World = myMath->MakeIdentity4x4();


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
	
	//materialDate->uvTransform = myMath->MakeIdentity4x4();
	//materialDataSprite->uvTransform = myMath->MakeIdentity4x4();
	
	transformationMatrixResourceSprite = spriteCommon->CreateBufferResource(spriteCommon->GetDevice(), sizeof(TransformationMatrix));
	transformationMatrixResourceSprite.Get()->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite));
	transformationMatrixDataSprite->World = myMath->MakeIdentity4x4();
	transformationMatrixDataSprite->WVP = myMath->MakeIdentity4x4();

	transformSprite = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };


}

void Sprite::Update()
{
	//vertexDataSprite[0].position = { 0.0f,1.0f,0.0f,1.0f };//lower left
	//vertexDataSprite[0].texCoord = { 0.0f,1.0f };
	//vertexDataSprite[0].normal = { 0.0f,0.0f, 1.0f };
	//
	//vertexDataSprite[1].position = { 0.0f,0.0f,0.0f,1.0f };//upper left
	//vertexDataSprite[1].texCoord = { 0.0f,0.0f };
	//vertexDataSprite[1].normal = { 0.0f,0.0f, 1.0f };
	//
	//vertexDataSprite[2].position = { 1.0f,1.0f,0.0f,1.0f };//lower right
	//vertexDataSprite[2].texCoord = { 1.0f,1.0f };
	//vertexDataSprite[2].normal = { 0.0f,0.0f, 1.0f };
	//
	//vertexDataSprite[3].position = { 1.0f,0.0f,0.0f,1.0f };//upper left
	//vertexDataSprite[3].texCoord = { 1.0f,0.0f };
	//vertexDataSprite[3].normal = { 0.0f,0.0f, 1.0f };

	transformSprite.translate = { position.x,position.y,0.0f };
	transformSprite.rotate = { 0.0f,0.0f,rotation };
	transformSprite.scale = { size.x,size.y,1.0f };


	//Matrix4x4 uvTransformMatrix = myMath->MakeScaleMatrix(uvTransformSprite.scale);
	//uvTransformMatrix = myMath->Multiply(uvTransformMatrix, myMath->MakeRotateZMatrix(uvTransformSprite.rotate.z));
	//uvTransformMatrix = myMath->Multiply(uvTransformMatrix, myMath->MakeTranslateMatrix(uvTransformSprite.translate));
	//materialDataSprite->uvTransform = uvTransformMatrix;


	Matrix4x4 worldMatrixSprite = myMath->MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
	Matrix4x4 viewMatrixSprite = myMath->MakeIdentity4x4();
	Matrix4x4 projectionMatrixSprite = myMath->MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kClientWidth), float(WinApp::kClientHeight), 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrixSprite = myMath->Multiply(worldMatrixSprite, myMath->Multiply(viewMatrixSprite, projectionMatrixSprite));
	transformationMatrixDataSprite->WVP = worldViewProjectionMatrixSprite;
	transformationMatrixDataSprite->World = worldViewProjectionMatrixSprite;

	

}


void Sprite::ChangeTexture(std::string textureFilePath)
{

	textureIndex = TextureManager::GetInstance()->GetTextureIndexByFilepath(textureFilePath);

}

void Sprite::Draw()
{

	//ID3D12DescriptorHeap* descriptorHeaps[] = { spriteCommon->GetSrvDescriptorHeap().Get() };
	//spriteCommon->GetCommandList()->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	spriteCommon->GetCommandList()->SetPipelineState(spriteCommon->GetGraphicsPipelineState().Get());

	spriteCommon->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	spriteCommon->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);
	spriteCommon->GetCommandList()->IASetIndexBuffer(&indexBufferViewSprite);
	spriteCommon->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResourceSprite.Get()->GetGPUVirtualAddress());
	spriteCommon->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite.Get()->GetGPUVirtualAddress());
	spriteCommon->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(textureIndex));
	//commandList->DrawInstanced(6, 1, 0, 0);
	spriteCommon->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);

}

void Sprite::Cleanup()
{

	if (vertexResourceSprite) {
		vertexResourceSprite->Release();
		vertexResourceSprite = nullptr;
	}
	if (transformationMatrixResourceSprite) {
		transformationMatrixResourceSprite->Release();
		transformationMatrixResourceSprite = nullptr;
	}
	if (indexResourceSprite) {
		indexResourceSprite->Release();
		indexResourceSprite = nullptr;
	}
	if (materialResourceSprite) {
		materialResourceSprite->Release();
		materialResourceSprite = nullptr;
	}
	//if (textureResource) {
	//	textureResource->Release();
	//	textureResource = nullptr;
	//}
	//if (textureResource2) {
	//	textureResource2->Release();
	//	textureResource2 = nullptr;
	//}
	//if (materialResource) {
	//	materialResource->Release();
	//	materialResource = nullptr;
	//}
	//if (wvpResoure) {
	//	wvpResoure->Release();
	//	wvpResoure = nullptr;
	//}


	//if (mipImages) {
   //	 mipImages->Release();
   //	 mipImages = nullptr;
	//}
	delete myMath;
	myMath = nullptr;

}




