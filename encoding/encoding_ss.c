#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "encoding_ss.h"

LIST_HEAD(encoding_list);

int RegisterEncodingOpr(struct list_head *list)
{
	list_add(list, &encoding_list);
	return 0;
}

int AddFontOprForEncoding(PT_EncodingOpr ptEncodingOpr, struct FontOpr * ptFontOpr)
{
	list_add(&ptFontOpr->list, &ptEncodingOpr->ptFontOprSupportedHead);
	return 0;
}

int EncodingInit(void)
{
	int iError;

	iError = AsciiEncodingInit();
	if (iError)
	{
		printf("AsciiEncodingInit error!\n");
		return -1;
	}

	iError = Utf16leEncodingInit();
	if (iError)
	{
		printf("Utf16leEncodingInit error!\n");
		return -1;
	}

	iError = Utf16beEncodingInit();
	if (iError)
	{
		printf("Utf16beEncodingInit error!\n");
		return -1;
	}

	iError = Utf8EncodingInit();
	if (iError)
	{
		printf("Utf8EncodingInit error!\n");
		return -1;
	}

	return 0;
}

PT_EncodingOpr Encode(char *pcName)
{
	struct EncodingOpr *pModule;

	list_for_each_entry(pModule, &encoding_list, list)
	{
		if (strcmp(pModule->name, pcName) == 0)
			return pModule;
	}

	return NULL;
}

int GetCodeFrmBuf(unsigned char *pucBufStart, unsigned char *pucBufEnd, unsigned int *pdwCode)
{
    return Encode("utf-8")->GetCodeFrmBuf(pucBufStart, pucBufEnd, pdwCode);
}

void ShowEncodingModules(void)
{
	struct EncodingOpr *pModule;

	list_for_each_entry(pModule, &encoding_list, list)
		printf("%s module registered\n", pModule->name);
}
