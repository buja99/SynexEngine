#pragma once

struct GrayscaleSettings {
    float strength;
    float pad[3]; // 16바이트 정렬
};
struct VignetteSettings {
	float vignetteStrength;
    float pad[3];
};

struct RadialBlurSettings {
    float centerX;       // float 4바이트
    float centerY;       // float 4바이트
    float blurStrength;  // float 4바이트
    int   numSamples;    // int   4바이트
    float pad[3];
};