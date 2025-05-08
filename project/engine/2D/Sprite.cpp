#include "Sprite.h"
#include "SpriteCommon.h"
#ifdef _DEBUG
#include "imgui.h"
#endif // _DEBUG
#include "SrvManager.h"
#include "ResourceUtils.h"

using namespace ResourceUtils;

void Sprite::Initialize(SpriteCommon* common, const std::string& texturePath) {
    spriteCommon_ = common;
    texturePath_ = texturePath;

    CreateVertexBuffer();
    CreateIndexBuffer();
    CreateMaterialBuffer();
    CreateMatrixBuffer();

    SetColor({ 1, 1, 1, 1 });
}

void Sprite::Update() {
    UpdateVertices();
    UpdateMatrix();
}

void Sprite::Draw() {
    auto cmd = spriteCommon_->GetDxCommon()->GetCommandList();
    cmd->IASetVertexBuffers(0, 1, &vbView_);
    cmd->IASetIndexBuffer(&ibView_);
    cmd->SetGraphicsRootConstantBufferView(0, materialBuffer_->GetGPUVirtualAddress());
    cmd->SetGraphicsRootConstantBufferView(1, matrixBuffer_->GetGPUVirtualAddress());
    cmd->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(texturePath_));
    cmd->DrawIndexedInstanced(6, 1, 0, 0, 0);
}


void Sprite::CreateVertexBuffer() {
    vertexBuffer_ = CreateBufferResource(sizeof(VertexData) * 4);
    vbView_ = { vertexBuffer_->GetGPUVirtualAddress(), sizeof(VertexData) * 4, sizeof(VertexData) };
    vertexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&vertex_));
}

void Sprite::CreateIndexBuffer() {
    indexBuffer_ = CreateBufferResource(sizeof(uint32_t) * 6);
    ibView_ = { indexBuffer_->GetGPUVirtualAddress(), sizeof(uint32_t) * 6, DXGI_FORMAT_R32_UINT };
    indexBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&index_));
    uint32_t indices[] = { 0, 1, 2, 1, 3, 2 };
    memcpy(index_, indices, sizeof(indices));
}

void Sprite::CreateMaterialBuffer() {
    materialBuffer_ = CreateBufferResource(sizeof(Material));
    materialBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&material_));
    material_->enableLighting = 0;
    material_->uvTransform = MyMath::MakeIdentity4x4();
}

void Sprite::CreateMatrixBuffer() {
    matrixBuffer_ = CreateBufferResource(sizeof(TransformationMatrix));
    matrixBuffer_->Map(0, nullptr, reinterpret_cast<void**>(&matrix_));
    matrix_->WVP = MyMath::MakeIdentity4x4();
    matrix_->World = MyMath::MakeIdentity4x4();
}

void Sprite::UpdateVertices() {
    float left = (-anchor_.x) * size_.x;
    float right = (1.0f - anchor_.x) * size_.x;
    float top = (-anchor_.y) * size_.y;
    float bottom = (1.0f - anchor_.y) * size_.y;

    vertex_[0].position = { left,  bottom, 0.0f, 1.0f };
    vertex_[1].position = { left,  top,    0.0f, 1.0f };
    vertex_[2].position = { right, bottom, 0.0f, 1.0f };
    vertex_[3].position = { right, top,    0.0f, 1.0f };

    vertex_[0].texcoord = { 0, 1 };
    vertex_[1].texcoord = { 0, 0 };
    vertex_[2].texcoord = { 1, 1 };
    vertex_[3].texcoord = { 1, 0 };

    for (int i = 0; i < 4; ++i)
        vertex_[i].normal = { 0, 0, -1 };
}

void Sprite::UpdateMatrix() {
    Matrix4x4 scale = MyMath::MakeScaleMatrix({ size_.x, size_.y, 1.0f });
    Matrix4x4 rot = MyMath::MakeRotateZMatrix(rotation_);
    Matrix4x4 trans = MyMath::MakeTranslateMatrix({ position_.x, position_.y, z_ });
    Matrix4x4 world = MyMath::Multiply(scale, MyMath::Multiply(rot, trans));

    const Matrix4x4& view = spriteCommon_->GetViewMatrix();
    const Matrix4x4& proj = spriteCommon_->GetProjectionMatrix();

    matrix_->World = world;
    matrix_->WVP = MyMath::Multiply(world, MyMath::Multiply(view, proj));
}
