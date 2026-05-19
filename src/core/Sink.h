#pragma once

// ============================================================
// Sink — 目标槽位
// 球需要进入与其颜色匹配的目标槽位
// 对应文档中的 CSink，独立设计
// ============================================================

#include "BoardObject.h"

namespace inkball {

class Ball;

class Sink : public BoardObject {
public:
    Sink();
    Sink(double x, double y, BallColor expectedColor, int32_t capacity = 1);
    ~Sink() override = default;

    // ---- 颜色匹配 ----
    BallColor ExpectedColor() const { return m_expectedColor; }
    void SetExpectedColor(BallColor c) { m_expectedColor = c; }
    bool AcceptsColor(BallColor color) const;

    // ---- 填充 ----
    bool IsFilled() const { return m_filledCount >= m_capacity; }
    int32_t FilledCount() const { return m_filledCount; }
    int32_t Capacity() const { return m_capacity; }

    // 尝试将球放入该槽位，返回是否成功
    bool TryFill(Ball* ball);

    // ---- 渲染属性 ----
    int32_t SpriteId() const { return m_spriteId; }
    void SetSpriteId(int32_t id) { m_spriteId = id; }

private:
    BallColor m_expectedColor = BallColor::None;
    int32_t m_capacity = 1;
    int32_t m_filledCount = 0;
    int32_t m_spriteId = 0;
};

} // namespace inkball
