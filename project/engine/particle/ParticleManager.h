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
#include <functional>
#include <unordered_map>

struct ParticleVertex {
    Vector4 position;  // 3D 위치
    Vector2 texcoord;
};
struct Particle //파티클 1개의 상태 (위치, 속도, 색상, 생존시간 등)
{
    Transform transform;
    Vector3 velocity;
    Vector3 acceleration;
    Vector4 color;
    float lifeTime;
    float currentTime;

};
struct  ParticleForGPU    //GPU에 넘길 인스턴싱용 데이터 (WVP, 색상 등)
{
    Matrix4x4 WVP;
    Matrix4x4 World;
    Vector4 color;
};
struct Emitter          //파티클을 방출하는 발사기(Emitter) 상태 (위치, 속도, 색상, 생존시간 등)
{
    Transform transform;
    uint32_t count;
    float frequency;
    float frequencyTime;
};
struct ParticleGroup {         //파티클들의 묶음. 텍스처별로 나눔
    std::string textureFilePath;
    uint32_t textureIndex;
    std::list<Particle> particles;         // 파티클 리스트(particle list)
    int instanceSRVIndex;                  // 인스턴싱 데이터용 SRV 인덱스(SRV index for instancing data)
    ComPtr<ID3D12Resource> instanceBuffer; // 인스턴스 데이터 리소스(Instance data resource)
    int instanceCount;                     // 인스턴스 개수(Number of instances)
    ParticleForGPU* mappedInstanceData;    // GPU 메모리에 매핑된 데이터 포인터(Data pointer mapped to GPU memory)
    Emitter emitter;                       // 발사기(Emitter) 상태
};

const int32_t initialInstanceCount = 100;

using ParticleGenerator = std::function<Particle(std::mt19937&, const Vector3&)>;

class ParticleManager {
public:
    void Initialize(DirectXCommon* directXCommon, SrvManager* srvManager);
    void Update();
    void Draw();
    void calculationBillboardMatrix();

    Particle MakeRandomParticle(std::mt19937& randomEngine, const Vector3& translate);

    Particle MakeNewParticle(
        const Vector3& position,
        const Vector3& velocity,
        const Vector3& acceleration,
        const Vector3& scale,
        const Vector3& rotation,
        const Vector4& color,
        float lifeTime
    );

    std::list<Particle> Emit(const std::string& groupName, const Emitter& emitter, std::mt19937& randomEngine);

    void Emit(const std::string& groupName, const Particle& particle);

    void CreateParticleGroup(const std::string& name, const std::string& textureFilePath);

    void SetInitialInstanceCount(uint32_t count) { initialInstanceCount_ = count; }

    void SetCamera(Camera* camera) { camera_ = camera; }

    void SetUseBillboard(bool use) { useBillboard_ = use; }
    bool GetUseBillboard() const { return useBillboard_; }

    bool useRingMesh_ = false; // 기본은 정사각형
    bool useCylinderMesh_ = false; // 원통 모양으로 변경
    void SetUseRingMesh(bool flag);
    void SetUseCylinderMesh(bool flag);


    void SetEmitterPosition(const std::string& name, const Vector3& pos);

    void SetEmitterFrequency(const std::string& name, float freq);
    void SetEmitterCount(const std::string& name, uint32_t count);

    void RegisterGenerator(const std::string& name, const ParticleGenerator& generator);

private:


    ComPtr<ID3D12Resource> CreateBufferResource(ComPtr <ID3D12Device> device, size_t sizeInBytes);
    IDxcBlob* CompileShader(const std::wstring& filePath, const wchar_t* profile, IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler);

    void CreateRootSignature();
    void CreateGraphicsPipeline();
    void InitializeVertices();
    void CreateVertexBuffer();

    ComPtr<ID3D12RootSignature> rootSignature;
    ComPtr<ID3D12PipelineState> graphicsPipelineState;

    DirectXCommon* dxCommon_ = nullptr;
    SrvManager* srvManager_ = nullptr;
    SpriteCommon* spriteCommon_ = nullptr;
    const Camera* camera_ = nullptr;
    std::vector<ParticleVertex> vertices_;
    ComPtr<ID3D12Resource> vertexBufferResource_;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
    //uint32_t numInstance = 0;


    std::unordered_map<std::string, ParticleGroup> particleGroups;


    const uint32_t kNumMaxInstance = 100;
    Matrix4x4 billboardMatrix;

    bool useBillboard_ = true;

    const int32_t initialInstanceCount = 100;
    uint32_t initialInstanceCount_ = 100;

    std::mt19937 randomEngine_;

    std::unordered_map<std::string, ParticleGenerator> generators_;

};