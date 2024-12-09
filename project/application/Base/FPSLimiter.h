#pragma once
#include <chrono>
class FPSLimiter
{

public:

	void InitializeFixFPS();

	void UpdateFixFPS();



private:

	
	std::chrono::steady_clock::time_point reference_;


};

