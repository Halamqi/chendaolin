#ifndef THREAD_POOL_H_
#define TRHEAD_POOL_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <errno.h>

typedef struct task{
	void* (*thread_cb) (void*);
	void* arg;
	struct task* next;
}task_t;


typedef struct thread_pool{
	pthread_cond_t queueNotEmpty;		//任务就绪或者是线程池销毁的通知
	pthread_cond_t nothreadWorking;		//当没有线程在工作时，并且所有线程都退出后的通知

	pthread_mutex_t poolMutex;			//mutex互斥锁配合条件变量使用

	task_t* queueHead;					//任务队列的头指针
	task_t* queueTail;					//任务队列的尾指针

	int freeThread;						//空闲着在等待任务的线程
	int totalThread;					//现在线程池中总的线程数
	int maxThread;						//能开启的最大线程数

	int shutdown;						//是否关闭线程池

}thread_pool_t;

void threadPoolInit(thread_pool_t* pool,int thread_num,int maxThreadN);
void threadPoolAddTask(thread_pool_t* pool,void* (*cb)(void*),void* arg);
void threadPoolDestroy(thread_pool_t* pooll);

#endif 
