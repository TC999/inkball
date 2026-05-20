#pragma once
// BitmapRects — 精灵图集矩形切分管理器
// 将 BMP 图集（381×400）中的子精灵映射为可查询的矩形坐标
// 采用数据驱动架构，支持配置文件加载与自动检测两种模式
// 净室设计：基于泛洪填充的连通区域检测 + 外部 CSV 配置

#include "GameTypes.h"
#include <vector>
#include <string>
#include <cstdint>

namespace inkball {

class SpriteAtlas;

class BitmapRects {
public:
    static constexpr uint32_t kDefaultBgColor = 0xFFFF00FF;  // 品红透明色
    static constexpr int32_t  kMinSpriteArea  = 4;            // 最小精灵面积（像素）
    static constexpr int32_t  kMaxSpriteId    = 255;          // 最大精灵 ID

    BitmapRects() = default;

    // ---- 配置文件操作 ----

    // 从 CSV 配置文件加载精灵坐标
    // 格式: id,left,top,right,bottom（每行一个精灵，# 为注释）
    bool LoadConfiguration(const std::wstring& path);

    // 将当前配置导出为 CSV 文件
    bool ExportConfiguration(const std::wstring& path) const;

    // ---- 自动检测 ----

    // 通过泛洪填充分析图集，自动检测所有非背景色的精灵包围盒
    // atlas: 已加载的图集像素缓冲区
    // bgColor: 透明背景色（默认品红 0xFF00FF）
    // 返回检测到的精灵数量
    int32_t AutoDetect(const SpriteAtlas& atlas, uint32_t bgColor = kDefaultBgColor);

    // ---- 查询接口 ----

    // 通过 sprite ID 查询对应的源矩形（在图集中的像素坐标）
    // 若 ID 超出范围，返回安全默认矩形 (0,0,42,42)
    Rect GetSpriteRect(int32_t spriteId) const;

    // 精灵总数
    int32_t GetCount() const { return static_cast<int32_t>(m_rects.size()); }

    // 最大有效 sprite ID
    int32_t MaxSpriteId() const { return GetCount() > 0 ? GetCount() - 1 : -1; }

    // 是否已成功加载配置
    bool IsValid() const { return m_isValid; }

    // 清空所有数据
    void Clear();

private:
    // 解析单行 CSV 数据
    bool ParseLine(const std::string& line, int32_t& id,
                   int32_t& left, int32_t& top,
                   int32_t& right, int32_t& bottom) const;

    std::vector<Rect> m_rects;    // 按 sprite ID 索引的矩形数组
    bool m_isValid = false;
};

} // namespace inkball
