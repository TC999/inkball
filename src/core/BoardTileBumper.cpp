#include "BoardTileBumper.h"
#include "Ball.h"
namespace inkball {
void BoardTileBumper::DeflectBall(Ball* ball) {
    PerformStandardWallDeflection(ball);
    ball->ScaleVelocity(m_speedBoost);
}
void BoardTileBumper::InitializeFromEncoding(const TileEncoding& enc) {
    m_spriteId = 67 + enc.param0();
    m_speedBoost = 1.2 + enc.param1() * 0.1;
}
}
