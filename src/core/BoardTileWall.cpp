#include "BoardTileWall.h"
#include "Ball.h"
namespace inkball {
void BoardTileWall::DeflectBall(Ball* ball) {
    PerformStandardWallDeflection(ball);
}
void BoardTileWall::InitializeFromEncoding(const TileEncoding& enc) {
    m_spriteId = 37 + enc.param0();
}
}
