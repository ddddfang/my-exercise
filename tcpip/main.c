
#include "main.h"
#include "utils.h"
#include "tap_if.h"
#include "arp.h"

#ifdef TAG
#undef TAG
#define TAG "MAIN"
#endif

typedef enum {
    THREAD_CORE = 0,
    THREAD_TIMERS,
    THREAD_IPC,
    THREAD_SIGNAL,
    THREAD_MAXNUM
} THRREAD_NUMS;

static pthread_t threads[THREAD_MAXNUM];
int running = 1;

static void create_thread(pthread_t id, void *(*func) (void *), void *arg)
{
    if (pthread_create(&threads[id], NULL, func, arg) != 0) {
        ERROR_PRINT("Could not create core thread.\r\n");
    }
}

static void join_threads()
{
    //等待所有线程退出(signal handle的线程早就return了)
    for (int i = 0; i < THREAD_MAXNUM; i++) {
        if (i != THREAD_SIGNAL) {
            if (pthread_join(threads[i], NULL) != 0) {
                ERROR_PRINT("Error when joining threads.\r\n");
                exit(1);
            }
        }
    }
}

static void *stop_stack_handler(void *arg)
{
    sigset_t *mask = (sigset_t *)arg;
    int err, signo;

    while (1) {
        err = sigwait(mask, &signo);
        if (err != 0) {
            ERROR_PRINT("Sigwait failed: %d.\r\n", err);
        }

        switch (signo) {
            case SIGINT:
            case SIGQUIT:
                running = 0;
                for (int i = 0; i < THREAD_MAXNUM; i++) {
                    if (i != THREAD_SIGNAL) {
                        //提出线程取消请求,线程并不立即停止,而是等到某个
                        //取消点(就是检查stop flag的地方)
                        pthread_cancel(threads[THREAD_IPC]);
                    }
                }
                return 0;   //取消其他后,自己是立即退出
            default:
                DEBUG_PRINT("Unexpected signal %d\r\n", signo);
        }
    }
}

static void *timers_start(void *arg)
{
    int i = 0;
    while(running) {
        //DEBUG_PRINT("timers_start %d\r\n", i++);
        sleep(2);
    }
    return NULL;
}
static void *start_ipc_listener(void *arg)
{
    int i = 0;
    while(running) {
        //DEBUG_PRINT("start_ipc_listener %d\r\n", i++);
        sleep(2);
    }
    return NULL;
}

int main()
{
    //初始化 sigset_t
    sigset_t mask;
    int err;

    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGQUIT);

    if ((err = pthread_sigmask(SIG_BLOCK, &mask, NULL)) != 0) {
        ERROR_PRINT("SIG_BLOCK error\r\n");
        exit(1);
    }

    //
    tun_init();
    netdev_init(NULL, NULL);
    arp_init();

    create_thread(THREAD_CORE, netdev_rx_loop, NULL);
    create_thread(THREAD_TIMERS, timers_start, NULL);
    create_thread(THREAD_IPC, start_ipc_listener, NULL);
    create_thread(THREAD_SIGNAL, stop_stack_handler, &mask);

    join_threads(); //等待所有线程退出

    free_arp();
    free_netdev();
    free_tun();
    DEBUG_PRINT("Exit.\r\n");
    return 0;
}
