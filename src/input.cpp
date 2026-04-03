#include "data.h"
#include <conio.h>

void Input()
{
    if (_kbhit())
    {
        switch (_getch())
        {
        case 'a': dir = LEFT;  break;
        case 'd': dir = RIGHT; break;
        case 'w': dir = UP;    break;
        case 's': dir = DOWN;  break;
        case 'x': isGameOver = true; break;
        default: break;
        }
    }
}
