#pragma once
// GameManager - top-level game orchestrator
// Based on doc spec for CGameManager, clean-room design

#include "GameTypes.h"
#include <Windows.h>
#include <memory>
#include <vector>
#include <string>

namespace inkball {

class GameBoard;
class Display;
class Ink;

class GameManager {
public:
    GameManager();
    ~GameManager();

    bool Initialize(void* hInstance, int32_t nCmdShow);
    void Shutdown();

    void PerformGameUpdate();

    void LoadDifficulty();
    void SetDifficulty(Difficulty diff);
    Difficulty GetDifficulty() const { return m_difficultyLevel; }

    void StartNewLevel();
    void OnGameOver();
    void OnLevelComplete();

    void ProcessMouseEvent(int32_t x, int32_t y, int32_t buttons);
    void ProcessKeyboardEvent(int32_t keyCode, bool down);

    void* GetWindowHandle() const { return m_hWnd; }
    bool IsRunning() const { return m_isRunning; }

    Display* GetDisplay() const;
    GameBoard* GetBoard() const { return m_board.get(); }
    Ink* GetInk() const;

    bool ProcessCommandLine(const std::wstring& cmdLine);

private:
    bool CreateGameWindow(void* hInstance, int32_t nCmdShow);

    std::unique_ptr<GameBoard> m_board;
    void* m_hWnd = nullptr;
    bool m_isRunning = false;
    Difficulty m_difficultyLevel = Difficulty::Beginner;
    int32_t m_currentLevelIndex = 0;

    std::wstring m_windowClassName;
    class GameManagerImpl;
    std::unique_ptr<GameManagerImpl> m_impl;
};

} // namespace inkball
