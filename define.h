#ifndef __DEFINE_H__
#define __DEFINE_H__

#include <signal.h>

#define BODY_LENGTH_MAX     (100)   // 蛇身体的最大长度
#define START_LENGTH        (5)     // 蛇的初始长度
#define BODY_CHAR_UP		('^')	// 向上移动时显示的字符
#define BODY_CHAR_DOWN		('v')	// 向下移动时显示的字符
#define BODY_CHAR_LEFT		('<')	// 向左移动时显示的字符
#define BODY_CHAR_RIGHT		('>')	// 向右移动时显示的字符

#define APPLE_CHAR			("A")	// 苹果显示的字符

#define MAIN_AREA_STARTX    (5)     // 游戏区域开始的x坐标
#define MAIN_AREA_STARTY    (5)     // 游戏区域开始的y坐标
#define MAIN_AREA_WIDTH     (20)    // 游戏区域的宽度
#define MAIN_AREA_HEIGHT    (20)    // 游戏区域的高度

#define INFO_AREA_STARTX	(MAIN_AREA_STARTX + MAIN_AREA_WIDTH + 2)
#define INFO_AREA_STARTY	(MAIN_AREA_STARTY)

#define HIT_NOHIT           (0)     // 没有碰撞
#define HIT_HITBODER        (1)     // 碰撞到边界
#define HIT_HITSELF         (2)     // 碰撞到自己
#define HIT_HITAPPLE        (3)     // 碰撞到苹果

#define INIT_DELAY_TIME		(200000)// 每次移动前的等待时间

#define SIGPAUSE	(SIGUSR1)		// 暂停的信号	

// 蛇的前进方向
typedef enum {
    UP = 0,
    DOWN,
    LEFT,
    RIGHT,
} Dir;

// 组成蛇身体的一个点
typedef struct {
    char charactor;     // 显示的字符
    unsigned char posX; // X坐标
    unsigned char posY; // Y坐标
    unsigned int n;     // 从蛇头第n个点，蛇头是0
} BodyNode;

// 一条蛇
typedef struct {
    Dir dir;
    int length;
    BodyNode bodyNode[BODY_LENGTH_MAX];
} Snake;

// 苹果
typedef struct {
    int posX;
    int posY;
} Apple;

int initGame();
void exitGame(int exitCode);
int initWindow();
int initSnake();
int showSnake();
int moveSnake();
int genApple();
int hitCheck(BodyNode* headNode);
bool hitBorder(int x, int y);
bool hitSnake(int x, int y);
bool hitApple(int x, int y);
int eatApple();
void updateDelayTime();
void pauseGame(int);
int startInputThread();

#endif // __DEFINE_H__
