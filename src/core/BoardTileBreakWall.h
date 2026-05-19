// ============================================================
// 可破坏墙壁 — 承受一定次数碰撞后转换为地板
// 文档中的 CBoardTileBreakWall
// ============================================================
#pragma once
#include "BoardTile.h"
namespace inkball {
class BoardTileBreakWall : public BoardTile {
public:
    BoardTileBreakWall() : m_hitPoints(3) { m_category = TileCategory::BreakWall; }
    CollisionMode CareAboutCollisions() const override { return m_hitPoints > 0 ? CollisionMode::Deflecting : CollisionMode::None; }
    void DeflectBall(Ball* ball) override;
    void InitializeFromEncoding(const TileEncoding& enc) override;
    void ConvertToFloor() override;
    int32_t HitPoints() const { return m_hitPoints; }
private:
    int32_t m_hitPoints;
};
}
