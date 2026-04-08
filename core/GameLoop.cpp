#include "data.h"
// 游戏主循环模块：负责固定步长更新、输入轮询和状态流转调度。
#include <windows.h>

static const DWORD kIdleSleepMs = 1;
static const int kMaxUpdatesPerFrame = 5;

static void UpdateGame(bool &running)
{
    (void)running;
    if (gameState != STATE_PLAYING)
    {
        return;
    }

    Logic();
    if (isGameOver)
    {
        gameState = STATE_OVER;
    }
}

void RunGameLoop()
{
    bool running = true;
    bool showLoadError = false;
    DWORD previousTick = GetTickCount();
    DWORD accumulatedMs = 0;

    RenderState(showLoadError);

    while (running)
    {
        ProcessStateInput(running, showLoadError);

        DWORD nowTick = GetTickCount();
        DWORD elapsedMs = nowTick - previousTick;
        previousTick = nowTick;

        if (gameState == STATE_PLAYING)
        {
            // 累积经过时间，达到固定步长后才推进一次游戏逻辑。
            accumulatedMs += elapsedMs;
            const DWORD tickIntervalMs = GetTickIntervalMs();
            const DWORD maxAccumulated = tickIntervalMs * static_cast<DWORD>(kMaxUpdatesPerFrame);
            if (accumulatedMs > maxAccumulated)
            {
                // 卡顿时限制补帧数量，避免一次追赶太多导致画面突变。
                accumulatedMs = maxAccumulated;
            }

            int updateCount = 0;
            while (accumulatedMs >= tickIntervalMs && gameState == STATE_PLAYING && running && updateCount < kMaxUpdatesPerFrame)
            {
                UpdateGame(running);
                accumulatedMs -= tickIntervalMs;
                updateCount++;
            }
        }
        else
        {
            accumulatedMs = 0;
        }

        RenderState(showLoadError);
        Sleep(kIdleSleepMs);
    }
}
