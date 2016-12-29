#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <string.h>
#include <fcntl.h>

#include <tslib.h>
#include "input_ss.h"
#include "display_ss.h"

static struct tsdev *g_tTSDev;
static int giXres;
static int giYres;

static int TouchScreenDevInit(void)
{
	char *pcTSName = NULL;
	int iBpp;

	if ((pcTSName = getenv("TSLIB_TSDEVICE")) != NULL )
	{
		g_tTSDev = ts_open(pcTSName, 0);  /* 以阻塞方式打开 */
	}
	else
	{
		g_tTSDev = ts_open("/dev/event0", 1);
	}

	if (!g_tTSDev) {
		printf("ts_open error!\n");
		return -1;
	}

	if (ts_config(g_tTSDev)) {
		printf("ts_config error!\n");
		return -1;
	}

	if (GetDispResolution(&giXres, &giYres, &iBpp))
	{
		return -1;
	}

	return 0;
}

static int TouchScreenDevExit(void)
{
	return 0;
}

static int TouchScreenGetInputEvent(PT_InputEvent ptInputEvent)
{
	struct ts_sample tSamp;
	int iRet;

	while (1)
	{
		iRet = ts_read(g_tTSDev, &tSamp, 1); /* 如果无数据则休眠 */
		if (iRet == 1)
		{
			ptInputEvent->tTime     = tSamp.tv;
			ptInputEvent->iType     = INPUT_TYPE_TOUCHSCREEN;
			ptInputEvent->iX        = tSamp.x;
			ptInputEvent->iY        = tSamp.y;
			ptInputEvent->iPressure = tSamp.pressure;
			return 0;
		}
		else
		{
			return -1;
		}
	}

	return 0;
}

static T_InputOpr input_module = {
	.name          = "touchscreen",
	.use_as_default = 0,
	.DeviceInit    = TouchScreenDevInit,
	.DeviceExit    = TouchScreenDevExit,
	.GetInputEvent = TouchScreenGetInputEvent,
};

int TouchScreenInit(void)
{
	printf("Display module fb init\n");
	/* 调用子系统提供的注册接口向子系统注册模块 */
	return input_register(&input_module.list);
}
