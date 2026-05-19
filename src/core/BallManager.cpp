// ============================================================
// BallManager 实现
// ============================================================

#include "BallManager.h"
#include <algorithm>

namespace inkball {

BallManager::BallManager() = default;

BallManager::~BallManager() {
    ClearAllBalls();
}

void BallManager::Initialize() {
    ClearAllBalls();
}

bool BallManager::InitSurface(Display* /*display*/) {
    // DirectDraw 表面初始化
    m_surfaceReady = true;
    return true;
}

Ball* BallManager::AddBall(const Ball& ball) {
    if (m_balls.size() >= m_maxBalls) return nullptr;
    Ball* newBall = new Ball(ball);
    m_balls.push_back(newBall);
    return newBall;
}

Ball* BallManager::AddBall(double x, double y, int32_t radius, BallColor color) {
    if (m_balls.size() >= m_maxBalls) return nullptr;
    Ball* newBall = new Ball(x, y, radius, color);
    m_balls.push_back(newBall);
    return newBall;
}

void BallManager::RemoveBall(Ball* ball) {
    auto it = std::find(m_balls.begin(), m_balls.end(), ball);
    if (it != m_balls.end()) {
        delete *it;
        m_balls.erase(it);
    }
}

void BallManager::RemoveDeactivatedBalls() {
    m_balls.erase(
        std::remove_if(m_balls.begin(), m_balls.end(),
            [](Ball* b) {
                if (!b->IsActive()) { delete b; return true; }
                return false;
            }),
        m_balls.end());
}

void BallManager::RemoveBallAt(size_t index) {
    if (index < m_balls.size()) {
        delete m_balls[index];
        m_balls.erase(m_balls.begin() + index);
    }
}

void BallManager::UpdateBallPositions(double deltaTime) {
    for (Ball* ball : m_balls) {
        if (ball->IsActive()) {
            ball->UpdatePosition(deltaTime);
        }
    }
}

size_t BallManager::ActiveBallCount() const {
    return m_balls.size();
}

Ball* BallManager::GetBallAt(size_t index) {
    if (index < m_balls.size()) return m_balls[index];
    return nullptr;
}

const Ball* BallManager::GetBallAt(size_t index) const {
    if (index < m_balls.size()) return m_balls[index];
    return nullptr;
}

void BallManager::CheckBallBallCollisions() {
    size_t count = m_balls.size();
    for (size_t i = 0; i < count; ++i) {
        Ball* a = m_balls[i];
        if (!a->IsActive()) continue;

        for (size_t j = i + 1; j < count; ++j) {
            Ball* b = m_balls[j];
            if (!b->IsActive()) continue;

            if (a->IntersectsWith(*b)) {
                // 两球相交，分别计算碰撞
                double dx = b->CenterX() - a->CenterX();
                double dy = b->CenterY() - a->CenterY();
                double dist = std::sqrt(dx * dx + dy * dy);
                if (dist < 1e-10) continue;

                Vector2D normal = { dx / dist, dy / dist };
                Vector2D invNormal = { -normal.dx, -normal.dy };

                a->Deflect(normal);
                b->Deflect(invNormal);

                // 分离两个球防止渗透
                double overlap = a->Radius() + b->Radius() - dist;
                double sepX = normal.dx * overlap * 0.5;
                double sepY = normal.dy * overlap * 0.5;
                a->SetPosition(
                    a->PositionX() - sepX,
                    a->PositionY() - sepY);
                b->SetPosition(
                    b->PositionX() + sepX,
                    b->PositionY() + sepY);
            }
        }
    }
}

void BallManager::RenderBalls(Display* /*display*/) {
    // 渲染每个活动球到 DirectDraw 表面
    for (Ball* ball : m_balls) {
        if (ball->IsActive()) {
            // display->DrawBall(ball);
        }
    }
}

void BallManager::ClearAllBalls() {
    for (Ball* ball : m_balls) {
        delete ball;
    }
    m_balls.clear();
}

} // namespace inkball
