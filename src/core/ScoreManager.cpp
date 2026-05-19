// ============================================================
// ScoreManager 实现
// ============================================================

#include "ScoreManager.h"
#include <algorithm>

namespace inkball {

ScoreManager::ScoreManager() {
    LoadHighScore();
}

ScoreManager::~ScoreManager() = default;

void ScoreManager::ResetScore() {
    m_levelScore = 0;
    m_comboCount = 0;
}

void ScoreManager::AddScore(int32_t points) {
    m_currentScore = std::min(m_currentScore + points, Config::kMaxScore);
    m_levelScore += points;
}

void ScoreManager::AddComboPoints(int32_t basePoints, int32_t comboLevel) {
    m_comboCount++;
    int32_t multiplier = std::min(comboLevel, 10);
    AddScore(basePoints * multiplier);
}

bool ScoreManager::IsNewHighScore() const {
    return m_currentScore > m_highScore;
}

void ScoreManager::SaveHighScore() {
    m_highScore = m_currentScore;
    // Windows Registry 持久化:
    // CRegistryManager::WriteRegValueDWORD("HighScore", m_highScore);
}

void ScoreManager::LoadHighScore() {
    // 从注册表或配置文件加载历史最高分
    // 默认值 0
    m_highScore = 0;
    m_scoresLoaded = true;
}

void ScoreManager::EndLevel(bool won) {
    if (won) {
        m_currentScore += m_levelScore;
        if (IsNewHighScore()) {
            SaveHighScore();
        }
    }
    m_levelScore = 0;
    m_comboCount = 0;
}

} // namespace inkball
