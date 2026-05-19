#pragma once

// ============================================================
// Display — DirectDraw 显示包装器
// 管理主表面、后缓冲、面板缓冲和墨水缓冲的四层合成管道
// 对应文档中的 CDisplay，独立设计
// ============================================================

#include "GameTypes.h"
#include <cstdint>
#include <vector>

namespace inkball {

class BoardObject;
class Ball;

class Display {
public:
    Display();
    ~Display();

    // ---- 生命周期 ----
    bool Initialize(void* hWnd, int32_t width, int32_t height);
    void Shutdown();
    bool RestoreAllSurfaces();

    // ---- 表面创建 ----
    bool CreateWindowedDisplay(int32_t width, int32_t height);
    bool CreateOffscreenSurfaces();

    // ---- 访问器 ----
    bool IsReady() const { return m_isReady; }
    int32_t Width() const { return m_width; }
    int32_t Height() const { return m_height; }
    void* NativeHandle() const { return m_hWnd; }

    // ---- 渲染操作 ----
    void BeginFrame();
    void EndFrame();
    void Present();

    // ---- 绘制基元 ----
    void FillRect(int32_t x, int32_t y, int32_t w, int32_t h,
        uint32_t color);
    void BlitSprite(int32_t destX, int32_t destY,
        int32_t spriteId, int32_t spriteSize);
    void DrawObject(const BoardObject* obj, int32_t spriteId);

    // ---- 颜色键 ----
    void SetColorKey(uint32_t lowColor, uint32_t highColor);

    // ---- 表面信息 ----
    Rect GetBoardRect() const;

    // ---- 墨水专用 ----
    void BltBoardToInk();

private:
    void* m_hWnd = nullptr;
    int32_t m_width = 800;
    int32_t m_height = 600;
    bool m_isReady = false;

    // 模拟的表面缓冲区（当没有真实DDraw时使用GDI后备）
    struct Surface {
        std::vector<uint32_t> pixels;
        int32_t width = 0;
        int32_t height = 0;
    };

    Surface m_boardSurface;    // 面板缓冲
    Surface m_inkSurface;      // 墨水缓冲
    Surface m_backSurface;     // 后缓冲

    uint32_t m_colorKeyLow = 0x010101;
    uint32_t m_colorKeyHigh = 0xFFFFFF;
};

} // namespace inkball
