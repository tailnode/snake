#include <unistd.h>
#include <ncurses.h>
#include <pthread.h>
#include "define.h"

extern Snake snake;
extern pthread_mutex_t mp;
extern pthread_t mainThread;

static pthread_t inputThread;
static bool isPause = false;
Dir nextDir;

// 取得键盘输入，改变蛇的移动方向或退出
static void* getInput(void* arg)
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

			case 'r':
			case 'R':
				// 重新开始
				pthread_kill(mainThread, SIGRESTART);
				break;

			default:
				break;
		}
	} while (true);

	return NULL;
}

// 启动线程，接收按键输入
int startInputThread()
{
	if (0 != pthread_create(&inputThread, NULL, getInput, NULL))
	{
		return -1;
	}

	return 0;
}
