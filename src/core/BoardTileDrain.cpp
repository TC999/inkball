#include "BoardTileDrain.h"
#include "Ball.h"
namespace inkball {
void BoardTileDrain::DeflectBall(Ball* ball) {
    // 排水口：球被消耗/丢失
    ball->Deactivate();
}
}
