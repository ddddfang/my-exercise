#ifndef _WORKTHREAD_H
#define _WORKTHREAD_H
#ifdef __cplusplus
extern "C"
{
#endif

#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <sys/prctl.h>
#include "list.h"
#include "bsem.h"

typedef struct {
	struct list_node linkNode;
	pthread_t pth;
	char threadName[128];
	void *(*usrFn)(void *);
	void *pArg;
	bsem stBinarySem;
} workThread;

typedef struct {
	char bInit;
	int threadsNum;
	struct list_node linkHead;
} workManager;

workThread *workerCreate(void *(*usrFn)(void *),void *pArg,char *workerName);
void workerDestroy(workThread *worker);
void workerPause(workThread *worker);
void workerResume(workThread *worker);


#ifdef __cplusplus
}
#endif
#endif //