#pragma once
#include "Framework.h"

class MyGame : public Framework {

public:
	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;


};