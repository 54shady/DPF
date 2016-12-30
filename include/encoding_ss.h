#ifndef _ENCODING_MANAGER_H
#define _ENCODING_MANAGER_H

#include "display_ss.h"
#include "klist.h"
#include "font_ss.h"

typedef struct EncodingOpr {
	char *name;    /* 编码模块的名字 */
	int iHeadLen;  /* 文件头的长度: 一般在文件的开始用几个字节来表示它的编码方式 */
	//PT_FontOpr ptFontOprSupportedHead;  /* 把能支持这种编码的"字体模块", 放在这个链表里 */
	struct list_head ptFontOprSupportedHead;
	int (*isSupport)(unsigned char *pucBufHead);  /* 用这个函数来判断是否支持某个文件 */
	int (*GetCodeFrmBuf)(unsigned char *pucBufStart, unsigned char *pucBufEnd, unsigned int *pdwCode);  /* 取出一个字符的编码值 */
	//struct EncodingOpr *ptNext;  /* 链表 */
	struct list_head list;
}T_EncodingOpr, *PT_EncodingOpr;


int RegisterEncodingOpr(struct list_head *list);


PT_EncodingOpr Encode(char *pcName);
void ShowEncodingOpr(void);
//PT_DispOpr GetDispOpr(char *pcName);
PT_EncodingOpr SelectEncodingOprForFile(unsigned char *pucFileBufHead);
int AddFontOprForEncoding(PT_EncodingOpr ptEncodingOpr, PT_FontOpr ptFontOpr);

int DelFontOprFrmEncoding(PT_EncodingOpr ptEncodingOpr, PT_FontOpr ptFontOpr);

int EncodingInit(void);

int AsciiEncodingInit(void);

int  Utf16beEncodingInit(void);

int  Utf16leEncodingInit(void);

int  Utf8EncodingInit(void);

int GetCodeFrmBuf(unsigned char *pucBufStart, unsigned char *pucBufEnd, unsigned int *pdwCode);

#endif /* _ENCODING_MANAGER_H */
