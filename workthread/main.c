
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "buffList.h"
#include "workThread.h"
#include "bsem.h"



typedef struct{

	unsigned char inputShouldStop;
	workThread *pInputWorker;

	unsigned char processShouldStop;
	workThread *pProcessWorker;

} mySt;

static mySt g_pes2esSt={0};

static void *inputTask(void *pArg)
{
	mySt *p = (mySt *)pArg;
	printf("enter inputTask.\n");
	int i=0;
	while(p->inputShouldStop == 0)
	{
		printf("inputTask,%d\n",i++);
		sleep(1);
	}
	printf("leave inputTask.\n");
	return NULL;
}

static void *processTask(void *pArg)
{
	mySt *p = (mySt *)pArg;
	printf("enter processTask.\n");
	int i=0;

	while(p->processShouldStop == 0)
	{
		printf("processTask,%d\n",i++);
		if(i==10)
			workerPause(g_pes2esSt.pInputWorker);
		if(i==15)
			workerResume(g_pes2esSt.pInputWorker);
		sleep(1);
	}
	printf("leave processTask.\n");
	return NULL;
}

int main(int argc, char *argv[])
{
	g_pes2esSt.processShouldStop = 0;
	g_pes2esSt.pProcessWorker = workerCreate(processTask,&g_pes2esSt,"processTask");

	g_pes2esSt.inputShouldStop = 0;
	g_pes2esSt.pInputWorker = workerCreate(inputTask,&g_pes2esSt,"inputTask");

	static int i=0;
    while(i++ < 50)
    {
		//printf("i is %d\n",i);
		if(i==8)
			workerPause(g_pes2esSt.pProcessWorker);

		if(i==16)
			workerResume(g_pes2esSt.pProcessWorker);

		sleep(1);
    }
	g_pes2esSt.processShouldStop = 1;
	g_pes2esSt.inputShouldStop = 1;
	workerDestroy(g_pes2esSt.pInputWorker);
	workerDestroy(g_pes2esSt.pProcessWorker);
}

