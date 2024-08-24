#include "GameScene.h"




void GameScene::Initialize(DirectXCommon* dxCommon)
{

	dxCommon_ = dxCommon;

	mipImages = dxCommon_->LoadTexture("resources/uvChecker.png");
	metadata = mipImages.GetMetadata();
	textureResource = dxCommon_->CreateTextureResource(dxCommon_->GetDevice(), metadata);
	dxCommon_->UploadTextureDate(textureResource, mipImages);

	mipImages2 = dxCommon_->LoadTexture("resources/monsterBall.png");
	const DirectX::TexMetadata& metadata2 = mipImages2.GetMetadata();
	textureResource2 = dxCommon_->CreateTextureResource(dxCommon_->GetDevice(), metadata2);
	dxCommon_->UploadTextureDate(textureResource2, mipImages2);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = UINT(metadata.mipLevels);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
	srvDesc2.Format = metadata2.format;
	srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc2.Texture2D.MipLevels = UINT(metadata2.mipLevels);

	textureSrvHandleCPU = dxCommon_->GetSrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
	textureSrvHandleGPU = dxCommon_->GetSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();

	textureSrvHandleCPU2 = dxCommon_->GetCPUDescriptorHandle(dxCommon_->GetSrvDescriptorHeap(), dxCommon_->GetDescriptorSizeSRV(), 2);
	textureSrvHandleGPU2 = dxCommon_->GetGPUDescriptorHandle(dxCommon_->GetSrvDescriptorHeap(), dxCommon_->GetDescriptorSizeSRV(), 2);

	textureSrvHandleCPU.ptr += dxCommon_ ->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureSrvHandleGPU.ptr += dxCommon_->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	dxCommon_->GetDevice()->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);

	dxCommon_->GetDevice()->CreateShaderResourceView(textureResource2.Get(), &srvDesc2, textureSrvHandleCPU2);

	//Material Resource
	materialResource = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(Material));
	
	materialResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&materialDate));
	materialDate->color = { 1.0f,1.0f,1.0f,1.0f };
	materialDate->enableLighting = true;
	
	//TransformationMatrix Resource
    wvpResoure = dxCommon_ ->CreateBufferResource(dxCommon_->GetDevice(), sizeof(TransformationMatrix));
	
	wvpResoure.Get()->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrix));
	transformationMatrix->WVP = myMath->MakeIdentity4x4();
	transformationMatrix->World = myMath->MakeIdentity4x4();


    vertexResourceSprite = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(VertexData) * 6);
    vertexBufferViewSprite.BufferLocation = vertexResourceSprite.Get()->GetGPUVirtualAddress();
	vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 6;
	vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);

	
	vertexResourceSprite.Get()->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));

	vertexDataSprite[0].position = { 0.0f,360.0f,0.0f,1.0f };//lower left
	vertexDataSprite[0].texCoord = { 0.0f,1.0f };
	vertexDataSprite[0].normal = { 0.0f,0.0f, 1.0f };
	vertexDataSprite[1].position = { 0.0f,0.0f,0.0f,1.0f };//upper left
	vertexDataSprite[1].texCoord = { 0.0f,0.0f };
	vertexDataSprite[1].normal = { 0.0f,0.0f, 1.0f };
	vertexDataSprite[2].position = { 640.0f,360.0f,0.0f,1.0f };//lower right
	vertexDataSprite[2].texCoord = { 1.0f,1.0f };
	vertexDataSprite[2].normal = { 0.0f,0.0f, 1.0f };
	vertexDataSprite[3].position = { 640.0f,0.0f,0.0f,1.0f };//upper left
	vertexDataSprite[3].texCoord = { 1.0f,0.0f };
	vertexDataSprite[3].normal = { 0.0f,0.0f, 1.0f };

	indexResourceSprite = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(uint32_t) * 6);

	indexBufferViewSprite.BufferLocation = indexResourceSprite.Get()->GetGPUVirtualAddress();
	indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6;
	indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;


	indexResourceSprite.Get()->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));

	indexDataSprite[0] = 0; indexDataSprite[1] = 1; indexDataSprite[2] = 2;
	indexDataSprite[3] = 1; indexDataSprite[4] = 3; indexDataSprite[5] = 2;

	materialResourceSprite = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(Material));

	materialResourceSprite.Get()->Map(0, nullptr, reinterpret_cast<void**>(&materialDataSprite));

	materialDataSprite->color = { 1.0f,1.0f,1.0f,1.0f };
	materialDataSprite->enableLighting = false;

	materialDate->uvTransform = myMath->MakeIdentity4x4();
	materialDataSprite->uvTransform = myMath->MakeIdentity4x4();

	transformationMatrixResourceSprite = dxCommon_->CreateBufferResource(dxCommon_->GetDevice(), sizeof(TransformationMatrix));
	transformationMatrixResourceSprite.Get()->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite));
	transformationMatrixDataSprite->World = myMath->MakeIdentity4x4();
	transformationMatrixDataSprite->WVP = myMath->MakeIdentity4x4();

	transformSprite = { {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

}

void GameScene::Update()
{

	Matrix4x4 worldMatrix = myMath->MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 cameraMatrix = myMath->MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
	Matrix4x4 viewMatrix = myMath->Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = myMath->MakePerspectiveFovMatrix(0.45f, float(WinApp::kClientWidth) / float(WinApp::kClientHeight), 0.1f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = myMath->Multiply(worldMatrix, myMath->Multiply(viewMatrix, projectionMatrix));
	transformationMatrix->WVP = worldViewProjectionMatrix;
	transformationMatrix->World = worldViewProjectionMatrix;


	Matrix4x4 uvTransformMatrix = myMath->MakeScaleMatrix(uvTransformSprite.scale);
	uvTransformMatrix = myMath->Multiply(uvTransformMatrix, myMath->MakeRotateZMatrix(uvTransformSprite.rotate.z));
	uvTransformMatrix = myMath->Multiply(uvTransformMatrix, myMath->MakeTranslateMatrix(uvTransformSprite.translate));
	materialDataSprite->uvTransform = uvTransformMatrix;


	Matrix4x4 worldMatrixSprite = myMath->MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
	Matrix4x4 viewMatrixSprite = myMath->MakeIdentity4x4();
	Matrix4x4 projectionMatrixSprite = myMath->MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kClientWidth), float(WinApp::kClientHeight), 0.0f, 100.0f);
	Matrix4x4 worldViewProjectionMatrixSprite = myMath->Multiply(worldMatrixSprite, myMath->Multiply(viewMatrixSprite, projectionMatrixSprite));
	transformationMatrixDataSprite->WVP = worldViewProjectionMatrixSprite;
	transformationMatrixDataSprite->World = worldViewProjectionMatrixSprite;


}

void GameScene::Draw()
{
	dxCommon_->GetCommandList()->SetPipelineState(dxCommon_->GetGraphicsPipelineState().Get());

	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);
	dxCommon_->GetCommandList()->IASetIndexBuffer(&indexBufferViewSprite);
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResourceSprite.Get()->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite.Get()->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);
	//commandList->DrawInstanced(6, 1, 0, 0);
	dxCommon_->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);

}
