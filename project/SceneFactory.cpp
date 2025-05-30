#include "SceneFactory.h"

std::unique_ptr<BaseScene> SceneFactory::CreateScene(const std::string& sceneName) {
	std::unique_ptr<BaseScene> newScene = nullptr;

    if (sceneName == "TITLE") {
        newScene = std::make_unique<TitleScene>();
    } else if (sceneName == "GAME") {
        newScene = std::make_unique<GameScene>();
    }
    else if (sceneName == "PARTICLE") {
        newScene = std::make_unique<ParticleScene>();
    }


    return newScene;

}
