#include "BoardTileRLGray.h"
#include "Ball.h"
namespace inkball {
void BoardTileRLGray::DeflectBall(Ball* ball) {
    PerformStandardWallDeflection(ball);
    double currentSpeed = ball->Speed();
    if (currentSpeed < m_entrySpeed * 50.0) {
        ball->ScaleVelocity(m_exitSpeed);
    }
}
void BoardTileRLGray::InitializeFromEncoding(const TileEncoding& enc) {
    m_spriteId = 77 + enc.param0();
    m_entrySpeed = 1.0 + enc.param1() * 0.2;
    m_exitSpeed = 1.5 + enc.param2() * 0.2;
}
}
