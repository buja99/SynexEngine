#include "MyGame.h"



void MyGame::Initialize() {

	Framework::Initialize();

	sceneFactory_ = std::make_unique<SceneFactory>();
	sceneManager_->SetSceneFactory(sceneFactory_.get());

	sceneManager_->ChangeScene("TITLE");

}

void MyGame::Finalize() {

	Framework::Finalize();

}

void MyGame::Update() {

	Framework::Update();


}

void MyGame::Draw() {

	dxCommon_->PreDraw();
	srvManager_->PreDraw();

	sceneManager_->Draw();

#ifdef _DEBUG

	imGuiManager_->Draw();

#endif // _DEBUG

	dxCommon_->PostDraw();


}