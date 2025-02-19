#include "Framework.h"

void Framework::Initialize() {



}

void Framework::Finalize() {
}

void Framework::Update() {
}

void Framework::Run() {

	Initialize();

	while (true) {
		Update();

		if(GetIsEndReqest()) {
			break;
		}
		Draw();
	}
	Finalize();
}
