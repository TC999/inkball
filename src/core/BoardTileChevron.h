#pragma once
// 方向引导器 — 将球推向固定方向
#include "BoardTile.h"
namespace inkball {
class BoardTileChevron : public BoardTile {
public:
    BoardTileChevron() : m_direction(Direction::Right) { m_category = TileCategory::Chevron; }
    CollisionMode CareAboutCollisions() const override { return CollisionMode::Deflecting; }
    void DeflectBall(Ball* ball) override;
    void InitializeFromEncoding(const TileEncoding& enc) override;
    Direction GetDirection() const { return m_direction; }
private:
    Direction m_direction;
};
}
