#pragma once
#include "Vector3.h"
#include "Matrix4x4.h"
#include <wrl.h>
#include <d3d12.h>

using Microsoft::WRL::ComPtr;


/// <summary>オブジェクトのスケール・回転・平行移動からワールド行列を生成しGPUへ転送するクラス。</summary>
class WorldTransform {
public:
    WorldTransform();
    ~WorldTransform();
    // 定数バッファ用データ構造体
    struct ConstBufferDataWorldTransform {
        Matrix4x4 matWorld;
    };

    // メンバ変数
    Vector3 scale_ = { 1.0f, 1.0f, 1.0f };
    Vector3 rotate_ = { 0.0f, 0.0f, 0.0f };
    Vector3 translate_ = { 0.0f, 0.0f, 0.0f };
    Matrix4x4 matWorld_;
    WorldTransform* parent_ = nullptr;

private:
    ComPtr<ID3D12Resource> constBuffer_;
    ConstBufferDataWorldTransform* constMap = nullptr;

public:
    // 初期化処理
    void Initialize();

    // 行列更新処理
    void UpdateMatrix();

    // GPUにデータ転送
    void TransferMatrix();

	// 親ワールド行列の設定
    void SetParent(WorldTransform* parent);

    // ワールド行列の取得
    const Matrix4x4& GetWorldMatrix() const { return matWorld_; }

    // 定数バッファ取得（描画などで使う場合）
    ComPtr<ID3D12Resource> GetConstBuffer() const { return constBuffer_.Get(); }

    void Cleanup();
};
//
//// 定数バッファ生成関数の宣言（実装은 보통別ファイル에서 따로 작성함）
//ComPtr<ID3D12Resource> CreateBufferResource(size_t size);