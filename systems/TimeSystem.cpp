#include "data.h"
// 时间系统模块：负责把速度等级映射为固定的游戏更新间隔。

DWORD GetTickIntervalMs()
{
    switch (speedLevel)
    {
    case SPEED_MEDIUM:
        return 110;
    case SPEED_HIGH:
        return 65;
    case SPEED_LOW:
    default:
        return 170;
    }
}
