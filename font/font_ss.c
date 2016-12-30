#include "font_ss.h"
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

/* 将该子系统里所有模块都装入链表 */
LIST_HEAD(font_list);
//static PT_FontOpr g_ptFontOprHead = NULL;
static int g_dwFontSize;

int RegisterFontOpr(struct list_head *list)
{
	list_add(list, &font_list);
	return 0;
}

void ShowFontOpr(void)
{
	struct FontOpr *pModule;

	list_for_each_entry(pModule, &font_list, list)
	{
		printf("%s\n", pModule->name);
	}
}

PT_FontOpr GetFontOpr(char *name)
{
	struct FontOpr *pModule;

	list_for_each_entry(pModule, &font_list, list)
	{
		if (!strcmp(name, pModule->name))
			return pModule;
	}

	printf("no sub module ERROR\n");
	return NULL;
}


void SetFontSize(unsigned int dwFontSize)
{
	struct FontOpr *pModule;

	g_dwFontSize = dwFontSize;
	list_for_each_entry(pModule, &font_list, list)
	{
		if (pModule->SetFontSize)
			pModule->SetFontSize(dwFontSize);
	}
}

unsigned int GetFontSize(void)
{
	return g_dwFontSize;
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
	PT_FontOpr ptFontOpr;

	ptFontOpr = GetFontOpr(pcFontsName);
	if (NULL == ptFontOpr)
	{
		return -1;
	}

	g_dwFontSize = dwFontSize;

	iError = ptFontOpr->FontInit(pcFontsFile, dwFontSize);

	return iError;
}


int FontsInit(void)
{
#if 0
	int iError;

	iError = ASCIIInit();
	if (iError)
	{
		printf("ASCIIInit error!\n");
		return -1;
	}

	iError = GBKInit();
	if (iError)
	{
		printf("GBKInit error!\n");
		return -1;
	}
#endif

#if 0
	iError = FreeTypeInit();
	if (iError)
	{
		printf("FreeTypeInit error!\n");
		return -1;
	}
#endif

	return 0;
}

