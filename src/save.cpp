#include "data.h"
#include <cstdio>

static int GetTailCount()
{
    int count = 0;
    for (Node* p = tailHead; p != nullptr; p = p->next)
    {
        count++;
    }
    return count;
}

void SaveGame(const char* filename)
{
    FILE* fp = fopen(filename, "wb");
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

    for (Node* p = tailHead; p != nullptr; p = p->next)
    {
        fwrite(&p->x, sizeof(p->x), 1, fp);
        fwrite(&p->y, sizeof(p->y), 1, fp);
    }

    fclose(fp);
}

bool LoadGame(const char* filename)
{
    FILE* fp = fopen(filename, "rb");
    if (!fp)
    {
        return false;
    }

    ClearTail();

    int savedState = STATE_PAUSED;
    int nTail = 0;
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

    for (int i = 0; i < nTail; i++)
    {
        Node* node = new Node;
        if (fread(&node->x, sizeof(node->x), 1, fp) != 1 ||
            fread(&node->y, sizeof(node->y), 1, fp) != 1)
        {
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

    fclose(fp);
    isGameOver = false;
    gameState = (savedState == STATE_PLAYING || savedState == STATE_PAUSED)
        ? STATE_PAUSED
        : STATE_MENU;
    return true;
}
