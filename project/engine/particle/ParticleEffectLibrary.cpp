#include "ParticleEffectLibrary.h"
#include <numbers>

void ParticleEffectLibrary::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager, Camera* camera) {
    // primitive
    primitive_ = std::make_unique<ParticleManager>();
    primitive_->Initialize(dxCommon, srvManager);
    primitive_->SetCamera(camera);
    primitive_->SetUseBillboard(false);
	primitive_->SetUseCylinderMesh(false); // 부드러운 원형 표현
    primitive_->SetUseRingMesh(false);
    primitive_->CreateParticleGroup("primitive", "resources/circle.png");

    primitive_->RegisterGenerator("primitive", [this](std::mt19937& randomEngine, const Vector3& translate) {
        std::uniform_real_distribution<float> distRotate(-std::numbers::pi_v<float>, std::numbers::pi_v<float>);
        std::uniform_real_distribution<float> distScale(0.4f, 1.5f);

        Particle p;
        p.transform.translate = translate;
        p.transform.scale = { 0.05f, distScale(randomEngine), 1.0f };
        p.transform.rotate = { 0, 0, distRotate(randomEngine) };
        p.velocity = {};
        p.acceleration = {};
        p.color = { 1, 1, 1, 1 };
        p.lifeTime = 0.3f;
        p.currentTime = 0.0f;
        return p;
        });

    primitiveEmitter_.count = 12;
    primitiveEmitter_.frequency = 0.0f;

    // ring
    ring_ = std::make_unique<ParticleManager>();
    ring_->Initialize(dxCommon, srvManager);
    ring_->SetCamera(camera);
    ring_->SetUseBillboard(false);
	ring_->SetUseCylinderMesh(false); // 부드러운 원형 표현
    ring_->SetUseRingMesh(true);
    ring_->CreateParticleGroup("ring", "resources/gradationLine.png");

    ring_->RegisterGenerator("ring", [this](std::mt19937& randomEngine, const Vector3& translate) {
        std::uniform_real_distribution<float> distRotate(-std::numbers::pi_v<float>, std::numbers::pi_v<float>);

        Particle p;
        p.transform.translate = { 0, 5, 0 };
        p.transform.scale = { 3, 3, 3 };
        p.transform.rotate = {
            distRotate(randomEngine),
            distRotate(randomEngine),
            distRotate(randomEngine)
        };
        p.velocity = {};
        p.acceleration = {};
        p.color = { 1, 1, 1, 1 };
        p.lifeTime = 1.0f;
        p.currentTime = 0.0f;
        return p;
        });

    ringEmitter_.count = 5;
    ringEmitter_.frequency = 0.0f;

	// cylinder
    cylinder_ = std::make_unique<ParticleManager>();
    cylinder_->Initialize(dxCommon, srvManager);
    cylinder_->SetCamera(camera);
    cylinder_->SetUseBillboard(true);
	cylinder_->SetUseCylinderMesh(true);
    cylinder_->SetUseRingMesh(false); // 부드러운 원형 표현
    cylinder_->CreateParticleGroup("cylinder", "resources/gradationLine.png");

    cylinder_->RegisterGenerator("cylinder", [this](std::mt19937& randomEngine, const Vector3& translate) {
       
        std::uniform_real_distribution<float> distColor(0.3f, 1.0f);

        Particle p;
        p.color = {
            distColor(randomEngine),
            distColor(randomEngine),
            distColor(randomEngine),
            1.0f
        };
        p.transform.rotate = { 0.0f, 0.0f, 0.0f };
        p.transform.scale = { 3.0f, 3.0f, 3.0f };
        p.transform.translate = { translate };
        p.velocity = { 0.0f,0.0f,0.0f };
        
        p.lifeTime = 1.0f;
        p.currentTime = 0.0f;
        return p;
        });

    cylinderEmitter_.count = 1;
    cylinderEmitter_.frequency = 0.7f;
    cylinderEmitter_.frequencyTime = 0.0f;

    // longLine
    longLine_ = std::make_unique<ParticleManager>();
    longLine_->Initialize(dxCommon, srvManager);
    longLine_->SetCamera(camera);
    longLine_->SetUseBillboard(false);
    longLine_->CreateParticleGroup("longLine", "resources/circle.png");

    longLine_->RegisterGenerator("longLine", [this](std::mt19937& randomEngine, const Vector3& translate) {
        Particle p;
        p.transform.translate = translate;
        p.transform.scale = { 0.1f, 2.5f, 1.0f };
        p.transform.rotate = { 0.0f, 0.0f, 0.0f }; // 필요 시 회전 추가
        p.velocity = {};
        p.acceleration = {};
        p.color = { 1, 1, 1, 1 };
        p.lifeTime = 0.3f;
        p.currentTime = 0.0f;
        return p;
        });
    longLineEmitter_.count = 1;
    longLineEmitter_.frequency = 0.0f;

    // gradationRing
    gradationRing_ = std::make_unique<ParticleManager>();
    gradationRing_->Initialize(dxCommon, srvManager);
    gradationRing_->SetCamera(camera);
    gradationRing_->SetUseBillboard(false);
    gradationRing_->SetUseRingMesh(true);
    gradationRing_->CreateParticleGroup("gradationRing", "resources/gradationLine.png");

    gradationRing_->RegisterGenerator("gradationRing", [this](std::mt19937& randomEngine, const Vector3& center) {
        std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * std::numbers::pi_v<float>);
        float angle = angleDist(randomEngine);
        float radius = 2.5f;

        Particle p;
        p.transform.translate = {
            center.x + std::cos(angle) * radius,
            center.y,
            center.z + std::sin(angle) * radius
        };
        p.transform.scale = { 0.3f, 1.0f, 1.0f };
        p.transform.rotate = { 0.0f, -angle, 0.0f };
        p.velocity = {};
        p.acceleration = {};
        p.color = { 1, 1, 1, 1 };
        p.lifeTime = 1.0f;
        p.currentTime = 0.0f;
        return p;
        });
    gradationRingEmitter_.count = 12;
    gradationRingEmitter_.frequency = 0.0f;
}

void ParticleEffectLibrary::Update() {

   
    if (usePrimitiveAutoEmit_) {
        primitiveElapsedTime_ += 1.0f / 60.0f;
        if (primitiveElapsedTime_ >= primitiveInterval_) {
            primitiveElapsedTime_ = 0.0f;
            EmitPrimitive(primitiveEmitter_.transform.translate, randomEngine_);
        }
    }

    if (useRingAutoEmit_) {
        ringElapsedTime_ += 1.0f / 60.0f;
        if (ringElapsedTime_ >= ringInterval_) {
            ringElapsedTime_ = 0.0f;
            EmitRing(ringEmitter_.transform.translate, randomEngine_);
        }
    }

    if (useCylinderAutoEmit_) {
        cylinderElapsedTime_ += 1.0f / 60.0f;
        if (cylinderElapsedTime_ >= cylinderInterval_) {
            cylinderElapsedTime_ = 0.0f;
            EmitCylinder(cylinderEmitter_.transform.translate, randomEngine_);
        }
    }

    primitive_->Update();
    ring_->Update();
	cylinder_->Update();
    longLine_->Update();
    gradationRing_->Update();
}

void ParticleEffectLibrary::Draw() {
    primitive_->Draw();
    ring_->Draw();
    cylinder_->Draw();
    longLine_->Draw();
    gradationRing_->Draw();
}

void ParticleEffectLibrary::DrawPrimitive() {
    if (primitive_) {
        primitive_->Draw();
    }
}

void ParticleEffectLibrary::DrawRing() {
    if (ring_) {
        ring_->Draw();
    }
}

void ParticleEffectLibrary::DrawCylinder() {
    if (cylinder_) {
        cylinder_->Draw();
    }
}

void ParticleEffectLibrary::DrawLongLine() {
    if (longLine_) {
        longLine_->Draw();
    }
}

void ParticleEffectLibrary::DrawGradationRing() {
    if (gradationRing_) {
        gradationRing_->Draw();
    }
}

void ParticleEffectLibrary::EmitPrimitive(const Vector3& pos, std::mt19937& randomEngine) {
    primitiveEmitter_.transform.translate = pos;
    primitive_->Emit("primitive", primitiveEmitter_, randomEngine);
    //OutputDebugStringA(std::format("Emit at pos: {:.1f}, {:.1f}, {:.1f}\n", pos.x, pos.y, pos.z).c_str());
}

void ParticleEffectLibrary::EmitRing(const Vector3& pos, std::mt19937& randomEngine) {
    ringEmitter_.transform.translate = pos;
    ring_->Emit("ring", ringEmitter_, randomEngine);
}

void ParticleEffectLibrary::EmitCylinder(const Vector3& pos, std::mt19937& randomEngine) {
    cylinderEmitter_.transform.translate = pos;
	cylinder_->Emit("cylinder", cylinderEmitter_, randomEngine);
}

void ParticleEffectLibrary::EmitLongLine(const Vector3& pos, std::mt19937& randomEngine) {
    longLineEmitter_.transform.translate = pos;
    longLine_->Emit("longLine", longLineEmitter_, randomEngine);
}

void ParticleEffectLibrary::EmitGradationRing(const Vector3& pos, std::mt19937& randomEngine) {
    gradationRingEmitter_.transform.translate = pos;
    gradationRing_->Emit("gradationRing", gradationRingEmitter_, randomEngine);
}

void ParticleEffectLibrary::SetPrimitiveInterval(float interval) {
    primitiveInterval_ = interval;
}

void ParticleEffectLibrary::SetRingInterval(float interval) {
    ringInterval_ = interval;
}

void ParticleEffectLibrary::SetCylinderInterval(float interval) {
	cylinderInterval_ = interval;
}

void ParticleEffectLibrary::SetUsePrimitiveAutoEmit(bool flag) {
    usePrimitiveAutoEmit_ = flag;
}

void ParticleEffectLibrary::SetUseRingAutoEmit(bool flag) {
    useRingAutoEmit_ = flag;
}

void ParticleEffectLibrary::SetUseCylinderAutoEmit(bool flag) {
	useCylinderAutoEmit_ = flag;
}


