#pragma once

// ============================================================
// BoardTile — 所有瓦片的基类
// 提供碰撞接口 (CareAboutCollisions / DeflectBall)
// 和靠近侧边检测 (GetClosestSide)
// 对应文档中的 CBoardTile 基类，独立设计
// ============================================================

#include "BoardObject.h"

namespace inkball {

// 前向声明
class Ball;

class BoardTile : public BoardObject {
public:
    BoardTile() = default;
    ~BoardTile() override = default;

    // ---- 瓷砖类别 ----
    TileCategory GetCategory() const { return m_category; }
    void SetCategory(TileCategory c) { m_category = c; }

    // ---- 精灵ID ----
    int32_t SpriteId() const { return m_spriteId; }
    void SetSpriteId(int32_t id) { m_spriteId = id; }

    // ---- 碰撞接口 ----
    // 返回此瓦片如何响应碰撞:
    //   0 = 不参与 (None)
    //   1 = 非反射碰撞 (NonDeflecting) — 排水口、单向力场等
    //   2 = 反射碰撞 (Deflecting) — 墙壁、弹跳器等
    virtual CollisionMode CareAboutCollisions() const {
        return CollisionMode::None;
    }

    // 当球碰撞到该瓦片时执行的操作
    // 默认实现为空操作（地板）
    virtual void DeflectBall(Ball* ball);

    // ---- 最近侧边 ----
    // 计算球距离该瓦片四个边中最近的一个
    // 用于确定碰撞法线
    virtual Vector2D GetClosestSideNormal(const Ball& ball) const;

    // ---- 编码初始化 ----
    // 子类覆盖以解码特定参数
    virtual void InitializeFromEncoding(const TileEncoding& enc) {
        // 基类默认: 根据 category 设置精灵ID
        switch (static_cast<TileCategory>(enc.category())) {
        case TileCategory::Floor:   m_spriteId = 0;  break;
        case TileCategory::Drain:   m_spriteId = 17; break;
        case TileCategory::Wall:    m_spriteId = 37; break;
        case TileCategory::BreakWall: m_spriteId = 42; break;
        case TileCategory::OneWayForce: m_spriteId = 47; break;
        case TileCategory::RailGray: m_spriteId = 77; break;
        case TileCategory::RailColored: m_spriteId = 77; break;
        case TileCategory::Chevron: m_spriteId = 102; break;
        case TileCategory::Bumper:  m_spriteId = 67; break;
        default: m_spriteId = 0; break;
        }
    }

    // ---- 转换为地板 ----
    virtual void ConvertToFloor() {
        m_category = TileCategory::Floor;
        m_spriteId = 0;
    }

    // ---- 状态查询 ----
    bool IsWall() const { return m_category == TileCategory::Wall; }
    bool IsBreakable() const { return m_category == TileCategory::BreakWall; }
    bool IsDrain() const { return m_category == TileCategory::Drain; }
    bool IsBumper() const { return m_category == TileCategory::Bumper; }

    // ---- 子类工厂 ----
    // 根据编码创建正确的瓦片子类
    static BoardTile* CreateFromEncoding(const TileEncoding& enc,
        int32_t gridX, int32_t gridY,
        int32_t tileSize);

protected:
    // 标准墙壁反射: 找到最近边方向，弹回
    void PerformStandardWallDeflection(Ball* ball, bool changeColor = false);

    TileCategory m_category = TileCategory::Floor;
    int32_t m_spriteId = 0;
};

} // namespace inkball
