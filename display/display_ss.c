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

	printf("%s, %d, no sub module ERROR\n", __FUNCTION__, __LINE__);
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

	printf("%s, %d, no sub module ERROR\n", __FUNCTION__, __LINE__);
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

PT_VideoMem GetVideoMem(int iID, int bCur)
{
	struct VideoMem *pModule;

	/* 1. 优先: 取出空闲的、ID相同的videomem */
	list_for_each_entry(pModule, &videomem_list, list)
	{
		if ((pModule->eVideoMemState == VMS_FREE) && (pModule->iID == iID))
		{
			pModule->eVideoMemState = bCur ? VMS_USED_FOR_CUR : VMS_USED_FOR_PREPARE;
			return pModule;
		}
	}

	/* 2. 如果前面不成功, 取出一个空闲的并且里面没有数据(ptVideoMem->ePicState = PS_BLANK)的VideoMem */
	list_for_each_entry(pModule, &videomem_list, list)
	{
		if ((pModule->eVideoMemState == VMS_FREE) && (pModule->ePicState == PS_BLANK))
		{
			pModule->iID = iID;
			pModule->eVideoMemState = bCur ? VMS_USED_FOR_CUR : VMS_USED_FOR_PREPARE;
			return pModule;
		}
	}

	/* 3. 如果前面不成功: 取出任意一个空闲的VideoMem */
	list_for_each_entry(pModule, &videomem_list, list)
	{
		if (pModule->eVideoMemState == VMS_FREE)
		{
			pModule->iID = iID;
			pModule->ePicState = PS_BLANK;
			pModule->eVideoMemState = bCur ? VMS_USED_FOR_CUR : VMS_USED_FOR_PREPARE;
			return pModule;
		}
	}

    /* 4. 如果没有空闲的VideoMem并且bCur为1, 则取出任意一个VideoMem(不管它是否空闲) */
    if (bCur)
    {
		pModule = list_first_entry(&videomem_list, struct VideoMem, list);

		pModule->iID = iID;
		pModule->ePicState = PS_BLANK;
		pModule->eVideoMemState = bCur ? VMS_USED_FOR_CUR : VMS_USED_FOR_PREPARE;
		return pModule;
    }

	return NULL;
}

void PutVideoMem(PT_VideoMem ptVideoMem)
{
	ptVideoMem->eVideoMemState = VMS_FREE;  /* 设置VideoMem状态为空闲 */
    if (ptVideoMem->iID == -1)
    {
        ptVideoMem->ePicState = PS_BLANK;  /* 表示里面的数据没有用了 */
    }
}

PT_VideoMem GetDevVideoMem(void)
{
	struct VideoMem *pModule;

	list_for_each_entry(pModule, &videomem_list, list)
	{
		if (pModule->bDevFrameBuffer)
			return pModule;
	}
	return NULL;
}

void ClearVideoMem(PT_VideoMem ptVideoMem, unsigned int dwColor)
{
	unsigned char *pucVM;
	unsigned short *pwVM16bpp;
	unsigned int *pdwVM32bpp;
	unsigned short wColor16bpp; /* 565 */
	int iRed;
	int iGreen;
	int iBlue;
	int i = 0;

	pucVM	   = ptVideoMem->tPixelDatas.aucPixelDatas;
	pwVM16bpp  = (unsigned short *)pucVM;
	pdwVM32bpp = (unsigned int *)pucVM;

	switch (ptVideoMem->tPixelDatas.iBpp)
	{
		case 8:
		{
			memset(pucVM, dwColor, ptVideoMem->tPixelDatas.iTotalBytes);
			break;
		}
		case 16:
		{
			/* 先根据32位的dwColor构造出16位的wColor16bpp */
			iRed   = (dwColor >> (16+3)) & 0x1f;
			iGreen = (dwColor >> (8+2)) & 0x3f;
			iBlue  = (dwColor >> 3) & 0x1f;
			wColor16bpp = (iRed << 11) | (iGreen << 5) | iBlue;
			while (i < ptVideoMem->tPixelDatas.iTotalBytes)
			{
				*pwVM16bpp	= wColor16bpp;
				pwVM16bpp++;
				i += 2;
			}
			break;
		}
		case 32:
		{
			while (i < ptVideoMem->tPixelDatas.iTotalBytes)
			{
				*pdwVM32bpp = dwColor;
				pdwVM32bpp++;
				i += 4;
			}
			break;
		}
		default :
		{
			printf("can't support %d bpp\n", ptVideoMem->tPixelDatas.iBpp);
			return;
		}
	}

}

PT_DispOpr GetDefaultDispDev(void)
{
	struct DispOpr *pModule;

	list_for_each_entry(pModule, &display_list, list)
	{
		if (pModule->use_as_default)
			return pModule;
	}

	return NULL;
}

void ShowDispModules(void)
{
	struct DispOpr *pModule;

	list_for_each_entry(pModule, &display_list, list)
		printf("%s module registered\n", pModule->name);
}
