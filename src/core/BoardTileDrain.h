#pragma once
// 排水口 — 球进入后丢失（触发扣除生命/分数）
#include "BoardTile.h"
namespace inkball {
class BoardTileDrain : public BoardTile {
public:
    BoardTileDrain() { m_category = TileCategory::Drain; }
    CollisionMode CareAboutCollisions() const override { return CollisionMode::NonDeflecting; }
    void DeflectBall(Ball* ball) override;
};
}
