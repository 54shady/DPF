#ifndef _FONTS_MANAGER_H
#define _FONTS_MANAGER_H

#include "klist.h"

typedef struct FontBitMap {
	int iXLeft;  /* 位图左上角X座标 */
	int iYTop;   /* 位图左上角Y座标 */
	int iXMax;   /* 位图的最大X座标值 */
	int iYMax;   /* 位图的最大Y座标值 */
	int iBpp;    /* 位图中一个象素用多少位来表示 */
	int iPitch;  /* 对于单色位图, 两行象素之间的跨度, 即第N行、第N+1行象素数据之间的偏移值 */
	int iCurOriginX;  /* 位图的原点X座标(一般是左下角), 使用原点来确定相邻字符的位置 */
	int iCurOriginY;  /* 位图的原点Y座标 */
	int iNextOriginX; /* 下一个字符即右边字符的原点X座标 */
	int iNextOriginY; /* 下一个字符即右边字符的原点Y座标 */
	unsigned char *pucBuffer;  /* 存有字符的位图数据 */
}T_FontBitMap, *PT_FontBitMap;

typedef struct FontOpr {
	char *name;             /* 字体模块的名字 */
	int (*FontInit)(char *pcFontFile, unsigned int dwFontSize);  /* 字体模块的初始化函数 */
	int (*GetFontBitmap)(unsigned int dwCode, PT_FontBitMap ptFontBitMap);  /* 根据编码值获得字符的位图 */
	void (*SetFontSize)(unsigned int dwFontSize);   /* 设置字体尺寸(单位:象素) */
	//struct FontOpr *ptNext;  /* 链表 */
	struct list_head list;
}T_FontOpr, *PT_FontOpr;


int RegisterFontOpr(struct list_head *list);

void ShowFontOpr(void);

int FontsInit(void);

int ASCIIInit(void);

int GBKInit(void);

int FreeTypeInit(void);

PT_FontOpr GetFontOpr(char *pcName);

void SetFontSize(unsigned int dwFontSize);

unsigned int GetFontSize(void);

int GetFontBitmap(unsigned int dwCode, PT_FontBitMap ptFontBitMap);

int SetFontsDetail(char *pcFontsName, char *pcFontsFile, unsigned int dwFontSize);

#endif /* _FONTS_MANAGER_H */
