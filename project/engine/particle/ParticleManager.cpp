#include "ParticleManager.h"
#include "Logger.h"
#include "StringUtility.h"
#include <Model.h>



using namespace Logger;
using namespace StringUtility;

void ParticleManager::Initialize(DirectXCommon* directXCommon, SrvManager* srvManager)
{
#ifdef _DEBUG
	// 유효성 검사(Validation)
	assert(directXCommon != nullptr);
	assert(srvManager != nullptr);
#endif // _DEBUG

	dxCommon_ = directXCommon;
	srvManager_ = srvManager;

	std::random_device seedGenerator;   // 시드 생성기(Seed Generator)//난수 제공
	std::mt19937 randomEngine(seedGenerator()); // 랜덤 엔진 초기화(Random Engine Initialization)//seedGenerator 초기화

	InitializeVertices();

	CreateGraphicsPipeline();

	CreateVertexBuffer();


}

void ParticleManager::Update()
{
	if (!camera_) return;

	// ✅ 반드시 호출!
	calculationBillboardMatrix();

	Matrix4x4 cameraMatrix = camera_->GetWorldMatrix();
	Matrix4x4 viewMatrix = MyMath::Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = camera_->GetProjectionMatrix();
	Matrix4x4 viewProjectionMatrix = MyMath::Multiply(viewMatrix, projectionMatrix);

	constexpr float deltaTime = 1.0f / 60.0f;

	// 파티클 그룹 순회
	for (auto& [name, group] : particleGroups) {
		// 수명 갱신 및 제거
		group.particles.remove_if([deltaTime](Particle& p) {
			p.currentTime += deltaTime;
			return p.currentTime >= p.lifeTime;
			});

		int i = 0;
		for (auto& p : group.particles) {
			p.transform.translate = MyMath::Add(p.transform.translate, p.velocity);

			if (group.mappedInstanceData && i < initialInstanceCount) {
				// scale → billboard → 위치
				Matrix4x4 scaleMatrix = MyMath::MakeScaleMatrix(p.transform.scale);
				Matrix4x4 rotateXMatrix = MyMath::MakeRotateXMatrix(p.transform.rotate.x);
				Matrix4x4 rotateYMatrix = MyMath::MakeRotateYMatrix(p.transform.rotate.y);
				Matrix4x4 rotateZMatrix = MyMath::MakeRotateZMatrix(p.transform.rotate.z);
				//Matrix4x4 scaleRotate = MyMath::Multiply(scaleMatrix, rotateZMatrix);
				
				Matrix4x4 rotationMatrix = MyMath::Multiply(rotateZMatrix, MyMath::Multiply(rotateYMatrix, rotateXMatrix));
				Matrix4x4 scaleRotate = MyMath::Multiply(scaleMatrix, rotationMatrix);

				//billboard
				//Matrix4x4 worldMatrix = MyMath::Multiply(scaleRotate, billboardMatrix);
				Matrix4x4 worldMatrix = scaleRotate;
				worldMatrix.m[3][0] = p.transform.translate.x;
				worldMatrix.m[3][1] = p.transform.translate.y;
				worldMatrix.m[3][2] = p.transform.translate.z;

				Matrix4x4 worldViewProjectionMatrix = MyMath::Multiply(worldMatrix, viewProjectionMatrix);
				group.mappedInstanceData[i].WVP = worldViewProjectionMatrix;
				group.mappedInstanceData[i].World = worldMatrix;

				// 알파 감소
				float alpha = 1.0f - (p.currentTime / p.lifeTime);
				group.mappedInstanceData[i].color = p.color;
				group.mappedInstanceData[i].color.w = alpha;
			}
			++i;
		}
		group.instanceCount = i;
	}
}

void ParticleManager::Draw()
{
	auto commandList = dxCommon_->GetCommandList();
	commandList->SetGraphicsRootSignature(rootSignature.Get());
	commandList->SetPipelineState(graphicsPipelineState.Get());

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);
	for (auto& [name, group] : particleGroups)
	{

		if (group.instanceCount > 0) {

			// 🔹 GPU에 올바른 데이터가 전달되었는지 확인
			if (group.instanceCount <= 0 || !group.mappedInstanceData) {
				continue; // 그릴 필요 없음
			}
			D3D12_GPU_DESCRIPTOR_HANDLE instanceHandle = srvManager_->GetGPUDescriptorHandle(group.instanceSRVIndex);
			commandList->SetGraphicsRootDescriptorTable(0, instanceHandle);

			srvManager_->SetGraphicsRootDesciptorTable(1, group.textureIndex);

			commandList->DrawInstanced(static_cast<UINT>(vertices_.size()), group.instanceCount, 0, 0);
		}
	}
}

void ParticleManager::calculationBillboardMatrix() {
	// 1. Z+ → Z- 보정 (뒤 → 앞 전환용 Y축 180도 회전 행렬)
	Matrix4x4 backToFrontMatrix = MyMath::MakeRotateYMatrix(std::numbers::pi_v<float>);

	// 2. 카메라의 회전값만 가져오기
	Vector3 cameraRotation = camera_->GetRotate();

	// 3. 회전 행렬 생성 (Y→X→Z 순)
	Matrix4x4 rotateX = MyMath::MakeRotateXMatrix(cameraRotation.x);
	Matrix4x4 rotateY = MyMath::MakeRotateYMatrix(cameraRotation.y);
	Matrix4x4 rotateZ = MyMath::MakeRotateZMatrix(cameraRotation.z);

	Matrix4x4 cameraRotationOnly = MyMath::Multiply(rotateY, MyMath::Multiply(rotateX, rotateZ));

	// 4. billboard = backToFront * cameraRotationOnly
	billboardMatrix = MyMath::Multiply(backToFrontMatrix, cameraRotationOnly);

	// 5. 위치 성분 제거
	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;
}

Particle ParticleManager::MakeNewParticle(std::mt19937& randomEngine, const Vector3& translate)
{
	std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);
	std::uniform_real_distribution<float> distRotate(-std::numbers::pi_v<float>, std::numbers::pi_v<float>);
	std::uniform_real_distribution<float> distScale(0.4f, 1.5f);

	


	Particle particle;
	float randomScale = distScale(randomEngine);
	particle.transform.rotate = { distRotate(randomEngine), distRotate(randomEngine), distRotate(randomEngine)};
	
	//particle.transform.translate = MyMath::Add(translate, spread);
	particle.transform.scale = { 3.0f, 3.0f, 3.0f };
	particle.transform.translate = { 0.0f,5.0f,0.0f };
	particle.velocity = { 0.0f,0.0f,0.0f };

	//Vector3 randomTranslate{ distribution(randomEngine), distribution(randomEngine), distribution(randomEngine) };
	//particle.transform.translate = MyMath::Add(translate, randomTranslate);

	//particle.velocity = { distribution(randomEngine), distribution(randomEngine), distribution(randomEngine) };
	std::uniform_real_distribution<float> distColor(0.0f, 1.0f);
	//particle.color = { distColor(randomEngine), distColor(randomEngine), distColor(randomEngine), 1.0f };
	particle.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	//std::uniform_real_distribution<float> distTime(1.0f, 3.0f);
	//particle.lifeTime = distTime(randomEngine);
	particle.lifeTime = 1.0f;
	particle.currentTime = 0.0f;
	return particle;
}

std::list<Particle> ParticleManager::Emit(const std::string& groupName, const Emitter& emitter, std::mt19937& randomEngine) {
	std::list<Particle> particles;
	for (uint32_t count = 0; count < emitter.count; ++count) {
		particles.push_back(MakeNewParticle(randomEngine, emitter.transform.translate));
	}
	if (particleGroups.contains(groupName)) {
		particleGroups[groupName].particles.insert(
			particleGroups[groupName].particles.end(),
			particles.begin(),
			particles.end()
		);
	}
	return particles;
}



void ParticleManager::CreateParticleGroup(const std::string& name, const std::string& textureFilePath)
{
	if (particleGroups.find(name) != particleGroups.end()) {
		// 이미 등록된 이름이 있다면 assert로 중단(If the name is already registered, stop with assert)
		assert(false && "Particle group with the given name already exists!");
		return; // 디버깅 중이 아니라면 중단하지 않고 반환(If not debugging, return without stopping)
	}


	//new ParticleGroup generation
	ParticleGroup newGroup;

	newGroup.textureIndex = TextureManager::GetInstance()->GetTextureIndexByFilepath(textureFilePath);

	const DirectX::TexMetadata& metadata =
		TextureManager::GetInstance()->GetMetaData(textureFilePath);

	newGroup.instanceCount = 0;
	newGroup.mappedInstanceData = nullptr;


	newGroup.instanceBuffer= CreateBufferResource(dxCommon_->GetDevice(), sizeof(ParticleForGPU) * kNumMaxInstance);

	// 리소스 매핑
	HRESULT hr = newGroup.instanceBuffer->Map(0, nullptr, reinterpret_cast<void**>(&newGroup.mappedInstanceData));
	assert(SUCCEEDED(hr));
	/*for (int i = 0; i < initialInstanceCount; i++)
	{
		newGroup.mappedInstanceData[i].WVP = MyMath::MakeIdentity4x4();
		newGroup.mappedInstanceData[i].World = MyMath::MakeIdentity4x4();
		newGroup.mappedInstanceData[i].color = Vector4{ 1.0f,1.0f,1.0f,1.0f };
	}*/


	// SRV 생성 (Structured Buffer )
	newGroup.instanceSRVIndex = srvManager_->Allocate() + TextureManager::kSRVIndexTop;
	srvManager_->CreatSRVforStruturedBuffer(
		newGroup.instanceSRVIndex,
		newGroup.instanceBuffer.Get(),
		static_cast<UINT>(initialInstanceCount),
		sizeof(ParticleForGPU)
	);

	//컨테이너 설정(Container Settings)
	particleGroups[name] = std::move(newGroup);

}

ComPtr<ID3D12Resource> ParticleManager::CreateBufferResource(ComPtr<ID3D12Device> device, size_t sizeInBytes)
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

	ComPtr<ID3D12Resource> vertexResource = nullptr;
	HRESULT hr = device->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
		IID_PPV_ARGS(&vertexResource));
	assert(SUCCEEDED(hr));
	vertexResource->SetName(L"bufferResource");

	return vertexResource;
}

IDxcBlob* ParticleManager::CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler)
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

void ParticleManager::CreateRootSignature()
{
	HRESULT hr;

	//DescriptorRange
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;
	descriptorRange[0].NumDescriptors = 1;
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing[1] = {};
	descriptorRangeForInstancing[0].BaseShaderRegister = 0;
	descriptorRangeForInstancing[0].NumDescriptors = 1;
	descriptorRangeForInstancing[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	descriptorRangeForInstancing[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	//RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	//複数設定できるので配列。今回は結果１つだけなので長さ1の配列
	D3D12_ROOT_PARAMETER rootParameters[2] = {};
	//rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //CBVを使う
	//rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //PixelShaderで使う
	//rootParameters[0].Descriptor.ShaderRegister = 0; //レジスタ番号0とバインド
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX; ///VertexShaderで使う
	rootParameters[0].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing;
	rootParameters[0].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing);
	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; //DescriptorTableを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //PixelShaderで使う
	rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRange;
	rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);
	//rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV; //CBVを使う
	//rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //PixelShaderで使う
	//rootParameters[3].Descriptor.ShaderRegister = 1; //レジスタ番号1
	descriptionRootSignature.pParameters = rootParameters; //rootParameters配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters); //配列の長さ

	//Sampler
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
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

	hr = dxCommon_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
		signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));

	rootSignature->SetName(L"ParticleRootSignature");
}

void ParticleManager::CreateGraphicsPipeline()
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
	ComPtr<IDxcBlob> vertexShaderBlob = CompileShader(L"resources/shaders/Particle.VS.hlsl",
		L"vs_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(vertexShaderBlob != nullptr);

	ComPtr<IDxcBlob> pixelShaderBlob = CompileShader(L"resources/shaders/Particle.PS.hlsl",
		L"ps_6_0", dxcUtils, dxcCompiler, includeHandler);
	assert(pixelShaderBlob != nullptr);




	// InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[2] = {};

	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	//BlendState
	D3D12_BLEND_DESC blendDesc{};
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;

	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

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

	hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState));
	assert(SUCCEEDED(hr));

}

void ParticleManager::InitializeVertices()
{
	vertices_.clear();
	const uint32_t kRingDivide = 32;
	const float kOuterRadius = 1.0f;
	const float kInnerRadius = 0.2f;
	const float radianPrDivide = 2.0f * std::numbers::pi_v<float> / float(kRingDivide);

	for (uint32_t index = 0; index < kRingDivide; ++index) {
		float sin = std::sin(index * radianPrDivide);
		float cos = std::cos(index * radianPrDivide);
		float sinNext = std::sin((index + 1) * radianPrDivide);
		float cosNext = std::cos((index + 1) * radianPrDivide);
		float u = float(index) / float(kRingDivide);
		float uNext = float(index + 1) / float(kRingDivide);

		// 삼각형 1
		vertices_.push_back({ { -sin * kOuterRadius, cos * kOuterRadius, 0.0f, 1.0f }, { u, 0.0f } });
		vertices_.push_back({ { -sinNext * kOuterRadius, cosNext * kOuterRadius, 0.0f, 1.0f }, { uNext, 0.0f } });
		vertices_.push_back({ { -sin * kInnerRadius, cos * kInnerRadius, 0.0f, 1.0f }, { u, 1.0f } });

		// 삼각형 2
		vertices_.push_back({ { -sin * kInnerRadius, cos * kInnerRadius, 0.0f, 1.0f }, { u, 1.0f } });
		vertices_.push_back({ { -sinNext * kOuterRadius, cosNext * kOuterRadius, 0.0f, 1.0f }, { uNext, 0.0f } });
		vertices_.push_back({ { -sinNext * kInnerRadius, cosNext * kInnerRadius, 0.0f, 1.0f }, { uNext, 1.0f } });
	}
	//// 삼각형 1: 좌상, 좌하, 우상
	//vertices_.push_back({ { -1.0f,  1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } }); // 좌상
	//vertices_.push_back({ { -1.0f, -1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } }); // 좌하
	//vertices_.push_back({ {  1.0f,  1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } }); // 우상

	//// 삼각형 2: 우상, 좌하, 우하
	//vertices_.push_back({ {  1.0f,  1.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } }); // 우상
	//vertices_.push_back({ { -1.0f, -1.0f, 0.0f, 1.0f }, { 0.0f, 1.0f } }); // 좌하
	//vertices_.push_back({ {  1.0f, -1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f } }); // 우하
	//vertices_.push_back({ { -1.0f, -1.0f, 0.0f,1.0f }, { 1.0f, 1.0f } }); // lower right
}

void ParticleManager::CreateVertexBuffer()
{

	auto device = dxCommon_->GetDevice();

	// GPU 메모리에 버퍼 생성(Create a buffer in GPU memory)
	vertexBufferResource_ = CreateBufferResource(device, sizeof(ParticleVertex) * vertices_.size());
	// 버퍼 뷰 설정(Setting the buffer view)
	vertexBufferView_.BufferLocation = vertexBufferResource_->GetGPUVirtualAddress();
	vertexBufferView_.SizeInBytes = static_cast<UINT>(sizeof(ParticleVertex) * vertices_.size());
	vertexBufferView_.StrideInBytes = sizeof(ParticleVertex);
	// GPU 메모리 매핑(GPU Memory Mapping)
	ParticleVertex* vertexDataParticle = nullptr;
	vertexBufferResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexDataParticle));
	// 정점 데이터 복사(Copy vertex data)
	memcpy(vertexDataParticle, vertices_.data(), sizeof(ParticleVertex) * vertices_.size());
	// 매핑 해제(Unmap)
	vertexBufferResource_->Unmap(0, nullptr); 
}
