#include "data.h"
// 输入系统模块：负责按键读取、合法性校验以及 180 度反向输入过滤。
#include <conio.h>

static bool TryMapDirectionKey(char key, eDirection &outDirection)
{
    switch (key)
    {
    case 'a':
    case 'A':
        outDirection = LEFT;
        return true;
    case 'd':
    case 'D':
        outDirection = RIGHT;
        return true;
    case 'w':
    case 'W':
        outDirection = UP;
        return true;
    case 's':
    case 'S':
        outDirection = DOWN;
        return true;
    default:
        return false;
    }
}

bool IsOpposite(eDirection dir1, eDirection dir2)
{
    return (dir1 == LEFT && dir2 == RIGHT) ||
           (dir1 == RIGHT && dir2 == LEFT) ||
           (dir1 == UP && dir2 == DOWN) ||
           (dir1 == DOWN && dir2 == UP);
}

bool TryGetValidatedDirection(char key, eDirection currentDirection, eDirection &outDirection)
{
    eDirection candidate = STOP;
    if (!TryMapDirectionKey(key, candidate))
    {
        // 非方向键直接判定为无效输入。
        return false;
    }

    if (currentDirection != STOP && IsOpposite(candidate, currentDirection))
    {
        // 当前方向的 180 度反向必须禁止，否则会立刻撞到自己。
        return false;
    }

    outDirection = candidate;
    return true;
}

void Input()
{
    if (!_kbhit())
    {
        return;
    }

    // 旧输入入口也走统一校验，避免不同路径出现不一致的方向处理。
    const char key = static_cast<char>(_getch());
    eDirection nextDirection = dir;
    if (TryGetValidatedDirection(key, dir, nextDirection))
    {
        dir = nextDirection;
    }
    else if (key == 'x' || key == 'X')
    {
        isGameOver = true;
    }
}
