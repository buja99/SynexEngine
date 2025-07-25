#pragma once

struct GrayscaleSettings {
    float strength;
    float pad[3]; // 16바이트 정렬
};
struct VignetteSettings {
	float vignetteStrength;
    float pad[3];
};