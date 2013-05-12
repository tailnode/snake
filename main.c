#include <unistd.h>
#include <ncurses.h>
#include "define.h"

int bodyLenth = START_LENGTH;   // 蛇的长度
WINDOW* mainArea;
Snake snake;

int initWindow();
int initSnake();
int showSnake();
int moveSnake();

int main()
{
    initscr();

    initWindow();

    initSnake();
    showSnake();

    while (true)
    {
        moveSnake();
        usleep(300000);
    }

    sleep(20);
    endwin();
    return 0;
}

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

int initWindow()
{
    refresh();
    mainArea = newwin(MAIN_AREA_HEIGHT + 2, MAIN_AREA_WIDTH + 2, 0, 0);
    box(mainArea, 0, 0);
    wrefresh(mainArea);

    return 0;
}

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

int hitCheck(BodyNode* headNode)
{
    return HIT_NOHIT;
}
