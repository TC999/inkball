// ============================================================
// Display 实现 — 软件后备实现
// 当 DirectDraw 不可用时，使用 GDI 软件渲染作为后备
// ============================================================

#include "Display.h"
#include "BoardObject.h"
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

    if (!CreateWindowedDisplay(width, height)) {
        return false;
    }

    m_isReady = true;
    return true;
}

void Display::Shutdown() {
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
    // 分配像素缓冲区
    m_boardSurface.width = m_width;
    m_boardSurface.height = m_height;
    m_boardSurface.pixels.resize(m_width * m_height, 0xFF808080); // 灰色背景

    m_inkSurface.width = m_width;
    m_inkSurface.height = m_height;
    m_inkSurface.pixels.resize(m_width * m_height, 0x00000000); // 透明

    m_backSurface.width = m_width;
    m_backSurface.height = m_height;
    m_backSurface.pixels.resize(m_width * m_height, 0xFF000000); // 黑色

    return true;
}

bool Display::RestoreAllSurfaces() {
    CreateOffscreenSurfaces();
    return true;
}

void Display::BeginFrame() {
    // 清除后缓冲
    std::memset(m_backSurface.pixels.data(), 0,
        m_backSurface.pixels.size() * sizeof(uint32_t));
}

void Display::EndFrame() {
    // 合成: 面板缓冲 → 后缓冲
    for (int32_t y = 0; y < m_height; ++y) {
        for (int32_t x = 0; x < m_width; ++x) {
            int32_t idx = y * m_width + x;
            uint32_t boardPx = m_boardSurface.pixels[idx];
            // 非透明像素拷贝到后缓冲
            if ((boardPx & 0x00FFFFFF) != m_colorKeyLow) {
                m_backSurface.pixels[idx] = boardPx;
            }
        }
    }

    // 合成: 墨水缓冲 → 后缓冲
    for (int32_t y = 0; y < m_height; ++y) {
        for (int32_t x = 0; x < m_width; ++x) {
            int32_t idx = y * m_width + x;
            uint32_t inkPx = m_inkSurface.pixels[idx];
            if ((inkPx & 0x00FFFFFF) != 0x00000000) {
                m_backSurface.pixels[idx] = inkPx;
            }
        }
    }
}

void Display::Present() {
    if (!m_hWnd) return;

    // 使用 GDI 将后缓冲呈现到窗口
    HDC hdc = GetDC(static_cast<HWND>(m_hWnd));
    if (!hdc) return;

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = m_width;
    bmi.bmiHeader.biHeight = -m_height; // 自上而下
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    SetDIBitsToDevice(hdc,
        0, 0, m_width, m_height,
        0, 0, 0, m_height,
        m_backSurface.pixels.data(),
        &bmi, DIB_RGB_COLORS);

    ReleaseDC(static_cast<HWND>(m_hWnd), hdc);
}

void Display::FillRect(int32_t x, int32_t y, int32_t w, int32_t h,
    uint32_t color)
{
    for (int32_t py = y; py < y + h && py < m_height; ++py) {
        for (int32_t px = x; px < x + w && px < m_width; ++px) {
            if (py >= 0 && px >= 0) {
                m_boardSurface.pixels[py * m_width + px] = color;
            }
        }
    }
}

void Display::BlitSprite(int32_t destX, int32_t destY,
    int32_t spriteId, int32_t spriteSize)
{
    // 简化的精灵渲染: 绘制彩色矩形作为占位符
    uint32_t color = 0xFF0000FF; // 默认蓝色
    switch (spriteId % 8) {
    case 0: color = 0xFF808080; break; // 灰
    case 1: color = 0xFFFF0000; break; // 蓝
    case 2: color = 0xFF00FF00; break; // 绿
    case 3: color = 0xFFFF0000; break; // 红
    case 4: color = 0xFFFFFF00; break; // 青
    case 5: color = 0xFFFF00FF; break; // 品红
    case 6: color = 0xFFFFA500; break; // 橙
    case 7: color = 0xFFA0A0A0; break; // 亮灰
    }
    FillRect(destX, destY, spriteSize, spriteSize, color);
}

void Display::DrawObject(const BoardObject* obj, int32_t spriteId) {
    if (!obj) return;
    BlitSprite(
        static_cast<int32_t>(obj->PositionX()),
        static_cast<int32_t>(obj->PositionY()),
        spriteId,
        obj->ObjectWidth());
}

void Display::SetColorKey(uint32_t lowColor, uint32_t highColor) {
    m_colorKeyLow = lowColor;
    m_colorKeyHigh = highColor;
}

Rect Display::GetBoardRect() const {
    return { 0, 0, m_width, m_height };
}

void Display::BltBoardToInk() {
    // 将面板缓冲的内容复制到墨水缓冲
    m_inkSurface.pixels = m_boardSurface.pixels;
}

} // namespace inkball
