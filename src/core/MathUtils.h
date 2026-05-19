#pragma once

#include "GameTypes.h"

namespace inkball {
namespace MathUtils {

// ---- 三角函数（度数版本） ----
inline double SinDeg(double degrees) {
    static constexpr double kDegToRad = 3.14159265358979323846 / 180.0;
    return std::sin(degrees * kDegToRad);
}

inline double CosDeg(double degrees) {
    static constexpr double kDegToRad = 3.14159265358979323846 / 180.0;
    return std::cos(degrees * kDegToRad);
}

inline double Atan2Deg(double y, double x) {
    static constexpr double kRadToDeg = 180.0 / 3.14159265358979323846;
    return std::atan2(y, x) * kRadToDeg;
}

// ---- 向量操作 ----
inline double DistanceSq(double x1, double y1, double x2, double y2) {
    double dx = x2 - x1;
    double dy = y2 - y1;
    return dx * dx + dy * dy;
}

inline double Distance(double x1, double y1, double x2, double y2) {
    return std::sqrt(DistanceSq(x1, y1, x2, y2));
}

// ---- 反射计算 ----
// 标准向量反射: v' = v - 2 * (v·n) / (n·n) * n
inline void Reflect(Vector2D& velocity, const Vector2D& normal) {
    double dot = velocity.Dot(normal);
    double normSq = normal.LengthSq();
    if (normSq < 1e-10) return;
    double factor = 2.0 * dot / normSq;
    velocity.dx -= factor * normal.dx;
    velocity.dy -= factor * normal.dy;
}

inline void Reflect(Vector2D& velocity, double normalX, double normalY) {
    Vector2D n = { normalX, normalY };
    Reflect(velocity, n);
}

// ---- 值域裁剪 ----
template<typename T>
inline T Clamp(T value, T minVal, T maxVal) {
    return std::max(minVal, std::min(maxVal, value));
}

// ---- 取模（处理负数） ----
inline int32_t Modulo(int32_t a, int32_t b) {
    int32_t r = a % b;
    return r < 0 ? r + b : r;
}

// ---- 线段相交测试 ----
inline bool LineSegmentsIntersect(
    double p0_x, double p0_y, double p1_x, double p1_y,
    double p2_x, double p2_y, double p3_x, double p3_y)
{
    double s1_x = p1_x - p0_x;
    double s1_y = p1_y - p0_y;
    double s2_x = p3_x - p2_x;
    double s2_y = p3_y - p2_y;

    double denom = (-s2_x * s1_y + s1_x * s2_y);
    if (std::abs(denom) < 1e-10) return false;

    double s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / denom;
    double t = ( s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / denom;

    return s >= 0.0 && s <= 1.0 && t >= 0.0 && t <= 1.0;
}

// ---- 点是否在圆内 ----
inline bool PointInCircle(double px, double py, double cx, double cy, double radius) {
    return DistanceSq(px, py, cx, cy) <= radius * radius;
}

// ---- 圆与矩形是否相交 ----
inline bool CircleRectIntersect(double cx, double cy, double radius, const Rect& r) {
    double closestX = Clamp(cx, static_cast<double>(r.left), static_cast<double>(r.right));
    double closestY = Clamp(cy, static_cast<double>(r.top), static_cast<double>(r.bottom));
    return DistanceSq(cx, cy, closestX, closestY) <= radius * radius;
}

// ---- 角度转方向向量 ----
inline Vector2D AngleToDirection(double degrees) {
    return { CosDeg(degrees), SinDeg(degrees) };
}

} // namespace MathUtils
} // namespace inkball
