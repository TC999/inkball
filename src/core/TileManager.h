#pragma once

// ============================================================
// TileManager — 瓦片网格管理器
// 负责维护面板上的瓦片网格，包括创建、查询和批量操作
// 对应文档中的 CTileManager，独立设计
// ============================================================

#include "GameTypes.h"
#include "BoardTile.h"
#include <vector>
#include <memory>

namespace inkball {

class Display;

class TileManager {
public:
    TileManager();
    ~TileManager();

    // ---- 生命周期 ----
    void Reset(int32_t cols, int32_t rows, int32_t tileSize);
    bool InitSurface(Display* display);

    // ---- 瓦片创建 ----
    BoardTile* CreateTileAt(int32_t gridX, int32_t gridY,
        const TileEncoding& enc);
    BoardTile* CreateTileAt(int32_t gridX, int32_t gridY,
        uint32_t encodedValue);

    // ---- 查询 ----
    BoardTile* GetTileAt(int32_t gridX, int32_t gridY);
    const BoardTile* GetTileAt(int32_t gridX, int32_t gridY) const;

    // ---- 统计 ----
    int32_t ColumnCount() const { return m_columns; }
    int32_t RowCount() const { return m_rows; }
    int32_t TileSize() const { return m_tileSize; }
    int32_t TileCount() const { return m_tileCount; }

    // ---- 批量渲染 ----
    void Render(Display* display);

    // ---- 碰撞辅助 ----
    // 给定一个世界坐标点，找到它所属的瓦片
    BoardTile* GetTileAtWorldPos(double worldX, double worldY);

    // 获取包围该矩形的所有候选瓦片（四个角）
    void GetSurroundingTiles(
        double centerX, double centerY,
        double radius,
        std::vector<BoardTile*>& outTiles);

private:
    int32_t GridIndex(int32_t gx, int32_t gy) const {
        return gy * m_columns + gx;
    }
    bool IsValidGrid(int32_t gx, int32_t gy) const {
        return gx >= 0 && gx < m_columns && gy >= 0 && gy < m_rows;
    }

    int32_t m_columns = 0;
    int32_t m_rows = 0;
    int32_t m_tileSize = Config::kTileSize;
    int32_t m_tileCount = 0;
    int32_t m_boardOriginX = 0;
    int32_t m_boardOriginY = 0;

    // 瓦片网格 — 按行主序
    std::vector<BoardTile*> m_grid;
};

} // namespace inkball
