#include "TitleScene.h"
#include "SceneManager.h"

TitleScene::~TitleScene() {
	Finalize();
}

void TitleScene::Initialize() {

	audio_ = Sound::GetInstance();
	input_ = Input::GetInstance();

	/*TextureManager::GetInstance()->LoadTexture("resources/title.png");

	title = std::make_unique<Sprite>();
	title->Initialize(SpriteCommon::GetInstance(), "resources/title.png");*/
	/*title->SetPosition(Vector2(0.0f, 0.0f));
	title->SetSize(Vector2(1280.0f, 720.0f));
	title->SetTextureLeftTop(Vector2(0.0f, 0.0f));
	title->SetTextureSize(Vector2(1280.0f, 720.0f));*/
}

void TitleScene::Update() {

	//title->Update();

	if (input_->TriggerKey(DIK_SPACE)) {
		sceneManager_->ChangeScene("GAME");

		return;
	}

}

void TitleScene::Draw() {

	SpriteCommon::GetInstance()->CommonDrawSettings();
	//title->Draw();	
}

void TitleScene::Finalize() {

	title.reset();
}
