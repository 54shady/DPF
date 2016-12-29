#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "display_ss.h"
#include "input_ss.h"

int main(int argc, char *argv[])
{
	/* 注册显示设备 */
	DisplayInit();

	/* 可能支持多个显示设备: 选择和初始化指定的显示设备 */
	SelectAndInitDefaultDispDev("fb");
	display_modules_init();

	/*
	 * VideoMem: 为加快显示速度,我们事先在内存中构造好显示的页面的数据,
	             (这个内存称为VideoMem)
	 *           显示时再把VideoMem中的数据复制到设备的显存上
	 * 参数的含义就是分配的多少个VideoMem
	 * 参数可取为0, 这意味着所有的显示数据都是在显示时再现场生成,然后写入显存
	 */
	AllocVideoMem(5);

    /* 注册输入设备 */
	InputInit();

    /* 调用所有输入设备的初始化函数 */
	AllInputDevicesInit();

	return 0;
}