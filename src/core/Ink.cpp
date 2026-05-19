// ============================================================
// Ink 实现
// ============================================================

#include "Ink.h"
#include "MathUtils.h"
#include <algorithm>

namespace inkball {

// ---- InkStroke 内部实现 ----

void Ink::InkStroke::CalculateBounds() {
    if (points.empty()) return;
    int32_t minX = points[0].x, maxX = points[0].x;
    int32_t minY = points[0].y, maxY = points[0].y;
    for (const auto& p : points) {
        minX = std::min(minX, p.x); maxX = std::max(maxX, p.x);
        minY = std::min(minY, p.y); maxY = std::max(maxY, p.y);
    }
    boundingRect = { minX, minY, maxX + 1, maxY + 1 };
}

bool Ink::InkStroke::IsPointInStroke(int32_t px, int32_t py) const {
    // 简单的线段最近距离检测
    if (points.size() < 1) return false;

    for (size_t i = 1; i < points.size(); ++i) {
        const InkPoint& a = points[i - 1];
        const InkPoint& b = points[i];

        // 计算点到线段的最短距离
        double abx = b.x - a.x;
        double aby = b.y - a.y;
        double apx = px - a.x;
        double apy = py - a.y;

        double abLenSq = abx * abx + aby * aby;
        if (abLenSq < 1e-10) continue;

        double t = MathUtils::Clamp(
            (apx * abx + apy * aby) / abLenSq, 0.0, 1.0);

        double closestX = a.x + abx * t;
        double closestY = a.y + aby * t;

        double distSq = MathUtils::DistanceSq(px, py, closestX, closestY);
        // 墨水笔画宽度约3像素
        if (distSq <= 9.0) return true;
    }
    return false;
}

Vector2D Ink::InkStroke::GetClosestNormal(int32_t px, int32_t py) const {
    if (points.size() < 2) return { 0.0, 0.0 };

    double minDist = 1e10;
    Vector2D normal = { 0.0, 0.0 };

    for (size_t i = 1; i < points.size(); ++i) {
        const InkPoint& a = points[i - 1];
        const InkPoint& b = points[i];

        double abx = b.x - a.x;
        double aby = b.y - a.y;
        double apx = px - a.x;
        double apy = py - a.y;

        double abLenSq = abx * abx + aby * aby;
        if (abLenSq < 1e-10) continue;

        double t = MathUtils::Clamp(
            (apx * abx + apy * aby) / abLenSq, 0.0, 1.0);

        double closestX = a.x + abx * t;
        double closestY = a.y + aby * t;
        double distSq = MathUtils::DistanceSq(px, py, closestX, closestY);

        if (distSq < minDist) {
            minDist = distSq;
            // 法线方向垂直于线段
            normal = { -aby, abx };
            double len = normal.Length();
            if (len > 1e-10) {
                normal.dx /= len;
                normal.dy /= len;
            }
        }
    }
    return normal;
}

// ---- Ink ----

Ink::Ink() = default;

Ink::~Ink() {
    Shutdown();
}

bool Ink::Initialize(void* hWnd) {
    m_hWnd = hWnd;
    m_isActive = true;
    m_remainingInk = m_inkBudget;
    return true;
}

void Ink::Shutdown() {
    ClearAllStrokes();
    m_hWnd = nullptr;
}

void Ink::BeginStroke(int32_t x, int32_t y) {
    if (!ShouldAcceptInput()) return;

    ClearAllStrokes(); // 新笔画开始时清空旧笔画
    m_currentStroke = new InkStroke();
    m_currentStroke->points.push_back({ x, y });
    m_inkModified = true;
}

void Ink::ExtendStroke(int32_t x, int32_t y) {
    if (!m_currentStroke || !ShouldAcceptInput()) return;
    if (m_remainingInk <= 0) return;

    // 计算新增点距离，消耗墨水预算
    if (!m_currentStroke->points.empty()) {
        const InkPoint& last = m_currentStroke->points.back();
        double dist = MathUtils::Distance(last.x, last.y, x, y);
        m_remainingInk -= static_cast<int32_t>(dist * m_difficultyScale);
        if (m_remainingInk < 0) m_remainingInk = 0;
    }

    m_currentStroke->points.push_back({ x, y });
    m_currentStroke->CalculateBounds();
    m_inkModified = true;
}

void Ink::EndStroke() {
    if (m_currentStroke) {
        m_currentStroke->CalculateBounds();
        m_currentStroke->committed = true;
        m_strokes.push_back(m_currentStroke);
        m_currentStroke = nullptr;
    }
    m_inkModified = true;
}

void Ink::ClearAllStrokes() {
    for (InkStroke* s : m_strokes) {
        delete s;
    }
    m_strokes.clear();
    m_currentStroke = nullptr;
    m_inkModified = true;
    m_inkRedraw = true;
}

bool Ink::HitCircleTest(int32_t centerX, int32_t centerY,
    int32_t radius, int32_t mode)
{
    // mode 0 = 仅查询, mode 1 = 查询并删除碰撞笔画
    for (auto it = m_strokes.begin(); it != m_strokes.end(); ) {
        InkStroke* stroke = *it;

        // 快速包围盒测试
        if (stroke->boundingRect.left > centerX + radius ||
            stroke->boundingRect.right < centerX - radius ||
            stroke->boundingRect.top > centerY + radius ||
            stroke->boundingRect.bottom < centerY - radius) {
            ++it;
            continue;
        }

        // 详细测试
        for (size_t i = 1; i < stroke->points.size(); ++i) {
            const InkPoint& a = stroke->points[i - 1];
            const InkPoint& b = stroke->points[i];

            // 线段到圆的最近距离测试
            double abx = b.x - a.x;
            double aby = b.y - a.y;
            double acx = centerX - a.x;
            double acy = centerY - a.y;

            double abLenSq = abx * abx + aby * aby;
            if (abLenSq < 1e-10) continue;

            double t = MathUtils::Clamp(
                (acx * abx + acy * aby) / abLenSq, 0.0, 1.0);

            double closestX = a.x + abx * t;
            double closestY = a.y + aby * t;

            if (MathUtils::DistanceSq(centerX, centerY,
                closestX, closestY) <= radius * radius) {
                if (mode == 1) {
                    // 删除模式: 移除碰撞的笔画
                    m_isActive = false;
                    delete stroke;
                    it = m_strokes.erase(it);
                    m_inkModified = true;
                    m_inkRedraw = true;
                    m_isActive = true;
                    return true;
                }
                return true;
            }
        }
        ++it;
    }
    return false;
}

bool Ink::InkHitTest(int32_t worldX, int32_t worldY) const {
    for (const InkStroke* stroke : m_strokes) {
        if (stroke->boundingRect.Contains(worldX, worldY)) {
            if (stroke->IsPointInStroke(worldX, worldY)) {
                return true;
            }
        }
    }
    return false;
}

bool Ink::ShouldAcceptInput() const {
    return m_isActive && m_remainingInk > 0;
}

void Ink::DrawToSurface(Display* /*display*/) {
    // 渲染所有墨水笔画到 DirectDraw 墨水表面
    // for (InkStroke* stroke : m_strokes) {
    //     display->DrawInkStroke(stroke);
    // }
}

void Ink::GetUpdateRect(Rect& outRect) const {
    outRect = m_cachedDirtyRect;
}

void Ink::RecalculateInkBoundingRect() {
    if (m_strokes.empty()) {
        m_cachedDirtyRect = { 0, 0, 0, 0 };
        return;
    }

    int32_t minX = m_strokes[0]->boundingRect.left;
    int32_t minY = m_strokes[0]->boundingRect.top;
    int32_t maxX = m_strokes[0]->boundingRect.right;
    int32_t maxY = m_strokes[0]->boundingRect.bottom;

    for (const InkStroke* s : m_strokes) {
        minX = std::min(minX, s->boundingRect.left);
        minY = std::min(minY, s->boundingRect.top);
        maxX = std::max(maxX, s->boundingRect.right);
        maxY = std::max(maxY, s->boundingRect.bottom);
    }

    // 扩展200像素以覆盖可能的"脏区域"
    m_cachedDirtyRect = {
        std::max(minX - 200, 0), std::max(minY - 200, 0),
        maxX + 200, maxY + 200
    };
}

} // namespace inkball
