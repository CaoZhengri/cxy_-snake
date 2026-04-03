#include "data.h"
#include <cwchar>
#include <windows.h>

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
                for (Node* p = tailHead; p != nullptr; p = p->next)
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
    }
}

void Show_Double_Buffer()
{
    HANDLE hBuf;
    WORD textColor;
    wchar_t scoreLine[64];

    Draw2();

    if (BufferSwapFlag == false)
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

    SetConsoleActiveScreenBuffer(hBuf);
}
