#include "font_ss.h"
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/* 将该子系统里所有模块都装入链表 */
LIST_HEAD(font_list);

int RegisterFontOpr(struct list_head *list)
{
	list_add(list, &font_list);
	return 0;
}

struct FontOpr * GetFontOpr(char *name)
{
	struct FontOpr *pModule;

	list_for_each_entry(pModule, &font_list, list)
	{
		if (!strcmp(name, pModule->name))
			return pModule;
	}

	printf("%s, %d, no sub module ERROR\n", __FUNCTION__, __LINE__);
	return NULL;
}

int GetFontBitmap(unsigned int dwCode, PT_FontBitMap ptFontBitMap)
{
	int iError = 0;
	struct FontOpr *pModule;

	list_for_each_entry(pModule, &font_list, list)
	{
		iError = pModule->GetFontBitmap(dwCode, ptFontBitMap);
		if (0 == iError)
			return 0;
	}

	return -1;
}

int SetFontsDetail(char *pcFontsName, char *pcFontsFile, unsigned int dwFontSize)
{
	int iError = 0;
	struct FontOpr * ptFontOpr;

	ptFontOpr = GetFontOpr(pcFontsName);
	if (NULL == ptFontOpr)
	{
		return -1;
	}

	iError = ptFontOpr->FontInit(pcFontsFile, dwFontSize);

	return iError;
}

int FontsInit(void)
{
	int iError;

	iError = FreeTypeInit();
	if (iError)
	{
		printf("FreeTypeInit error!\n");
		return -1;
	}

	return 0;
}

void ShowFontModules(void)
{
	struct FontOpr *pModule;

	list_for_each_entry(pModule, &font_list, list)
		printf("%s module registered\n", pModule->name);
}

void SetFontSize(unsigned int dwFontSize)
{
	struct FontOpr *pModule;

	list_for_each_entry(pModule, &font_list, list)
	{
		if (pModule->SetFontSize)
		{
			printf("set font %s size %d\n", pModule->name, dwFontSize);
			pModule->SetFontSize(dwFontSize);
		}
	}
}
