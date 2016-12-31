#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "input_ss.h"

static T_InputEvent g_tInputEvent;

/* 将该子系统里所有模块都装入链表 */
LIST_HEAD(input_list);

static pthread_mutex_t g_tMutex  = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t  g_tConVar = PTHREAD_COND_INITIALIZER;

/* 开放给底层具体模块的注册接口 */
int input_register(struct list_head *list)
{
	list_add(list, &input_list);
	return 0;
}

/* 开放给应用层调用 */
int InputInit(void)
{
	int iError;
	printf("Display subsystem init\n");
	/* 调用个模块初始化函数 */
	iError = TouchScreenInit();

	return iError;
}

struct InputOpr *input_get_module(const char *name)
{
	struct InputOpr *pModule;

	list_for_each_entry(pModule, &input_list, list)
	{
		if (!strcmp(name, pModule->name))
			return pModule;
	}

	printf("no sub module ERROR\n");
	return NULL;
}

/* 调用各个子模块的初始化函数 */
void input_modules_init(void)
{
	struct InputOpr *pModule;

	list_for_each_entry(pModule, &input_list, list)
	{
		if (pModule->DeviceInit)
			pModule->DeviceInit();
	}
}

static void *InputEventThreadFunction(void *pVoid)
{
	T_InputEvent tInputEvent;

	/* 定义函数指针 */
	int (*GetInputEvent)(PT_InputEvent ptInputEvent);
	GetInputEvent = (int (*)(PT_InputEvent))pVoid;

	while (1)
	{
		if(0 == GetInputEvent(&tInputEvent))
		{
			/* 唤醒主线程, 把tInputEvent的值赋给一个全局变量 */
			/* 访问临界资源前，先获得互斥量 */
			pthread_mutex_lock(&g_tMutex);
			g_tInputEvent = tInputEvent;

			/*  唤醒主线程 */
			pthread_cond_signal(&g_tConVar);

			/* 释放互斥量 */
			pthread_mutex_unlock(&g_tMutex);
		}
	}

	return NULL;
}

int AllInputDevicesInit(void)
{
	struct InputOpr *pModule;

	int iError = -1;

	list_for_each_entry(pModule, &input_list, list)
	{
		if (0 == pModule->DeviceInit())
		{
			/* 创建子线程 */
			pthread_create(&pModule->tTreadID, NULL, InputEventThreadFunction, pModule->GetInputEvent);
			iError = 0;
		}
	}
	return iError;
}

int GetInputEvent(PT_InputEvent ptInputEvent)
{
	/* 休眠 */
	pthread_mutex_lock(&g_tMutex);
	pthread_cond_wait(&g_tConVar, &g_tMutex);

	/* 被唤醒后,返回数据 */
	*ptInputEvent = g_tInputEvent;
	pthread_mutex_unlock(&g_tMutex);
	return 0;
}
