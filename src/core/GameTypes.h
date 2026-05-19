#pragma once
// InkBall Clean-Room Core Type Definitions
#include <cstdint>
#include <cmath>
#include <algorithm>

namespace inkball {

// ---- 坐标/几何基础类型 ----
struct Point2D {
    double x = 0.0;
    double y = 0.0;
};

struct Vector2D {
    double dx = 0.0;
    double dy = 0.0;

    double Dot(const Vector2D& other) const {
        return dx * other.dx + dy * other.dy;
    }

    double LengthSq() const {
        return dx * dx + dy * dy;
    }

    double Length() const {
        return std::sqrt(LengthSq());
    }

    Vector2D Normalized() const {
        double len = Length();
        if (len < 1e-10) return { 0.0, 0.0 };
        return { dx / len, dy / len };
    }
};

struct Size2D {
    int32_t width = 0;
    int32_t height = 0;
};

struct Rect {
    int32_t left = 0;
    int32_t top = 0;
    int32_t right = 0;
    int32_t bottom = 0;

    int32_t Width() const { return right - left; }
    int32_t Height() const { return bottom - top; }
    bool Contains(int32_t px, int32_t py) const {
        return px >= left && px < right && py >= top && py < bottom;
    }
    Point2D Center() const {
        return { (left + right) * 0.5, (top + bottom) * 0.5 };
    }
};

// ---- 球体颜色类型 ----
enum class BallColor : uint32_t {
    None = 0,
    Red = 1,
    Blue = 2,
    Green = 3,
    Yellow = 4,
    Count = 5
};

// ---- 瓦片类别枚举 ----
enum class TileCategory : uint32_t {
    Floor = 0,           // 地板/空地
    Drain = 2,           // 排水口（球丢失）
    Wall = 3,            // 实心墙
    BreakWall = 4,       // 可破坏墙
    OneWayForce = 5,     // 单向力场
    RailGray = 6,        // 灰色引导轨
    RailColored = 7,     // 彩色引导轨
    Chevron = 8,         // 方向引导器
    BallLauncher = 9,    // 球发射器
    Bumper = 10,         // 弹跳器
    Sink = 11            // 目标槽位
};

// ---- 瓦片编码结构 ----
// 12-bit 编码: [15-12]=category, [11-8]=param2, [7-4]=param1, [3-0]=param0
struct TileEncoding {
    uint32_t raw = 0;

    TileEncoding() : raw(0) {}
    explicit TileEncoding(uint32_t v) : raw(v) {}

    uint32_t category() const { return (raw >> 12) & 0xF; }
    uint32_t param2()  const { return (raw >> 8) & 0xF; }
    uint32_t param1()  const { return (raw >> 4) & 0xF; }
    uint32_t param0()  const { return raw & 0xF; }

    void SetCategory(uint32_t v) { raw = (raw & 0x0FFF) | ((v & 0xF) << 12); }
    void SetParam2(uint32_t v)  { raw = (raw & 0xF0FF) | ((v & 0xF) << 8); }
    void SetParam1(uint32_t v)  { raw = (raw & 0xFF0F) | ((v & 0xF) << 4); }
    void SetParam0(uint32_t v)  { raw = (raw & 0xFFF0) | (v & 0xF); }

    static TileEncoding Decode(uint32_t raw) {
        return TileEncoding(raw);
    }

    uint32_t Encode() const {
        return raw;
    }
};

// ---- 游戏面板状态 ----
enum class BoardState : uint32_t {
    Loading = 0,          // 正在加载关卡
    Playing = 1,          // 正常运行
    Paused = 2,           // 暂停
    GameOver = 3,         // 游戏失败
    LevelComplete = 4,    // 关卡完成
    Transitioning = 5     // 状态转换中
};

// ---- 难度等级 ----
enum class Difficulty : uint32_t {
    Beginner = 0,
    Intermediate = 1,
    Advanced = 2,
    Expert = 3,
    Count = 4
};

// ---- 碰撞模式 ----
enum class CollisionMode : int32_t {
    None = 0,             // 不参与碰撞
    NonDeflecting = 1,    // 非反射碰撞（消耗/吸收）
    Deflecting = 2        // 反射碰撞（反弹）
};

// ---- 方向枚举 ----
enum class Direction : uint32_t {
    Up = 0,
    Right = 1,
    Down = 2,
    Left = 3,
    None = 4
};

// ---- 球点预计算结构 ----
struct BallPoint {
    int32_t dx = 0;
    int32_t dy = 0;

    Vector2D ToVector() const {
        return { static_cast<double>(dx), static_cast<double>(dy) };
    }
};

// ---- 游戏配置常量 ----
namespace Config {
    constexpr int32_t kTileSize = 42;               // 每个瓦片像素大小
    constexpr int32_t kBoardWidth = 42;             // 面板瓦片列数
    constexpr int32_t kBoardHeight = 42;            // 面板瓦片行数
    constexpr int32_t kBoardPixelWidth = 445;       // 面板像素宽度
    constexpr int32_t kBoardPixelHeight = 445;      // 面板像素高度
    constexpr int32_t kMaxBalls = 100;              // 最大球数
    constexpr int32_t kCirclePoints = 32;           // 圆形采样点数
    constexpr int32_t kTileEncodingMask = 0xFFFF;   // 瓦片编码掩码
    constexpr int32_t kInkScaleFactor = 10;         // 墨水坐标缩放因子
    constexpr double kFrameRateMs = 15.0;           // 帧间隔（毫秒）
    constexpr int32_t kMaxScore = 999999;           // 最高分数
    constexpr double kDefaultTimer = 120.0;         // 默认游戏时间（秒）
    constexpr int32_t kDefaultInkBudget = 1000;     // 默认墨水余量
    constexpr uint32_t kTransparentColor = 0xFF00FF; // 透明色（品红）
}

} // namespace inkball
