#ifndef _ZX_MENU_H_
#define _ZX_MENU_H_
#ifdef __cplusplus
extern "C"
{
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "zxlist_node.h"
	
#define FRAMES_MAX		500
#define ITEMS_MAX		30
#define NAME_LEN_MAX	100
#define FACTOR 2

#define STR_LINE_MAX 2000
/****************************************** menu related */
struct _ITEM_S
{
	char pItemName[NAME_LEN_MAX];
	int (*fnItemExec)(int i);
	struct _UI_FRAME_S *pstLowerLevelFrame;
};

struct _UI_FRAME_S
{
	int s32ItemCnt;
	struct _ITEM_S stItem[ITEMS_MAX];
	int (*fnEsc)(int i);
	struct _ITEM_S *pstHigherLevelItem;
	struct _UI_FRAME_S *pstHigherLevelFrame;
};


typedef struct _ITEM_S ITEM_S;
typedef struct _UI_FRAME_S UI_FRAME_S;


typedef struct
{	//no need to create memory pool, so use this to manager a malloced memory
	int s32TotalUIs;	// equals to FRAMES_MAX
	int s32UsedUIs;		// equals to line count of menu_config.ini, No ! items Do!
	int s32CurAvailiableIndex;	//cur index that can be "malloced"
	UI_FRAME_S *pUIs;	// memory of UI_FRAME_S
	UI_FRAME_S *pTopUIFrame;	//pointer to the top layer menu
} UI_FRAME_MGR_S;


/****************************************** stack related */
typedef struct
{
	unsigned int u32StackSize;
	struct zxlist_node listNode;
}FRAME_UI_STACK_MGR;

typedef struct
{
	UI_FRAME_S *pUIFrame;
	struct zxlist_node listNode;
}FRAME_UI_STACK_NODE;


void frameMgrCreate();
void frameMgrDestroy();
int parseMenuConfig();

void showTotal();
void menuFrameworkRun();


#ifdef __cplusplus
}
#endif
#endif //_ZX_MENU_H_