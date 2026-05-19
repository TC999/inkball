// Display — software compositor with sprite atlas rendering
#include "Display.h"
#include "BoardObject.h"
#include "SpriteAtlas.h"
#include "BitmapRects.h"
#include <Windows.h>
#include <algorithm>
#include <cstring>

namespace inkball {

Display::Display() = default;

Display::~Display() {
    Shutdown();
}

bool Display::Initialize(void* hWnd, int32_t width, int32_t height) {
    m_hWnd = hWnd;
    m_width = width;
    m_height = height;

    if (!CreateWindowedDisplay(width, height)) return false;

    // try to load the sprite atlas from assets folder
    m_isReady = true;
    return true;
}

void Display::Shutdown() {
    m_atlas.reset();
    m_boardSurface.pixels.clear();
    m_inkSurface.pixels.clear();
    m_backSurface.pixels.clear();
    m_isReady = false;
    m_hWnd = nullptr;
}

bool Display::CreateWindowedDisplay(int32_t width, int32_t height) {
    m_width = width;
    m_height = height;
    return CreateOffscreenSurfaces();
}

bool Display::CreateOffscreenSurfaces() {
    m_boardSurface.width  = m_width;
    m_boardSurface.height = m_height;
    m_boardSurface.pixels.resize(m_width * m_height, 0xFF000000);

    m_inkSurface.width  = m_width;
    m_inkSurface.height = m_height;
    m_inkSurface.pixels.resize(m_width * m_height, 0x00000000);

    m_backSurface.width  = m_width;
    m_backSurface.height = m_height;
    m_backSurface.pixels.resize(m_width * m_height, 0xFF000000);
    return true;
}

bool Display::RestoreAllSurfaces() {
    CreateOffscreenSurfaces();
    return true;
}

// --- sprite atlas ---
bool Display::LoadSpriteAtlas(const std::wstring& path) {
    auto atlas = std::make_unique<SpriteAtlas>();
    if (!atlas->LoadFromFile(path)) return false;
    m_atlas = std::move(atlas);
    return true;
}

// --- frame lifecycle ---
void Display::BeginFrame() {
    std::memset(m_backSurface.pixels.data(), 0,
        m_backSurface.pixels.size() * sizeof(uint32_t));
}

void Display::EndFrame() {
    // composite board → back
    for (int32_t y = 0; y < m_height; ++y) {
        for (int32_t x = 0; x < m_width; ++x) {
            int32_t idx = y * m_width + x;
            uint32_t px = m_boardSurface.pixels[idx];
            // skip fully-transparent (alpha == 0) pixels
            if ((px >> 24) != 0) {
                m_backSurface.pixels[idx] = px;
            }
        }
    }

    // composite ink → back
    for (int32_t y = 0; y < m_height; ++y) {
        for (int32_t x = 0; x < m_width; ++x) {
            int32_t idx = y * m_width + x;
            uint32_t px = m_inkSurface.pixels[idx];
            if ((px & 0x00FFFFFF) != 0x00000000) {
                m_backSurface.pixels[idx] = px;
            }
        }
    }
}

void Display::Present() {
    if (!m_hWnd) return;

    HDC hdc = GetDC(static_cast<HWND>(m_hWnd));
    if (!hdc) return;

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize        = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth       = m_width;
    bmi.bmiHeader.biHeight      = -m_height;
    bmi.bmiHeader.biPlanes      = 1;
    bmi.bmiHeader.biBitCount    = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    SetDIBitsToDevice(hdc, 0, 0, m_width, m_height,
        0, 0, 0, m_height,
        m_backSurface.pixels.data(), &bmi, DIB_RGB_COLORS);

    ReleaseDC(static_cast<HWND>(m_hWnd), hdc);
}

// --- drawing ---
void Display::FillRect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color) {
    int32_t x1 = std::max(x, 0);
    int32_t y1 = std::max(y, 0);
    int32_t x2 = std::min(x + w, m_width);
    int32_t y2 = std::min(y + h, m_height);

    for (int32_t py = y1; py < y2; ++py) {
        size_t row = static_cast<size_t>(py) * m_width;
        for (int32_t px = x1; px < x2; ++px) {
            m_boardSurface.pixels[row + px] = color;
        }
    }
}

// Draw a sprite from the atlas onto the board surface.
// Applies magenta (0xFF00FF) transparency key.
void Display::DrawSprite(int32_t destX, int32_t destY, int32_t spriteId) {
    if (!m_atlas) {
        // fallback: colored rectangle
        static const uint32_t kFallbackColors[] = {
            0xFF808080, 0xFF0000FF, 0xFF00FF00, 0xFFFF0000,
            0xFFFFFF00, 0xFFFF00FF, 0xFFFFA500, 0xFFA0A0A0
        };
        FillRect(destX, destY, 42, 42, kFallbackColors[spriteId % 8]);
        return;
    }

    Rect src = BitmapRects::GetSpriteRect(spriteId);

    for (int32_t dy = 0; dy < src.Height(); ++dy) {
        int32_t screenY = destY + dy;
        if (screenY < 0 || screenY >= m_height) continue;

        size_t row = static_cast<size_t>(screenY) * m_width;
        int32_t atlasY = src.top + dy;

        for (int32_t dx = 0; dx < src.Width(); ++dx) {
            int32_t screenX = destX + dx;
            if (screenX < 0 || screenX >= m_width) continue;

            int32_t atlasX = src.left + dx;
            uint32_t px = m_atlas->Sample(atlasX, atlasY);

            // skip magenta transparency
            if (px == m_transparentColor) continue;

            m_boardSurface.pixels[row + screenX] = px;
        }
    }
}

void Display::DrawObject(const BoardObject* obj, int32_t spriteId) {
    if (!obj) return;
    DrawSprite(static_cast<int32_t>(obj->PositionX()),
               static_cast<int32_t>(obj->PositionY()),
               spriteId);
}

void Display::SetColorKey(uint32_t lowColor, uint32_t highColor) {
    m_colorKeyLow  = lowColor;
    m_colorKeyHigh = highColor;
}

Rect Display::GetBoardRect() const {
    return { 0, 0, m_width, m_height };
}

void Display::BltBoardToInk() {
    m_inkSurface.pixels = m_boardSurface.pixels;
}

} // namespace inkball
