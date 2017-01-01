#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "picfmt_ss.h"

/* 将该子系统里所有模块都装入链表 */
LIST_HEAD(picfmt_list);

/* 开放给底层具体模块的注册接口 */
int RegisterPicFileParser(struct list_head *list)
{
	list_add(list, &picfmt_list);
	return 0;
}

/* 开放给应用层调用 */
int PicFmtsInit(void)
{
	int iError;

	iError = BMPParserInit();
	iError |= JPGParserInit();

	return iError;
}

PT_PicFileParser GetParser(PT_FileMap ptFileMap)
{
	struct PicFileParser *pModule;

	list_for_each_entry(pModule, &picfmt_list, list)
	{
		if (pModule->isSupport(ptFileMap))
			return pModule;
	}

	return NULL;
}

PT_PicFileParser Parser(char *name)
{
	struct PicFileParser *pModule;

	list_for_each_entry(pModule, &picfmt_list, list)
	{
		if (strcmp(name, pModule->name) == 0)
			return pModule;
	}

	return NULL;
}

void ShowPicFmts(void)
{
	struct PicFileParser *pModule;

	list_for_each_entry(pModule, &picfmt_list, list)
		printf("%s module registered\n", pModule->name);
}
