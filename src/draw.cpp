#include "data.h"
#include <iostream>
#include <windows.h>
using namespace std;

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
                for (Node* p = tailHead; p != nullptr; p = p->next)
                {
                    if (p->x == j && p->y == i)
                    {
                        cout << "o";
                        drawn = true;
                        break;
                    }
                }
                if (!drawn)
                    cout << " ";
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
