#ifndef DATA_H
#define DATA_H

#include <windows.h>

extern HANDLE hOutput, hOutBuf;
extern HANDLE hConsole;
extern COORD coord;
extern DWORD bytes;
extern bool BufferSwapFlag;
extern bool isGameOver;
extern const int mWidth;
extern const int mHeight;
extern int headX, headY, fruitX, fruitY, mScore;
extern int speedLevel;

enum SpeedLevel
{
    SPEED_LOW = 0,
    SPEED_MEDIUM,
    SPEED_HIGH
};

enum eDirection
{
    STOP = 0,
    LEFT,
    RIGHT,
    UP,
    DOWN
};
extern enum eDirection dir;

enum GameState
{
    STATE_MENU,
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_OVER
};
extern enum GameState gameState;

struct Node
{
    int x, y;
    Node *next;
    Node *prev;
};
extern Node *tailHead;
extern Node *tailEnd;
extern char ScreenData[100][100];

void Initial();
void SpawnFood();
void ProcessStateInput(bool &running, bool &showLoadError);
void RenderState(bool showLoadError);
DWORD GetTickIntervalMs();
void Draw();
void Draw2();
void Show_Double_Buffer();
bool IsOpposite(eDirection dir1, eDirection dir2);
bool TryGetValidatedDirection(char key, eDirection currentDirection, eDirection &outDirection);
void Input();
void Logic();
void RunGameLoop();
void SaveGame(const char *filename);
bool LoadGame(const char *filename);
void ClearTail();

#endif
