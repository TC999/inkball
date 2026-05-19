#pragma once

// ============================================================
// MovingObject — 带速度向量的移动对象
// 扩展了 BoardObject，增加速度和移动相关能力
// 对应文档中的 CMovingObject，独立设计
// ============================================================

#include "BoardObject.h"

namespace inkball {

class MovingObject : public BoardObject {
public:
    MovingObject() = default;
    virtual ~MovingObject() = default;

    // ---- 速度 ----
    double VelocityX() const { return m_velocityX; }
    double VelocityY() const { return m_velocityY; }

    void SetVelocity(double vx, double vy) {
        m_velocityX = vx;
        m_velocityY = vy;
    }

    Vector2D GetVelocity() const {
        return { m_velocityX, m_velocityY };
    }

    double Speed() const {
        return std::sqrt(m_velocityX * m_velocityX + m_velocityY * m_velocityY);
    }

    // ---- 速度缩放 ----
    void ScaleVelocity(double factor) {
        m_velocityX *= factor;
        m_velocityY *= factor;
    }

    // ---- 位置更新 ----
    virtual void UpdatePosition(double deltaTime) {
        m_positionX += m_velocityX * deltaTime;
        m_positionY += m_velocityY * deltaTime;
    }

    // ---- 方向检测 ----
    // 检查是否正在朝向某个目标点移动
    bool MovingTowards(double targetX, double targetY) const {
        double dirX = targetX - CenterX();
        double dirY = targetY - CenterY();
        return (m_velocityX * dirX + m_velocityY * dirY) > 0.0;
    }

    // ---- 停止 ----
    void Stop() {
        m_velocityX = 0.0;
        m_velocityY = 0.0;
    }

protected:
    double m_velocityX = 0.0;
    double m_velocityY = 0.0;
};

} // namespace inkball
