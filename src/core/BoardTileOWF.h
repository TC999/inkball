#pragma once
// 单向力场 — 仅在一个方向阻挡球（从错误方向接近时反弹）
#include "BoardTile.h"
namespace inkball {
class BoardTileOWF : public BoardTile {
public:
    BoardTileOWF() : m_blockedDirection(Direction::Left) { m_category = TileCategory::OneWayForce; }
    CollisionMode CareAboutCollisions() const override { return CollisionMode::NonDeflecting; }
    void DeflectBall(Ball* ball) override;
    void InitializeFromEncoding(const TileEncoding& enc) override;
private:
    Direction m_blockedDirection;
};
}
