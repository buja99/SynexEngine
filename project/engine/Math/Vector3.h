#pragma once

/// <summary>3次元ベクトル</summary>
struct Vector3 final {
    float x;
    float y;
    float z;

    float& operator[](int index) {
        switch (index) {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        default: return x; // 기본값 (예외 대신 첫 요소 반환)
        }
    }

    const float& operator[](int index) const {
        switch (index) {
        case 0: return x;
        case 1: return y;
        case 2: return z;
        default: return x;
        }
    }
};