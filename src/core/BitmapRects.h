#pragma once
// BitmapRects — sprite-ID to atlas-pixel-rectangle lookup table
// Atlas: 381×400, cells of 42×42 in a 9×9 grid (81 cells)
// IDs that exceed 80 wrap modulo 81; this is clean-room behaviour
// and may differ from the original layout.

#include "GameTypes.h"
#include "MathUtils.h"
#include <array>

namespace inkball {

class BitmapRects {
public:
    static constexpr int32_t kSpriteSize = 42;
    static constexpr int32_t kCols       = 9;
    static constexpr int32_t kRows       = 9;
    static constexpr int32_t kMaxCells   = kCols * kRows;   // 81
    static constexpr int32_t kMaxSpriteId = 144;

    // Maps sprite ID (0..kMaxSpriteId) to a source rectangle in the atlas.
    // Uses a pre-built table for the standard ranges and wraps for out-of-range.
    static Rect GetSpriteRect(int32_t spriteId) {
        if (spriteId < 0) spriteId = 0;
        if (spriteId > kMaxSpriteId) spriteId = spriteId % (kMaxSpriteId + 1);

        // Simple grid mapping: spriteId -> (col, row) within 81-cell grid
        int32_t cell = spriteId % kMaxCells;
        int32_t col  = cell % kCols;
        int32_t row  = cell / kCols;
        int32_t left = col * kSpriteSize;
        int32_t top  = row * kSpriteSize;
        return { left, top, left + kSpriteSize, top + kSpriteSize };
    }

    static int32_t SpriteSize()  { return kSpriteSize; }
    static int32_t MaxSpriteId() { return kMaxSpriteId; }
};

} // namespace inkball
