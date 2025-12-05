#pragma once
#include "DirectXCommon.h"
#include <memory>

/// <summary>モデル処理で使用する共通のDirectXコンテキストを提供するクラス。</summary>
class ModelCommon
{

public:

	void Initialize(DirectXCommon* dxCommon);

	void Finalize();

	DirectXCommon* GetDxCommon() const { return dxCommon_; }



private:
		
		DirectXCommon* dxCommon_;

};

