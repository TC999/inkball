#include "BoardTileRLColored.h"
#include "Ball.h"
namespace inkball {
void BoardTileRLColored::DeflectBall(Ball* ball) {
    // 仅匹配颜色的球才受引导轨影响
    if (ball->GetColor() == m_targetColor) {
        PerformStandardWallDeflection(ball);
        ball->ScaleVelocity(1.3); // 加速
    } else {
        PerformStandardWallDeflection(ball);
    }
}
void BoardTileRLColored::InitializeFromEncoding(const TileEncoding& enc) {
    m_spriteId = 77 + enc.param0();
    m_targetColor = static_cast<BallColor>((enc.param1() % 5) + 1);
    m_variant = enc.param2();
}
}
