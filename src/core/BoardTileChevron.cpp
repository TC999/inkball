#include "BoardTileChevron.h"
#include "Ball.h"
#include "MathUtils.h"
namespace inkball {
void BoardTileChevron::DeflectBall(Ball* ball) {
    // 将球的方向强制改为引导器的固定方向
    double speed = ball->Speed();
    switch (m_direction) {
    case Direction::Up:    ball->SetVelocity(0.0, -speed); break;
    case Direction::Down:  ball->SetVelocity(0.0, speed); break;
    case Direction::Left:  ball->SetVelocity(-speed, 0.0); break;
    case Direction::Right: ball->SetVelocity(speed, 0.0); break;
    default: PerformStandardWallDeflection(ball); break;
    }
}
void BoardTileChevron::InitializeFromEncoding(const TileEncoding& enc) {
    m_spriteId = 102 + enc.param1();
    m_direction = static_cast<Direction>(enc.param0() % 4);
}
}
