#pragma once

// ============================================================
// GameBoard — 游戏面板，模拟容器
// 聚合所有子系统（球、瓦片、墨水、分数、时间、槽位）
// 负责任务协调、状态管理和每帧更新
// 对应文档中的 CGameBoard，独立设计
// ============================================================

#include "GameTypes.h"
#include <vector>
#include <memory>
#include <random>

namespace inkball {

class TileManager;
class BallManager;
class ScoreManager;
class TimeManager;
class Ink;
class Display;
class Sink;
class Ball;
class BoardTile;

class GameBoard {
public:
    GameBoard();
    ~GameBoard();

    // ---- 生命周期 ----
    bool Initialize(void* hWnd, int32_t width, int32_t height);
    void Shutdown();
    void StartNewGame();

    // ---- 关卡加载 ----
    bool LoadLevel(const std::vector<uint32_t>& tileData,
        const std::vector<uint32_t>& sinkData);
    bool LoadLevelFromResource(int32_t levelId);
    void DisplayLoadingMessage();

    // ---- 状态 ----
    BoardState GetState() const { return m_state; }
    void SetState(BoardState state);
    bool IsPlaying() const { return m_state == BoardState::Playing; }
    bool IsGameOver() const { return m_state == BoardState::GameOver; }

    // ---- 每帧更新 ----
    void Update(double deltaTime);
    void Render();

    // ---- 碰撞处理管道 ----
    void ProcessCollisions();

    // ---- 胜负判断 ----
    bool CheckWinCondition() const;
    bool CheckLoseCondition() const;
    void OnGameOver();
    void OnLevelComplete();

    // ---- 显示相关 ----
    void DisplayFrame();
    void RestoreSurfaces();
    void FreeDirectDraw();

    // ---- 访问器 ----
    TileManager* GetTileManager() const { return m_tileManager.get(); }
    BallManager* GetBallManager() const { return m_ballManager.get(); }
    ScoreManager* GetScoreManager() const { return m_scoreManager.get(); }
    TimeManager* GetTimeManager() const { return m_timeManager.get(); }
    Ink* GetInk() const { return m_ink.get(); }
    Display* GetDisplay() const { return m_display.get(); }
    BoardTile* GetTileAt(int32_t gx, int32_t gy);

    // ---- 随机数 ----
    int32_t GetRandomNumber(int32_t min, int32_t max);

    // ---- 面板尺寸 ----
    int32_t BoardWidth() const { return m_boardWidth; }
    int32_t BoardHeight() const { return m_boardHeight; }
    int32_t TileSize() const { return m_tileSize; }
    Rect GetBoardRect() const;

private:
    // 碰撞通道
    void CheckBoardBoundsCollision();
    void CheckDeflectingTileCollisions();
    void CheckNonDeflectingTileCollisions();
    void CheckBallBallCollisions();
    void CheckInkCollisions();

    // 槽位管理
    void UpdateSinks();

    // 子管理器（通过 unique_ptr 拥有）
    std::unique_ptr<TileManager> m_tileManager;
    std::unique_ptr<BallManager> m_ballManager;
    std::unique_ptr<ScoreManager> m_scoreManager;
    std::unique_ptr<TimeManager> m_timeManager;
    std::unique_ptr<Ink> m_ink;
    std::unique_ptr<Display> m_display;

    // 槽位
    std::vector<Sink*> m_sinks;

    // 状态
    BoardState m_state = BoardState::Loading;

    // 面板参数
    int32_t m_boardWidth = Config::kBoardPixelWidth;
    int32_t m_boardHeight = Config::kBoardPixelHeight;
    int32_t m_tileSize = Config::kTileSize;
    int32_t m_gridColumns = Config::kBoardWidth;
    int32_t m_gridRows = Config::kBoardHeight;
    int32_t m_boardOriginX = 0;
    int32_t m_boardOriginY = 0;

    // 难度
    Difficulty m_difficulty = Difficulty::Beginner;
    int32_t m_currentLevel = 0;

    // 随机数
    std::mt19937 m_rng;
    std::uniform_int_distribution<int32_t> m_dist;

    // 窗口句柄
    void* m_hWnd = nullptr;
};

} // namespace inkball
