// ============================================================
// Ball 实现
// 对应文档中的 CBall，净室独立设计
// ============================================================

#include "Ball.h"

namespace inkball {

Ball::Ball()
    : m_radius(10)
    , m_color(BallColor::None)
{
    m_pointData.Recalculate(m_radius);
}

Ball::Ball(double posX, double posY, int32_t radius, BallColor color)
    : m_radius(radius)
    , m_color(color)
{
    m_positionX = posX;
    m_positionY = posY;
    m_width = radius * 2;
    m_height = radius * 2;
    UpdateBounds();
    m_pointData.Recalculate(m_radius);
}

void Ball::SetRadius(int32_t r) {
    m_radius = r;
    m_width = r * 2;
    m_height = r * 2;
    UpdateBounds();
    m_pointData.Recalculate(r);
}

void Ball::InitializeBallPoints() {
    m_pointStart = FindForwardMostPoint();
    // 前向偏移8点（1/4圆），从前方开始扫描
    m_pointCurrent = MathUtils::Modulo(m_pointStart - 8, BallPointData::kNumPoints);
    m_pointEnd = m_pointCurrent;
}

int32_t Ball::FindForwardMostPoint() const {
    // 计算每个点与速度方向的点积，取最大值
    int32_t bestIndex = 0;
    double bestDot = -1e10;

    for (int32_t i = 0; i < BallPointData::kNumPoints; ++i) {
        BallPoint pt = m_pointData.GetPoint(i);
        double dot = m_velocityX * pt.dx + m_velocityY * pt.dy;
        if (dot > bestDot) {
            bestDot = dot;
            bestIndex = i;
        }
    }
    return bestIndex;
}

BallPoint Ball::GetCurrentPoint() const {
    return m_pointData.GetPoint(m_pointCurrent);
}

BallPoint Ball::GetNextPoint() {
    BallPoint pt = m_pointData.GetPoint(m_pointCurrent);
    m_pointCurrent = MathUtils::Modulo(m_pointCurrent + 1, BallPointData::kNumPoints);
    return pt;
}

BallPoint Ball::GetPointAt(int32_t index) const {
    return m_pointData.GetPoint(index);
}

BallPoint Ball::GetWorldPoint(int32_t index) const {
    BallPoint local = m_pointData.GetPoint(index);
    return {
        local.dx + static_cast<int32_t>(CenterX()),
        local.dy + static_cast<int32_t>(CenterY())
    };
}

void Ball::Deflect(const Vector2D& normal) {
    Vector2D vel = { m_velocityX, m_velocityY };
    MathUtils::Reflect(vel, normal);
    m_velocityX = vel.dx;
    m_velocityY = vel.dy;
}

void Ball::Deflect(double normalX, double normalY) {
    Vector2D vel = { m_velocityX, m_velocityY };
    MathUtils::Reflect(vel, normalX, normalY);
    m_velocityX = vel.dx;
    m_velocityY = vel.dy;
}

bool Ball::CheckBoardBounds(const Rect& boardRect) {
    bool collided = false;
    double cx = CenterX();
    double cy = CenterY();

    // 左边碰撞
    if (cx - m_radius <= boardRect.left) {
        m_positionX = static_cast<double>(boardRect.left + m_radius + 1);
        if (m_velocityX < 0.0) m_velocityX = -m_velocityX;
        collided = true;
    }
    // 右边碰撞
    if (cx + m_radius >= boardRect.right) {
        m_positionX = static_cast<double>(boardRect.right - m_radius - 1);
        if (m_velocityX > 0.0) m_velocityX = -m_velocityX;
        collided = true;
    }
    // 上边碰撞
    if (cy - m_radius <= boardRect.top) {
        m_positionY = static_cast<double>(boardRect.top + m_radius + 1);
        if (m_velocityY < 0.0) m_velocityY = -m_velocityY;
        collided = true;
    }
    // 下边碰撞
    if (cy + m_radius >= boardRect.bottom) {
        m_positionY = static_cast<double>(boardRect.bottom - m_radius - 1);
        if (m_velocityY > 0.0) m_velocityY = -m_velocityY;
        collided = true;
    }

    if (collided) {
        UpdateBounds();
    }
    return collided;
}

int32_t Ball::Collide(HitTestFunc hitTest) {
    if (!hitTest) return -1;

    InitializeBallPoints();

    int32_t firstHit = -1;
    int32_t lastHit = -1;
    bool inCollision = false;

    // 遍历所有32个点一次
    for (int32_t i = 0; i < BallPointData::kNumPoints; ++i) {
        BallPoint pt = GetNextPoint();
        int32_t wx = pt.dx + WorldCenterX();
        int32_t wy = pt.dy + WorldCenterY();

        if (hitTest(wx, wy)) {
            if (!inCollision) {
                // 碰撞开始
                firstHit = m_pointCurrent > 0
                    ? m_pointCurrent - 1
                    : BallPointData::kNumPoints - 1;
                inCollision = true;
            }
            lastHit = m_pointCurrent > 0
                ? m_pointCurrent - 1
                : BallPointData::kNumPoints - 1;
        } else {
            if (inCollision) {
                // 碰撞结束
                break;
            }
        }
    }

    if (firstHit < 0) return -1; // 无碰撞

    // 计算碰撞中点索引
    int32_t midIndex;
    if (lastHit >= firstHit) {
        midIndex = (firstHit + lastHit) / 2;
    } else {
        // 环形跨越
        int32_t span = lastHit + BallPointData::kNumPoints - firstHit;
        midIndex = (firstHit + span / 2) % BallPointData::kNumPoints;
    }

    // 使用中点法线进行反射
    BallPoint midPt = GetPointAt(midIndex);
    Deflect(static_cast<double>(midPt.dx), static_cast<double>(midPt.dy));

    return midIndex;
}

bool Ball::IntersectsWith(const Ball& other) const {
    double distSq = MathUtils::DistanceSq(
        CenterX(), CenterY(),
        other.CenterX(), other.CenterY());
    double sumR = m_radius + other.m_radius;
    return distSq <= sumR * sumR;
}

void Ball::UpdatePosition(double deltaTime) {
    m_positionX += m_velocityX * deltaTime;
    m_positionY += m_velocityY * deltaTime;
    UpdateBounds();
}

} // namespace inkball
