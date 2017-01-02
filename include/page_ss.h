#ifndef _PAGE_MANAGER_H
#define _PAGE_MANAGER_H

#include "input_ss.h"
#include "display_ss.h"
#include "klist.h"

typedef struct PageParams {
    int iPageID;                  /* 页面的ID */
    char strCurPictureFile[256];  /* 要处理的第1个图片文件 */
}T_PageParams, *PT_PageParams;

typedef struct PageLayout {
	int iTopLeftX;        /* 这个区域的左上角、右下角坐标 */
	int iTopLeftY;
	int iBotRightX;
	int iBotRightY;
	int iBpp;             /* 一个象素用多少位来表示 */
	int iMaxTotalBytes;
	PT_Layout atLayout;  /* 数组: 这个区域分成好几个小区域 */
}T_PageLayout, *PT_PageLayout;

typedef struct PageAction {
	char *name;            /* 页面名字 */
	void (*Run)(PT_PageParams ptParentPageParams);  /* 页面的运行函数 */
	int (*GetInputEvent)(PT_PageLayout ptPageLayout, PT_InputEvent ptInputEvent);  /* 获得输入数据的函数 */
	int (*Prepare)(void);         /* (未实现)后台准备函数: 为加快程序运行而同时处理某些事情 */
	//struct PageAction *ptNext;    /* 链表 */
	struct list_head list;
}T_PageAction, *PT_PageAction;

/* 页面配置信息 */
typedef struct PageCfg {
    int iIntervalSecond;      /* 连播模式下图片的显示间隔 */
    char strSeletedDir[256];  /* 连播模式下要显示哪个目录下的图片 */ 
}T_PageCfg, *PT_PageCfg;



//#define ID(name)   (int(name[0]) + int(name[1]) + int(name[2]) + int(name[3]))
int ID(char *strName);
int MainPageInit(void);
int SettingPageInit(void);
int IntervalPageInit(void);
int BrowsePageInit(void);
int AutoPageInit(void);
int ManualPageInit(void);
int RegisterPageAction(struct list_head *list);
int PagesInit(void);
int GeneratePage(PT_PageLayout ptPageLayout, PT_VideoMem ptVideoMem);
int GenericGetInputEvent(PT_PageLayout ptPageLayout, PT_InputEvent ptInputEvent);
int TimeMSBetween(struct timeval tTimeStart, struct timeval tTimeEnd);
void GetPageCfg(PT_PageCfg ptPageCfg);
void ShowPageModules(void);
void RunPage(const char *name, PT_PageParams ptParentPageParams);

#endif /* _PAGE_MANAGER_H */
