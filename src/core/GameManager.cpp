// ============================================================
// GameManager 实现
// ============================================================

#include "GameManager.h"
#include "GameBoard.h"
#include "Display.h"
#include "BallManager.h"
#include "ScoreManager.h"
#include "TimeManager.h"
#include "Ink.h"
#include <stdexcept>

namespace inkball {

// 内部实现细节
class GameManager::GameManagerImpl {
public:
    HINSTANCE hInstance = nullptr;
    HICON hIcon = nullptr;
    bool windowCreated = false;
};

GameManager::GameManager()
    : m_impl(std::make_unique<GameManagerImpl>())
{
}

GameManager::~GameManager() {
    Shutdown();
}

bool GameManager::Initialize(void* hInstance, int32_t nCmdShow) {
    m_impl->hInstance = static_cast<HINSTANCE>(hInstance);

    // 1. 创建窗口
    if (!CreateGameWindow(hInstance, nCmdShow)) {
        return false;
    }

    // 2. 创建游戏面板
    m_board = std::make_unique<GameBoard>();

    RECT clientRect;
    GetClientRect(static_cast<HWND>(m_hWnd), &clientRect);
    int32_t width = clientRect.right - clientRect.left;
    int32_t height = clientRect.bottom - clientRect.top;

    if (!m_board->Initialize(m_hWnd, width, height)) {
        return false;
    }

    // 3. 加载难度
    LoadDifficulty();

    // 4. 开始第一关
    StartNewLevel();

    m_isRunning = true;
    return true;
}

void GameManager::Shutdown() {
    m_isRunning = false;

    if (m_board) {
        m_board->Shutdown();
        m_board.reset();
    }

    if (m_hWnd) {
        DestroyWindow(static_cast<HWND>(m_hWnd));
        m_hWnd = nullptr;
    }

    if (m_impl->windowCreated) {
        UnregisterClassW(m_windowClassName.c_str(), m_impl->hInstance);
        m_impl->windowCreated = false;
    }
}

bool GameManager::CreateGameWindow(void* hInstance, int32_t nCmdShow) {
    m_windowClassName = L"InkBallCleanRoom";

    // 注册窗口类
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = DefWindowProcW;
    wc.hInstance = m_impl->hInstance;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    wc.lpszClassName = m_windowClassName.c_str();

    if (!RegisterClassExW(&wc)) {
        return false;
    }
    m_impl->windowCreated = true;

    // 创建窗口
    m_hWnd = CreateWindowExW(
        0,
        m_windowClassName.c_str(),
        L"InkBall (Clean Room Implementation)",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 650, // 有效游戏区域 + 标题栏
        nullptr, nullptr, m_impl->hInstance, nullptr);

    if (!m_hWnd) return false;

    ShowWindow(static_cast<HWND>(m_hWnd), nCmdShow);
    UpdateWindow(static_cast<HWND>(m_hWnd));

    return true;
}

void GameManager::PerformGameUpdate() {
    if (!m_isRunning || !m_board) return;

    BoardState state = m_board->GetState();

    switch (state) {
    case BoardState::Playing:
        // 使用固定帧间隔（15ms ≈ 66.7 FPS）
        m_board->Update(Config::kFrameRateMs / 1000.0);
        m_board->Render();
        break;

    case BoardState::LevelComplete:
        OnLevelComplete();
        break;

    case BoardState::GameOver:
        OnGameOver();
        break;

    case BoardState::Transitioning:
        break;

    case BoardState::Loading:
    case BoardState::Paused:
    default:
        break;
    }
}

void GameManager::LoadDifficulty() {
    // 从注册表或默认值加载难度设置
    // 默认从 Beginner 开始
    m_difficultyLevel = Difficulty::Beginner;

    // 通知 game board 更新相关参数
    if (m_board) {
        double timerSeconds = Config::kDefaultTimer;
        int32_t inkBudget = Config::kDefaultInkBudget;

        switch (m_difficultyLevel) {
        case Difficulty::Beginner:
            timerSeconds = 120.0;
            inkBudget = 1500;
            break;
        case Difficulty::Intermediate:
            timerSeconds = 90.0;
            inkBudget = 1200;
            break;
        case Difficulty::Advanced:
            timerSeconds = 60.0;
            inkBudget = 900;
            break;
        case Difficulty::Expert:
            timerSeconds = 45.0;
            inkBudget = 600;
            break;
        }

        m_board->GetTimeManager()->StartTimer(timerSeconds);
        m_board->GetInk()->SetInkBudget(inkBudget);
    }
}

void GameManager::SetDifficulty(Difficulty diff) {
    m_difficultyLevel = diff;
    LoadDifficulty();
}

void GameManager::StartNewLevel() {
    if (!m_board) return;

    m_board->StartNewGame();
    m_board->LoadLevelFromResource(m_currentLevelIndex);
}

void GameManager::OnGameOver() {
    if (!m_board) return;

    // 保存分数
    ScoreManager* scores = m_board->GetScoreManager();
    if (scores && scores->IsNewHighScore()) {
        scores->SaveHighScore();
    }

    // 短暂延迟后重新开始
    // 实际游戏中会显示"Game Over"画面并等待玩家输入
    StartNewLevel();
}

void GameManager::OnLevelComplete() {
    if (!m_board) return;

    m_currentLevelIndex++;
    if (m_currentLevelIndex >= 4) {
        // 循环难度
        m_currentLevelIndex = 0;
        SetDifficulty(static_cast<Difficulty>(
            (static_cast<int32_t>(m_difficultyLevel) + 1) %
            static_cast<int32_t>(Difficulty::Count)));
    }

    StartNewLevel();
}

void GameManager::ProcessMouseEvent(int32_t x, int32_t y, int32_t buttons) {
    if (!m_board || !m_board->IsPlaying()) return;

    Ink* ink = m_board->GetInk();
    if (!ink) return;

    if (buttons & MK_LBUTTON) {
        // 左键: 绘制墨水
        static bool wasDrawing = false;
        if (!wasDrawing) {
            ink->BeginStroke(x, y);
            wasDrawing = true;
        } else {
            ink->ExtendStroke(x, y);
        }
    } else if (buttons == 0) {
        // 释放: 结束笔画
        ink->EndStroke();
    }

    if (buttons & MK_RBUTTON) {
        // 右键: 清除所有墨水
        ink->ClearAllStrokes();
    }
}

void GameManager::ProcessKeyboardEvent(int32_t keyCode, bool down) {
    if (!down) return;

    switch (keyCode) {
    case VK_ESCAPE:
        m_isRunning = false;
        PostQuitMessage(0);
        break;
    case VK_SPACE:
        if (m_board) {
            if (m_board->IsPlaying()) {
                m_board->SetState(BoardState::Paused);
            } else if (m_board->GetState() == BoardState::Paused) {
                m_board->SetState(BoardState::Playing);
            }
        }
        break;
    case VK_RETURN:
        if (m_board && !m_board->IsPlaying()) {
            StartNewLevel();
        }
        break;
    case 'R':
        if (m_board) {
            StartNewLevel();
        }
        break;
    case '1': SetDifficulty(Difficulty::Beginner); StartNewLevel(); break;
    case '2': SetDifficulty(Difficulty::Intermediate); StartNewLevel(); break;
    case '3': SetDifficulty(Difficulty::Advanced); StartNewLevel(); break;
    case '4': SetDifficulty(Difficulty::Expert); StartNewLevel(); break;
    }
}

Display* GameManager::GetDisplay() const {
    return m_board ? m_board->GetDisplay() : nullptr;
}

Ink* GameManager::GetInk() const {
    return m_board ? m_board->GetInk() : nullptr;
}

bool GameManager::ProcessCommandLine(const std::wstring& cmdLine) {
    // 处理 /r:path 加载自定义关卡
    if (cmdLine.find(L"/r:") != std::wstring::npos) {
        // 从文件加载关卡
        return true;
    }

    // 默认: 随机加载资源中的关卡
    return true;
}

} // namespace inkball
