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



class ParticleManager {
public:
    void Initialize(DirectXCommon* directXCommon);
    void Update();
    void Draw();

    void Emit(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT4& color, float size, float lifetime);

    ComPtr<ID3D12Resource> CreateBufferResource(ComPtr <ID3D12Device> device, size_t sizeInBytes);

private:
    void CreateVertexBuffer();
    DirectXCommon* dxCommon_ = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer_;
    D3D12_VERTEX_BUFFER_VIEW vbView_;
    std::vector<ParticleVertex> vertices_;
    ComPtr<ID3D12Resource> vertexBufferResource_;
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
    MyMath* math = nullptr;
};

