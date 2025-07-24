#include "Skybox.h"
#include "d3dx12.h"
#include "ResourceUtils.h"

void Skybox::Initialize(DirectXCommon* dxCommon, const std::wstring& ddsFilePath) {
    dxCommon_ = dxCommon;

    // 1. Cubemap 로드 및 업로드
    DirectX::ScratchImage mipImages;
    HRESULT hr = DirectX::LoadFromDDSFile(ddsFilePath.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, mipImages);
    if (FAILED(hr)) {
        OutputDebugStringA("Failed to load DDS file!\n");
        return; // 또는 assert(false);
    } else {
        OutputDebugStringA("DDS file loading success\n");
    }

    auto result = TextureUploader::UploadAndDescribe(dxCommon_->GetDevice().Get(), dxCommon_->GetCommandList().Get(), mipImages);
    cubemapTexture_ = result.texture;
    srvDesc_ = result.srvDesc;
    assert(cubemapTexture_);
    // 2. SRVManager 등록
    auto srvMgr = SrvManager::GetInstance();
    srvIndex_ = srvMgr->Allocate();
    srvMgr->CreateSRV(srvIndex_, cubemapTexture_.Get(), srvDesc_);
    assert(srvIndex_ >= 0);
    // 3. 정점 버퍼 생성
    CreateVertexBuffer();

    // 4. 루트 시그니처 및 파이프라인 초기화
    CreateRootSignature();
    CreatePipeline();

    // 5. 상수 버퍼 생성 (ViewProjection)
    D3D12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    D3D12_RESOURCE_DESC cbDesc = CD3DX12_RESOURCE_DESC::Buffer((sizeof(Matrix4x4) + 255) & ~255);
    hr = dxCommon_->GetDevice()->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &cbDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&constantBuffer_)
    );
    assert(SUCCEEDED(hr));
    constantBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&mappedMatrix_));

}

void Skybox::Draw(const Matrix4x4& view, const Matrix4x4& projection) {
    if (!dxCommon_) return;

    ComPtr<ID3D12GraphicsCommandList> cmdList = dxCommon_->GetCommandList();

    // 파이프라인과 루트 시그니처 설정
    cmdList->SetPipelineState(pipelineState_.Get());
    cmdList->SetGraphicsRootSignature(rootSignature_.Get());

    // 디스크립터 힙 설정
    auto srvMgr = SrvManager::GetInstance();
    srvMgr->PreDraw();

    // 정점 버퍼 설정
    cmdList->IASetVertexBuffers(0, 1, &vbView_);
    cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    Matrix4x4 viewNoPos = view;
    viewNoPos.m[3][0] = 0.0f;
    viewNoPos.m[3][1] = 0.0f;
    viewNoPos.m[3][2] = 0.0f;

    // ViewProjection 역행렬 계산 후 상수 버퍼에 기록
    Matrix4x4 vp = MyMath::Multiply(viewNoPos, projection);
    *mappedMatrix_ = MyMath::Inverse(vp);

    // 루트 파라미터 설정
    cmdList->SetGraphicsRootConstantBufferView(0, constantBuffer_->GetGPUVirtualAddress());
    srvMgr->SetGraphicsRootDesciptorTable(1, srvIndex_);

    // 드로우 호출
    cmdList->DrawInstanced(36, 1, 0, 0);
}

void Skybox::CreateVertexBuffer() {
    struct Vertex {
        Vector4 position; 
    };

    static const Vertex vertices[] = {
        // +X 면
        {{1, -1, -1, 1}}, {{1, 1, -1, 1}}, {{1, 1, 1, 1}},
        {{1, -1, -1, 1}}, {{1, 1, 1, 1}}, {{1, -1, 1, 1}},

        // -X 면
        {{-1, -1, 1, 1}}, {{-1, 1, 1, 1}}, {{-1, 1, -1, 1}},
        {{-1, -1, 1, 1}}, {{-1, 1, -1, 1}}, {{-1, -1, -1, 1}},

        // +Y 면
        {{-1, 1, 1, 1}}, {{1, 1, 1, 1}}, {{1, 1, -1, 1}},
        {{-1, 1, 1, 1}}, {{1, 1, -1, 1}}, {{-1, 1, -1, 1}},

        // -Y 면
        {{-1, -1, -1, 1}}, {{1, -1, -1, 1}}, {{1, -1, 1, 1}},
        {{-1, -1, -1, 1}}, {{1, -1, 1, 1}}, {{-1, -1, 1, 1}},

        // +Z 면
        {{-1, -1, 1, 1}}, {{1, -1, 1, 1}}, {{1, 1, 1, 1}},
        {{-1, -1, 1, 1}}, {{1, 1, 1, 1}}, {{-1, 1, 1, 1}},

        // -Z 면
        {{1, -1, -1, 1}}, {{-1, -1, -1, 1}}, {{-1, 1, -1, 1}},
        {{1, -1, -1, 1}}, {{-1, 1, -1, 1}}, {{1, 1, -1, 1}},
    };

    const UINT sizeVB = static_cast<UINT>(sizeof(vertices));

    // 정점 버퍼 생성
    D3D12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    D3D12_RESOURCE_DESC resDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeVB);

    HRESULT hr = dxCommon_->GetDevice()->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &resDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&vertexBuffer_)
    );
    assert(SUCCEEDED(hr));

    // 정점 데이터 복사
    Vertex* mapped = nullptr;
    vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&mapped));
    memcpy(mapped, vertices, sizeVB);
    vertexBuffer_->Unmap(0, nullptr);

    // 뷰 설정
    vbView_.BufferLocation = vertexBuffer_->GetGPUVirtualAddress();
    vbView_.SizeInBytes = sizeVB;
    vbView_.StrideInBytes = sizeof(Vertex);

    assert(vbView_.SizeInBytes >= vbView_.StrideInBytes * 36);

}
void Skybox::CreatePipeline() {
    // DXC 유틸리티 준비
    ComPtr<IDxcUtils> dxcUtils;
    ComPtr<IDxcCompiler3> dxcCompiler;
    HRESULT hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils));
    assert(SUCCEEDED(hr));
    hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler));
    assert(SUCCEEDED(hr));

    ComPtr<IDxcIncludeHandler> includeHandler;
    dxcUtils->CreateDefaultIncludeHandler(&includeHandler);

    // VS/PS 컴파일
    ComPtr<IDxcBlob> vsBlob;
    vsBlob.Attach(dxCommon_->CompileShader(L"Resources/Shaders/Skybox.VS.hlsl", L"vs_6_0", dxcUtils.Get(), dxcCompiler.Get(), includeHandler.Get()));
    ComPtr<IDxcBlob> psBlob;
    psBlob.Attach(dxCommon_->CompileShader(L"Resources/Shaders/Skybox.PS.hlsl", L"ps_6_0", dxcUtils.Get(), dxcCompiler.Get(), includeHandler.Get()));

    // 입력 레이아웃
    D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
    };



    // 파이프라인 설정
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
    psoDesc.VS = { vsBlob->GetBufferPointer(), vsBlob->GetBufferSize() };
    psoDesc.PS = { psBlob->GetBufferPointer(), psBlob->GetBufferSize() };
    psoDesc.InputLayout = { inputLayout, _countof(inputLayout) };
    psoDesc.pRootSignature = rootSignature_.Get();
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = dxCommon_->GetRTVFormat();
    psoDesc.SampleDesc.Count = 1;
    psoDesc.DSVFormat = dxCommon_->GetDSVFormat();

    D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO; // Z값 쓰지 않음
    depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL; // 가장 뒤에서 항상 통과

    psoDesc.DepthStencilState = depthStencilDesc;

    hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState_));
    if (FAILED(hr)) {
        OutputDebugStringA("CreateGraphicsPipelineState failed\n");
        assert(false);
    }
    assert(SUCCEEDED(hr));
}

void Skybox::CreateRootSignature() {
    CD3DX12_ROOT_PARAMETER rootParams[2];

    // 0: ViewProjection 역행렬용 상수 버퍼
    rootParams[0].InitAsConstantBufferView(2, 0, D3D12_SHADER_VISIBILITY_VERTEX);

    // 1: Cubemap 텍스처용 디스크립터 테이블 (t3, space0)
    CD3DX12_DESCRIPTOR_RANGE texRange;
    texRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3); // t3
    rootParams[1].InitAsDescriptorTable(1, &texRange, D3D12_SHADER_VISIBILITY_PIXEL);

    // --- Sampler 커스터마이징 추가 ---
    D3D12_STATIC_SAMPLER_DESC samplerDesc{};
    samplerDesc.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    samplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    samplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_CLAMP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    samplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
    samplerDesc.MinLOD = 0.0f;
    samplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
    samplerDesc.ShaderRegister = 0; // s0
    samplerDesc.RegisterSpace = 0;
    samplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

    // 루트 시그니처 설정
    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc;
    rootSigDesc.Init(_countof(rootParams), rootParams,1, &samplerDesc,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
    );
    ComPtr<ID3DBlob> signatureBlob;
    ComPtr<ID3DBlob> errorBlob;

    HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
        &signatureBlob, &errorBlob);
    if (FAILED(hr)) {
        OutputDebugStringA((char*)errorBlob->GetBufferPointer());
        assert(false);
    }

    hr = dxCommon_->GetDevice()->CreateRootSignature(0, signatureBlob->GetBufferPointer(),
        signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature_));
    assert(SUCCEEDED(hr));
}

void Skybox::Finalize() {
    // 리소스 해제
    vertexBuffer_.Reset();
    constantBuffer_.Reset();
    cubemapTexture_.Reset();
    rootSignature_.Reset();
    pipelineState_.Reset();

    vertexMapped_ = nullptr;
    mappedMatrix_ = nullptr;

    // 디스크립터 반환
    if (srvIndex_ >= 0) {
        SrvManager::GetInstance()->Free(srvIndex_);
        srvIndex_ = -1;
    }
}