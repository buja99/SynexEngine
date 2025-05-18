#include "MyGame.h"



void MyGame::Initialize() {

	Framework::Initialize();

	sceneFactory_ = std::make_unique<SceneFactory>();
	sceneManager_->SetSceneFactory(std::move(sceneFactory_));

	sceneManager_->ChangeScene("TITLE");


}

void MyGame::Finalize() {

	Framework::Finalize();

}

void MyGame::Update() {

	Framework::Update();


}

void MyGame::Draw() {

	// 1. 오프스크린 렌더링
	dxCommon_->RenderTexturePreDraw();
	srvManager_->PreDraw();
	sceneManager_->Draw();
	dxCommon_->RenderTexturePostDraw();

	// 2. 스왑체인으로 복사
	dxCommon_->CopyRenderTextureToSwapChain();

#ifdef _DEBUG
	imGuiManager_->Draw();
#endif // _DEBUG

	// 3. 최종 렌더링 완료
	dxCommon_->PostDraw();
}