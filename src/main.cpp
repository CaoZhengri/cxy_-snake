#include "data.h"
#include <conio.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <windows.h>

HANDLE hOutput, hOutBuf, hConsole;
COORD coord = {0, 0};
DWORD bytes = 0;
bool BufferSwapFlag = false;
bool isGameOver;
const int mWidth = 50;
const int mHeight = 20;
int headX, headY, fruitX, fruitY, mScore;
eDirection dir;
GameState gameState;
Node* tailHead = nullptr;
Node* tailEnd = nullptr;
char ScreenData[100][100] = {};

static void ClearInputBuffer()
{
    while (_kbhit())
    {
        _getch();
    }
}

static void PrintWideLine(const wchar_t* text)
{
    DWORD written = 0;
    WriteConsoleW(hConsole, text, static_cast<DWORD>(wcslen(text)), &written, nullptr);
    WriteConsoleW(hConsole, L"\n", 1, &written, nullptr);
}

static void PrintWideText(const wchar_t* text)
{
    DWORD written = 0;
    WriteConsoleW(hConsole, text, static_cast<DWORD>(wcslen(text)), &written, nullptr);
}

static void ShowTextScreen(const wchar_t* title, const wchar_t* line1, const wchar_t* line2 = L"")
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

static void ShowMenuScreen()
{
    SetConsoleActiveScreenBuffer(hConsole);
    system("cls");
    PrintWideLine(L"=====================");
    PrintWideLine(L"      贪吃蛇");
    PrintWideLine(L"=====================");
    PrintWideLine(L"1. 开始新游戏");
    PrintWideLine(L"2. 读取存档");
    PrintWideLine(L"3. 退出游戏");
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
}

static void ResetGame()
{
    ClearTail();
    isGameOver = false;
    headX = mWidth / 2;
    headY = mHeight / 2;
    fruitX = rand() % (mWidth - 2) + 1;
    fruitY = rand() % (mHeight - 2) + 1;
    mScore = 0;
    dir = STOP;
}

static void HandlePlayingInput()
{
    if (!_kbhit())
    {
        return;
    }

    char key = static_cast<char>(_getch());
    switch (key)
    {
    case 'a':
    case 'A':
        if (dir != RIGHT) dir = LEFT;
        break;
    case 'd':
    case 'D':
        if (dir != LEFT) dir = RIGHT;
        break;
    case 'w':
    case 'W':
        if (dir != DOWN) dir = UP;
        break;
    case 's':
    case 'S':
        if (dir != UP) dir = DOWN;
        break;
    case 'p':
    case 'P':
        gameState = STATE_PAUSED;
        break;
    case 27:
        gameState = STATE_PAUSED;
        break;
    case 'x':
    case 'X':
        isGameOver = true;
        break;
    default:
        break;
    }
}

int main()
{
    srand(static_cast<unsigned int>(time(nullptr)));
    Initial();

    while (true)
    {
        if (gameState == STATE_MENU)
        {
            ShowMenuScreen();
            PrintWideText(L"请选择: ");
            char choice = static_cast<char>(_getch());

            if (choice == '1')
            {
                ResetGame();
                gameState = STATE_PLAYING;
                ClearInputBuffer();
            }
            else if (choice == '2')
            {
                ResetGame();
                if (LoadGame("save.dat"))
                {
                    ClearInputBuffer();
                }
                else
                {
                    ShowTextScreen(L"      贪吃蛇", L"读取失败：未找到存档或存档已损坏。", L"按任意键返回主菜单...");
                    _getch();
                    gameState = STATE_MENU;
                }
            }
            else if (choice == '3')
            {
                break;
            }

            continue;
        }

        while (gameState == STATE_PLAYING)
        {
            Show_Double_Buffer();
            HandlePlayingInput();

            if (gameState != STATE_PLAYING)
            {
                break;
            }

            Logic();
            if (isGameOver)
            {
                gameState = STATE_OVER;
                break;
            }

            Sleep(100);
        }

        while (gameState == STATE_PAUSED)
        {
            ShowTextScreen(L"      已暂停", L"R. 继续游戏   S. 保存游戏", L"Q. 返回主菜单");
            PrintWideText(L"请选择: ");
            char choice = static_cast<char>(_getch());

            if (choice == 'r' || choice == 'R')
            {
                ClearInputBuffer();
                gameState = STATE_PLAYING;
            }
            else if (choice == 's' || choice == 'S')
            {
                SaveGame("save.dat");
                ClearInputBuffer();
                gameState = STATE_MENU;
            }
            else if (choice == 'q' || choice == 'Q')
            {
                gameState = STATE_MENU;
            }
        }

        while (gameState == STATE_OVER)
        {
            ShowTextScreen(L"    游戏结束", L"1. 再来一局", L"2. 返回主菜单");
            wchar_t scoreLine[64];
            swprintf(scoreLine, 64, L"当前分数: %d", mScore);
            PrintWideLine(scoreLine);
            PrintWideText(L"请选择: ");
            char choice = static_cast<char>(_getch());

            if (choice == '1')
            {
                ResetGame();
                gameState = STATE_PLAYING;
                ClearInputBuffer();
            }
            else if (choice == '2')
            {
                gameState = STATE_MENU;
            }
        }
    }

    SetConsoleActiveScreenBuffer(hConsole);
    system("cls");
    ClearTail();
    return 0;
}
