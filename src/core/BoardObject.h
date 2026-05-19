#pragma once

// ============================================================
// BoardObject — 游戏面板上所有对象的基类
// 提供位置、大小、边界框等基本属性
// 对应文档中的 CBoardObject，独立设计
// ============================================================

#include "GameTypes.h"

namespace inkball {

class BoardObject {
public:
    BoardObject() = default;
    virtual ~BoardObject() = default;

    // ---- 位置 ----
    double PositionX() const { return m_positionX; }
    double PositionY() const { return m_positionY; }
    void SetPosition(double x, double y) {
        m_positionX = x;
        m_positionY = y;
        UpdateBounds();
    }

    Point2D GetPosition() const { return { m_positionX, m_positionY }; }

    // ---- 大小 ----
    int32_t ObjectWidth() const { return m_width; }
    int32_t ObjectHeight() const { return m_height; }
    void SetSize(int32_t w, int32_t h) {
        m_width = w;
        m_height = h;
        UpdateBounds();
    }

    Size2D GetSize() const { return { m_width, m_height }; }

    // ---- 边界框 ----
    Rect GetBounds() const { return m_bounds; }
    Rect GetTableCell() const { return m_tableCell; }

    double CenterX() const { return m_positionX + m_width * 0.5; }
    double CenterY() const { return m_positionY + m_height * 0.5; }

    // ---- 网格坐标 ----
    int32_t GridX() const { return m_gridX; }
    int32_t GridY() const { return m_gridY; }
    void SetGridPosition(int32_t gx, int32_t gy) {
        m_gridX = gx;
        m_gridY = gy;
    }

protected:
    void UpdateBounds() {
        int32_t px = static_cast<int32_t>(m_positionX);
        int32_t py = static_cast<int32_t>(m_positionY);
        m_bounds = { px, py, px + m_width, py + m_height };
        m_tableCell = m_bounds;
    }

    double m_positionX = 0.0;
    double m_positionY = 0.0;
    int32_t m_width = Config::kTileSize;
    int32_t m_height = Config::kTileSize;
    int32_t m_gridX = 0;
    int32_t m_gridY = 0;
    Rect m_bounds;
    Rect m_tableCell;
};

} // namespace inkball
