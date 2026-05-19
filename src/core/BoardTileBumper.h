#pragma once
// 弹跳器 — 球体碰撞后回弹，可带速度加成
#include "BoardTile.h"
namespace inkball {
class BoardTileBumper : public BoardTile {
public:
    BoardTileBumper() : m_speedBoost(1.2) { m_category = TileCategory::Bumper; }
    CollisionMode CareAboutCollisions() const override { return CollisionMode::Deflecting; }
    void DeflectBall(Ball* ball) override;
    void InitializeFromEncoding(const TileEncoding& enc) override;
private:
    double m_speedBoost;
};
}
