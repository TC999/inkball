// ============================================================
// TileManager 实现
// ============================================================

#include "TileManager.h"
#include "Display.h"
#include <stdexcept>

namespace inkball {

TileManager::TileManager() = default;

TileManager::~TileManager() {
    Reset(0, 0, 0);
}

void TileManager::Reset(int32_t cols, int32_t rows, int32_t tileSize) {
    // 释放所有现有瓦片
    for (BoardTile* tile : m_grid) {
        delete tile;
    }
    m_grid.clear();

    m_columns = cols;
    m_rows = rows;
    m_tileSize = tileSize;
    m_tileCount = 0;

    if (cols > 0 && rows > 0) {
        m_grid.resize(cols * rows, nullptr);
    }
}

bool TileManager::InitSurface(Display* /*display*/) {
    // DirectDraw 表面初始化在此处执行
    // 实际渲染时需要分配纹理表面
    return true;
}

BoardTile* TileManager::CreateTileAt(int32_t gridX, int32_t gridY,
    const TileEncoding& enc)
{
    return CreateTileAt(gridX, gridY, enc.Encode());
}

BoardTile* TileManager::CreateTileAt(int32_t gridX, int32_t gridY,
    uint32_t encodedValue)
{
    if (!IsValidGrid(gridX, gridY)) return nullptr;

    int32_t idx = GridIndex(gridX, gridY);

    // 若该位置已有瓦片，先释放
    if (m_grid[idx] != nullptr) {
        delete m_grid[idx];
        --m_tileCount;
    }

    TileEncoding enc = TileEncoding::Decode(encodedValue);
    BoardTile* tile = BoardTile::CreateFromEncoding(enc, gridX, gridY, m_tileSize);

    if (tile) {
        m_grid[idx] = tile;
        ++m_tileCount;
    }
    return tile;
}

BoardTile* TileManager::GetTileAt(int32_t gridX, int32_t gridY) {
    if (!IsValidGrid(gridX, gridY)) return nullptr;
    return m_grid[GridIndex(gridX, gridY)];
}

const BoardTile* TileManager::GetTileAt(int32_t gridX, int32_t gridY) const {
    if (!IsValidGrid(gridX, gridY)) return nullptr;
    return m_grid[GridIndex(gridX, gridY)];
}

BoardTile* TileManager::GetTileAtWorldPos(double worldX, double worldY) {
    int32_t gx = static_cast<int32_t>(worldX) / m_tileSize;
    int32_t gy = static_cast<int32_t>(worldY) / m_tileSize;
    return GetTileAt(gx, gy);
}

void TileManager::GetSurroundingTiles(
    double centerX, double centerY,
    double radius,
    std::vector<BoardTile*>& outTiles)
{
    outTiles.clear();

    // 获取球边界框的四个角所在的瓦片
    int32_t left = static_cast<int32_t>(centerX - radius) / m_tileSize;
    int32_t right = static_cast<int32_t>(centerX + radius) / m_tileSize;
    int32_t top = static_cast<int32_t>(centerY - radius) / m_tileSize;
    int32_t bottom = static_cast<int32_t>(centerY + radius) / m_tileSize;

    // 去重（使用简单的集合方法）
    auto addIfUnique = [&](int32_t gx, int32_t gy) {
        BoardTile* tile = GetTileAt(gx, gy);
        if (tile && tile->GetCategory() != TileCategory::Floor) {
            for (auto* existing : outTiles) {
                if (existing == tile) return;
            }
            outTiles.push_back(tile);
        }
    };

    addIfUnique(left, top);
    addIfUnique(right, top);
    addIfUnique(left, bottom);
    addIfUnique(right, bottom);
    addIfUnique((left + right) / 2, (top + bottom) / 2);
}

void TileManager::Render(Display* display) {
    if (!display) return;
    for (BoardTile* tile : m_grid) {
        if (tile && tile->GetCategory() != TileCategory::Floor) {
            display->DrawObject(tile, tile->SpriteId());
        }
    }
}

} // namespace inkball
