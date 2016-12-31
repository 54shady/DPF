#ifndef _FILE_H
#define _FILE_H

#include <stdio.h>

typedef struct FileMap {
	char strFileName[256];   /* �ļ��� */
	// int iFd;
	FILE * tFp;              /* �ļ���� */
	int iFileSize;           /* �ļ���С */
	unsigned char *pucFileMapMem;  /* ʹ��mmap����ӳ���ļ��õ����ڴ� */
}T_FileMap, *PT_FileMap;

/* �ļ���� */
typedef enum {
	FILETYPE_DIR = 0,  		/* Ŀ¼ */
	FILETYPE_FILE,     		/* �ļ� */
	FILETYPE_FILE_TXT,     	/* TXT�ļ� */
	FILETYPE_FILE_EXC,     	/* EXE�ļ� */
	FILETYPE_FILE_RC,     	/* RC�ļ� */
	FILETYPE_FILE_SH,     	/* SHELL�ļ� */
	FILETYPE_FILE_C,     	/* C�ļ� */
	FILETYPE_FILE_JPG,     	/* JPEG�ļ� */
}E_FileType;

/* Ŀ¼������� */
typedef struct DirContent {
	char strName[256];     /* ���� */
	E_FileType eFileType;  /* ��� */
}T_DirContent, *PT_DirContent;

int MapFile(PT_FileMap ptFileMap);
void UnMapFile(PT_FileMap ptFileMap);
int GetDirContents(char *strDirName, PT_DirContent **pptDirContents, int *piNumber);
void FreeDirContents(PT_DirContent *aptDirContents, int iNumber);
int GetFilesIndir(char *strDirName, int *piStartNumberToRecord, int *piCurFileNumber, int *piFileCountHaveGet, int iFileCountTotal, char apstrFileNames[][256]);

#endif /* _FILE_H */
