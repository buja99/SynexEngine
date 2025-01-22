#pragma once
#include "DirectXCommon.h"
#include "SrvManager.h"
#include <wrl.h>
#include <random>
#include "VertexData.h"
#include "Material.h"
#include "Transform.h"
#include "MyMath.h"
#include "TextureManager.h"
#include <numbers>
#include <list>
#include "Camera.h"
#include "Sprite.h"
#include "SpriteCommon.h"
#include "MyMath.h"



//struct ModelData
//{
//	std::vector<VertexData> vertices;
//	MaterialData material;
//};

struct ParticleVertex {
    Vector3 position;  // 3D 위치
    Vector4 color;     // RGBA 색상
    float size;        // 파티클 크기
};
struct Particle {
    Transform transform;
    Vector3 velocity;
    Vector4 color;
    float lifeTime;
    float currentTime;
};
struct  ParticleForGPU
{
    Matrix4x4 WVP;
    Matrix4x4 World;
    Vector4 color;
};
struct ParticleGroup {
    std::string textureFilePath;           // 텍스처 파일 경로(Texture file path)
    std::list<Particle> particles;         // 파티클 리스트(particle list)
    int instanceSRVIndex;                  // 인스턴싱 데이터용 SRV 인덱스(SRV index for instancing data)
    ComPtr<ID3D12Resource> instanceBuffer; // 인스턴스 데이터 리소스(Instance data resource)
    int instanceCount;                     // 인스턴스 개수(Number of instances)
    ParticleForGPU* mappedInstanceData;    // GPU 메모리에 매핑된 데이터 포인터(Data pointer mapped to GPU memory)
};

const int32_t initialInstanceCount = 1024;

class ParticleManager {
public:
    void Initialize(DirectXCommon* directXCommon, SrvManager* srvManager);
    void Update();
    void Draw();

    void Emit(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& color, float size, float lifetime);

    void CreateParticleGroup(const std::string& name, const std::string& textureFilePath);


private:
    ComPtr<ID3D12Resource> CreateBufferResource(ComPtr <ID3D12Device> device, size_t sizeInBytes);
    IDxcBlob* CompileShader(
        const std::wstring& filePath,
        const wchar_t* profile,
        IDxcUtils* dxcUtils,
        IDxcCompiler3* dxcCompiler,
        IDxcIncludeHandler* includeHandler);

    void CreateRootSignature();
    void CreateGraphicsPipeline();
    void InitializeVertices();
    void CreateVertexBuffer();

    ComPtr<ID3D12RootSignature> rootSignature;
    ComPtr<ID3D12PipelineState> graphicsPipelineState;

    DirectXCommon* dxCommon_ = nullptr;
    SrvManager* srvManager_ = nullptr;
    SpriteCommon* spriteCommon_ = nullptr;
    std::vector<ParticleVertex> vertices_;
    ComPtr<ID3D12Resource> vertexBufferResource_;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

    /// <summary>
    /// 문자열 키를 사용하여 파티클 그룹을 관리
    /// 각 키는 파티클 데이터, 텍스처 정보, 렌더링 정보를 포함한 ParticleGroup에 연결
    /// Manages particle groups using a unique string key.
    /// Each key corresponds to a ParticleGroup, which contains particles, texture data,rendering information.
    /// </summary>
    std::unordered_map<std::string, ParticleGroup> particleGroups;

    MyMath* math = nullptr;
};

