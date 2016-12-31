#ifndef _PIC_OPERATION_H
#define _PIC_OPERATION_H

#include "klist.h"
#include "file_ss.h"

/* 图片的像素数据 */
typedef struct PixelDatas {
	int iWidth;
	int iHeight;
	int iBpp;
	int iLineBytes;
	int iTotalBytes;
	unsigned char *aucPixelDatas;
}T_PixelDatas, *PT_PixelDatas;

typedef struct PicFileParser {
	char *name;                     /* 图片文件解析模块的名字 */
	int (*isSupport)(PT_FileMap ptFileMap);  /* 是否支持某文件 */
	int (*GetPixelDatas)(PT_FileMap ptFileMap, PT_PixelDatas ptPixelDatas);  /* 从文件中解析出图像的象素数据 */
	int (*FreePixelDatas)(PT_PixelDatas ptPixelDatas);  /* 释放图像的象素数据所占内存 */
//    struct PicFileParser *ptNext;  /* 链表 */
	struct list_head list;
}T_PicFileParser, *PT_PicFileParser;
#endif
