#include "BoardTileBreakWall.h"
#include "Ball.h"
namespace inkball {
void BoardTileBreakWall::DeflectBall(Ball* ball) {
    if (m_hitPoints <= 0) return;
    PerformStandardWallDeflection(ball);
    m_hitPoints--;
    if (m_hitPoints <= 0) {
        ConvertToFloor();
    }
}
void BoardTileBreakWall::InitializeFromEncoding(const TileEncoding& enc) {
    m_spriteId = 42 + enc.param0();
    m_hitPoints = std::max(1, static_cast<int32_t>(enc.param1()) + 1);
}
void BoardTileBreakWall::ConvertToFloor() {
    m_category = TileCategory::Floor;
    m_hitPoints = 0;
    m_spriteId = 0;
}
}
