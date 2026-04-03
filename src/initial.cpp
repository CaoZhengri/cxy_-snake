#include "data.h"
#include <cstdlib>
using namespace std;

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
    fruitX = rand() % (mWidth - 2) + 1;
    fruitY = rand() % (mHeight - 2) + 1;
    mScore = 0;
    dir = STOP;
    tailHead = nullptr;
    tailEnd = nullptr;
}
