#pragma once
// 实心墙壁 — 球体反弹，不可破坏
#include "BoardTile.h"
namespace inkball {
class BoardTileWall : public BoardTile {
public:
    BoardTileWall() { m_category = TileCategory::Wall; }
    CollisionMode CareAboutCollisions() const override { return CollisionMode::Deflecting; }
    void DeflectBall(Ball* ball) override;
    void InitializeFromEncoding(const TileEncoding& enc) override;
};
}
