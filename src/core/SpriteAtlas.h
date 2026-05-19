#pragma once
// SpriteAtlas — load external BMP sprite atlas into CPU-side pixel buffer
// Clean-room design: reads BMP from file system at runtime

#include "GameTypes.h"
#include <vector>
#include <string>
#include <cstdint>

namespace inkball {

class SpriteAtlas {
public:
    int32_t width = 0;
    int32_t height = 0;
    std::vector<uint32_t> pixels;  // 32-bit BGRA (converted from 24-bit BGR)

    bool LoadFromFile(const std::wstring& path);

    // sample a pixel at (px, py), returns 32-bit BGRA
    uint32_t Sample(int32_t px, int32_t py) const {
        if (px < 0 || px >= width || py < 0 || py >= height) return 0;
        return pixels[static_cast<size_t>(py) * width + px];
    }

    void Clear();
};

} // namespace inkball
