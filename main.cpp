#include "data.h"
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
int speedLevel;
eDirection dir;
GameState gameState;
Node *tailHead = nullptr;
Node *tailEnd = nullptr;
char ScreenData[100][100] = {};

int main()
{
    srand(static_cast<unsigned int>(time(nullptr)));
    Initial();

    RunGameLoop();

    SetConsoleActiveScreenBuffer(hConsole);
    system("cls");
    ClearTail();
    return 0;
}
