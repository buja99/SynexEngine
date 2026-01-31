#include "Object3dCommon.h"
#include "Logger.h"
#include "StringUtility.h"
#include <d3dx12.h>
#include "VertexData.h"
#include <cstddef>
using namespace Logger;
using namespace StringUtility;


void Object3dCommon::Initialize(DirectXCommon* dxCommon)
{

	dxCommon_ = dxCommon;

	device = dxCommon_->GetDevice();
	commandList = dxCommon_->GetCommandList();

	CreateGraphicsPipeline();
	
	SimpleVertex quadVertices[] = {
		{{-1.0f, -1.0f, 0.5f}},
		{{-1.0f,  1.0f, 0.5f}},
		{{ 1.0f, -1.0f, 0.5f}},
		{{ 1.0f, -1.0f, 0.5f}},
		{{-1.0f,  1.0f, 0.5f}},
		{{ 1.0f,  1.0f, 0.5f}},
	};

	
}

void Object3dCommon::Finalize() {
	stencilMaskPipelineState_.Reset();
	stencilMaskRootSignature_.Reset();
	stencilTestPipelineState_.Reset();
	rootSignature.Reset();
	graphicsPipelineState.Reset();
	stencilMaskVertexBuffer_.Reset();
	commandList.Reset();
	device.Reset();

	dxCommon_ = nullptr;
	defaultCamera = nullptr;
}

void Object3dCommon::CommonDrawSettings()
{

	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());

	dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState.Get());
	
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

void Object3dCommon::stencilMaskSettings() {
	auto commandList = dxCommon_->GetCommandList();

	// 마스크를 위한 파이프라인 상태 설정 (쓰기 모드)
	commandList->SetGraphicsRootSignature(stencilMaskRootSignature_.Get());
	commandList->SetPipelineState(stencilMaskPipelineState_.Get());
	commandList->OMSetStencilRef(1);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 정점 버퍼 설정 및 사각형 출력
	commandList->IASetVertexBuffers(0, 1, &stencilMaskVBView_);
	commandList->DrawInstanced(6, 1, 0, 0); // 사각형 마스크 그리기
}

void Object3dCommon::SetStencilTestDrawSettings() {
	auto commandList = dxCommon_->GetCommandList();
	commandList->SetGraphicsRootSignature(rootSignature.Get());
	commandList->SetPipelineState(stencilTestPipelineState_.Get());
	commandList->OMSetStencilRef(1);
	//commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}


void Object3dCommon::SetStencilQuadArea(float left, float right, float top, float bottom, float z) {
	SimpleVertex quadVertices[] = {
		{{left,  bottom, z}},
		{{left,  top,    z}},
		{{right, bottom, z}},
		{{right, bottom, z}},
		{{left,  top,    z}},
		{{right, top,    z}},
	};

	size_t vbSize = sizeof(quadVertices);
	void* mapped = nullptr;
	stencilMaskVertexBuffer_->Map(0, nullptr, &mapped);
	memcpy(mapped, quadVertices, vbSize);
	stencilMaskVertexBuffer_->Unmap(0, nullptr);
}

void Object3dCommon::SetStencilWriteMaskValue(uint8_t value) {
	auto commandList = dxCommon_->GetCommandList();
	commandList->OMSetStencilRef(value);
}



void Object3dCommon::SetStencilWritePipeline() {
	commandList->SetGraphicsRootSignature(stencilMaskRootSignature_.Get());
	commandList->SetPipelineState(stencilMaskPipelineState_.Get());
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}



IDxcBlob* Object3dCommon::CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler)
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

void Object3dCommon::CreateRootSignature()
{

	HRESULT hr;

	//DescriptorRange
	D3D12_DESCRIPTOR_RANGE descriptorRange[2] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	//cubemap用
	descriptorRange[1].BaseShaderRegister = 1; // t1
	descriptorRange[1].NumDescriptors = 1;
	descriptorRange[1].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[1].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	//複数設定できるので配列。今回は結果１つだけなので長さ1の配列
	D3D12_ROOT_PARAMETER rootParameters[12] = {};
	// b0: Material Constant Buffer
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //PixelShaderで使う
	rootParameters[0].Descriptor.ShaderRegister = 0; //レジスタ番号0とバインド

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //CBVを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; //VertexShaderで使う
	rootParameters[1].Descriptor.ShaderRegister = 0; //レジスタ番号0とバインド

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //PixelShaderで使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = &descriptorRange[0]; //Tableの中身の配列の指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges =1; //Tableで利用する数

	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //CBVを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //PixelShaderで使う
	rootParameters[3].Descriptor.ShaderRegister = 1; //レジスタ番号1

	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // Pixel Shader
	rootParameters[4].Descriptor.ShaderRegister = 2; //レジスタ番号2

	rootParameters[5].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[5].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[5].Descriptor.ShaderRegister = 3;

	rootParameters[6].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[6].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[6].Descriptor.ShaderRegister = 4;

	rootParameters[7].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[7].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[7].Descriptor.ShaderRegister = 5;

	rootParameters[8].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[8].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[8].Descriptor.ShaderRegister = 6;

	rootParameters[9].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[9].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[9].Descriptor.ShaderRegister = 7;

	rootParameters[10].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[10].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameters[10].DescriptorTable.pDescriptorRanges = &descriptorRange[1];
	rootParameters[10].DescriptorTable.NumDescriptorRanges = 1;

	rootParameters[11].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[11].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	rootParameters[11].Descriptor.ShaderRegister = 8;
	rootParameters[11].Descriptor.RegisterSpace = 0;

	descriptionRootSignature.pParameters = rootParameters; //rootParameters配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters); //配列の長さ

	//Sampler
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers[0].ShaderRegister = 0;
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);

	//シリアライズしてバイナリする
	ComPtr<ID3DBlob> signatureBlob = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	if (FAILED(hr)) {
		Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}
	//バイナリを元に生成

	hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));

	rootSignature->SetName(L"Object3DCommonRootSignature");

}

void Object3dCommon::CreateGraphicsPipeline()
{

	HRESULT hr;
	//DXC Compiler 初期化
	IDxcUtils* dxcUtils = nullptr;
	IDxcCompiler3* dxcCompiler = nullptr;
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(hr));
	IDxcIncludeHandler* includeHandler = nullptr;
	hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	assert(SUCCEEDED(hr));

	//Shader Compile
	ComPtr<IDxcBlob> vertexShaderBlob = CompileShader(L"resources/shaders/Object3D.VS.hlsl",
		L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(vertexShaderBlob != nullptr);

	ComPtr<IDxcBlob> pixelShaderBlob = CompileShader(L"resources/shaders/Object3D.PS.hlsl",
		L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(pixelShaderBlob != nullptr);

	ComPtr<IDxcBlob> SkinShaderBlob = CompileShader(L"resources/shaders/Object3D_Skin.VS.hlsl",
		L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(SkinShaderBlob != nullptr);


	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[5] = {};

	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].InputSlot = 0;
	inputElementDescs[0].AlignedByteOffset = (UINT)offsetof(VertexData, position);
	inputElementDescs[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	inputElementDescs[0].InstanceDataStepRate = 0;

	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].InputSlot = 0;
	inputElementDescs[1].AlignedByteOffset = (UINT)offsetof(VertexData, texCoord);
	inputElementDescs[1].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	inputElementDescs[1].InstanceDataStepRate = 0;

	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].InputSlot = 0;
	inputElementDescs[2].AlignedByteOffset = (UINT)offsetof(VertexData, normal);
	inputElementDescs[2].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	inputElementDescs[2].InstanceDataStepRate = 0;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	//skin
	D3D12_INPUT_ELEMENT_DESC skinInputElementDescs[5] = {};

	skinInputElementDescs[0].SemanticName = "POSITION";
	skinInputElementDescs[0].SemanticIndex = 0;
	skinInputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	skinInputElementDescs[0].InputSlot = 0;
	skinInputElementDescs[0].AlignedByteOffset = (UINT)offsetof(VertexData, position);
	skinInputElementDescs[0].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	skinInputElementDescs[0].InstanceDataStepRate = 0;

	skinInputElementDescs[1].SemanticName = "TEXCOORD";
	skinInputElementDescs[1].SemanticIndex = 0;
	skinInputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	skinInputElementDescs[1].InputSlot = 0;
	skinInputElementDescs[1].AlignedByteOffset = (UINT)offsetof(VertexData, texCoord);
	skinInputElementDescs[1].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	skinInputElementDescs[1].InstanceDataStepRate = 0;

	skinInputElementDescs[2].SemanticName = "NORMAL";
	skinInputElementDescs[2].SemanticIndex = 0;
	skinInputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	skinInputElementDescs[2].InputSlot = 0;
	skinInputElementDescs[2].AlignedByteOffset = (UINT)offsetof(VertexData, normal);
	skinInputElementDescs[2].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	skinInputElementDescs[2].InstanceDataStepRate = 0;

	skinInputElementDescs[3].SemanticName = "BLENDINDICES";
	skinInputElementDescs[3].SemanticIndex = 0;
	skinInputElementDescs[3].Format = DXGI_FORMAT_R32G32B32A32_UINT;
	skinInputElementDescs[3].InputSlot = 0;
	skinInputElementDescs[3].AlignedByteOffset = (UINT)offsetof(VertexData, boneIndex);
	skinInputElementDescs[3].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	skinInputElementDescs[3].InstanceDataStepRate = 0;

	skinInputElementDescs[4].SemanticName = "BLENDWEIGHT";
	skinInputElementDescs[4].SemanticIndex = 0;
	skinInputElementDescs[4].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	skinInputElementDescs[4].InputSlot = 0;
	skinInputElementDescs[4].AlignedByteOffset = (UINT)offsetof(VertexData, boneWeight);
	skinInputElementDescs[4].InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
	skinInputElementDescs[4].InstanceDataStepRate = 0;

	D3D12_INPUT_LAYOUT_DESC skinInputLayoutDesc{};
	skinInputLayoutDesc.pInputElementDescs = skinInputElementDescs;
	skinInputLayoutDesc.NumElements = _countof(skinInputElementDescs);


	//BlendState
	D3D12_BLEND_DESC blendDesc{};
	blendDesc.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;

	//RasterizerState
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	//rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	CreateRootSignature();

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };
	graphicsPipelineStateDesc.BlendState = blendDesc;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;

	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));

}

void Object3dCommon::CreateStencilWritePipeline() {
	HRESULT hr;

	// DXC 컴파일러 초기화
	ComPtr<IDxcUtils> dxcUtils;
	ComPtr<IDxcCompiler3> dxcCompiler;
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(hr));
	ComPtr<IDxcIncludeHandler> includeHandler;
	dxcUtils->CreateDefaultIncludeHandler(&includeHandler);

	// 간단한 사각형용 셰이더 컴파일 (Color만 출력하는 단순 셰이더)
	auto vs = CompileShader(L"resources/shaders/StencilMask.VS.hlsl", L"vs_6_0", dxcUtils.Get(), dxcCompiler.Get(), includeHandler.Get());
	auto ps = CompileShader(L"resources/shaders/StencilMask.PS.hlsl", L"ps_6_0", dxcUtils.Get(), dxcCompiler.Get(), includeHandler.Get());

	// 루트 시그니처 (입력 없음)
	CD3DX12_ROOT_SIGNATURE_DESC rsDesc;
	rsDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> sigBlob, errBlob;
	hr = D3D12SerializeRootSignature(&rsDesc, D3D_ROOT_SIGNATURE_VERSION_1, &sigBlob, &errBlob);
	assert(SUCCEEDED(hr));
	hr = device->CreateRootSignature(0, sigBlob->GetBufferPointer(), sigBlob->GetBufferSize(), IID_PPV_ARGS(&stencilMaskRootSignature_));
	assert(SUCCEEDED(hr));

	// PSO 설정
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.pRootSignature = stencilMaskRootSignature_.Get();
	psoDesc.VS = { vs->GetBufferPointer(), vs->GetBufferSize() };
	psoDesc.PS = { ps->GetBufferPointer(), ps->GetBufferSize() };
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	D3D12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;  
	psoDesc.RasterizerState = rasterizerDesc;
	psoDesc.DepthStencilState = {};
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = TRUE;
	psoDesc.DepthStencilState.StencilWriteMask = 0xFF;

	psoDesc.DepthStencilState.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	psoDesc.DepthStencilState.FrontFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
	psoDesc.DepthStencilState.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	psoDesc.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;

	psoDesc.DepthStencilState.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	psoDesc.DepthStencilState.BackFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
	psoDesc.DepthStencilState.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	psoDesc.DepthStencilState.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;

	psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	D3D12_INPUT_ELEMENT_DESC quadInputLayout[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
	psoDesc.InputLayout = { quadInputLayout, _countof(quadInputLayout) };
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	psoDesc.SampleDesc.Count = 1;

	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	hr = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&stencilMaskPipelineState_));
	assert(SUCCEEDED(hr));
}

void Object3dCommon::CreateStencilTestPipeline() {
	HRESULT hr;

	// DXC 초기화
	ComPtr<IDxcUtils> dxcUtils;
	ComPtr<IDxcCompiler3> dxcCompiler;
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
	assert(SUCCEEDED(hr));
	ComPtr<IDxcIncludeHandler> includeHandler;
	hr = dxcUtils->CreateDefaultIncludeHandler(&includeHandler);
	assert(SUCCEEDED(hr));

	// Object3d용 셰이더 컴파일
	auto vs = CompileShader(L"resources/shaders/Object3D.VS.hlsl", L"vs_6_0", dxcUtils.Get(), dxcCompiler.Get(), includeHandler.Get());
	auto ps = CompileShader(L"resources/shaders/Object3D.PS.hlsl", L"ps_6_0", dxcUtils.Get(), dxcCompiler.Get(), includeHandler.Get());

	// Input Layout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, (UINT)offsetof(VertexData, position), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,        0, (UINT)offsetof(VertexData, texCoord), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,     0, (UINT)offsetof(VertexData, normal), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	D3D12_INPUT_ELEMENT_DESC skinInputElementDescs[5] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, (UINT)offsetof(VertexData, position), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,        0, (UINT)offsetof(VertexData, texCoord), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,     0, (UINT)offsetof(VertexData, normal), D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICES", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0,(UINT)offsetof(VertexData, boneIndex),D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,(UINT)offsetof(VertexData, boneWeight),D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	// Blend / Rasterizer
	D3D12_BLEND_DESC blendDesc = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	D3D12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	// Depth-Stencil 설정 (💡 핵심!)
	D3D12_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = TRUE;
	dsDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	dsDesc.StencilEnable = TRUE;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0x00; // 쓰지 않음
	dsDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;
	dsDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;

	dsDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;
	dsDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	dsDesc.BackFace = dsDesc.FrontFace;

	// PSO 생성
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = rootSignature.Get();
	psoDesc.InputLayout = inputLayoutDesc;
	psoDesc.VS = { vs->GetBufferPointer(), vs->GetBufferSize() };
	psoDesc.PS = { ps->GetBufferPointer(), ps->GetBufferSize() };
	psoDesc.BlendState = blendDesc;
	psoDesc.RasterizerState = rasterizerDesc;
	psoDesc.DepthStencilState = dsDesc;
	psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	hr = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&stencilTestPipelineState_));
	assert(SUCCEEDED(hr));
}
