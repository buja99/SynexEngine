#pragma once
#include "DirectXCommon.h"
#include <memory>
class ModelCommon
{

public:

	void Initialize(DirectXCommon* dxCommon);

	void Finalize();

	DirectXCommon* GetDxCommon() const { return dxCommon_; }



private:
		
		DirectXCommon* dxCommon_;

};

