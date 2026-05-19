#pragma once

// ============================================================
// BallPoint 球面采样点预计算数据
// 对应文档中的 BallPoints 概念
// 每个球实例包含32个预计算点，用于碰撞检测
// ============================================================

#include "GameTypes.h"
#include "MathUtils.h"
#include <cstdint>
#include <array>

namespace inkball {

// 球体表面的32个采样点
class BallPointData {
public:
    static constexpr int32_t kNumPoints = 32;

    BallPointData() {
        PreComputePoints();
    }

    // 获取第index个采样点
    BallPoint GetPoint(int32_t index) const {
        int32_t idx = MathUtils::Modulo(index, kNumPoints);
        return m_points[idx];
    }

    // 根据球半径重新计算采样点
    void Recalculate(int32_t radius) {
        if (m_cachedRadius != radius) {
            m_cachedRadius = radius;
            PreComputePoints();
        }
    }

    int32_t Radius() const { return m_cachedRadius; }

private:
    // 预计算32个点在圆周上的偏移量
    // 等角度（360°/32 = 11.25°）分布
    void PreComputePoints() {
        for (int32_t i = 0; i < kNumPoints; ++i) {
            double angle = (360.0 / kNumPoints) * i;
            m_points[i].dx = static_cast<int32_t>(
                m_cachedRadius * MathUtils::CosDeg(angle));
            m_points[i].dy = static_cast<int32_t>(
                m_cachedRadius * MathUtils::SinDeg(angle));
        }
    }

    int32_t m_cachedRadius = 10;
    std::array<BallPoint, kNumPoints> m_points;
};

} // namespace inkball
