#include "data.h"
#include <cstdlib>

void ClearTail()
{
    Node* cur = tailHead;
    while (cur != nullptr)
    {
        Node* next = cur->next;
        delete cur;
        cur = next;
    }
    tailHead = nullptr;
    tailEnd = nullptr;
}

void Logic()
{
    int oldHeadX = headX;
    int oldHeadY = headY;

    switch (dir)
    {
    case LEFT:
        headX--;
        break;
    case RIGHT:
        headX++;
        break;
    case UP:
        headY--;
        break;
    case DOWN:
        headY++;
        break;
    default:
        break;
    }

    if (headX <= 0 || headX >= mWidth - 1 || headY < 0 || headY >= mHeight)
    {
        isGameOver = true;
        return;
    }

    Node* moveNode = tailEnd;
    while (moveNode != nullptr)
    {
        Node* followNode = moveNode->prev;
        moveNode->x = (followNode == nullptr) ? oldHeadX : followNode->x;
        moveNode->y = (followNode == nullptr) ? oldHeadY : followNode->y;
        moveNode = followNode;
    }

    if (headX == fruitX && headY == fruitY)
    {
        mScore += 10;
        fruitX = rand() % (mWidth - 2) + 1;
        fruitY = rand() % (mHeight - 2) + 1;

        Node* newNode = new Node;
        newNode->x = oldHeadX;
        newNode->y = oldHeadY;
        newNode->next = tailHead;
        newNode->prev = nullptr;
        if (tailHead != nullptr)
        {
            tailHead->prev = newNode;
        }
        tailHead = newNode;
        if (tailEnd == nullptr)
        {
            tailEnd = newNode;
        }
    }

    for (Node* p = tailHead; p != nullptr; p = p->next)
    {
        if (p->x == headX && p->y == headY)
        {
            isGameOver = true;
            return;
        }
    }
}
