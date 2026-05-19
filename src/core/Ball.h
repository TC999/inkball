#pragma once

// ============================================================
// Ball — 球体对象，游戏的核心物理实体
// 继承 MovingObject，增加圆形碰撞、32点采样检测、
// 反射计算、颜色匹配等能力
// 对应文档中的 CBall，独立设计
// ============================================================

#include "MovingObject.h"
#include "BallPoint.h"
#include "MathUtils.h"
#include <functional>

namespace inkball {

class Ball : public MovingObject {
public:
    // 碰撞命中测试回调类型
    // 参数: (worldX, worldY) — 世界坐标点
    // 返回: true 如果该点与碰撞体相交
    using HitTestFunc = std::function<bool(int32_t worldX, int32_t worldY)>;

    // ---- 构造 ----
    Ball();
    Ball(double posX, double posY, int32_t radius, BallColor color);
    ~Ball() override = default;

    // ---- 颜色 ----
    BallColor GetColor() const { return m_color; }
    void SetColor(BallColor c) { m_color = c; }

    // ---- 半径 ----
    int32_t Radius() const { return m_radius; }
    void SetRadius(int32_t r);

    // ---- 球点初始化 ----
    // 用速度方向确定前向扫描起点
    // 选择点积最大的点（最前向），向前偏移1/4圆
    void InitializeBallPoints();

    // ---- 点迭代 ----
    BallPoint GetCurrentPoint() const;
    BallPoint GetNextPoint();               // 循环迭代下一点
    BallPoint GetPointAt(int32_t index) const;
    int32_t GetCurrentPointIndex() const { return m_pointCurrent; }

    // ---- 反射 ----
    // 标准向量反射: v' = v - 2(v·n)/(n·n)·n
    void Deflect(const Vector2D& normal);
    void Deflect(double normalX, double normalY);

    // ---- 边界检测 ----
    // 检查并处理与面板边界的碰撞，若碰撞则反弹
    bool CheckBoardBounds(const Rect& boardRect);

    // ---- 碰撞计算 ----
    // 通用碰撞: 遍历32个点，使用 hitTest 回调判断碰撞
    // 找到第一个到最后一个碰撞点的范围，取中点法线反射
    // 返回碰撞中点索引，-1表示无碰撞
    int32_t Collide(HitTestFunc hitTest);

    // ---- 两球相交测试 ----
    bool IntersectsWith(const Ball& other) const;

    // ---- 位置更新（覆盖） ----
    void UpdatePosition(double deltaTime) override;

    // ---- 状态查询 ----
    bool IsActive() const { return m_isActive; }
    void Deactivate() { m_isActive = false; }
    void Activate() { m_isActive = true; }

    // 获取球在地图坐标系中的中心点
    int32_t WorldCenterX() const { return static_cast<int32_t>(CenterX()); }
    int32_t WorldCenterY() const { return static_cast<int32_t>(CenterY()); }

private:
    int32_t FindForwardMostPoint() const;
    BallPoint GetWorldPoint(int32_t index) const;

    int32_t m_radius = 10;
    BallColor m_color = BallColor::None;
    bool m_isActive = true;

    // 32点圆形采样数据
    BallPointData m_pointData;
    int32_t m_pointCurrent = 0;   // 当前迭代点索引
    int32_t m_pointStart = 0;     // 本轮迭代起始点
    int32_t m_pointEnd = 0;       // 本轮迭代结束点（含）
    int32_t m_pointWrap = 0;      // 迭代辅助变量
};

} // namespace inkball
