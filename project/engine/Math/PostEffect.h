#pragma once

/// <summary>グレースケール用パラメータ</summary>
struct GrayscaleSettings {
    float strength;
    float pad[3]; // 16바이트 정렬
};
/// <summary>ビネットエフェクト用パラメータ</summary>
struct VignetteSettings {
	float vignetteStrength;
    float pad[3];
};
/// <summary>ラジアルブラー用パラメータ</summary>
struct RadialBlurSettings {
    float centerX;       // float 4바이트
    float centerY;       // float 4바이트
    float blurStrength;  // float 4바이트
    int   numSamples;    // int   4바이트
    float pad[3];
};