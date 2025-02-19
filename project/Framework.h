#pragma once
#include <Windows.h>
#include "WinApp.h"
#include "Input.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Sprite.h"
#include "SpriteCommon.h"
#include "TextureManager.h"
#include "Object3d.h"
#include "Object3dCommon.h"
#include "ModelCommon.h"
#include "SrvManager.h"
#include "ParticleManager.h"
#include "Sound.h"

class Framework {
public:
	virtual ~Framework() = default;

	virtual void Initialize();

	virtual void Finalize();

	virtual void Update();

	virtual void Draw() = 0;

	virtual bool GetIsEndReqest() { return isEndReqest_; }


	void Run();

protected:
	bool isEndReqest_ = false;
};

