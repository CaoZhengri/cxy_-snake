#include "data.h"
// 食物模块：负责在合法区域内生成水果位置。
#include <cstdlib>

void SpawnFood()
{
    fruitX = rand() % (mWidth - 2) + 1;
    fruitY = rand() % (mHeight - 2) + 1;
}
