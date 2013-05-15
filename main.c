#include <unistd.h>
#include <ncurses.h>
#include <pthread.h>
#include "define.h"

int bodyLenth = START_LENGTH;   // 蛇的长度
WINDOW* mainArea;
Snake snake;
Dir nextDir;

int initWindow();
int initSnake();
int showSnake();
int moveSnake();
void* getInput(void* arg);

int main()
{
    int ret;
    int isHit;
    pthread_t getInputThread;

    initscr();

    initWindow();

    initSnake();
    showSnake();

    // 启动线程，接收按键输入
    ret = pthread_create(&getInputThread, NULL, getInput, NULL);
    if (ret != 0)
    {
        return -1;
    }

    while (true)
    {
        isHit = moveSnake();

        if (HIT_HIT == isHit)
        {
            break;
        }

        usleep(300000);
    }

    endwin();
    return 0;
}

// 初始化蛇的方向，长度，各节点的位置和显示的字符
int initSnake()
{
    int index;

    snake.dir = DOWN;
    nextDir = snake.dir;
    snake.length = START_LENGTH;

    for (index = 0; index < START_LENGTH; index++)
    {
        snake.bodyNode[index].charactor = BODY_CHAR + index;
        snake.bodyNode[index].posX = MAIN_AREA_STARTX + START_LENGTH - index;
        snake.bodyNode[index].posY = MAIN_AREA_STARTY + 1;
        snake.bodyNode[index].n = index;
    }

    return 0;
}

// 显示蛇
int showSnake()
{
    int index;

    for (index = 0; index < snake.length; index++)
    {
        move(snake.bodyNode[index].posY, snake.bodyNode[index].posX);
        printw("%c", snake.bodyNode[index].charactor);
    }

    move(0, 0);
    refresh();

    return 0;
}

// 画出游戏主区域的框
int initWindow()
{
    refresh();
/bin/bash: l: 未找到命令

    box(mainArea, 0, 0);
    wrefresh(mainArea);

    return 0;
}

// 移动，显示蛇
int moveSnake()
{
    int index;
    int oldTailNodeX = snake.bodyNode[bodyLenth - 1].posX;
    int oldTailNodeY = snake.bodyNode[bodyLenth - 1].posY;
    BodyNode tmpNode = snake.bodyNode[0];
    
    snake.dir = nextDir;

    switch (snake.dir)
    {
        case UP:
            tmpNode.posY--;
            break;

        case DOWN:
            tmpNode.posY++;
            break;

        case LEFT:
            tmpNode.posX--;
            break;

        case RIGHT:
            tmpNode.posX++;
            break;

        default:
            return -1;
    }

    if (HIT_HIT == hitCheck(&tmpNode))
    {
        return HIT_HIT;
    }

    for (index = snake.length - 1; index > 0; index--)
    {
        snake.bodyNode[index].posX = snake.bodyNode[index - 1].posX;
        snake.bodyNode[index].posY = snake.bodyNode[index - 1].posY;
    }

    snake.bodyNode[0] = tmpNode;

    move(oldTailNodeY, oldTailNodeX);
    printw(" ");
    showSnake();

    return HIT_NOHIT;
}

// 碰撞检测
int hitCheck(BodyNode* headNode)
{
    int index;
    
    // 撞到边界
    if (headNode->posX <= MAIN_AREA_STARTX ||
        headNode->posY <= MAIN_AREA_STARTY ||
        headNode->posX >= MAIN_AREA_STARTX + MAIN_AREA_WIDTH + 1 ||
        headNode->posY >= MAIN_AREA_STARTY + MAIN_AREA_HEIGHT + 1)
    {
        return HIT_HIT;
    }

    // 撞到自己
    for (index = 3; index < snake.length - 1; index++)
    {
        if (headNode->posX == snake.bodyNode[index].posX &&
            headNode->posY == snake.bodyNode[index].posY)
        {
            return HIT_HIT;
        }
    }

    return HIT_NOHIT;
}

// 取得键盘输入，改变蛇的移动方向或退出
void* getInput(void* arg)
{
    keypad(stdscr, TRUE);
    noecho();

    do
    {
        switch (getch())
        {
            case KEY_RIGHT:
                if (snake.dir != LEFT)
                {
                    nextDir = RIGHT;
                }
                break;

            case KEY_LEFT:
                if (snake.dir != RIGHT)
                {
                    nextDir = LEFT;
                }
                break;

            case KEY_UP:
                if (snake.dir != DOWN)
                {
                    nextDir = UP;
                }
                break;

            case KEY_DOWN:
                if (snake.dir != UP)
                {
                    nextDir = DOWN;
                }
                break;

            case 'q':
            case 'Q':
                return NULL;
                break;

            default:
                break;
        }
    } while (TRUE);

    return NULL;
}
