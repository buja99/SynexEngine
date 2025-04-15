#include "SpriteCommon.h"
#include "Logger.h"
#include "StringUtility.h"
#include <d3dx12.h>

using namespace Logger;
using namespace StringUtility;

void SpriteCommon::Initialize(DirectXCommon* dxCommon)
{
	dxCommon_ = dxCommon;

	device = dxCommon_->GetDevice();
	commandList = dxCommon_->GetCommandList();
	
	directionLightBuffer_ = CreateBufferResource(device, sizeof(DirectionalLight));

	DirectionalLight* directionLightData = nullptr;
	directionLightBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&directionLightData));

	directionLightData->color = { 1.0f, 1.0f, 1.0f, 1.0f };      // 흰색 조명
	directionLightData->direction = { 0.0f, -1.0f, 0.0f };      // 아래 방향으로 빛
	directionLightData->intensity = 1.0f;                     // 빛 강도 1.0

	directionLightBuffer_->Unmap(0, nullptr); // 필수!

	CreateGraphicsPipeline();
}

void SpriteCommon::Finalize() {

	directionLightBuffer_.Reset();

	if (rootSignature) {
		rootSignature.Reset();
	}

	// 그래픽 파이프라인 상태 해제
	if (graphicsPipelineState) {
		graphicsPipelineState.Reset();
	}

	// DirectXCommon이 관리하는 CommandList는 여기서 해제하지 않음
	commandList = nullptr;

	// Device 해제 (DirectXCommon이 소유하는 경우 여기서 해제하지 않음)
	device = nullptr;
}


void SpriteCommon::CommonDrawSettings()
{

	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());

	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	
}


IDxcBlob* SpriteCommon::CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler)
{
	//hlsl
	Log(ConvertString(std::format(L"Begin CompileShader,path:{},profile:{}\n", filePath, profile)));

	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);

	assert(SUCCEEDED(hr));

	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8;

	//Compile
	LPCWSTR arguments[] = {
		filePath.c_str(), //Compiler対象のhlalファイル名
		L"-E",L"main",
		L"-T",profile, // ShderProfileの設定
		L"-Zi",L"-Qembed_debug",
		L"-Od",
		L"-Zpr"
	};
	//Shader Compile
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer,         //読み込んだfile
		arguments,					 //Compile option
		_countof(arguments),		 //Compile optionの数
		includeHandler,				 //includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult)	 //Compile結果
	);
	assert(SUCCEEDED(hr));
	//警告・エラー
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Log(shaderError->GetStringPointer());
		assert(false);
	}
	//
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	Log(ConvertString(std::format(L"Compile Succeeded,path:{},profile:{}\n", filePath, profile)));
	shaderSource->Release();
	shaderResult->Release();
	return shaderBlob;
}

//D3D12_CPU_DESCRIPTOR_HANDLE SpriteCommon::GetSRVCPUDescriptorHandle(uint32_t index) const
//{
//	return GetCPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, index);
//}
//
//D3D12_GPU_DESCRIPTOR_HANDLE SpriteCommon::GetSRVGPUDescriptorHandle(uint32_t index) const
//{
//	return GetGPUDescriptorHandle(srvDescriptorHeap, descriptorSizeSRV, index);
//}

DirectX::ScratchImage SpriteCommon::LoadTexture(const std::string& filePath)
{
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);

	assert(SUCCEEDED(hr));

	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));


	return mipImages;
}

ComPtr<ID3D12Resource> SpriteCommon::CreateTextureResource(ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metadata)
{
	// metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);
	resourceDesc.Height = UINT(metadata.height);
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize);
	resourceDesc.Format = metadata.format;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION(metadata.dimension);
	// Heap設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;
	// Resource生成
	ComPtr<ID3D12Resource> resource;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,
		D3D12_HEAP_FLAG_NONE,
		&resourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&resource));
	assert(SUCCEEDED(hr));
	return resource;
}

void SpriteCommon::UploadTextureDate(ComPtr<ID3D12Resource>& texture, const DirectX::ScratchImage& mipImages)
{
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	//全MipMap
	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel)
	{
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);

		HRESULT hr = texture.Get()->WriteToSubresource(
			UINT(mipLevel),
			nullptr,
			img->pixels,
			UINT(img->rowPitch),
			UINT(img->slicePitch)
		);
		assert(SUCCEEDED(hr));
	}

}

ComPtr<ID3D12DescriptorHeap> SpriteCommon::CreateDescriptorHeap(ComPtr<ID3D12Device> device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible)
{
	ComPtr <ID3D12DescriptorHeap> descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;
	descriptorHeapDesc.NumDescriptors = numDescriptors;
	descriptorHeapDesc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	assert(SUCCEEDED(hr));
	descriptorHeap->SetName(L"SpriteDSVHeap");
	return descriptorHeap;
}

ComPtr<ID3D12Resource> SpriteCommon::CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeInBytes)
{
	//頂点Heap
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD;
	//頂点Resource
	D3D12_RESOURCE_DESC vertexResourceDesc{};
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeInBytes;

	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;

	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	ComPtr<ID3D12Resource> vertexResource;
	HRESULT hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertexResource));
	assert(SUCCEEDED(hr));

	return vertexResource;
}

D3D12_CPU_DESCRIPTOR_HANDLE SpriteCommon::GetCPUDescriptorHandle(const ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
	D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
	handleCPU.ptr += (descriptorSize * index);
	return handleCPU;
}

D3D12_GPU_DESCRIPTOR_HANDLE SpriteCommon::GetGPUDescriptorHandle(const ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index)
{
	D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
	handleGPU.ptr += (descriptorSize * index);
	return handleGPU;
}

void SpriteCommon::CreateRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE texRange;
	texRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0); // t0 Texture

	CD3DX12_ROOT_PARAMETER rootParams[4];

	rootParams[0].InitAsConstantBufferView(0); // b0 (Material)
	rootParams[1].InitAsConstantBufferView(1); // b1 (DirectionLight)
	rootParams[2].InitAsConstantBufferView(2); // b2 (TransformationMatrix 추가!)
	rootParams[3].InitAsDescriptorTable(1, &texRange); // t0 (Texture)

	CD3DX12_STATIC_SAMPLER_DESC samplerDesc(0);

	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc;
	rootSigDesc.Init(
		_countof(rootParams), rootParams,
		1, &samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
	);

	ComPtr<ID3DBlob> signatureBlob;
	ComPtr<ID3DBlob> errorBlob;

	HRESULT hr = D3D12SerializeRootSignature(
		&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		&signatureBlob, &errorBlob
	);

	if (FAILED(hr)) {
		if (errorBlob) OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		assert(false);
	}

	hr = device->CreateRootSignature(
		0,
		signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature)
	);

	assert(SUCCEEDED(hr));
	
}

void SpriteCommon::CreateGraphicsPipeline()
{

	HRESULT hr;
	ComPtr<IDxcUtils> dxcUtils;
	ComPtr<IDxcCompiler3> dxcCompiler;
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(hr));

	ComPtr<IDxcIncludeHandler> includeHandler;
	hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	assert(SUCCEEDED(hr));

	ComPtr<IDxcBlob> vsBlob = CompileShader(L"resources/shaders/Sprite.VS.hlsl", L"vs_6_0", dxcUtils.Get(), dxcCompiler.Get(), includeHandler.Get());
	ComPtr<IDxcBlob> psBlob = CompileShader(L"resources/shaders/Sprite.PS.hlsl", L"ps_6_0", dxcUtils.Get(), dxcCompiler.Get(), includeHandler.Get());

	D3D12_INPUT_ELEMENT_DESC inputElements[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
	inputLayoutDesc = { inputElements, _countof(inputElements) };

	D3D12_BLEND_DESC blendDesc = {};
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	D3D12_RASTERIZER_DESC rasterDesc = {};
	rasterDesc.FillMode = D3D12_FILL_MODE_SOLID;
	rasterDesc.CullMode = D3D12_CULL_MODE_NONE;

	CreateRootSignature();

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = inputLayoutDesc;
	psoDesc.pRootSignature = rootSignature.Get();
	psoDesc.VS = { vsBlob->GetBufferPointer(), vsBlob->GetBufferSize() };
	psoDesc.PS = { psBlob->GetBufferPointer(), psBlob->GetBufferSize() };
	psoDesc.BlendState = blendDesc;
	psoDesc.RasterizerState = rasterDesc;
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	psoDesc.SampleDesc.Count = 1;

	hr = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));
}
