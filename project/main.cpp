#include "MyGame.h"
#include "LeakCheck.h"
#include <Windows.h>


int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

	LeakCheck leakCheck;

	std::unique_ptr<MyGame> myGame = std::make_unique<MyGame>();

	myGame->Run();


	return 0;
}
