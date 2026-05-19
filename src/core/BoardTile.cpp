// ============================================================
// BoardTile 实现
// 包含瓦片工厂 BaseBuildTileObject
// ============================================================

#include "BoardTile.h"
#include "Ball.h"

// 子类头文件
#include "BoardTileWall.h"
#include "BoardTileBreakWall.h"
#include "BoardTileBumper.h"
#include "BoardTileChevron.h"
#include "BoardTileDrain.h"
#include "BoardTileOWF.h"
#include "BoardTileRLColored.h"
#include "BoardTileRLGray.h"

namespace inkball {

void BoardTile::DeflectBall(Ball* ball) {
    // 地板默认不反弹，仅重置状态
    (void)ball;
}

Vector2D BoardTile::GetClosestSideNormal(const Ball& ball) const {
    double bx = ball.CenterX();
    double by = ball.CenterY();

    // 计算到四条边的距离
    double distLeft = std::abs(bx - m_bounds.left);
    double distRight = std::abs(bx - m_bounds.right);
    double distTop = std::abs(by - m_bounds.top);
    double distBottom = std::abs(by - m_bounds.bottom);

    double minDist = std::min({ distLeft, distRight, distTop, distBottom });

    if (minDist == distLeft)  return { -1.0, 0.0 };
    if (minDist == distRight) return { 1.0, 0.0 };
    if (minDist == distTop)   return { 0.0, -1.0 };
    return { 0.0, 1.0 }; // bottom
}

void BoardTile::PerformStandardWallDeflection(Ball* ball, bool /*changeColor*/) {
    Vector2D normal = GetClosestSideNormal(*ball);
    ball->Deflect(normal);
}

// ---- 瓦片工厂 ----
BoardTile* BoardTile::CreateFromEncoding(const TileEncoding& enc,
    int32_t gridX, int32_t gridY,
    int32_t tileSize)
{
    BoardTile* tile = nullptr;

    switch (static_cast<TileCategory>(enc.category())) {
    case TileCategory::Wall:
        tile = new BoardTileWall();
        break;
    case TileCategory::BreakWall:
        tile = new BoardTileBreakWall();
        break;
    case TileCategory::Bumper:
        tile = new BoardTileBumper();
        break;
    case TileCategory::Chevron:
        tile = new BoardTileChevron();
        break;
    case TileCategory::Drain:
        tile = new BoardTileDrain();
        break;
    case TileCategory::OneWayForce:
        tile = new BoardTileOWF();
        break;
    case TileCategory::RailColored:
        tile = new BoardTileRLColored();
        break;
    case TileCategory::RailGray:
        tile = new BoardTileRLGray();
        break;
    case TileCategory::Floor:
    default:
        tile = new BoardTile();
        break;
    }

    if (tile) {
        // 设置网格位置
        tile->SetGridPosition(gridX, gridY);

        // 计算像素位置: boardOrigin + gridCoord * tileSize
        // boardOrigin 由绘制区域决定，这里简单使用0偏移
        double px = static_cast<double>(gridX * tileSize);
        double py = static_cast<double>(gridY * tileSize);
        tile->SetPosition(px, py);
        tile->SetSize(tileSize, tileSize);

        // 存储编码参数供子类使用
        // 精灵ID通过 param0/param1 计算
        tile->SetCategory(static_cast<TileCategory>(enc.category()));

        // 委派子类初始化编码参数
        tile->InitializeFromEncoding(enc);
    }

    return tile;
}

} // namespace inkball
