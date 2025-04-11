#include "WorldTransform.h"
#include "MyMath.h"
#include <cassert>
#include <DirectXMath.h>
#include "DirectXCommon.h"
using namespace DirectX;

WorldTransform::WorldTransform() = default;

WorldTransform::~WorldTransform() {
   if (constBuffer_) {
        constBuffer_->Release();
        constBuffer_ = nullptr;
    }
    // ComPtr이 자동으로 constBuffer_를 Release()함
}

void WorldTransform::Initialize() {
    // 定数バッファの生成
    constBuffer_ = CreateBufferResource(sizeof(ConstBufferDataWorldTransform));
    constBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&constMap));
}

void WorldTransform::UpdateMatrix() {
    // スケーリング、回転、平行移動の行列を作成
    Matrix4x4 matScale = MyMath::MakeScaleMatrix(scale_);
    Matrix4x4 matRotX = MyMath::MakeRotateXMatrix(rotate_.x);
    Matrix4x4 matRotY = MyMath::MakeRotateYMatrix(rotate_.y);
    Matrix4x4 matRotZ = MyMath::MakeRotateZMatrix(rotate_.z);
    Matrix4x4 matTrans = MyMath::MakeTranslateMatrix(translate_);

    // 回転の合成（Z → Y → X の順）
    Matrix4x4 matRot = MyMath::Multiply(matRotZ, MyMath::Multiply(matRotY, matRotX));

    // ローカルワールド行列の計算（S→R→T の順で）
    matWorld_ = MyMath::Multiply(matScale, matRot);
    matWorld_ = MyMath::Multiply(matWorld_, matTrans);

    // 親がいる場合は親のワールド行列を掛ける
    if (parent_) {
        matWorld_ = MyMath::Multiply(matWorld_, parent_->matWorld_);
    }
}

void WorldTransform::TransferMatrix() {
    assert(constMap);
    constMap->matWorld = matWorld_;
}

void WorldTransform::SetParent(WorldTransform* parent) {
    parent_ = parent;
}

Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t size) {
    DirectXCommon* dxCommon = DirectXCommon::GetInstance();
    return dxCommon->CreateBufferResource(dxCommon->GetDevice(), size).Detach();
}
