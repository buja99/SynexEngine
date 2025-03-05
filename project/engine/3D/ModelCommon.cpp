#include "ModelCommon.h"

void ModelCommon::Initialize(DirectXCommon* dxCommon)
{

	dxCommon_ = dxCommon;

}

void ModelCommon::Finalize() {
	dxCommon_ = nullptr;
}
