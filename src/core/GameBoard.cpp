// ============================================================
// GameBoard 实现
// ============================================================

#include "GameBoard.h"
#include "TileManager.h"
#include "BallManager.h"
#include "ScoreManager.h"
#include "TimeManager.h"
#include "Ink.h"
#include "Display.h"
#include "Sink.h"
#include "Ball.h"
#include "BoardTile.h"
#include <Windows.h>

namespace inkball {

GameBoard::GameBoard()
    : m_rng(std::random_device{}())
    , m_dist(0, 1000)
{
    m_tileManager = std::make_unique<TileManager>();
    m_ballManager = std::make_unique<BallManager>();
    m_scoreManager = std::make_unique<ScoreManager>();
    m_timeManager = std::make_unique<TimeManager>();
    m_ink = std::make_unique<Ink>();
    m_display = std::make_unique<Display>();
}

GameBoard::~GameBoard() {
    Shutdown();
}

bool GameBoard::Initialize(void* hWnd, int32_t width, int32_t height) {
    m_hWnd = hWnd;
    m_boardWidth = width;
    m_boardHeight = height;

    // 1. 初始化显示系统
    if (!m_display->Initialize(hWnd, width, height)) {
        return false;
    }

    // 1b. 加载外置精灵图集
    m_display->LoadSpriteAtlas(L"assets\\BITMAP501_1.bmp");

    // 1c. 加载精灵切分配置（CSV 配置或自动检测）
    m_display->LoadBitmapRects(L"assets\\sprite_rects.csv");

    // 2. 初始化瓦片管理器
    m_tileManager->Reset(m_gridColumns, m_gridRows, m_tileSize);
    m_tileManager->InitSurface(m_display.get());

    // 3. 初始化球管理器
    m_ballManager->Initialize();
    m_ballManager->InitSurface(m_display.get());

    // 4. 初始化分数管理器
    m_scoreManager->ResetScore();

    // 5. 初始化时间管理器
    m_timeManager->StartTimer(Config::kDefaultTimer);

    // 6. 初始化墨水系统
    m_ink->Initialize(hWnd);

    return true;
}

void GameBoard::Shutdown() {
    // 按依赖顺序释放
    for (Sink* sink : m_sinks) {
        delete sink;
    }
    m_sinks.clear();

    m_ballManager->ClearAllBalls();
    m_ink->Shutdown();
    m_display->Shutdown();
}

void GameBoard::StartNewGame() {
    SetState(BoardState::Loading);
    m_scoreManager->ResetScore();
    m_timeManager->Reset();
    m_ballManager->ClearAllBalls();
    m_ink->ClearAllStrokes();
    m_currentLevel = 0;
}

bool GameBoard::LoadLevel(const std::vector<uint32_t>& tileData,
    const std::vector<uint32_t>& /*sinkData*/)
{
    DisplayLoadingMessage();

    // 重置瓦片网格
    m_tileManager->Reset(m_gridColumns, m_gridRows, m_tileSize);

    // 解析瓦片数据：每项是一个12位编码的瓦片定义
    // 格式: [gridY][gridX][encodedValue]...
    for (size_t i = 0; i + 2 < tileData.size(); i += 3) {
        int32_t gy = static_cast<int32_t>(tileData[i]);
        int32_t gx = static_cast<int32_t>(tileData[i + 1]);
        uint32_t encoded = tileData[i + 2];

        m_tileManager->CreateTileAt(gx, gy, encoded);
    }

    // 根据难度添加初始球
    int32_t initialBalls = 3 + static_cast<int32_t>(m_difficulty);
    double baseSpeed = 50.0 + static_cast<int32_t>(m_difficulty) * 20.0;

    for (int32_t i = 0; i < initialBalls; ++i) {
        double angle = (360.0 / initialBalls) * i;
        double vx = baseSpeed * MathUtils::CosDeg(angle);
        double vy = baseSpeed * MathUtils::SinDeg(angle);

        // 在面板中心偏左上生成球
        double sx = m_boardWidth * 0.5;
        double sy = m_boardHeight * 0.5;
        BallColor color = static_cast<BallColor>((i % 4) + 1);

        Ball* ball = m_ballManager->AddBall(sx, sy, 10, color);
        if (ball) {
            ball->SetVelocity(vx, vy);
        }
    }

    SetState(BoardState::Playing);
    return true;
}

bool GameBoard::LoadLevelFromResource(int32_t levelId) {
    // 从内嵌资源加载关卡数据 (.bin 文件)
    // 生成简单的测试关卡数据
    std::vector<uint32_t> tileData;

    // 生成测试墙壁边界
    for (int32_t x = 0; x < m_gridColumns; ++x) {
        TileEncoding wallEnc;
        wallEnc.SetCategory(static_cast<uint32_t>(TileCategory::Wall));
        wallEnc.SetParam0(0);

        // 顶部和底部墙壁
        tileData.push_back(0); tileData.push_back(x);
        tileData.push_back(wallEnc.Encode());

        tileData.push_back(static_cast<uint32_t>(m_gridRows - 1));
        tileData.push_back(x);
        tileData.push_back(wallEnc.Encode());
    }

    for (int32_t y = 1; y < m_gridRows - 1; ++y) {
        TileEncoding wallEnc2;
        wallEnc2.SetCategory(static_cast<uint32_t>(TileCategory::Wall));
        wallEnc2.SetParam0(0);

        // 左右墙壁
        tileData.push_back(y); tileData.push_back(0);
        tileData.push_back(wallEnc2.Encode());

        tileData.push_back(y);
        tileData.push_back(static_cast<uint32_t>(m_gridColumns - 1));
        tileData.push_back(wallEnc2.Encode());
    }

    // 添加一些内部墙壁
    TileEncoding innerWallEnc;
    innerWallEnc.SetCategory(static_cast<uint32_t>(TileCategory::Wall));
    innerWallEnc.SetParam0(0);

    tileData.push_back(10); tileData.push_back(20);
    tileData.push_back(innerWallEnc.Encode());
    tileData.push_back(10); tileData.push_back(21);
    tileData.push_back(innerWallEnc.Encode());

    // 添加弹跳器
    TileEncoding bumperEnc;
    bumperEnc.SetCategory(static_cast<uint32_t>(TileCategory::Bumper));
    bumperEnc.SetParam0(0);
    bumperEnc.SetParam1(1);

    tileData.push_back(15); tileData.push_back(30);
    tileData.push_back(bumperEnc.Encode());

    // 添加排水口
    TileEncoding drainEnc;
    drainEnc.SetCategory(static_cast<uint32_t>(TileCategory::Drain));

    tileData.push_back(20); tileData.push_back(10);
    tileData.push_back(drainEnc.Encode());

    return LoadLevel(tileData, {});
}

void GameBoard::DisplayLoadingMessage() {
    // 在显示器上绘制"Loading..."提示
    if (m_display && m_display->IsReady()) {
        // m_display->DrawText("Loading...");
    }
}

void GameBoard::SetState(BoardState state) {
    m_state = state;
}

void GameBoard::Update(double deltaTime) {
    if (m_state != BoardState::Playing) return;

    // 1. 更新时间
    m_timeManager->SetFrameDelta(deltaTime);
    m_timeManager->Update(0);

    if (m_timeManager->IsExpired()) {
        OnGameOver();
        return;
    }

    // 2. 更新球位置
    m_ballManager->UpdateBallPositions(deltaTime);

    // 3. 处理碰撞（按文档描述的五通道顺序）
    ProcessCollisions();

    // 4. 清除失活球
    m_ballManager->RemoveDeactivatedBalls();

    // 5. 检查槽位
    UpdateSinks();

    // 6. 检查胜负
    if (CheckWinCondition()) {
        OnLevelComplete();
        return;
    }
    if (CheckLoseCondition()) {
        OnGameOver();
        return;
    }
}

void GameBoard::Render() {
    m_display->BeginFrame();

    // 绘制所有瓦片
    m_tileManager->Render(m_display.get());

    // 绘制槽位
    for (Sink* sink : m_sinks) {
        m_display->DrawObject(sink, sink->SpriteId());
    }

    // 绘制球
    m_ballManager->RenderBalls(m_display.get());

    // 绘制墨水
    m_ink->DrawToSurface(m_display.get());

    m_display->EndFrame();
    m_display->Present();
}

// ---- 碰撞处理五通道 ----

void GameBoard::ProcessCollisions() {
    CheckBoardBoundsCollision();
    CheckDeflectingTileCollisions();
    CheckNonDeflectingTileCollisions();
    CheckBallBallCollisions();
    CheckInkCollisions();
}

void GameBoard::CheckBoardBoundsCollision() {
    Rect boardRect = m_display->GetBoardRect();
    for (size_t i = 0; i < m_ballManager->ActiveBallCount(); ++i) {
        Ball* ball = m_ballManager->GetBallAt(i);
        if (ball->IsActive()) {
            ball->CheckBoardBounds(boardRect);
        }
    }
}

void GameBoard::CheckDeflectingTileCollisions() {
    for (size_t i = 0; i < m_ballManager->ActiveBallCount(); ++i) {
        Ball* ball = m_ballManager->GetBallAt(i);
        if (!ball->IsActive()) continue;

        // 获取球周围的候选瓦片
        std::vector<BoardTile*> candidates;
        m_tileManager->GetSurroundingTiles(
            ball->CenterX(), ball->CenterY(),
            static_cast<double>(ball->Radius() + m_tileSize),
            candidates);

        // 在反射瓦片中找到最近的碰撞
        double bestDist = 1e10;
        BoardTile* bestTile = nullptr;

        for (BoardTile* tile : candidates) {
            if (!tile) continue;
            if (tile->CareAboutCollisions() != CollisionMode::Deflecting) continue;

            double dist = MathUtils::DistanceSq(
                ball->CenterX(), ball->CenterY(),
                tile->CenterX(), tile->CenterY());
            if (dist < bestDist) {
                bestDist = dist;
                bestTile = tile;
            }
        }

        // 验证碰撞并执行反射
        if (bestTile) {
            Rect tileBounds = bestTile->GetBounds();
            bool collided = MathUtils::CircleRectIntersect(
                ball->CenterX(), ball->CenterY(),
                static_cast<double>(ball->Radius()), tileBounds);

            if (collided) {
                bestTile->DeflectBall(ball);
            }
        }
    }
}

void GameBoard::CheckNonDeflectingTileCollisions() {
    for (size_t i = 0; i < m_ballManager->ActiveBallCount(); ++i) {
        Ball* ball = m_ballManager->GetBallAt(i);
        if (!ball->IsActive()) continue;

        std::vector<BoardTile*> candidates;
        m_tileManager->GetSurroundingTiles(
            ball->CenterX(), ball->CenterY(),
            static_cast<double>(ball->Radius() + m_tileSize),
            candidates);

        double bestDist = 1e10;
        BoardTile* bestTile = nullptr;

        for (BoardTile* tile : candidates) {
            if (!tile) continue;
            if (tile->CareAboutCollisions() != CollisionMode::NonDeflecting) continue;

            double dist = MathUtils::DistanceSq(
                ball->CenterX(), ball->CenterY(),
                tile->CenterX(), tile->CenterY());
            if (dist < bestDist) {
                bestDist = dist;
                bestTile = tile;
            }
        }

        if (bestTile) {
            Rect tileBounds = bestTile->GetBounds();
            bool collided = MathUtils::CircleRectIntersect(
                ball->CenterX(), ball->CenterY(),
                static_cast<double>(ball->Radius()), tileBounds);

            if (collided) {
                bestTile->DeflectBall(ball);
            }
        }
    }
}

void GameBoard::CheckBallBallCollisions() {
    m_ballManager->CheckBallBallCollisions();
}

void GameBoard::CheckInkCollisions() {
    for (size_t i = 0; i < m_ballManager->ActiveBallCount(); ++i) {
        Ball* ball = m_ballManager->GetBallAt(i);
        if (!ball->IsActive()) continue;

        int32_t cx = ball->WorldCenterX();
        int32_t cy = ball->WorldCenterY();
        int32_t radius = ball->Radius();

        // 先查询（不删除）
        if (m_ink->HitCircleTest(cx, cy, radius, 0)) {
            // 使用球的 Collide 方法计算反射，墨水作为碰撞几何
            auto inkCallback = [this](int32_t wx, int32_t wy) -> bool {
                return m_ink->InkHitTest(wx, wy);
            };

            int32_t result = ball->Collide(inkCallback);
            if (result >= 0) {
                // 反射成功，现在删除碰撞的墨水量
                m_ink->HitCircleTest(cx, cy, radius, 1);
                m_ink->SetInkModified(true);
            }
        }
    }
}

// ---- 槽位 ----

void GameBoard::UpdateSinks() {
    for (Sink* sink : m_sinks) {
        if (sink->IsFilled()) continue;

        for (size_t i = 0; i < m_ballManager->ActiveBallCount(); ++i) {
            Ball* ball = m_ballManager->GetBallAt(i);
            if (!ball->IsActive()) continue;

            // 检查球是否进入槽位范围
            if (MathUtils::CircleRectIntersect(
                ball->CenterX(), ball->CenterY(),
                static_cast<double>(ball->Radius() + 10),
                sink->GetBounds()))
            {
                if (sink->TryFill(ball)) {
                    m_scoreManager->AddScore(100);
                    break;
                }
            }
        }
    }
}

// ---- 胜负 ----

bool GameBoard::CheckWinCondition() const {
    if (m_sinks.empty()) return false;
    for (const Sink* sink : m_sinks) {
        if (!sink->IsFilled()) return false;
    }
    return true;
}

bool GameBoard::CheckLoseCondition() const {
    return m_ballManager->ActiveBallCount() == 0 ||
        m_timeManager->IsExpired();
}

void GameBoard::OnGameOver() {
    SetState(BoardState::GameOver);
    m_scoreManager->EndLevel(false);
}

void GameBoard::OnLevelComplete() {
    SetState(BoardState::LevelComplete);
    m_scoreManager->EndLevel(true);
    m_currentLevel++;
}

void GameBoard::DisplayFrame() {
    Render();
}

void GameBoard::RestoreSurfaces() {
    m_display->RestoreAllSurfaces();
}

void GameBoard::FreeDirectDraw() {
    m_display->Shutdown();
}

BoardTile* GameBoard::GetTileAt(int32_t gx, int32_t gy) {
    return m_tileManager->GetTileAt(gx, gy);
}

int32_t GameBoard::GetRandomNumber(int32_t min, int32_t max) {
    std::uniform_int_distribution<int32_t> dist(min, max);
    return dist(m_rng);
}

Rect GameBoard::GetBoardRect() const {
    return m_display->GetBoardRect();
}

} // namespace inkball
