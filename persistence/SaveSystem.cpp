#include "data.h"
// 存档模块：负责游戏数据的保存、读取和完整性校验。
#include <cstdio>

static bool IsDirectionValid(eDirection direction)
{
    return direction == STOP || direction == LEFT || direction == RIGHT || direction == UP || direction == DOWN;
}

static bool IsHeadPositionValid(int x, int y)
{
    return x > 0 && x < mWidth - 1 && y >= 0 && y < mHeight;
}

static bool IsFruitPositionValid(int x, int y)
{
    return x > 0 && x < mWidth - 1 && y > 0 && y < mHeight - 1;
}

static bool IsTailNodePositionValid(int x, int y)
{
    return x > 0 && x < mWidth - 1 && y >= 0 && y < mHeight;
}

static int GetTailCount()
{
    int count = 0;
    for (Node *p = tailHead; p != nullptr; p = p->next)
    {
        count++;
    }
    return count;
}

void SaveGame(const char *filename)
{
    FILE *fp = fopen(filename, "wb");
    if (!fp)
    {
        return;
    }

    int savedState = STATE_PAUSED;
    int nTail = GetTailCount();

    fwrite(&savedState, sizeof(savedState), 1, fp);
    fwrite(&dir, sizeof(dir), 1, fp);
    fwrite(&mScore, sizeof(mScore), 1, fp);
    fwrite(&headX, sizeof(headX), 1, fp);
    fwrite(&headY, sizeof(headY), 1, fp);
    fwrite(&fruitX, sizeof(fruitX), 1, fp);
    fwrite(&fruitY, sizeof(fruitY), 1, fp);
    fwrite(&nTail, sizeof(nTail), 1, fp);

    for (Node *p = tailHead; p != nullptr; p = p->next)
    {
        fwrite(&p->x, sizeof(p->x), 1, fp);
        fwrite(&p->y, sizeof(p->y), 1, fp);
    }

    fwrite(&speedLevel, sizeof(speedLevel), 1, fp);

    fclose(fp);
}

bool LoadGame(const char *filename)
{
    FILE *fp = fopen(filename, "rb");
    if (!fp)
    {
        return false;
    }

    ClearTail();

    int savedState = STATE_PAUSED;
    int nTail = 0;
    // 先读取核心字段，任何一项失败都说明存档已经不完整或已损坏。
    bool ok =
        fread(&savedState, sizeof(savedState), 1, fp) == 1 &&
        fread(&dir, sizeof(dir), 1, fp) == 1 &&
        fread(&mScore, sizeof(mScore), 1, fp) == 1 &&
        fread(&headX, sizeof(headX), 1, fp) == 1 &&
        fread(&headY, sizeof(headY), 1, fp) == 1 &&
        fread(&fruitX, sizeof(fruitX), 1, fp) == 1 &&
        fread(&fruitY, sizeof(fruitY), 1, fp) == 1 &&
        fread(&nTail, sizeof(nTail), 1, fp) == 1;

    if (!ok || nTail < 0)
    {
        fclose(fp);
        ClearTail();
        return false;
    }

    if (!IsDirectionValid(dir) || mScore < 0 || !IsHeadPositionValid(headX, headY) || !IsFruitPositionValid(fruitX, fruitY))
    {
        // 基础状态不合法，直接判定为坏档。
        fclose(fp);
        ClearTail();
        return false;
    }

    for (int i = 0; i < nTail; i++)
    {
        Node *node = new Node;
        if (fread(&node->x, sizeof(node->x), 1, fp) != 1 ||
            fread(&node->y, sizeof(node->y), 1, fp) != 1)
        {
            delete node;
            fclose(fp);
            ClearTail();
            return false;
        }

        if (!IsTailNodePositionValid(node->x, node->y))
        {
            // 尾巴坐标越界通常意味着存档内容被破坏。
            delete node;
            fclose(fp);
            ClearTail();
            return false;
        }

        node->next = nullptr;
        node->prev = tailEnd;
        if (tailEnd != nullptr)
        {
            tailEnd->next = node;
        }
        tailEnd = node;
        if (tailHead == nullptr)
        {
            tailHead = node;
        }
    }

    int loadedSpeedLevel = SPEED_LOW;
    if (fread(&loadedSpeedLevel, sizeof(loadedSpeedLevel), 1, fp) == 1)
    {
        if (loadedSpeedLevel < SPEED_LOW || loadedSpeedLevel > SPEED_HIGH)
        {
            loadedSpeedLevel = SPEED_LOW;
        }
        // 速度参数独立保存，读档后直接恢复用户上次的选择。
        speedLevel = loadedSpeedLevel;
    }
    else
    {
        speedLevel = SPEED_LOW;
    }

    fclose(fp);
    isGameOver = false;
    gameState = (savedState == STATE_PLAYING || savedState == STATE_PAUSED) ? STATE_PAUSED : STATE_MENU;
    return true;
}
