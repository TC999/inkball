#pragma once

// ============================================================
// Ink — 墨水输入系统
// 管理玩家在屏幕上绘制的墨水笔画
// 提供笔画捕捉、碰撞检测（命中圆形测试）和表面渲染
// 对应文档中的 CInk，独立设计
// ============================================================

#include "GameTypes.h"
#include <vector>
#include <cstdint>

namespace inkball {

class Display;
struct InkStroke;

class Ink {
public:
    Ink();
    ~Ink();

    // ---- 生命周期 ----
    bool Initialize(void* hWnd); // HWND
    void Shutdown();

    // ---- 笔画捕获 ----
    void BeginStroke(int32_t x, int32_t y);
    void ExtendStroke(int32_t x, int32_t y);
    void EndStroke();
    void ClearAllStrokes();

    // ---- 碰撞检测 ----
    // 在指定圆心和半径处检查是否与墨水笔画相交
    // mode: 0 = 仅查询; 1 = 查询并删除
    bool HitCircleTest(int32_t centerX, int32_t centerY,
        int32_t radius, int32_t mode);

    // ---- 命中回调（供Collide使用） ----
    bool InkHitTest(int32_t worldX, int32_t worldY) const;

    // ---- 状态控制 ----
    void SetActive(bool active) { m_isActive = active; }
    bool IsActive() const { return m_isActive; }
    bool ShouldAcceptInput() const;
    void SetInkModified(bool modified) { m_inkModified = modified; }
    bool IsInkModified() const { return m_inkModified; }

    // ---- 渲染 ----
    void DrawToSurface(Display* display);
    void GetUpdateRect(Rect& outRect) const;

    // ---- 配置 ----
    void SetInkBudget(int32_t budget) { m_inkBudget = budget; }
    int32_t InkBudget() const { return m_inkBudget; }
    int32_t RemainingInk() const { return m_remainingInk; }
    void SetDifficultyScale(double scale) { m_difficultyScale = scale; }

private:
    struct InkPoint {
        int32_t x, y;
    };

    struct InkStroke {
        std::vector<InkPoint> points;
        Rect boundingRect;
        bool committed = false;

        void CalculateBounds();
        bool IsPointInStroke(int32_t px, int32_t py) const;
        Vector2D GetClosestNormal(int32_t px, int32_t py) const;
    };

    void RecalculateInkBoundingRect();

    std::vector<InkStroke*> m_strokes;
    InkStroke* m_currentStroke = nullptr;

    void* m_hWnd = nullptr;
    bool m_isActive = true;
    bool m_inkModified = false;
    bool m_inkRedraw = false;

    int32_t m_inkBudget = Config::kDefaultInkBudget;
    int32_t m_remainingInk = Config::kDefaultInkBudget;
    double m_difficultyScale = 1.0;

    Rect m_cachedDirtyRect;
};

} // namespace inkball
