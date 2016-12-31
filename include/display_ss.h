#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <linux/fb.h>
#include "klist.h"
#include "pic_operation.h"

/* 显示区域,含该区域的左上角/右下角座标
 * 如果是图标,还含有图标的文件名
 */
typedef struct Layout {
	int iTopLeftX;
	int iTopLeftY;
	int iBotRightX;
	int iBotRightY;
	char *strIconName;
}T_Layout, *PT_Layout;

/* VideoMem中内存里图片的状态:
 * 空白/正在生成/已经生成
 */
typedef enum {
	PS_BLANK = 0,
	PS_GENERATING,
	PS_GENERATED,
}E_PicState;

/* VideoMem的状态:
 * 空闲/用于预先准备显示内容/用于当前线程
 */
typedef enum {
	VMS_FREE = 0,
	VMS_USED_FOR_PREPARE,
	VMS_USED_FOR_CUR,
}E_VideoMemState;

typedef struct VideoMem {
	int iID;                        /* ID值,用于标识不同的页面 */
	int bDevFrameBuffer;            /* 1: 这个VideoMem是显示设备的显存; 0: 只是一个普通缓存 */
	E_VideoMemState eVideoMemState; /* 这个VideoMem的状态 */
	E_PicState ePicState;           /* VideoMem中内存里图片的状态 */
	T_PixelDatas tPixelDatas;       /* 内存: 用来存储图像 */
	struct list_head list;
}T_VideoMem, *PT_VideoMem;

struct DispOpr {
	/* variables */
	char *name;
	int iXres;
	int iYres;
	int iBpp;
	int iLineWidth;
	int use_as_default;

	/* 显示屏大小*/
	unsigned int dwScreenSize;

	/* 显存地址 */
	unsigned char *pucFbMem;

	/* functions */
	int (*DeviceInit)(struct DispOpr *pModule);
	int (*CleanScreen)(struct DispOpr *pModule, unsigned int dwBackColor);
	int (*ShowPixel)(struct DispOpr *pModule, int iX, int iY, unsigned int dwColor);

	int (*ShowPage)(struct DispOpr *pModule, PT_PixelDatas ptPixelDatas);
	struct fb_var_screeninfo fb_var;
	struct fb_fix_screeninfo fb_fix;

	struct list_head list;
};

int display_register(struct list_head *list);
int fb_init(void);

/* call for app */
int DisplayInit(void);
int GetDispResolution(int *piXres, int *piYres, int *piBpp);
void display_modules_init(void);
void SelectAndInitDefaultDispDev(const char *name);
void FlushPixelDatasToDev(PT_PixelDatas ptPixelDatas);
int AllocVideoMem(int iNum);

#endif
