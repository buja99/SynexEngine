#pragma once

#include "BaseScene.h"
#include <string>
#include <memory>

/// <summary>シーン生成を抽象化するためのファクトリ基底クラス。</summary>
class AbstractSceneFactory {
public:
	/// 仮想デストラクタ
	virtual ~AbstractSceneFactory() = default;

	/// シーン生成
	virtual std::unique_ptr<BaseScene> CreateScene(const std::string& sceneName) = 0;
};