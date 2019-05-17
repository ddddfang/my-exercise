#include "menu.h"
#include "zxlist_node.h"
#include "myFunc.h"


int fnDefault(int i)
{
	printf("%d not implement\n",i);
	return 0;
}

static void showFrame(UI_FRAME_S * pUIFrame);	//
static void getInputAndProcess(UI_FRAME_S * pUIFrame);
int fnEnterOrEsc(int i)	//param is the handle to the next frame
{
	UI_FRAME_S *pUIFrame = (UI_FRAME_S *)i;
	if(i != 0)
	{
		showFrame(pUIFrame);
		getInputAndProcess(pUIFrame);
	}
	return 0;
}


/**/
typedef int (*fnItemExec)(int i);
static fnItemExec g_fnItemExec[]={
	f_caA1_test,
	f_caA2_test,
	f_caB_test,
	f_caC_test,
	f_tunerA_test,
	f_tunerB01_test,
	f_tunerB02_test,
	f_tunerB03_test,
	f_hehe_test,
	fnDefault,
	fnDefault,
	fnDefault,
	fnDefault,
	fnDefault,
	fnDefault,
	fnDefault,
	fnDefault,
	fnDefault,
	fnDefault,
	fnDefault,
	fnDefault,
};





/* global menu context manager */
static UI_FRAME_MGR_S g_stFrameMgr={0};

/************************************************* utils for g_stFrameMgr memory alloc */
static void bufInit(int s32FrameTotal)
{
	if(g_stFrameMgr.pUIs)
	{
		printf("bufInit has been called and not call bufDeinit \n");
		return ;
	}
	g_stFrameMgr.pUIs = (UI_FRAME_S *)malloc(s32FrameTotal * sizeof(UI_FRAME_S));
	if(g_stFrameMgr.pUIs == NULL)
	{
		printf("no avail memory\n");
		g_stFrameMgr.s32CurAvailiableIndex = -1;
		g_stFrameMgr.s32TotalUIs = 0;
		return ;
	}
	memset(g_stFrameMgr.pUIs,0,g_stFrameMgr.s32TotalUIs * sizeof(UI_FRAME_S));	//
	g_stFrameMgr.s32CurAvailiableIndex = 0;
	g_stFrameMgr.s32TotalUIs = s32FrameTotal;
	printf("bufInit ok\n");
}

static void bufDeinit()
{
	if(g_stFrameMgr.pUIs == NULL)
	{
		printf("bufDeinit has been called \n");
		return ;
	}
	free(g_stFrameMgr.pUIs);
	g_stFrameMgr.pUIs = NULL;
	g_stFrameMgr.s32TotalUIs = 0;
	g_stFrameMgr.s32CurAvailiableIndex = -1;
	printf("bufDeinit ok\n");
}

static int bufGet(UI_FRAME_S **ppUIFrame)	//zxhandle *p
{
	if(ppUIFrame == NULL)
	{
		printf("bufMalloc error: invalid param\n");
		return -1;
	}
	if(g_stFrameMgr.pUIs == NULL || g_stFrameMgr.s32CurAvailiableIndex == -1)
	{
		printf("bufMalloc error: you have not call bufInit!\n");
		return -1;
	}
	if(g_stFrameMgr.s32CurAvailiableIndex >= g_stFrameMgr.s32TotalUIs)	//or use remalloc to increase dynamic
	{
		printf("bufMalloc error: no more availiable memory, should adjust FRAMES_MAX!\n");
		return -1;
	}
	*ppUIFrame = &(g_stFrameMgr.pUIs[g_stFrameMgr.s32CurAvailiableIndex++]);
	g_stFrameMgr.s32UsedUIs++;
	return 0;
}



/************************************************* in order to implement a stack, for parser */

static FRAME_UI_STACK_MGR gstFrameUIStack = {0};

static int frameUIStackInit()
{
	gstFrameUIStack.u32StackSize = 0;
	ZX_ListInit(&gstFrameUIStack.listNode);
	return 0;
}

static int frameUIStackDeinit()	//descroy the stack
{
	zxlist_node *pListNode = NULL;
	FRAME_UI_STACK_NODE *pStackNode = NULL;
	while(!ZX_ListEmpty(&gstFrameUIStack.listNode))
	{
		pListNode = ZX_ListTail(&gstFrameUIStack.listNode);
		pStackNode = ZX_ListNodeToItem(pListNode, FRAME_UI_STACK_NODE, listNode);
		ZX_ListRemove(pListNode);
		free(pStackNode);
	}
	gstFrameUIStack.u32StackSize = 0;
	return 0;
}

static int frameUIStackPush(UI_FRAME_S *pUIFrame)
{
	if(pUIFrame == NULL)
	{
		printf("frameUIStackPush error: invalid param\n");
		return -1;
	}
	FRAME_UI_STACK_NODE *pUINode = (FRAME_UI_STACK_NODE *)malloc(sizeof(FRAME_UI_STACK_NODE));
	if(pUINode == NULL)
	{
		printf("frameUIStackPush add node error: no more availiable mem\n");
		return -1;
	}
	pUINode->pUIFrame = pUIFrame;
	ZX_ListAddTail(&gstFrameUIStack.listNode,&(pUINode->listNode));
	gstFrameUIStack.u32StackSize++;
	return 0;
}

static int frameUIStackTop(UI_FRAME_S **ppUIFrame)
{
	if(ppUIFrame == NULL)
	{
		printf("frameUIStackTop error: invalid param\n");
		return -1;
	}
	if(gstFrameUIStack.u32StackSize == 0)
	{
		if(ZX_ListEmpty(&gstFrameUIStack.listNode))	//confirm
		{
			printf("frameUIStackTop stack is empty!\n");
			*ppUIFrame = NULL;
			return 0;
		}
		else
		{
			printf("frameUIStackTop error!\n");
			*ppUIFrame = NULL;
			return -1;
		}
	}
	zxlist_node *pListNode = ZX_ListTail(&gstFrameUIStack.listNode);
	FRAME_UI_STACK_NODE *pStackNode = ZX_ListNodeToItem(pListNode, FRAME_UI_STACK_NODE, listNode);
	*ppUIFrame = pStackNode->pUIFrame;
	return 0;
}

static int frameUIStackPop(UI_FRAME_S **ppUIFrame)
{
	if(gstFrameUIStack.u32StackSize == 0)
	{
		if(ZX_ListEmpty(&gstFrameUIStack.listNode))
		{
			printf("frameUIStackPop stack is empty!\n");
			*ppUIFrame = NULL;
			return 0;
		}
		else
		{
			printf("frameUIStackPop error!\n");
			*ppUIFrame = NULL;
			return -1;
		}
	}
	zxlist_node *pListNode = ZX_ListTail(&gstFrameUIStack.listNode);
	FRAME_UI_STACK_NODE *pStackNode = ZX_ListNodeToItem(pListNode, FRAME_UI_STACK_NODE, listNode);
	if(ppUIFrame != NULL)	//if param is null, just discard top element
	{
		*ppUIFrame = pStackNode->pUIFrame;
	}
	ZX_ListRemove(pListNode);
	free(pStackNode);
	gstFrameUIStack.u32StackSize--;
	return 0;
}

/*  */
static int parseLine(char *strLine,UI_FRAME_S *pUIFrame)	//
{
	char *p = strLine;
	int s32LevelCnt = 0;			//count of "-"
	int s32FuncIndex = -1;			//
	
	char pItemName[NAME_LEN_MAX]={0};	//item name;
	int s32ItemNameIndex = 0;

	char pFuncIndexTmp[10]={0};	//tmp for store FuncIndex(string)
	int s32FuncIndexTmpIndex = 0;
	
	bool bFlag=false;					//flag of found "
	bool bFuncFlag=false;				//flag of found <

	if(p == NULL)
	{
		return -1;
	}
	while(*p)	//we care abount -, \", and <>
	{
		if(!bFlag && !bFuncFlag)	//has not found "(or right " has found) and has not found <
		{
			switch(*p)
			{
				case '-':
					s32LevelCnt++;	//
					break;
				case '\"':
					bFlag = true;
					break;
				case '<':
					bFuncFlag = true;
					break;
				default:		//just eat it, do nothing,include blank ' '
					break;
			}

		}
		else	//bFlag is true. which means has found left half "
		{
			switch(*p)
			{
				case '\"':
					pItemName[s32ItemNameIndex] = '\0';
					//memcpy
					bFlag = false;
					break;
				case '>':
					pFuncIndexTmp[s32FuncIndexTmpIndex] = '\0';
					s32FuncIndex = atoi(pFuncIndexTmp);
					bFuncFlag = false;
					break;
				default:
					if(bFlag)
						pItemName[s32ItemNameIndex++] = *p;
					else if(bFuncFlag)
						pFuncIndexTmp[s32FuncIndexTmpIndex++] = *p;
					else
					{
						//printf
					}
					break;
			}
		}
		printf("%c",*p);
		p++;
	}
	printf("s32LevelCnt=%d,pItemName is %s, s32FuncIndex is %d\n\n",s32LevelCnt,pItemName,s32FuncIndex);
	
	memcpy(pUIFrame->stItem[0].pItemName,pItemName,strlen(pItemName)+1);
	if(s32FuncIndex == -1)
	{
		pUIFrame->stItem[0].fnItemExec = fnEnterOrEsc;
	}
	else
	{
		pUIFrame->stItem[0].fnItemExec = g_fnItemExec[s32FuncIndex];
	}
	return s32LevelCnt;
}

int parseMenuConfig()
{
	char strLine[STR_LINE_MAX]={0};
	UI_FRAME_S *pUIFrame = NULL;
	UI_FRAME_S *pUIFrameInStack = NULL;
	UI_FRAME_S *pTmpUIFrame = NULL;	//will get a position in global malloc
	int s32Level = 0;
	int s32LastLevel = 0;
	int s32LineIndex = 0;	//
	
	FILE * fMenuConfig = fopen("menu_config.ini", "r+");
	if(!fMenuConfig)
	{
		printf("parser error: file open error, check whether menu_config.ini is exist\n");
		return -1;
	}
	if(bufGet(&pTmpUIFrame) < 0)				//get a temp pUIFrame
	{
		printf("error when bufGet\n");
		return -1;
	}

	while(!feof(fMenuConfig))
	{
		fgets(strLine,STR_LINE_MAX,fMenuConfig);
		if(strlen(strLine)>0)
		{
			s32LineIndex++;		//the No.s32LineIndex none blank line, should be equal to count of pUIFrame ? No! items Do!
			
			s32LastLevel = s32Level;
			s32Level = parseLine(strLine,pTmpUIFrame);	//parseLine will also operate ItemName and FuncIndex of temp pUIFrame

			if(s32Level > s32LastLevel)
			{
				if(bufGet(&pUIFrame) < 0)				//get new pUIFrame
				{
					printf("error when bufGet\n");
					return -1;
				}
				memset(pUIFrame,0,sizeof(UI_FRAME_S));
				memcpy(&(pUIFrame->stItem[pUIFrame->s32ItemCnt]),&(pTmpUIFrame->stItem[0]),sizeof(ITEM_S));
				pUIFrame->fnEsc = fnEnterOrEsc;
				if(frameUIStackTop(&pUIFrameInStack) == 0)
				{
					if(pUIFrameInStack == NULL)	//stack is empty,means you are the top menu
					{
						g_stFrameMgr.pTopUIFrame = pUIFrame;
						pUIFrame->pstHigherLevelItem = NULL;
						pUIFrame->pstHigherLevelFrame = NULL;
					}
					else
					{
						pUIFrameInStack->stItem[pUIFrameInStack->s32ItemCnt].pstLowerLevelFrame = pUIFrame;
						pUIFrame->pstHigherLevelFrame = pUIFrameInStack;
						pUIFrame->pstHigherLevelItem = &(pUIFrameInStack->stItem[pUIFrameInStack->s32ItemCnt]);
						pUIFrameInStack->s32ItemCnt++;	//遇到新的下一级后上一级的frame的item[i]才初始化完全了
					}
				}
				frameUIStackPush(pUIFrame);
			}
			else if(s32Level < s32LastLevel)	//s32Level < s32LastLevel case : no need to get new frameUI, just get from stack 
			{
				if(frameUIStackPop(&pUIFrameInStack) == 0)	//this is Pop ,not Top
				{
					if(pUIFrameInStack == NULL)	//
					{
						printf("abnormal001\n");
					}
					else
					{
						pUIFrameInStack->stItem[pUIFrameInStack->s32ItemCnt].pstLowerLevelFrame = NULL;
						pUIFrameInStack->s32ItemCnt++;	//遇到新的下一级后上一级的frame的item[i]才初始化完全了	
					}
				}
				for(int t=0;t<(s32LastLevel-s32Level)/FACTOR - 1; t++)	//still need pop
					frameUIStackPop(&pUIFrameInStack);

				if(frameUIStackTop(&pUIFrameInStack) == 0)	//TTTTop
				{
					if(pUIFrameInStack == NULL)	//stack is empty,means you are the top menu
					{
						printf("abnormal002\n");
					}
					else
					{
						memcpy(&(pUIFrameInStack->stItem[pUIFrameInStack->s32ItemCnt]),&(pTmpUIFrame->stItem[0]),sizeof(ITEM_S));
					}
				}
			}
			else	//s32Level == s32LastLevel
			{
				if(frameUIStackTop(&pUIFrameInStack) == 0)	//TTTTop
				{
					if(pUIFrameInStack == NULL)	//stack is empty,means you are the top menu
					{
						//g_stFrameMgr.pTopUIFrame = pUIFrame;	//abnormal if in here
						printf("abnormal\n");
					}
					else
					{
						pUIFrameInStack->stItem[pUIFrameInStack->s32ItemCnt].pstLowerLevelFrame = NULL;
						pUIFrameInStack->s32ItemCnt++;	//上面已经确定item没有下一级frame了,初始化完全了				
						memcpy(&(pUIFrameInStack->stItem[pUIFrameInStack->s32ItemCnt]),&(pTmpUIFrame->stItem[0]),sizeof(ITEM_S));
					}
				}
			}
			memset(pTmpUIFrame,0,sizeof(UI_FRAME_S));
			memset(strLine,0,STR_LINE_MAX);	//if a blank line is wirtten, strLine should be "" ,but should not be last line
		}
	}
	if(frameUIStackTop(&pUIFrameInStack) == 0)
	{
		if(pUIFrameInStack != NULL)
		{
			pUIFrameInStack->stItem[pUIFrameInStack->s32ItemCnt].pstLowerLevelFrame = NULL;
			pUIFrameInStack->s32ItemCnt++;
		}
	}
	//destroy the stack
	
	fclose(fMenuConfig);
	return 0;
}

void frameMgrCreate()
{
	UI_FRAME_S *pUIFrame = NULL;
	bufInit(FRAMES_MAX);

	frameUIStackInit();
}

void frameMgrDestroy()
{
	frameUIStackDeinit();
	bufDeinit();
}

static void showFrame(UI_FRAME_S * pUIFrame)
{
	int i=0;
	char inputStr[STR_LINE_MAX]={0};
	int itemIndexSelected;
	ITEM_S *pSelecedItem = NULL;

	if(pUIFrame->pstHigherLevelItem)
	{
		printf("************* ");
		printf("%s",pUIFrame->pstHigherLevelItem->pItemName);
		printf(" *************\n");
	}
	else
	{
		printf("************* ");
		printf("TOP menu");
		printf(" *************\n");
	}
	for(i = 0;i<pUIFrame->s32ItemCnt;i++)
	{
		printf("** %d. %s\n",i,pUIFrame->stItem[i].pItemName);
	}
	printf("** %d. back  ",i++);
	printf(" %d. quit  ",i++);
	printf(" %d. show total **\n",i++);
}

static void getInputAndProcess(UI_FRAME_S * pUIFrame)
{
	//int i=0;
	char inputStr[STR_LINE_MAX]={0};
	int itemIndexSelected;
	ITEM_S *pSelecedItem = NULL;

	if(pUIFrame->s32ItemCnt >0)
	{
		while(true)
		{
			printf("input your selection:");
			gets(inputStr);
			//printf("your input is %s\n",inputStr);
			if(strspn(inputStr,"0123456789") == strlen(inputStr))//如果 inputStr 中数字的个数和strlen相等,则说明字符串为全数字的字符串
			{
				itemIndexSelected = atoi(inputStr);
				if(itemIndexSelected < pUIFrame->s32ItemCnt+2)	//item_cnt + 1(back to high) + 1 quit
					break;
				else if(itemIndexSelected == pUIFrame->s32ItemCnt+2)	//(show total)
				{
					showTotal();
					showFrame(pUIFrame);
				}
				else
					printf("your input exceed valid range,please reinput\n");
			}
			else
			{
				printf("your input include invalid param ,please reinput\n");
			}
		}

		if(itemIndexSelected < pUIFrame->s32ItemCnt)		//
		{
			pSelecedItem = &(pUIFrame->stItem[itemIndexSelected]);
			if(pSelecedItem->pstLowerLevelFrame)
			{
				pSelecedItem->fnItemExec((int)pSelecedItem->pstLowerLevelFrame);	//enter
			}
			else
			{
				pSelecedItem->fnItemExec(0);	//exec
				showFrame(pUIFrame);
				getInputAndProcess(pUIFrame);
			}
		}
		else if (itemIndexSelected == pUIFrame->s32ItemCnt)	//esc
		{
			pUIFrame->fnEsc((int)pUIFrame->pstHigherLevelFrame);
		}
		else												//show total
		{
			printf("quit.\n");
		}
	}
}

static void _showTotal(UI_FRAME_S * pUIFrame,int level)
{
	int i=0,j=0;
	UI_FRAME_S *pUI = pUIFrame;
	if(pUI == NULL)
		return;
	for(i=0;i<pUI->s32ItemCnt;i++)
	{
		for(j=0;j<level;j++)printf("--");
		printf("%s\n",pUI->stItem[i].pItemName);
		_showTotal(pUI->stItem[i].pstLowerLevelFrame,level+1);
	}
}

void showTotal()
{
	_showTotal(g_stFrameMgr.pTopUIFrame,1);
	printf("\n");

}
void menuFrameworkRun()
{
	//showFrame(g_stFrameMgr.pTopUIFrame);
	UI_FRAME_S *pUIFrame = g_stFrameMgr.pTopUIFrame;

	showFrame(pUIFrame);
	getInputAndProcess(pUIFrame);	//should a dead while unless you select quit
}

