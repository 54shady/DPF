#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "display_ss.h"

/* 将该子系统里所有模块都装入链表 */
LIST_HEAD(display_list);

/* VideoMem */
LIST_HEAD(videomem_list);

/* 开放给底层具体模块的注册接口 */
int display_register(struct list_head *list)
{
	list_add(list, &display_list);
	return 0;
}

/* 开放给应用层调用 */
int DisplayInit(void)
{
	int iError;
	printf("Display subsystem init\n");
	/* 调用个模块初始化函数 */
	iError = fb_init();

	return iError;
}

struct DispOpr *display_get_module(const char *name)
{
	struct DispOpr *pModule;

	list_for_each_entry(pModule, &display_list, list)
	{
		if (!strcmp(name, pModule->name))
			return pModule;
	}

	printf("no sub module ERROR\n");
	return NULL;
}

static struct DispOpr *get_default_module(void)
{
	struct DispOpr *pModule;

	list_for_each_entry(pModule, &display_list, list)
	{
		if (pModule->use_as_default)
			return pModule;
	}

	printf("no sub module ERROR\n");
	return NULL;

}

int GetDispResolution(int *piXres, int *piYres, int *piBpp)
{
	struct DispOpr *pModule;

	pModule = get_default_module();
	*piXres = pModule->iXres;
	*piYres = pModule->iYres;
	*piBpp = pModule->iBpp;
	return 0;
}

/* 调用各个子模块的初始化函数 */
void display_modules_init(void)
{
	struct DispOpr *pModule;

	list_for_each_entry(pModule, &display_list, list)
	{
		if (pModule->DeviceInit)
			pModule->DeviceInit(pModule);

		if (pModule->CleanScreen)
			pModule->CleanScreen(pModule, 0);
	}
}

void SelectAndInitDefaultDispDev(const char *name)
{
	struct DispOpr *pModule;

	list_for_each_entry(pModule, &display_list, list)
		if (!strcmp(name, pModule->name))
			pModule->use_as_default = 1;
}

void FlushPixelDatasToDev(PT_PixelDatas ptPixelDatas)
{
	struct DispOpr *pModule;

	pModule = get_default_module();

    pModule->ShowPage(pModule, ptPixelDatas);
}

int AllocVideoMem(int iNum)
{
	struct DispOpr *pModule;
	int i;

	int iXres = 0;
	int iYres = 0;
	int iBpp  = 0;

	int iVMSize;
	int iLineBytes;

	PT_VideoMem ptNew;

	/* 确定VideoMem的大小
	 */
	GetDispResolution(&iXres, &iYres, &iBpp);
	iVMSize = iXres * iYres * iBpp / 8;
	iLineBytes = iXres * iBpp / 8;

	/* 先把设备本身的framebuffer放入链表
	 * 分配一个T_VideoMem结构体, 注意我们没有分配里面的tPixelDatas.aucPixelDatas
	 * 而是让tPixelDatas.aucPixelDatas指向显示设备的framebuffer
	 */
	ptNew = malloc(sizeof(T_VideoMem));
	if (ptNew == NULL)
	{
		return -1;
	}

	/* 指向framebuffer */
	pModule = get_default_module();
	ptNew->tPixelDatas.aucPixelDatas = pModule->pucFbMem;

	ptNew->iID = 0;
	ptNew->bDevFrameBuffer = 1;        /* 表示这个VideoMem是设备本身的framebuffer, 而不是用作缓存作用的VideoMem */
	ptNew->eVideoMemState  = VMS_FREE;
	ptNew->ePicState	   = PS_BLANK;
	ptNew->tPixelDatas.iWidth  = iXres;
	ptNew->tPixelDatas.iHeight = iYres;
	ptNew->tPixelDatas.iBpp    = iBpp;
	ptNew->tPixelDatas.iLineBytes  = iLineBytes;
	ptNew->tPixelDatas.iTotalBytes = iVMSize;

	if (iNum != 0)
	{
		/* 如果下面要分配用于缓存的VideoMem,
		 * 把设备本身framebuffer对应的VideoMem状态设置为VMS_USED_FOR_CUR,
		 * 表示这个VideoMem不会被作为缓存分配出去
		 */
		ptNew->eVideoMemState = VMS_USED_FOR_CUR;
	}

	/* 放入链表 */
	list_add(&ptNew->list, &videomem_list);

	/*
	 * 分配用于缓存的VideoMem
	 */
	for (i = 0; i < iNum; i++)
	{
		/* 分配T_VideoMem结构体本身和"跟framebuffer同样大小的缓存" */
		ptNew = malloc(sizeof(T_VideoMem) + iVMSize);
		if (ptNew == NULL)
		{
			return -1;
		}
		/* 在T_VideoMem结构体里记录上面分配的"跟framebuffer同样大小的缓存" */
		ptNew->tPixelDatas.aucPixelDatas = (unsigned char *)(ptNew + 1);

		ptNew->iID = 0;
		ptNew->bDevFrameBuffer = 0;
		ptNew->eVideoMemState = VMS_FREE;
		ptNew->ePicState      = PS_BLANK;
		ptNew->tPixelDatas.iWidth  = iXres;
		ptNew->tPixelDatas.iHeight = iYres;
		ptNew->tPixelDatas.iBpp    = iBpp;
		ptNew->tPixelDatas.iLineBytes = iLineBytes;
		ptNew->tPixelDatas.iTotalBytes = iVMSize;

		/* 放入链表 */
		list_add(&ptNew->list, &videomem_list);
	}

	return 0;
}
