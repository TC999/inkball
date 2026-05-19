#pragma once

// ============================================================
// BallManager — 球体池管理器
// 维护所有活动球体的生命周期，负责球的添加、删除、
// 位置更新和渲染
// 对应文档中的 CBallManager，独立设计
// ============================================================

#include "GameTypes.h"
#include "Ball.h"
#include <vector>
#include <memory>

namespace inkball {

class Display;
class GameBoard;

class BallManager {
public:
    BallManager();
    ~BallManager();

    // ---- 生命周期 ----
    void Initialize();
    bool InitSurface(Display* display);

    // ---- 球管理 ----
    Ball* AddBall(const Ball& ball);
    Ball* AddBall(double x, double y, int32_t radius, BallColor color);
    void RemoveBall(Ball* ball);
    void RemoveDeactivatedBalls();
    void RemoveBallAt(size_t index);

    // ---- 更新 ----
    void UpdateBallPositions(double deltaTime);

    // ---- 查询 ----
    size_t ActiveBallCount() const;
    Ball* GetBallAt(size_t index);
    const Ball* GetBallAt(size_t index) const;
    const std::vector<Ball*>& GetBalls() const { return m_balls; }

    // ---- 批量检查 ----
    // 检查所有球对是否相交
    void CheckBallBallCollisions();

    // ---- 渲染 ----
    void RenderBalls(Display* display);

    // ---- 清除 ----
    void ClearAllBalls();

private:
    std::vector<Ball*> m_balls;
    size_t m_maxBalls = Config::kMaxBalls;
    bool m_surfaceReady = false;
};

} // namespace inkball
