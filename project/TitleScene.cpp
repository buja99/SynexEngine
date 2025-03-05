#include "TitleScene.h"
#include "SceneManager.h"

TitleScene::~TitleScene() {
}

void TitleScene::Initialize() {

	audio_ = Sound::GetInstance();
	input_ = Input::GetInstance();

}

void TitleScene::Update() {

	if (input_->TriggerKey(DIK_RETURN)) {
		sceneManager_->ChangeScene("GAME");

		return;
	}

}

void TitleScene::Draw() {
}

void TitleScene::Finalize() {
}
