#ifndef _PAGE_MANAGER_H
#define _PAGE_MANAGER_H

#include "input_ss.h"
#include "display_ss.h"
#include "klist.h"

typedef struct PageParams {
    int iPageID;                  /* ҳ���ID */
    char strCurPictureFile[256];  /* Ҫ����ĵ�1��ͼƬ�ļ� */
}T_PageParams, *PT_PageParams;

typedef struct PageLayout {
	int iTopLeftX;        /* �����������Ͻǡ����½����� */
	int iTopLeftY;
	int iBotRightX;
	int iBotRightY;
	int iBpp;             /* һ�������ö���λ����ʾ */
	int iMaxTotalBytes;
	PT_Layout atLayout;  /* ����: �������ֳɺü���С���� */
}T_PageLayout, *PT_PageLayout;

typedef struct PageAction {
	char *name;            /* ҳ������ */
	void (*Run)(PT_PageParams ptParentPageParams);  /* ҳ������к��� */
	int (*GetInputEvent)(PT_PageLayout ptPageLayout, PT_InputEvent ptInputEvent);  /* ����������ݵĺ��� */
	int (*Prepare)(void);         /* (δʵ��)��̨׼������: Ϊ�ӿ�������ж�ͬʱ����ĳЩ���� */
	//struct PageAction *ptNext;    /* ���� */
	struct list_head list;
}T_PageAction, *PT_PageAction;

/* ҳ��������Ϣ */
typedef struct PageCfg {
    int iIntervalSecond;      /* ����ģʽ��ͼƬ����ʾ��� */
    char strSeletedDir[256];  /* ����ģʽ��Ҫ��ʾ�ĸ�Ŀ¼�µ�ͼƬ */ 
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
