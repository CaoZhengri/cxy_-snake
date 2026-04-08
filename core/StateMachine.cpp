#include "data.h"
// 状态机模块：负责菜单、游戏中、暂停、结束等状态下的输入处理与界面切换。
#include <conio.h>
#include <cstdio>
#include <cstdlib>
#include <cwchar>

static const int kMaxInputEventsPerFrame = 64;
static const DWORD kHintDurationMs = 900;

static DWORD gHintExpireTick = 0;
static bool gForceMenuRefresh = false;

static void ClearInputBuffer()
{
    while (_kbhit())
    {
        _getch();
    }
}

static void ShowTransientHint(const wchar_t *hint)
{
    wchar_t title[128];
    swprintf(title, 128, L"贪吃蛇 - %ls", hint);
    SetConsoleTitleW(title);
    gHintExpireTick = GetTickCount() + kHintDurationMs;
}

static void RefreshTransientHint()
{
    if (gHintExpireTick == 0)
    {
        return;
    }

    DWORD now = GetTickCount();
    if (now >= gHintExpireTick)
    {
        SetConsoleTitleW(L"贪吃蛇");
        gHintExpireTick = 0;
    }
}

static const wchar_t *GetSpeedName(int level)
{
    switch (level)
    {
    case SPEED_MEDIUM:
        return L"中速";
    case SPEED_HIGH:
        return L"高速";
    case SPEED_LOW:
    default:
        return L"低速";
    }
}

static void PrintWideLine(const wchar_t *text)
{
    DWORD written = 0;
    WriteConsoleW(hConsole, text, static_cast<DWORD>(wcslen(text)), &written, nullptr);
    WriteConsoleW(hConsole, L"\n", 1, &written, nullptr);
}

static void PrintWideText(const wchar_t *text)
{
    DWORD written = 0;
    WriteConsoleW(hConsole, text, static_cast<DWORD>(wcslen(text)), &written, nullptr);
}

static void ShowTextScreen(const wchar_t *title, const wchar_t *line1, const wchar_t *line2 = L"")
{
    SetConsoleActiveScreenBuffer(hConsole);
    system("cls");
    PrintWideLine(L"=====================");
    PrintWideLine(title);
    PrintWideLine(L"=====================");
    PrintWideLine(line1);
    if (line2[0] != L'\0')
    {
        PrintWideLine(line2);
    }
}

static void ShowMenuScreen(bool showLoadError)
{
    wchar_t speedLine[64];
    swprintf(speedLine, 64, L"当前速度：%ls", GetSpeedName(speedLevel));

    SetConsoleActiveScreenBuffer(hConsole);
    system("cls");
    PrintWideLine(L"=====================");
    PrintWideLine(L"      贪吃蛇");
    PrintWideLine(L"=====================");
    PrintWideLine(L"1. 开始新游戏");
    PrintWideLine(L"2. 读取存档");
    PrintWideLine(L"3. 退出游戏");
    PrintWideLine(L"4. 速度设置");
    PrintWideLine(speedLine);
    PrintWideLine(L"");
    PrintWideLine(L"-------- 玩法说明 --------");
    PrintWideLine(L"移动：使用 W A S D 控制方向");
    PrintWideLine(L"目标：吃到 F 可以加分并让蛇变长");
    PrintWideLine(L"暂停：游戏中按 P");
    PrintWideLine(L"存档：暂停后按 S，可存档并返回主菜单");
    PrintWideLine(L"读档：主菜单选择 2，可继续之前的游戏");
    PrintWideLine(L"结束：游戏中按 X，可结束当前对局");
    PrintWideLine(L"注意：撞墙或撞到自己都会游戏结束");
    PrintWideLine(L"--------------------------");
    if (showLoadError)
    {
        PrintWideLine(L"读取失败：未找到存档或存档已损坏。");
    }
    PrintWideText(L"请选择: ");
}

static void ShowSpeedScreen()
{
    // 速度页只负责选择，不在这里推进游戏状态。
    ShowTextScreen(L"    速度设置", L"1. 低速   2. 中速", L"3. 高速   Q. 返回主菜单");
    wchar_t currentSpeedLine[64];
    swprintf(currentSpeedLine, 64, L"当前速度: %ls", GetSpeedName(speedLevel));
    PrintWideLine(currentSpeedLine);
    PrintWideText(L"请选择: ");
}

static void HandleSpeedSelection()
{
    ShowSpeedScreen();

    while (true)
    {
        // 这里使用阻塞式等待，避免速度选择页反复重绘造成闪屏。
        char key = static_cast<char>(_getch());
        if (key == '1')
        {
            speedLevel = SPEED_LOW;
            break;
        }
        if (key == '2')
        {
            speedLevel = SPEED_MEDIUM;
            break;
        }
        if (key == '3')
        {
            speedLevel = SPEED_HIGH;
            break;
        }
        if (key == 'q' || key == 'Q')
        {
            break;
        }

        ShowTransientHint(L"无效速度输入");
    }

    ClearInputBuffer();
    // 返回主菜单时强制重绘，让当前速度立刻显示为新值。
    gForceMenuRefresh = true;
}

static void ResetGame()
{
    ClearTail();
    isGameOver = false;
    headX = mWidth / 2;
    headY = mHeight / 2;
    SpawnFood();
    mScore = 0;
    dir = STOP;
}

static void HandlePlayingInput()
{
    const eDirection frameBaseDirection = dir;
    eDirection latestLegalDirection = dir;
    bool hasLatestLegalDirection = false;
    bool pauseRequested = false;
    bool gameOverRequested = false;
    int processedEvents = 0;

    while (_kbhit() && processedEvents < kMaxInputEventsPerFrame)
    {
        processedEvents++;
        char key = static_cast<char>(_getch());

        if (key == 'p' || key == 'P' || key == 27)
        {
            pauseRequested = true;
            continue;
        }
        if (key == 'x' || key == 'X')
        {
            gameOverRequested = true;
            continue;
        }

        eDirection candidateDirection = STOP;
        if (!TryGetValidatedDirection(key, frameBaseDirection, candidateDirection))
        {
            eDirection probeDirection = STOP;
            if (TryGetValidatedDirection(key, STOP, probeDirection))
            {
                ShowTransientHint(L"非法方向：不能180度反向");
            }
            else
            {
                ShowTransientHint(L"无效按键");
            }
            continue;
        }

        latestLegalDirection = candidateDirection;
        hasLatestLegalDirection = true;
    }

    if (_kbhit())
    {
        ClearInputBuffer();
    }

    if (hasLatestLegalDirection)
    {
        dir = latestLegalDirection;
    }
    if (gameOverRequested)
    {
        isGameOver = true;
    }
    else if (pauseRequested)
    {
        gameState = STATE_PAUSED;
    }
}

void Initial()
{
    hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    hOutBuf = CreateConsoleScreenBuffer(
        GENERIC_WRITE,
        FILE_SHARE_WRITE,
        NULL,
        CONSOLE_TEXTMODE_BUFFER,
        NULL);
    hOutput = CreateConsoleScreenBuffer(
        GENERIC_WRITE,
        FILE_SHARE_WRITE,
        NULL,
        CONSOLE_TEXTMODE_BUFFER,
        NULL);

    CONSOLE_CURSOR_INFO cci;
    cci.bVisible = 0;
    cci.dwSize = 1;
    SetConsoleCursorInfo(hOutBuf, &cci);
    SetConsoleCursorInfo(hOutput, &cci);

    isGameOver = false;
    gameState = STATE_MENU;
    headX = mWidth / 2;
    headY = mHeight / 2;
    SpawnFood();
    mScore = 0;
    speedLevel = SPEED_LOW;
    dir = STOP;
    tailHead = nullptr;
    tailEnd = nullptr;
    SetConsoleTitleW(L"贪吃蛇");
}

void ProcessStateInput(bool &running, bool &showLoadError)
{
    RefreshTransientHint();

    switch (gameState)
    {
    case STATE_MENU:
        if (!_kbhit())
        {
            return;
        }
        switch (_getch())
        {
        case '1':
            ResetGame();
            gameState = STATE_PLAYING;
            showLoadError = false;
            ClearInputBuffer();
            break;
        case '2':
            ResetGame();
            if (LoadGame("save.dat"))
            {
                showLoadError = false;
                ClearInputBuffer();
            }
            else
            {
                // 读档失败时停留在菜单，并提示用户重新选择。
                showLoadError = true;
                gameState = STATE_MENU;
                gForceMenuRefresh = true;
            }
            break;
        case '3':
            running = false;
            break;
        case '4':
            // 速度设置是一个独立子流程，选完后自动回到主菜单。
            HandleSpeedSelection();
            showLoadError = false;
            break;
        default:
            ShowTransientHint(L"无效菜单输入");
            break;
        }
        break;

    case STATE_PLAYING:
        // 游戏中允许高频输入，但最终只保留当前帧最后一个合法方向。
        HandlePlayingInput();
        break;

    case STATE_PAUSED:
        if (!_kbhit())
        {
            return;
        }
        switch (_getch())
        {
        case 'r':
        case 'R':
            gameState = STATE_PLAYING;
            ClearInputBuffer();
            break;
        case 's':
        case 'S':
            SaveGame("save.dat");
            gameState = STATE_MENU;
            ClearInputBuffer();
            gForceMenuRefresh = true;
            break;
        case 'q':
        case 'Q':
            gameState = STATE_MENU;
            gForceMenuRefresh = true;
            break;
        default:
            ShowTransientHint(L"无效暂停输入");
            break;
        }
        break;

    case STATE_OVER:
        if (!_kbhit())
        {
            return;
        }
        switch (_getch())
        {
        case '1':
            ResetGame();
            gameState = STATE_PLAYING;
            ClearInputBuffer();
            break;
        case '2':
            gameState = STATE_MENU;
            gForceMenuRefresh = true;
            break;
        default:
            ShowTransientHint(L"无效结算输入");
            break;
        }
        break;
    }
}

void RenderState(bool showLoadError)
{
    static GameState lastNonPlayingState = STATE_PLAYING;

    if (gameState == STATE_PLAYING)
    {
        Show_Double_Buffer();
        lastNonPlayingState = STATE_PLAYING;
        return;
    }

    if (lastNonPlayingState == gameState && !(gameState == STATE_MENU && (showLoadError || gForceMenuRefresh)))
    {
        return;
    }

    if (gameState == STATE_MENU)
    {
        // 菜单页会显示当前速度，便于用户确认设置结果。
        ShowMenuScreen(showLoadError);
    }
    else if (gameState == STATE_PAUSED)
    {
        ShowTextScreen(L"      已暂停", L"R. 继续游戏   S. 保存游戏", L"Q. 返回主菜单");
        PrintWideText(L"请选择: ");
    }
    else if (gameState == STATE_OVER)
    {
        ShowTextScreen(L"    游戏结束", L"1. 再来一局", L"2. 返回主菜单");
        wchar_t scoreLine[64];
        swprintf(scoreLine, 64, L"当前分数: %d", mScore);
        PrintWideLine(scoreLine);
        PrintWideText(L"请选择: ");
    }

    lastNonPlayingState = gameState;
    gForceMenuRefresh = false;
}
