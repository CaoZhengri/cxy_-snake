#include "data.h"
// 渲染系统模块：负责控制台绘制、双缓冲显示和分数输出。
#include <cwchar>
#include <iostream>
#include <windows.h>

using namespace std;

static const wchar_t *GetDifficultyName()
{
    switch (speedLevel)
    {
    case SPEED_MEDIUM:
        return L"中级";
    case SPEED_HIGH:
        return L"高级";
    case SPEED_LOW:
    default:
        return L"初级";
    }
}

void Draw()
{
    system("cls");
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

    for (int i = 0; i < mWidth; i++)
    {
        SetConsoleTextAttribute(h, 0x86);
        cout << "#";
    }
    cout << endl;

    for (int i = 0; i < mHeight; i++)
    {
        for (int j = 0; j < mWidth; j++)
        {
            if (j == 0 || j == mWidth - 1)
            {
                SetConsoleTextAttribute(h, 0x86);
                cout << "#";
            }
            else if (i == headY && j == headX)
            {
                SetConsoleTextAttribute(h, 0x8a);
                cout << "O";
            }
            else if (i == fruitY && j == fruitX)
            {
                SetConsoleTextAttribute(h, 0x84);
                cout << "F";
            }
            else
            {
                bool drawn = false;
                for (Node *p = tailHead; p != nullptr; p = p->next)
                {
                    if (p->x == j && p->y == i)
                    {
                        cout << "o";
                        drawn = true;
                        break;
                    }
                }
                if (!drawn)
                {
                    cout << " ";
                }
            }
        }
        cout << endl;
    }

    for (int i = 0; i < mWidth; i++)
    {
        SetConsoleTextAttribute(h, 0x86);
        cout << "#";
    }
    cout << endl;
    cout << "分数:" << mScore << endl;
}

void Draw2()
{
    for (int i = 0; i < mWidth; i++)
    {
        ScreenData[0][i] = '#';
    }

    for (int i = 0; i < mHeight; i++)
    {
        for (int j = 0; j < mWidth; j++)
        {
            if (j == 0 || j == mWidth - 1)
            {
                ScreenData[i + 1][j] = '#';
            }
            else if (i == headY && j == headX)
            {
                ScreenData[i + 1][j] = 'O';
            }
            else if (i == fruitY && j == fruitX)
            {
                ScreenData[i + 1][j] = 'F';
            }
            else
            {
                bool drawn = false;
                for (Node *p = tailHead; p != nullptr; p = p->next)
                {
                    if (p->x == j && p->y == i)
                    {
                        ScreenData[i + 1][j] = 'o';
                        drawn = true;
                        break;
                    }
                }

                if (!drawn)
                {
                    ScreenData[i + 1][j] = ' ';
                }
            }
        }
    }

    for (int j = 0; j < mWidth; j++)
    {
        ScreenData[mHeight + 1][j] = '#';
        ScreenData[mHeight + 2][j] = ' ';
        ScreenData[mHeight + 3][j] = ' ';
    }
}

void Show_Double_Buffer()
{
    HANDLE hBuf;
    WORD textColor;
    wchar_t scoreLine[64];
    wchar_t difficultyLine[64];

    Draw2();

    if (!BufferSwapFlag)
    {
        BufferSwapFlag = true;
        hBuf = hOutBuf;
    }
    else
    {
        BufferSwapFlag = false;
        hBuf = hOutput;
    }

    for (int j = 0; j < mHeight + 5; j++)
    {
        coord.Y = static_cast<SHORT>(j);
        for (int i = 0; i < mWidth + 5; i++)
        {
            coord.X = static_cast<SHORT>(i);
            if (ScreenData[j][i] == 'O')
            {
                textColor = 0x03;
            }
            else if (ScreenData[j][i] == 'o')
            {
                textColor = 0x0a;
            }
            else if (ScreenData[j][i] == 'F')
            {
                textColor = 0x04;
            }
            else
            {
                textColor = 0x06;
            }
            WriteConsoleOutputAttribute(hBuf, &textColor, 1, coord, &bytes);
        }

        coord.X = 0;
        WriteConsoleOutputCharacterA(hBuf, ScreenData[j], mWidth, coord, &bytes);
    }

    swprintf(scoreLine, 64, L"分数:%d", mScore);
    coord.X = 0;
    coord.Y = static_cast<SHORT>(mHeight + 2);
    WriteConsoleOutputCharacterW(hBuf, L"                                                  ", mWidth, coord, &bytes);
    WriteConsoleOutputCharacterW(hBuf, scoreLine, static_cast<DWORD>(wcslen(scoreLine)), coord, &bytes);

    swprintf(difficultyLine, 64, L"当前难度:%ls", GetDifficultyName());
    coord.X = 0;
    coord.Y = static_cast<SHORT>(mHeight + 3);
    WriteConsoleOutputCharacterW(hBuf, L"                                                  ", mWidth, coord, &bytes);
    WriteConsoleOutputCharacterW(hBuf, difficultyLine, static_cast<DWORD>(wcslen(difficultyLine)), coord, &bytes);

    SetConsoleActiveScreenBuffer(hBuf);
}
