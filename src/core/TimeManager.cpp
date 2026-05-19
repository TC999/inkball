// ============================================================
// TimeManager 实现
// ============================================================

#include "TimeManager.h"

namespace inkball {

TimeManager::TimeManager() = default;
TimeManager::~TimeManager() = default;

void TimeManager::StartTimer(double seconds) {
    m_totalTime = seconds;
    m_remaining = seconds;
    m_isExpired = false;
    m_isPaused = false;
}

void TimeManager::Update(double /*deltaTime*/) {
    if (m_isPaused || m_isExpired) return;
    m_remaining -= m_frameDelta;
    m_elapsed += m_frameDelta;
    if (m_remaining <= 0.0) {
        m_remaining = 0.0;
        m_isExpired = true;
    }
}

bool TimeManager::IsExpired() const {
    return m_isExpired || m_remaining <= 0.0;
}

void TimeManager::Pause() { m_isPaused = true; }
void TimeManager::Resume() { m_isPaused = false; }

void TimeManager::Reset() {
    m_remaining = m_totalTime;
    m_elapsed = 0.0;
    m_isExpired = false;
    m_isPaused = false;
}

} // namespace inkball
