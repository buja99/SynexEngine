#pragma once
#include "DirectXCommon.h"
#include <memory>

/// <summary>モデル処理用の共通DirectXコンテキスト管理</summary>
class ModelCommon
{

public:

	void Initialize(DirectXCommon* dxCommon);

	void Finalize();

	DirectXCommon* GetDxCommon() const { return dxCommon_; }



private:
		
		DirectXCommon* dxCommon_;

};

