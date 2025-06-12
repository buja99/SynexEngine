#include "GameScene.h"
#include "SceneManager.h"
#include "ImGuiManager.h"


GameScene::~GameScene() {
}



void GameScene::Initialize() {

	audio_ = Sound::GetInstance();
	input_ = Input::GetInstance();


	TextureManager::GetInstance()->LoadTexture("resources/back.png");


	back_ = std::make_unique<Sprite>();
	back_->Initialize(SpriteCommon::GetInstance(), "resources/back.png");
	back_->SetPosition({ 0.0f, 0.0f });









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
camera_->SetEye({ 0.0f, 4.0f, -10.0f });
camera_->SetTarget({ 0.0f, 0.0f, 0.0f });

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
	// file_name이 같은 Object3d가 이미 존재하는지 확인
	auto it = std::find_if(levelObjects_.begin(), levelObjects_.end(),
		[&objectData](const std::unique_ptr<Object3d>& obj) {
			return obj->GetModelName() == objectData.fileName;
		});

	if (it != levelObjects_.end()) {
		// 🔁 기존 Object3d가 존재하면 transform만 덮어씌움
		(*it)->SetTranslate(objectData.translation);
		(*it)->SetRotate(objectData.rotation);
		(*it)->SetScale(objectData.scaling);
	}
	// ❌ 존재하지 않는 경우는 무시 (생성하지 않음)
}

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

	ImGui::End();
#endif


	back_->Update();




	camera_->UpdateMatrix();
	camera_->Update();
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

	player_->Updata();
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

	treeModel_->Draw(); 
		treeModel2_->Draw();

	weedsModel_->Draw();
	weedsModel2_->Draw();

	player_->Draw();

	SpriteCommon::GetInstance()->Set3DOverlayPipeline();
	

}

void GameScene::Finalize() {
	/*for (int i = 0; i < kPlayerPartCount; ++i) {

		playerParts_[i]->Cleanup();
		playerParts_[i].reset();
		playerTransforms_[i]->Cleanup();
		playerTransforms_[i].reset();
	}*/

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

