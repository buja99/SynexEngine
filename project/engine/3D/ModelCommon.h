#pragma once
#include "DirectXCommon.h"
class ModelCommon
{

public:

	void Initialize(DirectXCommon* dxCommon);



	DirectXCommon* GetDxCommon() const { return dxCommon_; }



private:
		
		DirectXCommon* dxCommon_;

};

