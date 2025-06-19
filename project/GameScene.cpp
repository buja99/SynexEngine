#include "GameScene.h"
#include "SceneManager.h"
#include "ImGuiManager.h"
#include "MyMath.h"

GameScene::~GameScene() {
}



void GameScene::Initialize() {

	audio_ = Sound::GetInstance();
	input_ = Input::GetInstance();
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();

	TextureManager::GetInstance()->LoadTexture("resources/back.png");


	back_ = std::make_unique<Sprite>();
	back_->Initialize(SpriteCommon::GetInstance(), "resources/back.png");
	back_->SetPosition({ 0.0f, 0.0f });

	ground_ = std::make_unique<Ground>();
	ground_->Initialize();






ModelManager::GetInstance()->Initialize(DirectXCommon::GetInstance());

ModelManager::GetInstance()->LoadModel("resources/obj/tree", "tree.obj");
ModelManager::GetInstance()->LoadModel("resources/obj/weeds", "weeds.obj");

treeWorldTransform_ = std::make_unique<WorldTransform>();
treeWorldTransform_->Initialize();

tree2WorldTransform_ = std::make_unique<WorldTransform>();
tree2WorldTransform_->Initialize();

treeModel_ = std::make_unique<Object3d>();
treeModel_->Initialize(Object3dCommon::GetInstance(), treeWorldTransform_.get());
treeModel_->SetModel("tree.obj");

treeModel2_ = std::make_unique<Object3d>();
treeModel2_->Initialize(Object3dCommon::GetInstance(), tree2WorldTransform_.get());
treeModel2_->SetModel("tree.obj");

weedsWorldTransform_ = std::make_unique<WorldTransform>();
weedsWorldTransform_->Initialize();

weeds2WorldTransform_ = std::make_unique<WorldTransform>();
weeds2WorldTransform_->Initialize();

weedsModel_ = std::make_unique<Object3d>();
weedsModel_->Initialize(Object3dCommon::GetInstance(), weedsWorldTransform_.get());
weedsModel_->SetModel("weeds.obj");

weedsModel2_ = std::make_unique<Object3d>();
weedsModel2_->Initialize(Object3dCommon::GetInstance(), weeds2WorldTransform_.get());
weedsModel2_->SetModel("weeds.obj");

camera_ = std::make_unique<Camera>();
ground_->SetCamera(camera_.get());

treeModel_->SetCamera(camera_.get());
treeModel2_->SetCamera(camera_.get());
weedsModel_->SetCamera(camera_.get());
weedsModel2_->SetCamera(camera_.get());

	//weedsWorldTransform_->translate_ = { -10.0f, 0.0f, 0.0f };
	

player_ = std::make_unique<Player>();
player_->Initialize();

	

player_->SetCamera(camera_.get());

Object3dCommon::GetInstance()->SetDefaultCamera(camera_.get());



//json

modelTable["treeModel_"] = treeModel_.get();
modelTable["treeModel2_"] = treeModel2_.get();
modelTable["weedsModel_"] = weedsModel_.get();
modelTable["weedsModel2_"] = weedsModel2_.get();

transformTable["treeModel_"] = treeWorldTransform_.get();
transformTable["treeModel2_"] = tree2WorldTransform_.get();
transformTable["weedsModel_"] = weedsWorldTransform_.get();
transformTable["weedsModel2_"] = weeds2WorldTransform_.get();


const std::string fillpath = std::string("resources/json/") + "test1.json";

std::ifstream file;

file.open(fillpath);

if (file.fail()) {
	assert(0);
	}

nlohmann::json deserialized;
file >> deserialized;

assert(deserialized.is_object());
assert(deserialized.contains("name"));
assert(deserialized["name"].is_string());

std::string name =
deserialized["name"].get<std::string>();
assert(name.compare("scene") == 0);

LevelData* levelData = new LevelData();

for (nlohmann::json& object : deserialized["objects"]) {
	DeserializeObjectRecursive(object, levelData);
	}
for (auto& objectData : levelData->objects) {
	const std::string& key = objectData.fileName;

	if (transformTable.contains(key)) {
		WorldTransform* wt = transformTable[key];
		wt->translate_ = objectData.translation;
		wt->rotate_ = objectData.rotation;
		wt->scale_ = objectData.scaling;
	}
	}


DirectXCommon* dx = DirectXCommon::GetInstance();
dx->SetViewport(200.0f, 100.0f, 600.0f, 400.0f);
dx->SetScissorRect(200, 100, 800, 500);



}

void GameScene::Update() {


#ifdef _DEBUG
	ImGui::Begin("Model Transform");
	if (ImGui::CollapsingHeader("Tree")) {
		ImGui::DragFloat3("Tree Position", &treeWorldTransform_->translate_.x, 0.1f);
		ImGui::DragFloat3("Tree Rotation", &treeWorldTransform_->rotate_.x, 0.1f);
		ImGui::DragFloat3("Tree Scale", &treeWorldTransform_->scale_.x, 0.1f);
	}

	// Weeds Model Transform
	if (ImGui::CollapsingHeader("Weeds")) {
		ImGui::DragFloat3("Weeds Position", &weedsWorldTransform_->translate_.x, 0.1f);
		ImGui::DragFloat3("Weeds Rotation", &weedsWorldTransform_->rotate_.x, 0.1f);
		ImGui::DragFloat3("Weeds Scale", &weedsWorldTransform_->scale_.x, 0.1f);
	}


	// 🆕 Viewport / Scissor 설정
	static float viewportX = 200.0f;
	static float viewportY = 100.0f;
	static float viewportW = 600.0f;
	static float viewportH = 400.0f;

	ImGui::DragFloat("Viewport X", &viewportX, 1.0f);
	ImGui::DragFloat("Viewport Y", &viewportY, 1.0f);
	ImGui::DragFloat("Viewport W", &viewportW, 1.0f);
	ImGui::DragFloat("Viewport H", &viewportH, 1.0f);

	// 적용
	DirectXCommon::GetInstance()->SetViewport(viewportX, viewportY, viewportW, viewportH);
	DirectXCommon::GetInstance()->SetScissorRect(
		(int)viewportX, (int)viewportY,
		(int)(viewportX + viewportW), (int)(viewportY + viewportH));







	ImGui::End();
#endif


	back_->Update();

	ground_->Update();

	player_->Updata();

	Vector3 playerPos = player_->GetPosition();
	Vector3 eye = MyMath::Add( playerPos , Vector3{0.0f, 8.0f, -15.0f}); // 위에서 바라보는 시점
	Vector3 target = playerPos;

	camera_->SetEye(eye);
	camera_->SetTarget(target);
	camera_->SetUp({ 0.0f, 1.0f, 0.0f }); // 기본 업벡터
	camera_->UpdateMatrix();

	treeModel_->Update();
	treeModel2_->Update();
	weedsModel_->Update();
	weedsModel2_->Update();
	


	treeWorldTransform_->UpdateMatrix();
	treeWorldTransform_->TransferMatrix();

	tree2WorldTransform_->UpdateMatrix();
	tree2WorldTransform_->TransferMatrix();

	weedsWorldTransform_->UpdateMatrix();
	weedsWorldTransform_->TransferMatrix();

	weeds2WorldTransform_->UpdateMatrix();
	weeds2WorldTransform_->TransferMatrix();

	
	if (input_->TriggerKey(DIK_R)) {
		sceneManager_->ChangeScene("TITLE");

		return;
	}

	if (input_->TriggerKey(DIK_A)) {
		moveObj();
		treeModel_->SetRotate({ -1.7f, 0.0f, 0.0f });
		treeModel2_->SetRotate({ -1.7f, 0.0f, 0.0f });
		weedsModel_->SetRotate({ -1.7f, 0.0f, 0.0f });
		weedsModel2_->SetRotate({ -1.7f, 0.0f, 0.0f });
	}




	


}

void GameScene::Draw() {
	SpriteCommon::GetInstance()->SetUIPipeline();
	
	back_->Draw();

	SrvManager::GetInstance()->PreDraw();
	Object3dCommon::GetInstance()->CommonDrawSettings();

	ground_->Draw();

	DirectXCommon::GetInstance()->PreDrawToOffscreen(); 
	Object3dCommon::GetInstance()->CommonDrawSettings();

	/*treeModel_->Draw(); 
	treeModel2_->Draw();

	weedsModel_->Draw();
	weedsModel2_->Draw();*/

	player_->Draw();

	SpriteCommon::GetInstance()->Set3DOverlayPipeline();
	

}

void GameScene::Finalize() {
	

	camera_.reset();
}

void GameScene::moveObj() {
	

}

void GameScene::DeserializeObjectRecursive(nlohmann::json& object, LevelData* levelData) {

	if (!object.contains("type")) return;

	std::string type = object["type"].get<std::string>();

	if (type == "MESH") {
		levelData->objects.emplace_back(LevelData::ObjectData{});
		LevelData::ObjectData& objectData = levelData->objects.back();

		if (object.contains("file_name")) {
			objectData.fileName = object["file_name"];
		}
		auto& transform = object["transform"];
		objectData.translation = {
			(float)transform["translation"][0],
			(float)transform["translation"][2],
			(float)transform["translation"][1],
		};
		objectData.rotation = {
			-(float)transform["rotation"][0],
			-(float)transform["rotation"][2],
			-(float)transform["rotation"][1],
		};
		objectData.scaling = {
			(float)transform["scaling"][0],
			(float)transform["scaling"][2],
			(float)transform["scaling"][1],
		};
	}

	// 🔁 자식이 있다면 재귀 호출
	if (object.contains("children")) {
		for (auto& child : object["children"]) {
			DeserializeObjectRecursive(child, levelData);
		}
	}
}

