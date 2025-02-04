#include "TitleScene.h"

TitleScene::TitleScene() : input_(nullptr), titleSprite_(nullptr) {}

TitleScene::~TitleScene() {
	delete titleSprite_;
}

void TitleScene::Initialize(SpriteCommon* spriteCommon, Input* input) {
	input_ = input;

	titleSprite_ = new Sprite();
	titleSprite_->Initialize(spriteCommon, "resources/title.png");
}

void TitleScene::Update() {
	input_->Update();

	// 스페이스 키를 누르면 게임 씬으로 변경
	if (input_->PushKey(DIK_SPACE)) {
		SceneManager::GetInstance()->ChangeScene(SceneType::Game);
	}
}

void TitleScene::Draw() {
	titleSprite_->Draw();
}
