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
bool isPause = false;
pthread_mutex_t mp = PTHREAD_MUTEX_INITIALIZER;

int initGame();
void exitGame(int exitCode);
int initWindow();
int initSnake();
int showSnake();
int moveSnake();
void* getInput(void* arg);
int genApple();
int hitCheck(BodyNode* headNode);
bool hitBorder(int x, int y);
bool hitSnake(int x, int y);
bool hitApple(int x, int y);
int eatApple();
void updateDelayTime();
void pauseGame(int);

int main()
{
	int isHit;

	initscr();

	if (0 != initGame())
	{
		exitGame(-1);
	}

	while (true)
	{
		isHit = moveSnake();

		if (HIT_NOHIT != isHit)
		{
			break;
		}

		usleep(delayTime);
	}
	
	exitGame(0);
}

// 初始化蛇的方向，长度，各节点的位置和显示的字符
int initSnake()
{
	snake.dir = DOWN;
	nextDir = snake.dir;
	snake.length = START_LENGTH;

	for (size_t i = 0; i < START_LENGTH; i++)
	{
		snake.bodyNode[i].charactor = BODY_CHAR_DOWN;
		snake.bodyNode[i].posX = MAIN_AREA_STARTX + START_LENGTH - i;
		snake.bodyNode[i].posY = MAIN_AREA_STARTY + 1;
		snake.bodyNode[i].n = i;
	}

	return 0;
}

// 显示蛇
int showSnake()
{
	for (size_t i = 0; i < snake.length; i++)
	{
		move(snake.bodyNode[i].posY, snake.bodyNode[i].posX);
		printw("%c", snake.bodyNode[i].charactor);
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
	int hitResult;
	int oldTailNodeX = snake.bodyNode[snake.length - 1].posX;
	int oldTailNodeY = snake.bodyNode[snake.length - 1].posY;
	BodyNode tmpNode = snake.bodyNode[0];

	snake.dir = nextDir;

	tmpNode.charactor = matrix[snake.dir].charactor;
	tmpNode.posX += matrix[snake.dir].posX_offset;
	tmpNode.posY += matrix[snake.dir].posY_offset;

	hitResult = hitCheck(&tmpNode);
	if (HIT_NOHIT == hitResult)
	{
		for (size_t i = snake.length - 1; i > 0; i--)
		{
			snake.bodyNode[i].posX = snake.bodyNode[i - 1].posX;
			snake.bodyNode[i].posY = snake.bodyNode[i - 1].posY;
			snake.bodyNode[i].charactor = snake.bodyNode[i - 1].charactor;
		}

		snake.bodyNode[0] = tmpNode;

		move(oldTailNodeY, oldTailNodeX);
		printw(" ");
	}
	else if (HIT_HITAPPLE == hitResult)
	{
		score++;
		updateDelayTime();

		mvprintw(INFO_AREA_STARTY, INFO_AREA_STARTX + 2, "score: %-4d", score);
		mvprintw(INFO_AREA_STARTY + 1, INFO_AREA_STARTX + 2, "speed level: %-4d", speedLevel);
		move(0, 0);
		eatApple();
		genApple();
	}
	else
	{
		return -1;
	}

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
		return HIT_HITAPPLE;
	}

	return HIT_NOHIT;
}

// 取得键盘输入，改变蛇的移动方向或退出
void* getInput(void* arg)
{
	keypad(stdscr, true);
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
				exitGame(0);
				break;

			case 'p':
			case 'P':
				// 向主线程发送信号使其阻塞
				if (false == isPause)
				{
					pthread_mutex_lock(&mp);
					isPause = true;

					pthread_kill(mainThread, SIGPAUSE);
				}
				// 解开互斥锁，使主线程恢复运行
				else
				{
					pthread_mutex_unlock(&mp);
					isPause = false;
				}
				break;	

			default:
				break;
		}
	} while (true);

	return NULL;
}

int genApple()
{
	int tmpX;
	int tmpY;

	while (true)
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

	mvprintw(apple.posY, apple.posX, APPLE_CHAR);
	move(0, 0);

	return 0;
}

bool hitBorder(int x, int y)
{
	if (x <= MAIN_AREA_STARTX ||
			y <= MAIN_AREA_STARTY ||
			x >= MAIN_AREA_STARTX + MAIN_AREA_WIDTH + 1 ||
			y >= MAIN_AREA_STARTY + MAIN_AREA_HEIGHT + 1)
	{
		return true;
	}

	return false;
}

bool hitSnake(int x, int y)
{
	for (size_t i = 0; i < snake.length - 1; i++)
	{
		if (x == snake.bodyNode[i].posX &&
				y == snake.bodyNode[i].posY)
		{
			return true;
		}
	}

	return false;
}

bool hitApple(int x, int y)
{
	if (x == apple.posX &&
			y == apple.posY)
	{
		return true;
	}

	return false;
}

int eatApple()
{
	BodyNode* tmp = (BodyNode*)malloc(snake.length * sizeof(BodyNode));
	memcpy(tmp, &snake.bodyNode[0], snake.length * sizeof(BodyNode));
	memcpy(&snake.bodyNode[1], tmp, snake.length * sizeof(BodyNode));

	snake.length++;

	snake.bodyNode[0].posX = apple.posX;
	snake.bodyNode[0].posY = apple.posY;
	snake.bodyNode[0].charactor = matrix[snake.dir].charactor;

	for (size_t i = 0; i < snake.length; i++)
	{
		snake.bodyNode[i].n = i;
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

int initGame()
{
	pthread_t getInputThread;
	
	// 启动线程，接收按键输入
	if (0 != pthread_create(&getInputThread, NULL, getInput, NULL))
	{
		return -1;
	}

	mainThread = pthread_self();
	signal(SIGPAUSE, pauseGame);
	srand((int)time(0));
	initWindow();
	initSnake();
	showSnake();
	genApple();

	return 0;
}

void exitGame(int exitCode)
{
	endwin();
	if (0 != exitCode)
	{
		printf("error.\n");
	}
	exit(exitCode);
}
