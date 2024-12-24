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

//struct ModelData
//{
//	std::vector<VertexData> vertices;
//	MaterialData material;
//};

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
	MaterialData material;                   // 텍스처 및 소재 데이터
	std::list<Particle> particles;           // 파티클 리스트
	int srvIndex;                            // Shader Resource View 인덱스
	ComPtr<ID3D12Resource> instanceResource; // 인스턴싱 데이터를 저장할 GPU 리소스
	ParticleForGPU* mappedInstanceData;      // 인스턴싱 데이터에 접근할 포인터
	size_t instanceCount;                    // 인스턴스의 개수
};
struct AABB {
	Vector3 min;
	Vector3 max;
};
struct AccelerationField {
	Vector3 acceleration;
	AABB area;

};

using Microsoft::WRL::ComPtr;
class ParticleManager
{

public:
	void Initialize(DirectXCommon* directXCommon,SrvManager* srvManager, std::mt19937* randomEngine);


	void CommonDrawSettings();
	void CreateRootSignature();
	void CreateGraphicsPipeline();
	void InitializeVertexBuffer();
	//void UpdateVertexBuffer();

	void Updata();
	void Draw();

	IDxcBlob* CompileShader(
		const std::wstring& filePath,
		const wchar_t* profile,
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcCompiler,
		IDxcIncludeHandler* includeHandler);
	ComPtr<ID3D12Resource> CreateBufferResource(ComPtr <ID3D12Device> device, size_t sizeInBytes);

	Particle MakeNewParticle(std::mt19937& randomEngine, const Vector3& translate);

	void CreateParticleGroup(const std::string name, const std::string textureFilePath);

	bool IsCollision(const AABB& aabb, const Vector3& point);

	
private:
	DirectXCommon* dxCommon_ = nullptr;
	SrvManager* srvManager_ = nullptr;
	std::mt19937* randomEngine_ = nullptr;

	ComPtr<ID3D12Device> device;
	ComPtr<ID3D12RootSignature> rootSignature;
	ComPtr<ID3D12PipelineState> graphicsPipelineState;

	ComPtr<ID3D12Resource> vertexBufferResource_; 
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;  

	std::unordered_map<std::string, ParticleGroup> particleGroups_;

	MyMath* myMath = new MyMath;
	Camera* camera = new Camera;

	AccelerationField accelerationField
	{
		{ 0.0f, 3.0f, 0.0f }, // acceleration
	{ { -1.0f, -1.0f, -1.0f }, { 1.0f, 1.0f, 1.0f } } // AABB area (min, max)
	};
	const float kDeltaTime = 1.0f / 60.0f;
	Matrix4x4 backToFrontMatrix = myMath->MakeRotateYMatrix(std::numbers::pi_v<float>);
	Transform transform{ {1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f} };
	TransformationMatrix* transformationMatrixData = nullptr;
	static const size_t kNumMaxInstance = 100;
	ModelData modelData;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
};

