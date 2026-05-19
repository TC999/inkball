#pragma once
// 灰色引导轨 — 无条件加速任何颜色的球
#include "BoardTile.h"
namespace inkball {
class BoardTileRLGray : public BoardTile {
public:
    BoardTileRLGray() : m_entrySpeed(1.0), m_exitSpeed(1.5) { m_category = TileCategory::RailGray; }
    CollisionMode CareAboutCollisions() const override { return CollisionMode::Deflecting; }
    void DeflectBall(Ball* ball) override;
    void InitializeFromEncoding(const TileEncoding& enc) override;
private:
    double m_entrySpeed;
    double m_exitSpeed;
};
}
