#pragma once

// ============================================================
// ScoreManager — 分数管理器
// 跟踪当前分数、连击、最高分持久化
// 对应文档中的 CScoreManager，独立设计
// ============================================================

#include "GameTypes.h"
#include <cstdint>

namespace inkball {

class ScoreManager {
public:
    ScoreManager();
    ~ScoreManager();

    // ---- 分数操作 ----
    void ResetScore();
    void AddScore(int32_t points);
    void AddComboPoints(int32_t basePoints, int32_t comboLevel);

    // ---- 查询 ----
    int32_t CurrentScore() const { return m_currentScore; }
    int32_t HighScore() const { return m_highScore; }
    int32_t ComboCount() const { return m_comboCount; }

    // ---- 最高分 ----
    bool IsNewHighScore() const;
    void SaveHighScore();
    void LoadHighScore();

    // ---- 关卡结算 ----
    void EndLevel(bool won);

private:
    int32_t m_currentScore = 0;
    int32_t m_highScore = 0;
    int32_t m_comboCount = 0;
    int32_t m_levelScore = 0;
    bool m_scoresLoaded = false;
};

} // namespace inkball
