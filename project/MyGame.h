#pragma once
#include "Framework.h"
#include "SceneFactory.h"

class MyGame : public Framework {

public:
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

private:

	std::unique_ptr<AbstractSceneFactory> sceneFactory_ = nullptr;

};
