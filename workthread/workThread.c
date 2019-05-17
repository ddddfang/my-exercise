#include "workThread.h"

#define ENABLE_DEBUG
#ifdef ENABLE_DEBUG
#define err(str,...) printf(str,##__VA_ARGS__)
#else
#define err(str,...)
#endif

workManager gManager;


static void workerHold(int sig_id)
{
	(void)sig_id;
	struct list_node *linkNode;
	workThread *pWorker;

	//找到对应的thread
	pthread_t pth = pthread_self();
	ListForEach(linkNode, &(gManager.linkHead))
	{
		pWorker = ListNodeToItem(linkNode, workThread, linkNode);
		if(pWorker->pth == pth)
			break;
	}
	if(linkNode == &(gManager.linkHead))
		err("%s cannot find the thread(%lu) in worker list",__func__,pth);

	err("%s(%lu) paused ~ \n",pWorker->threadName,pWorker->pth);
	//等待信号量
	bsem_wait(&(pWorker->stBinarySem));
	err("%s(%lu) resumed ~ \n",pWorker->threadName,pWorker->pth);
}

static void *workerDo(void *pArg)
{
	void *pRet = NULL;
	workThread *pWork = (workThread *)pArg;

	//添加到global链表
	if(!gManager.bInit)
	{
		ListInit(&(gManager.linkHead));
		gManager.threadsNum = 0;
		gManager.bInit = 1;
	}
	ListAddTail(&(gManager.linkHead), &(pWork->linkNode));
	gManager.threadsNum++;

	//修改thread名字
	//prctl 是为进程设计的一个接口,PR_SET_NAME 标 使用 threadName 作为本进程的名字
	prctl(PR_SET_NAME, pWork->threadName);

	//注册信号处理函数
	struct sigaction act;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	act.sa_handler = workerHold;	//SIGUSR1事件发生时转去执行 workerHold . 睡眠,直到 threads_on_hold 清零
	if (sigaction(SIGUSR1, &act, NULL) == -1)
		err("%s cannot handle SIGUSR1",__func__);

	//执行用户函数
	err("%s(%lu) enter ~ \n",pWork->threadName,pWork->pth);
	if(pWork->usrFn)
		pRet = (*(pWork->usrFn))(pWork->pArg);
	err("%s(%lu) exit ~ \n",pWork->threadName,pWork->pth);

	//从global链表删除
	ListRemove(&pWork->linkNode);
	gManager.threadsNum--;
	return pRet;
}

workThread *workerCreate(void *(*usrFn)(void *),void *pArg,char *workerName)
{
	workThread *worker = (workThread *)malloc(sizeof(workThread));
	if(!worker)
	{
		err("%s: cannot malloc space for workThread.",__func__);
		return NULL;
	}
	memset(worker,0,sizeof(workThread));

	//init worker context
	ListInit(&(worker->linkNode));
	worker->usrFn = usrFn;
	worker->pArg = pArg;
	if(workerName)
		strcpy(worker->threadName,workerName);
	bsem_init(&(worker->stBinarySem),1); //这里不用初始化也可以

	//create thread and exec
	pthread_create(&worker->pth, NULL, (void *)workerDo, worker);
	//创建子进程的父进程应该负责回收子进程的资源,使用 pthread_join,不然子进程将成为 zombie
	//或者父进程直接退出,把回收资源的事情交给 init 来做
	//如果父进程不想等待(pthread_join 会导致父进程阻塞),可以将子进程设置为 detach,
	//这样子进程运行结束后自动释放所有资源
	//pthread_detach(worker->pth);
	return worker;
}

void workerDestroy(workThread *worker)
{
	if(worker)
	{
		pthread_join(worker->pth, NULL);	//等待子进程退出
		free(worker);
	}
}

void workerPause(workThread *worker)
{
	if(worker)
	{
		bsem_reset(&(worker->stBinarySem));
		pthread_kill(worker->pth, SIGUSR1);	
	}
}

void workerResume(workThread *worker)
{
	if(worker)
	{
		bsem_post(&(worker->stBinarySem));
	}
}
