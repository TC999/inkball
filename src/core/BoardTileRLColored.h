#pragma once
// 彩色引导轨 — 匹配该颜色的球沿轨道被加速
#include "BoardTile.h"
namespace inkball {
class BoardTileRLColored : public BoardTile {
public:
    BoardTileRLColored() : m_targetColor(BallColor::Red), m_variant(0) { m_category = TileCategory::RailColored; }
    CollisionMode CareAboutCollisions() const override { return CollisionMode::Deflecting; }
    void DeflectBall(Ball* ball) override;
    void InitializeFromEncoding(const TileEncoding& enc) override;
private:
    BallColor m_targetColor;
    int32_t m_variant;
};
}
