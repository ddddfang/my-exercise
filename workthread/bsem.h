#ifndef _BSEM_H
#define _BSEM_H
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>
#include <pthread.h>


/* Binary semaphore */
typedef struct bsem {
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	int v;
} bsem;


/* Init semaphore to 1 or 0 */
static void bsem_init(bsem *pBsem, int value)
{
	if (value < 0)
	{
		//err("bsem_init(): Binary semaphore can take only values 1 or 0");
		exit(1);
	}
	pthread_mutex_init(&(pBsem->mutex), NULL);
	pthread_cond_init(&(pBsem->cond), NULL);
	pBsem->v = value;
}

/* Reset semaphore to 0 */
static void bsem_reset(bsem *pBsem)
{
	bsem_init(pBsem, 0);
}

/* Post to at least one thread */
static void bsem_post(bsem *pBsem)
{
	pthread_mutex_lock(&pBsem->mutex);
	pBsem->v += 1;
	//这只会唤醒一个处于 pthread_cond_wait 的线程
	pthread_cond_signal(&pBsem->cond);
	pthread_mutex_unlock(&pBsem->mutex);
}

/* Post to all threads */
static void bsem_post_all(bsem *pBsem)
{
	pthread_mutex_lock(&pBsem->mutex);
	pBsem->v += 1;
	//这只会唤醒所有处于 pthread_cond_wait 的线程
	pthread_cond_broadcast(&pBsem->cond);
	pthread_mutex_unlock(&pBsem->mutex);
}

/* Wait on semaphore until semaphore has value 0 */
static void bsem_wait(bsem *pBsem)
{
	//mutex 用于保护 cond,上锁过程可能导致阻塞
	pthread_mutex_lock(&pBsem->mutex);
	while (pBsem->v == 0) {
		//1.pthread_cond_wait 需要在已经 lock mutex 的环境下进行
		//2.因为 pthread_cond_wait 会自动 unlock()
		//3.这么做是为了"在本task会因为调用pthread_cond_wait 而被放入等待队列休眠时,别人有机会拿到锁来唤醒本task"
		//4."unlock+将task放入等待队列" 通常是原子化的
		//5.因为unlock瞬间若别人lock并signal了(那么此唤醒机被错过了!),本task可能再无机会被唤醒
		//6.当条件 cond 满足,本task从等待队列移除并返回时,pthread_cond_wait 又会自动 lock
		//7.因为等待cond的task可能有很多,本task从休眠状态出来的第一件事就是再去check value,并操作value
		pthread_cond_wait(&pBsem->cond, &pBsem->mutex);
	}
	//pBsem->v = 0;
	pBsem->v -= 1;
	pthread_mutex_unlock(&pBsem->mutex);
}




#ifdef __cplusplus
}
#endif
#endif //