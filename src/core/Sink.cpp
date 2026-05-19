// ============================================================
// Sink 实现
// ============================================================

#include "Sink.h"
#include "Ball.h"

namespace inkball {

Sink::Sink() = default;

Sink::Sink(double x, double y, BallColor expectedColor, int32_t capacity)
    : m_expectedColor(expectedColor)
    , m_capacity(capacity)
{
    m_positionX = x;
    m_positionY = y;
    UpdateBounds();
}

bool Sink::AcceptsColor(BallColor color) const {
    return color == m_expectedColor && !IsFilled();
}

bool Sink::TryFill(Ball* ball) {
    if (!ball || !ball->IsActive()) return false;
    if (!AcceptsColor(ball->GetColor())) return false;

    m_filledCount++;
    ball->Deactivate();
    return true;
}

} // namespace inkball
