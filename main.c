#include <unistd.h>
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include "define.h"

WINDOW* mainArea;
Snake snake;
Apple apple;
Dir nextDir;
int score = 0;
int speedLevel = 1;
int delayTime = INIT_DELAY_TIME;
static const struct {
	char charactor;
	char posX_offset;
	char posY_offset;	
} matrix[4] = {
	{BODY_CHAR_UP,		0,	-1},	
	{BODY_CHAR_DOWN,	0,	1},	
	{BODY_CHAR_LEFT,	-1,	0},	
	{BODY_CHAR_RIGHT,	1,	0}};

pthread_t mainThread;
BOOL isPause = FALSE;
pthread_mutex_t mp = PTHREAD_MUTEX_INITIALIZER;

int initWindow();
int initSnake();
int showSnake();
int moveSnake();
void* getInput(void* arg);
int genApple();
int hitCheck(BodyNode* headNode);
BOOL hitBorder(int x, int y);
BOOL hitSnake(int x, int y);
BOOL hitApple(int x, int y);
int eatApple();
void updateDelayTime();
void pauseGame(int);

int main()
{
	int ret;
	int isHit;
	pthread_t getInputThread;
	mainThread = pthread_self();
	signal(SIGPAUSE, pauseGame);

	srand((int)time(0));

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

	genApple();

	while (TRUE)
	{
		isHit = moveSnake();

		if (HIT_NOHIT != isHit)
		{
			break;
		}

		usleep(delayTime);
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
		snake.bodyNode[index].charactor = BODY_CHAR_DOWN;
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
	mainArea = newwin(MAIN_AREA_HEIGHT + 2, MAIN_AREA_WIDTH + 2, MAIN_AREA_STARTY, MAIN_AREA_STARTX);
	box(mainArea, 0, 0);
	wrefresh(mainArea);

	mvprintw(INFO_AREA_STARTY, INFO_AREA_STARTX + 2, "score: %-4d", score);
	mvprintw(INFO_AREA_STARTY + 1, INFO_AREA_STARTX + 2, "speed level: %-4d", speedLevel);
	move(0, 0);

	return 0;
}

// 移动，显示蛇
int moveSnake()
{
	int index;
	int hitResult;
	int oldTailNodeX = snake.bodyNode[snake.length - 1].posX;
	int oldTailNodeY = snake.bodyNode[snake.length - 1].posY;
	BodyNode tmpNode = snake.bodyNode[0];

	snake.dir = nextDir;

	tmpNode.charactor = matrix[snake.dir].charactor;
	tmpNode.posX += matrix[snake.dir].posX_offset;
	tmpNode.posY += matrix[snake.dir].posY_offset;

	hitResult = hitCheck(&tmpNode);
	if (HIT_HITAPPLE == hitResult)
	{
		showSnake();
		return 0;
	}
	else if (HIT_NOHIT != hitResult)
	{
		return -1;
	}

	for (index = snake.length - 1; index > 0; index--)
	{
		snake.bodyNode[index].posX = snake.bodyNode[index - 1].posX;
		snake.bodyNode[index].posY = snake.bodyNode[index - 1].posY;
		snake.bodyNode[index].charactor = snake.bodyNode[index - 1].charactor;
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
	// 撞到边界
	if (hitBorder(headNode->posX, headNode->posY))
	{
		return HIT_HITBODER;
	}

	// 撞到自己
	if (hitSnake(headNode->posX, headNode->posY))
	{
		return HIT_HITSELF;
	}

	// 撞到苹果
	if (hitApple(headNode->posX, headNode->posY))
	{
		score++;
		updateDelayTime();

		mvprintw(INFO_AREA_STARTY, INFO_AREA_STARTX + 2, "score: %-4d", score);
		mvprintw(INFO_AREA_STARTY + 1, INFO_AREA_STARTX + 2, "speed level: %-4d", speedLevel);
		move(0, 0);
		eatApple();
		genApple();

		return HIT_HITAPPLE;
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
				endwin();
				exit(0);
				break;

			case 'p':
			case 'P':
				// 向主线程发送信号使其阻塞
				if (FALSE == isPause)
				{
					pthread_mutex_lock(&mp);
					isPause = TRUE;

					pthread_kill(mainThread, SIGPAUSE);
				}
				// 解开互斥锁，使主线程恢复运行
				else
				{
					pthread_mutex_unlock(&mp);
					isPause = FALSE;
				}
				break;	

			default:
				break;
		}
	} while (TRUE);

	return NULL;
}

int genApple()
{
	int tmpX;
	int tmpY;

	while (TRUE)
	{
		tmpX = rand() % MAIN_AREA_WIDTH + MAIN_AREA_STARTX + 1;
		tmpY = rand() % MAIN_AREA_HEIGHT + MAIN_AREA_STARTY + 1;

		if (!hitBorder(tmpX, tmpY) &&
				!hitSnake(tmpX, tmpY))
		{
			apple.posX = tmpX;
			apple.posY = tmpY;

			break;
		}
	}

	mvprintw(apple.posY, apple.posX, "A");
	move(0, 0);

	return 0;
}

BOOL hitBorder(int x, int y)
{
	if (x <= MAIN_AREA_STARTX ||
			y <= MAIN_AREA_STARTY ||
			x >= MAIN_AREA_STARTX + MAIN_AREA_WIDTH + 1 ||
			y >= MAIN_AREA_STARTY + MAIN_AREA_HEIGHT + 1)
	{
		return TRUE;
	}

	return FALSE;
}

BOOL hitSnake(int x, int y)
{
	int index;

	for (index = 0; index < snake.length - 1; index++)
	{
		if (x == snake.bodyNode[index].posX &&
				y == snake.bodyNode[index].posY)
		{
			return TRUE;
		}
	}

	return FALSE;
}

BOOL hitApple(int x, int y)
{
	if (x == apple.posX &&
			y == apple.posY)
	{
		return TRUE;
	}

	return FALSE;
}

int eatApple()
{
	int index;
	BodyNode* tmp = (BodyNode*)malloc(snake.length * sizeof(BodyNode));
	memcpy(tmp, &snake.bodyNode[0], snake.length * sizeof(BodyNode));
	memcpy(&snake.bodyNode[1], tmp, snake.length * sizeof(BodyNode));

	snake.length++;

	snake.bodyNode[0].posX = apple.posX;
	snake.bodyNode[0].posY = apple.posY;
	snake.bodyNode[0].charactor = matrix[snake.dir].charactor;

	for (index = 0; index < snake.length; index++)
	{
		snake.bodyNode[index].n = index;
	}

	free(tmp);
	tmp = NULL;

	return 0;
}

// 更新移动速度
void updateDelayTime()
{
	if (score != 0 && score % 10 == 0)
	{
		speedLevel++;
		delayTime = delayTime * 9 / 10;
	}
}

void pauseGame(int arg)
{
	// 试图锁住互斥锁，导致线程阻塞
	pthread_mutex_lock(&mp);
	pthread_mutex_unlock(&mp);
}
