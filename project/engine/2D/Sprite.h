#pragma once
#include "MyMath.h"
#include <wrl.h>
#include "WinApp.h"
#include <Windows.h>
#include "DirectXTex.h"
#include "Transform.h"
#include "Material.h"
#include "VertexData.h"
//#include "Lighting.h"
#include "TextureManager.h"

class SpriteCommon;

using Microsoft::WRL::ComPtr; 

/// <summary>2Dスプライトの変換・描画処理を行うクラス </summary>
class Sprite
{

public:
    /// <summary>
    /// スプライト描画に必要なGPUリソースを生成し初期化
    /// </summary>
    /// <param name="common">スプライト共通描画管理クラス </param>
    /// <param name="texturePath">使用するテクスチャのパス</param>
	void Initialize(SpriteCommon* common, const std::string& texturePath);
	void Update();
	void Draw();

    void SetPosition(const Vector2& pos) { position_ = pos; }
    void SetSize(const Vector2& size) { size_ = size; }
    void SetRotation(float rot) { rotation_ = rot; }
    void SetAnchorPoint(const Vector2& anchor);
    void SetColor(const Vector4& color) { material_->color = color_; color_ = color; }
    void SetZ(float z) { z_ = z; }
    void SetTextureSize();

    void SetFlipX(bool flag) { isFlipX_ = flag; }
    void SetFlipY(bool flag) { isFlipY_ = flag; }
private:

	void CreateVertexBuffer();
	void CreateIndexBuffer();
	void CreateMaterialBuffer();
	void CreateMatrixBuffer();
	void UpdateVertices();
	void UpdateMatrix();


    // 상태 값
    Vector2 position_ = {};
    Vector2 size_ = { 100, 100 };
    Vector2 anchor_ = { 0.0f, 0.0f };
    float rotation_ = 0.0f;
    float z_ = 0.0f;
    Vector4 color_ = { 1, 1, 1, 1 };

    bool anchorManuallySet_ = false;
    bool isFlipX_ = false;
    bool isFlipY_ = false;

    // 렌더링 리소스
    ComPtr<ID3D12Resource> vertexBuffer_;
    ComPtr<ID3D12Resource> indexBuffer_;
    ComPtr<ID3D12Resource> materialBuffer_;
    ComPtr<ID3D12Resource> matrixBuffer_;

    SpriteVertexData* vertex_ = nullptr;
    uint32_t* index_ = nullptr;
    SpriteMaterial* material_ = nullptr;
    SpriteMatrix* matrix_ = nullptr;

    D3D12_VERTEX_BUFFER_VIEW vbView_{};
    D3D12_INDEX_BUFFER_VIEW ibView_{};

    std::string texturePath_;
    SpriteCommon* spriteCommon_ = nullptr;
	

	
};

