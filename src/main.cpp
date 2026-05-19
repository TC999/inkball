// ============================================================
// Main entry - console bridge + game loop
// All window messages handled internally by GameManager
// ============================================================

#include <Windows.h>
#include <random>
#include <string>
#include <memory>
#include <cstdio>

#include "core/GameManager.h"

using namespace inkball;

static std::unique_ptr<GameManager> g_pGameManager;

void DebugPrint(const wchar_t* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vwprintf(fmt, args);
    va_end(args);
    fflush(stdout);
}

void ShowError(const wchar_t* msg) {
    MessageBoxW(nullptr, msg, L"Error", MB_ICONERROR | MB_OK);
}

void InitTracing() {
    DebugPrint(L"[TRACE] Engine tracing initialized\n");
}

void CleanupTracing() {
    DebugPrint(L"[TRACE] Engine tracing cleaned up\n");
}

bool InitializeInkInput() {
    DebugPrint(L"[INPUT] Mouse-based ink input initialized\n");
    return true;
}

static int RunGame(HINSTANCE hInstance, LPWSTR lpCmdLine, int nCmdShow) {
    InitTracing();
    HeapSetInformation(nullptr, HeapEnableTerminationOnCorruption, nullptr, 0);
    SetProcessDPIAware();

    if (!InitializeInkInput()) {
        ShowError(L"Failed to initialize ink input system");
        return 1;
    }

    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(hr)) {
        ShowError(L"COM initialization failed");
        return 1;
    }

    g_pGameManager = std::make_unique<GameManager>();
    if (!g_pGameManager->Initialize(hInstance, nCmdShow)) {
        ShowError(L"Game initialization failed");
        g_pGameManager.reset();
        CoUninitialize();
        return 1;
    }

    g_pGameManager->ProcessCommandLine(lpCmdLine);

    auto lastFrameTime = GetTickCount64();
    constexpr double kFrameIntervalMs = 15.0;

    MSG msg = {};
    while (g_pGameManager->IsRunning()) {
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                g_pGameManager.reset();
                CoUninitialize();
                CleanupTracing();
                return static_cast<int>(msg.wParam);
            }
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }

        auto now = GetTickCount64();
        if (now - lastFrameTime >= static_cast<uint64_t>(kFrameIntervalMs)) {
            g_pGameManager->PerformGameUpdate();
            lastFrameTime = now;
        } else {
            Sleep(1);
        }
    }

    g_pGameManager.reset();
    CoUninitialize();
    CleanupTracing();
    return 0;
}

// Console subsystem bridge
int main() {
    return RunGame(GetModuleHandleW(nullptr), GetCommandLineW(), SW_SHOWDEFAULT);
}

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR lpCmdLine, int nCmdShow) {
    AllocConsole();
    FILE* pStdout = nullptr;
    freopen_s(&pStdout, "CONOUT$", "w", stdout);
    freopen_s(&pStdout, "CONOUT$", "w", stderr);
    return RunGame(hInstance, lpCmdLine, nCmdShow);
}
