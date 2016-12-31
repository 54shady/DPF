#ifndef _INPUT_H_
#define _INPUT_H_

#include <sys/time.h>
#include <pthread.h>
#include "klist.h"

/* 输入事件类别 */
#define INPUT_TYPE_STDIN        0
#define INPUT_TYPE_TOUCHSCREEN  1

typedef struct InputEvent {
	struct timeval tTime;   /* 发生这个输入事件时的时间 */
	int iType;  /* 类别: stdin, touchsceen */
	int iX;     /* X/Y座标 */
	int iY;
	int iKey;   /* 按键值 */
	int iPressure; /* 压力值 */
}T_InputEvent, *PT_InputEvent;


typedef struct InputOpr {
	char *name;          /* 输入模块的名字 */
	int use_as_default;
	pthread_t tTreadID;  /* 子线程ID */
	int (*DeviceInit)(void);  /* 设备初始化函数 */
	int (*DeviceExit)(void);  /* 设备退出函数 */
	int (*GetInputEvent)(PT_InputEvent ptInputEvent);  /* 获得输入数据 */

	struct list_head list;
}T_InputOpr, *PT_InputOpr;

int InputInit(void);
int input_register(struct list_head *list);
int AllInputDevicesInit(void);
int StdinInit(void);
int TouchScreenInit(void);
int GetInputEvent(PT_InputEvent ptInputEvent);
#endif
