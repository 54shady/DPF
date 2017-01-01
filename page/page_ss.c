#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "page_ss.h"
#include "render.h"
#include "input_ss.h"

#define COLOR_BACKGROUND   0xE7DBB5  /* 刑仔議崕 */

/* 将该子系统里所有模块都装入链表 */
LIST_HEAD(page_list);

/* 开放给底层具体模块的注册接口 */
int RegisterPageAction(struct list_head *list)
{
	list_add(list, &page_list);
	return 0;
}

/* 开放给应用层调用 */
int PagesInit(void)
{
	int iError;

	iError  = MainPageInit();
	iError |= SettingPageInit();
	//iError |= IntervalPageInit();
	//iError |= BrowsePageInit();
    //iError |= AutoPageInit();
    //iError |= ManualPageInit();
	return iError;
}

PT_PageAction Page(char *name)
{
	struct PageAction *pModule;

	list_for_each_entry(pModule, &page_list, list)
	{
		if (!strcmp(name, pModule->name))
			return pModule;
	}

	printf("%s, %d, no sub module ERROR\n", __FUNCTION__, __LINE__);
	return NULL;
}

int ID(char *strName)
{
	return (int)(strName[0]) + (int)(strName[1]) + (int)(strName[2]) + (int)(strName[3]);
}

int GeneratePage(PT_PageLayout ptPageLayout, PT_VideoMem ptVideoMem)
{
	T_PixelDatas tOriginIconPixelDatas;
	T_PixelDatas tIconPixelDatas;
	int iError;
	PT_Layout atLayout = ptPageLayout->atLayout;

	/* 描画数据: VideoMem中的页面数据未生成的情况下才执行下面操作 */
	if (ptVideoMem->ePicState != PS_GENERATED)
	{
		ClearVideoMem(ptVideoMem, COLOR_BACKGROUND);

		tIconPixelDatas.iBpp          = ptPageLayout->iBpp;
        /* 分配一个临时的内存, 它用来存放缩放后的图标数据 */
		tIconPixelDatas.aucPixelDatas = malloc(ptPageLayout->iMaxTotalBytes);
		if (tIconPixelDatas.aucPixelDatas == NULL)
		{
			return -1;
		}

		while (atLayout->strIconName)
		{
            /* 取得图标文件的象素数据 */
			iError = GetPixelDatasForIcon(atLayout->strIconName, &tOriginIconPixelDatas);
			if (iError)
			{
				printf("GetPixelDatasForIcon %s error!\n", atLayout->strIconName);
                free(tIconPixelDatas.aucPixelDatas);
				return -1;
			}
 			tIconPixelDatas.iHeight = atLayout->iBotRightY - atLayout->iTopLeftY + 1;
			tIconPixelDatas.iWidth  = atLayout->iBotRightX - atLayout->iTopLeftX+ 1;
			tIconPixelDatas.iLineBytes  = tIconPixelDatas.iWidth * tIconPixelDatas.iBpp / 8;
			tIconPixelDatas.iTotalBytes = tIconPixelDatas.iLineBytes * tIconPixelDatas.iHeight;

            /* 把原始的图标象素数据缩放为指定大小 */
 			PicZoom(&tOriginIconPixelDatas, &tIconPixelDatas);

            /* 把缩放后的图标数据,合并到VideoMem的指定区域 */
 			PicMerge(atLayout->iTopLeftX, atLayout->iTopLeftY, &tIconPixelDatas, &ptVideoMem->tPixelDatas);

            /* 释放原始的图标象素数据 */
 			FreePixelDatasForIcon(&tOriginIconPixelDatas);
 			atLayout++;
		}

        /* 数据上面分配的临时内存 */
		free(tIconPixelDatas.aucPixelDatas);
		ptVideoMem->ePicState = PS_GENERATED;
	}

	return 0;
}

int GenericGetInputEvent(PT_PageLayout ptPageLayout, PT_InputEvent ptInputEvent)
{
	T_InputEvent tInputEvent;
	int iRet;
	int i = 0;
	PT_Layout atLayout = ptPageLayout->atLayout;

	/* 获得原始的触摸屏数据
	 * 它是调用input_manager.c的函数，此函数会让当前线否休眠
	 * 当触摸屏线程获得数据后，会把它唤醒
	 */
	iRet = GetInputEvent(&tInputEvent);
	if (iRet)
	{
		return -1;
	}

	if (tInputEvent.iType != INPUT_TYPE_TOUCHSCREEN)
	{
		return -1;
	}

	*ptInputEvent = tInputEvent;

	/* 处理数据 */
	/* 确定触点位于哪一个按钮上 */
	while (atLayout[i].strIconName)
	{
		if ((tInputEvent.iX >= atLayout[i].iTopLeftX) && (tInputEvent.iX <= atLayout[i].iBotRightX) && \
			 (tInputEvent.iY >= atLayout[i].iTopLeftY) && (tInputEvent.iY <= atLayout[i].iBotRightY))
		{
			/* 找到了被点中的按钮 */
			return i;
		}
		else
		{
			i++;
		}
	}

	/* 触点没有落在按钮上 */
	return -1;
}
