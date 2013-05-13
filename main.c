#include <unistd.h>
#include <ncurses.h>
#include <pthread.h>
#include "define.h"

int bodyLenth = START_LENGTH;   // 蛇的长度
WINDOW* mainArea;
Snake snake;

int initWindow();
int initSnake();
int showSnake();
int moveSnake();
void* getInput(void* arg);

int main()
{
    int ret;
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
        moveSnake();
        usleep(300000);
    }

    sleep(20);
    endwin();
    return 0;
}

// 初始化蛇的方向，长度，各节点的位置和显示的字符
int initSnake()
{
    int index;

    snake.dir = DOWN;
    snake.length = START_LENGTH;

    for (index = 0; index < START_LENGTH; index++)
    {
        snake.bodyNode[index].charactor = BODY_CHAR + index;
        snake.bodyNode[index].posX = START_LENGTH - index;
        snake.bodyNode[index].posY = 1;
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
    mainArea = newwin(MAIN_AREA_HEIGHT + 2, MAIN_AREA_WIDTH + 2, 0, 0);
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

    return 0;
}

// 碰撞检测
int hitCheck(BodyNode* headNode)
{
    return HIT_NOHIT;
}

// 取得键盘输入，改变蛇的移动方向或退出
void* getInput(void* arg)
{
    int key;
    
    keypad(stdscr, TRUE);
    noecho();

    do
    {
        //key = getch();
        switch (getch())
        {
            case KEY_RIGHT:
                if (snake.dir != LEFT)
                {
                    snake.dir = RIGHT;
                }
                break;

            case KEY_LEFT:
                if (snake.dir != RIGHT)
                {
                    snake.dir = LEFT;
                }
                break;

            case KEY_UP:
                if (snake.dir != DOWN)
                {
                    snake.dir = UP;
                }
                break;

            case KEY_DOWN:
                if (snake.dir != UP)
                {
                    snake.dir = DOWN;
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
