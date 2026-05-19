#pragma once
// Display — software renderer with sprite atlas support
// Manages front/back/board/ink compositing pipeline
// Clean-room design based on CDisplay spec

#include "GameTypes.h"
#include <cstdint>
#include <vector>
#include <memory>
#include <string>

namespace inkball {

class BoardObject;
class Ball;
class SpriteAtlas;

class Display {
public:
    Display();
    ~Display();

    bool Initialize(void* hWnd, int32_t width, int32_t height);
    void Shutdown();
    bool RestoreAllSurfaces();

    bool CreateWindowedDisplay(int32_t width, int32_t height);
    bool CreateOffscreenSurfaces();

    // --- sprite atlas ---
    bool LoadSpriteAtlas(const std::wstring& path);

    // --- accessors ---
    bool IsReady() const { return m_isReady && m_atlas != nullptr; }
    int32_t Width() const { return m_width; }
    int32_t Height() const { return m_height; }
    void* NativeHandle() const { return m_hWnd; }

    // --- frame lifecycle ---
    void BeginFrame();
    void EndFrame();
    void Present();

    // --- drawing ---
    void FillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);
    void DrawSprite(int32_t destX, int32_t destY, int32_t spriteId);
    void DrawObject(const BoardObject* obj, int32_t spriteId);

    void SetColorKey(uint32_t lowColor, uint32_t highColor);
    Rect GetBoardRect() const;
    void BltBoardToInk();

private:
    struct Surface {
        std::vector<uint32_t> pixels;
        int32_t width = 0;
        int32_t height = 0;
    };

    void* m_hWnd = nullptr;
    int32_t m_width = 800;
    int32_t m_height = 600;
    bool m_isReady = false;

    Surface m_boardSurface;
    Surface m_inkSurface;
    Surface m_backSurface;

    std::unique_ptr<SpriteAtlas> m_atlas;

    uint32_t m_colorKeyLow  = 0x010101;
    uint32_t m_colorKeyHigh = 0xFFFFFF;
    uint32_t m_transparentColor = 0xFFFF00FF;  // magenta = key color for atlas
};

} // namespace inkball
