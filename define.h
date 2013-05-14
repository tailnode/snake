#ifndef __DEFINE_H__
#define __DEFINE_H__

#define BODY_LENGTH_MAX     (100)   // 蛇身体的最大长度
#define START_LENGTH        (5)     // 蛇的初始长度
#define BODY_CHAR           ('0')   // 蛇的身体显示的字符
#define MAIN_AREA_STARTX    (0)     // 游戏区域开始的x坐标
#define MAIN_AREA_STARTY    (0)     // 游戏区域开始的y坐标
#define MAIN_AREA_WIDTH     (25)    // 游戏区域的宽度
#define MAIN_AREA_HEIGHT    (25)    // 游戏区域的高度

#define HIT_NOHIT           (0)     // 没有碰撞
#define HIT_HIT             (1)     // 碰撞

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

#endif // __DEFINE_H__
