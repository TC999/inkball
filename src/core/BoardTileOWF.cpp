#include "BoardTileOWF.h"
#include "Ball.h"
namespace inkball {
void BoardTileOWF::DeflectBall(Ball* ball) {
    // 检查球是否从被阻挡方向接近
    bool fromBlockedSide = false;
    switch (m_blockedDirection) {
    case Direction::Left:
        fromBlockedSide = ball->VelocityX() < 0.0;
        break;
    case Direction::Right:
        fromBlockedSide = ball->VelocityX() > 0.0;
        break;
    case Direction::Up:
        fromBlockedSide = ball->VelocityY() < 0.0;
        break;
    case Direction::Down:
        fromBlockedSide = ball->VelocityY() > 0.0;
        break;
    }
    if (fromBlockedSide) {
        PerformStandardWallDeflection(ball);
    }
    // 否则放行
}
void BoardTileOWF::InitializeFromEncoding(const TileEncoding& enc) {
    m_spriteId = 47 + enc.param0();
    m_blockedDirection = static_cast<Direction>(enc.param1() % 4);
}
}
