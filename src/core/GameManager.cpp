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
        SetWindowLongPtrW(static_cast<HWND>(m_hWnd), GWLP_USERDATA, 0);
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

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(wc);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wc.lpfnWndProc = &GameManager::StaticWndProc;
    wc.hInstance = m_impl->hInstance;
    wc.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
    wc.lpszClassName = m_windowClassName.c_str();

    if (!RegisterClassExW(&wc)) {
        return false;
    }
    m_impl->windowCreated = true;

    m_hWnd = CreateWindowExW(
        0,
        m_windowClassName.c_str(),
        L"InkBall (Clean Room Implementation)",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        800, 650,
        nullptr, nullptr, m_impl->hInstance,
        this);   // 通过 lpParam 传递 GameManager 指针

    if (!m_hWnd) return false;

    ShowWindow(static_cast<HWND>(m_hWnd), nCmdShow);
    UpdateWindow(static_cast<HWND>(m_hWnd));

    return true;
}

// ---- 静态窗口过程 ----
LRESULT CALLBACK GameManager::StaticWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    GameManager* pThis = nullptr;

    if (msg == WM_NCCREATE) {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        pThis = static_cast<GameManager*>(pCreate->lpCreateParams);
        if (pThis) {
            pThis->m_hWnd = hWnd;
            SetWindowLongPtrW(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
        }
    } else {
        pThis = reinterpret_cast<GameManager*>(GetWindowLongPtrW(hWnd, GWLP_USERDATA));
    }

    if (pThis) {
        return pThis->HandleMessage(hWnd, msg, wParam, lParam);
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam);
}

// ---- 消息处理 ----
LRESULT GameManager::HandleMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);

        // 立即触发一帧渲染
        if (m_board) {
            RECT clientRect;
            GetClientRect(hWnd, &clientRect);
            if (clientRect.right - clientRect.left > 0 &&
                clientRect.bottom - clientRect.top > 0) {
                m_board->Render();
            }
        }

        EndPaint(hWnd, &ps);
        return 0;
    }

    case WM_ERASEBKGND:
        return 1; // 不擦除背景，由我们的渲染接管

    case WM_LBUTTONDOWN: {
        POINT pt = { static_cast<int32_t>(LOWORD(lParam)), static_cast<int32_t>(HIWORD(lParam)) };
        ProcessMouseEvent(pt.x, pt.y, MK_LBUTTON);
        SetCapture(hWnd);
        return 0;
    }

    case WM_LBUTTONUP:
        ReleaseCapture();
        if (GetInk()) {
            GetInk()->EndStroke();
        }
        return 0;

    case WM_RBUTTONDOWN: {
        POINT pt = { static_cast<int32_t>(LOWORD(lParam)), static_cast<int32_t>(HIWORD(lParam)) };
        ProcessMouseEvent(pt.x, pt.y, MK_RBUTTON);
        return 0;
    }

    case WM_MOUSEMOVE: {
        POINT pt = { static_cast<int32_t>(LOWORD(lParam)), static_cast<int32_t>(HIWORD(lParam)) };
        ProcessMouseEvent(pt.x, pt.y, static_cast<int32_t>(wParam));
        return 0;
    }

    case WM_KEYDOWN:
        ProcessKeyboardEvent(static_cast<int32_t>(wParam), true);
        return 0;

    case WM_SIZE:
        if (m_board) {
            RECT clientRect;
            GetClientRect(hWnd, &clientRect);
            int32_t w = clientRect.right - clientRect.left;
            int32_t h = clientRect.bottom - clientRect.top;
            if (w > 0 && h > 0) {
                m_board->GetDisplay()->RestoreAllSurfaces();
            }
        }
        return 0;

    case WM_SYSCOMMAND:
        if ((wParam & 0xFFF0) == SC_SCREENSAVE ||
            (wParam & 0xFFF0) == SC_MONITORPOWER) {
            return 0;
        }
        break;

    case WM_CLOSE:
        m_isRunning = false;
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProcW(hWnd, msg, wParam, lParam);
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
