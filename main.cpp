#include<Windows.h>
#include<cstdint>
#include<string>
#include<format>
#include<d3d12.h>
#include<dxgi1_6.h>
#include<cassert>
#include<dxgidebug.h>
#include <dxcapi.h>
#include"Math.h"
#include "Vector2.h"
#include <cmath>
#include"Matrix3x3.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_dx12.h"
#include "externals/imgui/imgui_impl_win32.h"
#include "externals/DirectXTex/DirectXTex.h"
#include<wrl.h>
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#include <fstream>
#include <sstream>
#include "ResourceObject.h"
#include "Input.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Logger.h"
#include "StringUtility.h"


#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")
#pragma comment(lib,"dxcompiler.lib")

using Microsoft::WRL::ComPtr;

using namespace Logger;
using namespace StringUtility;


Math* math = new Math();

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

struct D3DResourceLeakChecker {
	~D3DResourceLeakChecker() {

		ComPtr<IDXGIDebug1> debug;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug)))) {
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
			debug->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
		}
	}
};

Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
Transform cameraTransform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f} ,{0.0f,0.0f,-10.0f} };

Transform uvTransformSprite
{
	{1.0f,1.0f,1.0f},
	{0.0f,0.0f,0.0f},
	{0.0f,0.0f,0.0f}
};



float length(Vector3 distance) {
	return sqrtf(powf(distance.x, 2.0f) + powf(distance.y, 2.0f) + powf(distance.z, 2.0f));
}
Vector3 normalize(Vector3 distance) {
	return { distance.x / length(distance),distance.y / length(distance),distance.z / length(distance) };
}



//CompileShader関数
IDxcBlob* CompileShader(

	const std::wstring& filePath,

	const wchar_t* profile,

	IDxcUtils* dxcUtils,
	IDxcCompiler3* dxcCompiler,
	IDxcIncludeHandler* includeHandler)
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


ResourceObject CreateBufferResource(ComPtr <ID3D12Device> device, size_t sizeInBytes) {

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

	ID3D12Resource* vertexResource = nullptr;
	HRESULT hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertexResource));
	assert(SUCCEEDED(hr));

	return vertexResource;
}




DirectX::ScratchImage LoadTexture(const std::string& filePath)
{
	DirectX::ScratchImage image{};
	std::wstring filePathW = ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	//HRESULT hr = DirectX::LoadFromWICFile(L"resources/uvChecker.png", DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);

	assert(SUCCEEDED(hr));

	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImages);
	assert(SUCCEEDED(hr));


	return mipImages;
}

ResourceObject CreateTextureResource(ComPtr<ID3D12Device> device, const DirectX::TexMetadata& metadata)
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
	ID3D12Resource* resource = nullptr;
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
void UploadTextureDate(ResourceObject& texture, const DirectX::ScratchImage& mipImages)
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


MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	MaterialData materialData;
	std::string line;
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		if (identifier == "map_Kd")
		{
			std::string textureFilename;
			s >> textureFilename;

			materialData.textureFilePath = directoryPath + "/" + textureFilename;

		}


	}

	return materialData;

}

//obj
ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename)
{
	//Declaring variables
	ModelData modelData;
	std::vector<Vector4> positions;
	std::vector<Vector3> normals;
	std::vector<Vector2> texCoords;
	std::string line;
	//Open the file
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());
	//Read the file and build ModelData
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;
		if (identifier == "v") {
			Vector4 position;
			s >> position.x >> position.y >> position.z;
			position.w = 1.0f;
			position.x *= -1.0f;
			//position.z *= -1.0f;
			positions.push_back(position);
		} else if (identifier == "vt") {
			Vector2 texCoord;
			s >> texCoord.x >> texCoord.y;
			texCoord.y = -1.0f - texCoord.y;
			texCoords.push_back(texCoord);
		} else if (identifier == "vn") {
			Vector3 normal;
			s >> normal.x >> normal.y >> normal.z;
			normal.x *= -1.0f;
			//normal.z *= -1.0f;
			normals.push_back(normal);
		} else if (identifier == "f") {
			VertexData triangle[3];
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;

				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element)
				{
					std::string index;
					std::getline(v, index, '/');
					elementIndices[element] = std::stoi(index);
				}
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texCoord = texCoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				//VertexData vertex = { position,texCoord,normal };
				//modelDate.vertices.push_back(vertex);
				triangle[faceVertex] = { position, texCoord, normal };
			}
			modelData.vertices.push_back(triangle[2]);
            modelData.vertices.push_back(triangle[1]);
            modelData.vertices.push_back(triangle[0]);
		} else if (identifier == "mtllib") {
			std::string materialFilename;
			s >> materialFilename;

			modelData.material = LoadMaterialTemplateFile(directoryPath, materialFilename);
		}
	}

	//Return ModelData

	return modelData;

}



int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	D3DResourceLeakChecker leakCheck;
	
	//COMの初期化
	//CoInitialize(NULL);

	WinApp* winApp = new WinApp();
	DirectXCommon* dxCommon = new DirectXCommon();
	Input* input = new Input();

	winApp->Initialize();
	dxCommon->Initialize(winApp);
	input->Initialize(winApp);


	//HRESULT hr = winApp->GetHRESULT();
	//assert(SUCCEEDED(hr));



	



//#ifdef _DEBUG
//	ID3D12Debug1* debugController = nullptr;
//	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
//		debugController->EnableDebugLayer();
//		debugController->SetEnableGPUBasedValidation(TRUE);
//	}
//#endif // _DEBUG

	//ShowWindow(hwnd, SW_SHOW);



	OutputDebugStringA("Hello,DirectX!\n");

	// //DXGIFactoryの生成
	//ComPtr<IDXGIFactory7> dxgiFactory = nullptr;
	//
	//hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory));
	//SUCCEEDEDマクロで判定
	//assert(SUCCEEDED(hr));

	//使用するAdapterに最初 nullptrを入れる
	//ComPtr<IDXGIAdapter4> useAdapter = nullptr;


	//for (UINT i = 0; dxgiFactory->EnumAdapterByGpuPreference(i,
	//	DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) !=
	//	DXGI_ERROR_NOT_FOUND; ++i) {
	//	//Adapterの情報を取得する
	//	DXGI_ADAPTER_DESC3 adapterDesc{};
	//	hr = useAdapter->GetDesc3(&adapterDesc);
	//	assert(SUCCEEDED(hr));
	//	if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
	//		//wstringでAdapterの情報Logで出力
	//		Log(ConvertString(std::format(L"Use Adapter:{}\n", adapterDesc.Description)));
	//		break;
	//
	//	}
	//	useAdapter = nullptr;
	//}

	


//#ifdef _DEBUG

	//ComPtr<ID3D12InfoQueue> infoQueue = nullptr;
	//if (SUCCEEDED(device->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
	//	//ヤバイエラー
	//	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
	//	//エラー
	//	infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
	//	//警告
	//	//infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
	//	//抑制するメッセージのID
	//	D3D12_MESSAGE_ID denyIds[] = {
	//	D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
	//	};
	//
	//	//抑制するレベル
	//	D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
	//	D3D12_INFO_QUEUE_FILTER filter{};
	//	filter.DenyList.NumIDs = _countof(denyIds);
	//	filter.DenyList.pIDList = denyIds;
	//	filter.DenyList.NumSeverities = _countof(severities);
	//	filter.DenyList.pSeverityList = severities;
	//	//指定したメッセージの表示を抑制する
	//	infoQueue->PushStorageFilter(&filter);
	//}
//#endif // _DEBUG




	
	////Textureを読んで転送する
	//DirectX::ScratchImage mipImages = LoadTexture("resources/uvChecker.png");
	//const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	//ResourceObject textureResource = CreateTextureResource(device, metadata);
	//UploadTextureDate(textureResource, mipImages);
	//
	//DirectX::ScratchImage mipImages2 = LoadTexture("resources/monsterBall.png");
	//const DirectX::TexMetadata& metadata2 = mipImages2.GetMetadata();
	//ResourceObject textureResource2 = CreateTextureResource(device, metadata2);
	//UploadTextureDate(textureResource2, mipImages2);
	//
	//
	////metadataを基にSRVの設定
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
	

	//SRVを作成するDescriptorHeapの場所を決める
	//D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU = srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	//D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU = srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	//
	//D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU2 = GetCPUDescriptorHandle(srvDescriptorHeap.Get(), descriptorSizeSRV, 2);
	//D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU2 = GetGPUDescriptorHandle(srvDescriptorHeap.Get(), descriptorSizeSRV, 2);
	//
	//textureSrvHandleCPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//textureSrvHandleGPU.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//
	//device->CreateShaderResourceView(textureResource.Get(), &srvDesc, textureSrvHandleCPU);
	//
	//device->CreateShaderResourceView(textureResource2.Get(), &srvDesc2, textureSrvHandleCPU2);
	//
	//
	//
	////DescriptorRange
	//D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	//descriptorRange[0].BaseShaderRegister = 0;
	//descriptorRange[0].NumDescriptors = 1;
	//descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	//descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	//
	////RootSignature作成
	//D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	//descriptionRootSignature.Flags =
	//	D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	////複数設定できるので配列。今回は結果１つだけなので長さ1の配列
	//D3D12_ROOT_PARAMETER rootParameters[4] = {};
	//rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //CBVを使う
	//rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //PixelShaderで使う
	//rootParameters[0].Descriptor.ShaderRegister = 0; //レジスタ番号0とバインド
	//rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //CBVを使う
	//rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; //VertexShaderで使う
	//rootParameters[1].Descriptor.ShaderRegister = 0; //レジスタ番号0とバインド
	//rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	//rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //PixelShaderで使う
	//rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange; //Tableの中身の配列の指定
	//rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange); //Tableで利用する数
	//rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //CBVを使う
	//rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //PixelShaderで使う
	//rootParameters[3].Descriptor.ShaderRegister = 1; //レジスタ番号1
	//descriptionRootSignature.pParameters = rootParameters; //rootParameters配列へのポインタ
	//descriptionRootSignature.NumParameters = _countof(rootParameters); //配列の長さ
	//
	////Sampler
	//D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	//staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	//staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	//staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	//staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	//staticSamplers[0].ShaderRegister = 0;
	//staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	//descriptionRootSignature.pStaticSamplers = staticSamplers;
	//descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);
	//
	////シリアライズしてバイナリする
	//ComPtr<ID3DBlob> signatureBlob = nullptr;
	//ComPtr<ID3DBlob> errorBlob = nullptr;
	//hr = D3D12SerializeRootSignature(&descriptionRootSignature,
	//	D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	//if (FAILED(hr)) {
	//	Log(reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
	//	assert(false);
	//}
	////バイナリを元に生成
	//ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	//hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
	//	signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	//assert(SUCCEEDED(hr));
	//
	//// InputLayout
	//D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	//
	//inputElementDescs[0].SemanticName = "POSITION";
	//inputElementDescs[0].SemanticIndex = 0;
	//inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	//inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	//inputElementDescs[1].SemanticName = "TEXCOORD";
	//inputElementDescs[1].SemanticIndex = 0;
	//inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	//inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	//inputElementDescs[2].SemanticName = "NORMAL";
	//inputElementDescs[2].SemanticIndex = 0;
	//inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	//inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	//D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	//inputLayoutDesc.pInputElementDescs = inputElementDescs;
	//inputLayoutDesc.NumElements = _countof(inputElementDescs);
	//
	////BlendState
	//D3D12_BLEND_DESC blendDesc{};
	//blendDesc.RenderTarget[0].RenderTargetWriteMask =
	//	D3D12_COLOR_WRITE_ENABLE_ALL;
	//
	////RasterizerState
	//D3D12_RASTERIZER_DESC rasterizerDesc{};
	////rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
	//rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	//
	////Shader Compile
	//ComPtr<IDxcBlob> vertexShaderBlob = CompileShader(L"resources/shaders/Object3D.VS.hlsl",
	//	L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	//assert(vertexShaderBlob != nullptr);
	//
	//ComPtr<IDxcBlob> pixelShaderBlob = CompileShader(L"resources/shaders/Object3D.PS.hlsl",
	//	L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	//assert(pixelShaderBlob != nullptr);
	//
	//
	////PSO
	//D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	//graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();
	//graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;
	//graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	//vertexShaderBlob->GetBufferSize() };
	//graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	//pixelShaderBlob->GetBufferSize() };
	//graphicsPipelineStateDesc.BlendState = blendDesc;
	//graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;
	//
	//graphicsPipelineStateDesc.NumRenderTargets = 1;
	//graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//
	//graphicsPipelineStateDesc.PrimitiveTopologyType =
	//	D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//
	//graphicsPipelineStateDesc.SampleDesc.Count = 1;
	//graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
	//
	//D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//depthStencilDesc.DepthEnable = true;
	//depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	//
	//graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	//graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//
	//ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;
	//hr = device->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
	//	IID_PPV_ARGS(&graphicsPipelineState));
	//assert(SUCCEEDED(hr));
	//
	//
	//
	//
    ////model
    //ModelData modelData = LoadObjFile("resources", "axis.obj");
	//ResourceObject vertexResoure = CreateBufferResource(device.Get(), sizeof(VertexData) * modelData.vertices.size());
	//D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	//vertexBufferView.BufferLocation = vertexResoure.Get()->GetGPUVirtualAddress();
	//vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	//vertexBufferView.StrideInBytes = sizeof(VertexData);
	//
	//VertexData* vertexData = nullptr;
	//vertexResoure.Get()->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	//std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData)* modelData.vertices.size());
	//
	////Lighting
	//
	//ResourceObject directionalLightResource = CreateBufferResource(device.Get(), sizeof(DirectionalLight));
	//
	//DirectionalLight* directionalLightData = nullptr;
	//
	//directionalLightResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
	//
	//directionalLightData->color = { 1.0f,1.0f,1.0f,1.0f };
	//directionalLightData->direction = { 0.0f,-1.0f,0.0f };
	//directionalLightData->intensity = 1.0f;
	//
	//
	//
	//bool useMonsterBall = false;
	//
	////Material Resource
	//ResourceObject materialResource = CreateBufferResource(device.Get(), sizeof(Material));
	//Material* materialDate = nullptr;
	//materialResource.Get()->Map(0, nullptr, reinterpret_cast<void**>(&materialDate));
	//materialDate->color = { 1.0f,1.0f,1.0f,1.0f };
	//materialDate->enableLighting = true;
	//
	////TransformationMatrix Resource
	//ResourceObject wvpResoure = CreateBufferResource(device.Get(), sizeof(TransformationMatrix));
	//TransformationMatrix* transformationMatrix = nullptr;
	//wvpResoure.Get()->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrix));
	//transformationMatrix->WVP = math->MakeIdentity4x4();
	//transformationMatrix->World = math->MakeIdentity4x4();
	//
	////Sprite
	//ResourceObject vertexResourceSprite = CreateBufferResource(device.Get(), sizeof(VertexData) * 6);
	//
	//D3D12_VERTEX_BUFFER_VIEW vertexBufferViewSprite{};
	//vertexBufferViewSprite.BufferLocation = vertexResourceSprite.Get()->GetGPUVirtualAddress();
	//vertexBufferViewSprite.SizeInBytes = sizeof(VertexData) * 6;
	//vertexBufferViewSprite.StrideInBytes = sizeof(VertexData);
	//
	//VertexData* vertexDataSprite = nullptr;
	//vertexResourceSprite.Get()->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataSprite));
	//
	////first triangle
	//vertexDataSprite[0].position = { 0.0f,360.0f,0.0f,1.0f };//lower left
	//vertexDataSprite[0].texCoord = { 0.0f,1.0f };
	//vertexDataSprite[0].normal = { 0.0f,0.0f, 1.0f };
	//vertexDataSprite[1].position = { 0.0f,0.0f,0.0f,1.0f };//upper left
	//vertexDataSprite[1].texCoord = { 0.0f,0.0f };
	//vertexDataSprite[1].normal = { 0.0f,0.0f, 1.0f };
	//vertexDataSprite[2].position = { 640.0f,360.0f,0.0f,1.0f };//lower right
	//vertexDataSprite[2].texCoord = { 1.0f,1.0f };
	//vertexDataSprite[2].normal = { 0.0f,0.0f, 1.0f };
	//vertexDataSprite[3].position = { 640.0f,0.0f,0.0f,1.0f };//upper left
	//vertexDataSprite[3].texCoord = { 1.0f,0.0f };
	//vertexDataSprite[3].normal = { 0.0f,0.0f, 1.0f };
	//
	//
	//ResourceObject indexResourceSprite = CreateBufferResource(device.Get(), sizeof(uint32_t) * 6);
	//
	//D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};
	//indexBufferViewSprite.BufferLocation = indexResourceSprite.Get()->GetGPUVirtualAddress();
	//indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6;
	//indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;
	//
	//
	//uint32_t* indexDataSprite = nullptr;
	//indexResourceSprite.Get()->Map(0, nullptr, reinterpret_cast<void**>(&indexDataSprite));
	//
	//indexDataSprite[0] = 0; indexDataSprite[1] = 1; indexDataSprite[2] = 2;
	//indexDataSprite[3] = 1; indexDataSprite[4] = 3; indexDataSprite[5] = 2;
	//
	//
	//ResourceObject materialResourceSprite = CreateBufferResource(device.Get(), sizeof(Material));
	//
	//Material* materialDataSprite = nullptr;
	//
	//materialResourceSprite.Get()->Map(0, nullptr, reinterpret_cast<void**>(&materialDataSprite));
	//
	//materialDataSprite->color = { 1.0f,1.0f,1.0f,1.0f };
	//materialDataSprite->enableLighting = false;
	//
	////UVTransform
	//materialDate->uvTransform = math->MakeIdentity4x4();
	//materialDataSprite->uvTransform = math->MakeIdentity4x4();
	//
	//ResourceObject transformationMatrixResourceSprite = CreateBufferResource(device.Get(), sizeof(TransformationMatrix));
	//TransformationMatrix* transformationMatrixDataSprite = nullptr;
	//transformationMatrixResourceSprite.Get()->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixDataSprite));
	//transformationMatrixDataSprite->World = math->MakeIdentity4x4();
	//transformationMatrixDataSprite->WVP = math->MakeIdentity4x4();
	//
	//Transform transformSprite{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };

	



	while (true) {

		//ImGui_ImplDX12_NewFrame();
		//ImGui_ImplWin32_NewFrame();
		//ImGui::NewFrame();
		//ImGui::ColorEdit4("Material", &directionalLightData->color.x);
		//ImGui::SliderFloat3("direction", &directionalLightData->direction.x, 0.1f,1.0f);
		//ImGui::SliderFloat("intensity ", &directionalLightData->intensity, 0.0f,5.0f);
		//ImGui::SliderFloat2("transform.rotate ", &transform.rotate.x, 0.0f, 6.28f);
		//ImGui::DragFloat3("transformSprite", &transformSprite.translate.x, 1.0f);
		//ImGui::DragFloat3("transformSpriteR", &transformSprite.rotate.x, 0.01f);
		//ImGui::Checkbox("useMonsterBall", &useMonsterBall);
		//ImGui::DragFloat2("UVTranslate", &uvTransformSprite.translate.x, 0.01f, -10.0f, 10.0f);
		//ImGui::DragFloat2("UvScale" ,&uvTransformSprite.scale.x, 0.0f, -10.0f, 10.0f);
		//ImGui::SliderAngle("UVRotate", &uvTransformSprite.rotate.z);
		//ImGui::Render();
		if (winApp->ProcessMessage()) {

			break;
		} 
		input->Update();
		dxCommon->PreDraw();
		dxCommon->PostDraw();
			//directionalLightData->direction = normalize(directionalLightData->direction);
			//
			//UINT backBufferIndex = swapChain->GetCurrentBackBufferIndex();
			//
			////TransitionBarrier
			//D3D12_RESOURCE_BARRIER barrier{};
			//
			//barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			//barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			//barrier.Transition.pResource = swapChainResources[backBufferIndex].Get();
			//barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
			//barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
			//
			//D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
			//
			////transform.rotate.y += 0.01f;
			////transform.rotate.y = 3.14f;
			//Matrix4x4 worldMatrix = math->MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
			//Matrix4x4 cameraMatrix = math->MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
			//Matrix4x4 viewMatrix = math->Inverse(cameraMatrix);
			//Matrix4x4 projectionMatrix = math->MakePerspectiveFovMatrix(0.45f, float(WinApp::kClientWidth) / float(WinApp::kClientHeight), 0.1f, 100.0f);
			//Matrix4x4 worldViewProjectionMatrix = math->Multiply(worldMatrix, math->Multiply(viewMatrix, projectionMatrix));
			//transformationMatrix->WVP = worldViewProjectionMatrix;
			//transformationMatrix->World = worldViewProjectionMatrix;
			//
			//Matrix4x4 uvTransformMatrix = math->MakeScaleMatrix(uvTransformSprite.scale);
			//uvTransformMatrix = math->Multiply(uvTransformMatrix, math->MakeRotateZMatrix(uvTransformSprite.rotate.z));
			//uvTransformMatrix = math->Multiply(uvTransformMatrix, math->MakeTranslateMatrix(uvTransformSprite.translate));
			//materialDataSprite->uvTransform = uvTransformMatrix;
			//
			//Matrix4x4 worldMatrixSprite = math->MakeAffineMatrix(transformSprite.scale, transformSprite.rotate, transformSprite.translate);
			//Matrix4x4 viewMatrixSprite = math->MakeIdentity4x4();
			//Matrix4x4 projectionMatrixSprite = math->MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kClientWidth), float(WinApp::kClientHeight), 0.0f, 100.0f);
			//Matrix4x4 worldViewProjectionMatrixSprite = math->Multiply(worldMatrixSprite, math->Multiply(viewMatrixSprite, projectionMatrixSprite));
			//transformationMatrixDataSprite->WVP = worldViewProjectionMatrixSprite;
			//transformationMatrixDataSprite->World = worldViewProjectionMatrixSprite;
			//
			//
			////Resource State Transition
			//commandList->ResourceBarrier(1, &barrier);
			//
			////Output Merger Stage
			//commandList->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, &dsvHandle);
			//
			////Clearing Render Targets
			//float clearColor[] = { 0.1f,0.25f,0.5f,1.0f };
			//commandList->ClearRenderTargetView(rtvHandles[backBufferIndex], clearColor, 0, nullptr);
			//commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
			//
			////描画用の DescriptorHeaps
			//ID3D12DescriptorHeap* descriptorHeaps[] = { srvDescriptorHeap.Get()};
			//commandList->SetDescriptorHeaps(1, descriptorHeaps);
			//
			////Viewport and Scissor Set
			//commandList->RSSetViewports(1, &viewport);
			//commandList->RSSetScissorRects(1, &scissorRect);
			//
			//
			////Set Root Signature and Pipeline State Object
			//commandList->SetGraphicsRootSignature(rootSignature.Get());
			//commandList->SetPipelineState(graphicsPipelineState.Get());
			//
			////Vertex Buffer Binding
			//commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
			//commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			//commandList->SetGraphicsRootConstantBufferView(0, materialResource.Get()->GetGPUVirtualAddress());
			//commandList->SetGraphicsRootConstantBufferView(1, wvpResoure.Get()->GetGPUVirtualAddress());
			//commandList->SetGraphicsRootDescriptorTable(2, useMonsterBall ? textureSrvHandleGPU2 : textureSrvHandleGPU);
			//commandList->SetGraphicsRootConstantBufferView(3, directionalLightResource.Get()->GetGPUVirtualAddress());
			//commandList->DrawInstanced(UINT(modelData.vertices.size()), 1, 0, 0);
			//
			//
			//commandList->IASetVertexBuffers(0, 1, &vertexBufferViewSprite);
			//commandList->IASetIndexBuffer(&indexBufferViewSprite);
			//commandList->SetGraphicsRootConstantBufferView(0, materialResourceSprite.Get()->GetGPUVirtualAddress());
			//commandList->SetGraphicsRootConstantBufferView(1, transformationMatrixResourceSprite.Get()->GetGPUVirtualAddress());
			//commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU);
			////commandList->DrawInstanced(6, 1, 0, 0);
			//commandList->DrawIndexedInstanced(6, 1, 0, 0, 0);
			//
			//
			//ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList.Get());
			//
			////Swap Chain Buffer Transition to Present
			//barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
			//barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
			//commandList->ResourceBarrier(1, &barrier);
			//
			//
			//
			//
			//
			//hr = commandList->Close();
			//assert(SUCCEEDED(hr));
			//
			//
			////Command List Execution
			//ID3D12CommandList* commandLists[] = { commandList.Get()};
			//commandQueue->ExecuteCommandLists(1, commandLists);
			//
			////Swap Chain Present
			//swapChain->Present(1, 0);
			//
			//
			//
			//
			//fenceValue++;
			//commandQueue->Signal(fence.Get(), fenceValue);
			//
			//if (fence->GetCompletedValue() < fenceValue)
			//{
			//	fence->SetEventOnCompletion(fenceValue, fenceEvent);
			//
			//	WaitForSingleObject(fenceEvent, INFINITE);
			//}
			//
			//
			//
			//hr = commandAllocator->Reset();
			//assert(SUCCEEDED(hr));
			//hr = commandList->Reset(commandAllocator.Get(), nullptr);
			//assert(SUCCEEDED(hr));



		
	}

	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	//CloseHandle(fenceEvent);

	delete math;


//#ifdef _DEBUG
//	if (debugController != nullptr) {
//		debugController->Release();
//	}
//#endif
	//CloseWindow(hwnd);

	//_CrtDumpMemoryLeaks();
	//CoUninitialize();

	winApp->Finalize();

	CoUninitialize();
	return 0;

}