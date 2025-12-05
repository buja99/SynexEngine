#pragma once
#include <chrono>

/// <summary>
/// 固定フレームレート（例：60FPS）を維持するためのシンプルなFPSリミッター。
/// フレーム経過時間を計測し、不足分をスリープして調整する。
/// </summary>

class FPSLimiter
{

public:

	void InitializeFixFPS();

	void UpdateFixFPS();



private:

	
	std::chrono::steady_clock::time_point reference_;


};

