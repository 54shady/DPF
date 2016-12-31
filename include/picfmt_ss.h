#ifndef _PIC_MANAGER_H
#define _PIC_MANAGER_H

#include "pic_operation.h"
#include "page_ss.h"
#include "file_ss.h"

int RegisterPicFileParser(struct list_head *list);

void ShowPicFmts(void);

int PicFmtsInit(void);

int JPGParserInit(void);

int BMPParserInit(void);

PT_PicFileParser Parser(char *pcName);
PT_PicFileParser GetParser(PT_FileMap ptFileMap);

#endif /* _PIC_MANAGER_H */
