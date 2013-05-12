#include <unistd.h>
#include <ncurses.h>
#include "define.h"

int bodyLenth = START_LENGTH;   // 蛇的长度
int initSnake(Snake* snake);
int showSnake(Snake* snake);

int main()
{
    Snake snake;
    initSnake(&snake);

    initscr();
    showSnake(&snake);

//    while (true)
//    {
//        usleep(300000);
//    }

    sleep(20);
    endwin();
    return 0;
}

int initSnake(Snake* snake)
{
    int index;

    snake->dir = RIGHT;
    snake->length = START_LENGTH;

    for (index = 0; index < START_LENGTH; index++)
    {
        snake->bodyNode[index].charactor = BODY_CHAR;
        snake->bodyNode[index].posX = START_LENGTH - 1- index;
        snake->bodyNode[index].posY = 0;
        snake->bodyNode[index].n = index;
    }

    return 0;
}

int showSnake(Snake* snake)
{
    int index;

    for (index = 0; index < snake->length; index++)
    {
        move(snake->bodyNode[index].posY, snake->bodyNode[index].posX);
        printw("%c", snake->bodyNode[index].charactor);
    }

    move(0, 0);
    refresh();
}
