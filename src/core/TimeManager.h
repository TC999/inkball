#pragma once
// TimeManager - countdown timer, frame delta, pause/resume
// Based on doc spec for CTimeManager, clean-room design

#include "GameTypes.h"

namespace inkball {

class TimeManager {
public:
    TimeManager();
    ~TimeManager();

    // ---- 倒计时 ----
    void StartTimer(double seconds);
    void Update(double /*deltaTime*/);
    double Remaining() const { return m_remaining; }

    // ---- 查询 ----
    bool IsExpired() const;
    void Pause();
    void Resume();
    bool IsPaused() const { return m_isPaused; }

    // ---- 帧时间 ----
    double FrameDelta() const { return m_frameDelta; }
    void SetFrameDelta(double dt) { m_frameDelta = dt; }

    // ---- 总游戏时间 ----
    double Elapsed() const { return m_elapsed; }
    void Reset();

private:
    double m_totalTime = Config::kDefaultTimer;
    double m_remaining = Config::kDefaultTimer;
    double m_elapsed = 0.0;
    double m_frameDelta = Config::kFrameRateMs / 1000.0;
    bool m_isExpired = false;
    bool m_isPaused = false;
};

} // namespace inkball
