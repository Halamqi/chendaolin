#include "thread_pool.h"
void* thread_runtine(void* arg){
	thread_pool_t* pool=(thread_pool_t*) arg;
	printf("thread %lu is starting\n",pthread_self());
	while(1){
		pthread_mutex_lock(&pool->poolMutex);
		while(pool->queueHead==NULL&&!pool->shutdown){
			printf("%lu thread is waiting...\n",pthread_self());
			pool->freeThread++;
			pthread_cond_wait(&pool->queueNotEmpty,&pool->poolMutex);
		}
		
		pool->freeThread--;
		if(pool->queueHead!=NULL){
			task_t* taskTodo=pool->queueHead;
			pool->queueHead=taskTodo->next;
			pthread_mutex_unlock(&pool->poolMutex);
			taskTodo->thread_cb(taskTodo->arg);
			free(taskTodo);
			pthread_mutex_lock(&pool->poolMutex);
		}
		if(pool->shutdown&&pool->queueHead==NULL){
			printf("%lu thread is exiting\n",pthread_self());
			pool->totalThread--;
			if(pool->totalThread==0){
				pthread_cond_signal(&pool->nothreadWorking);
			}
			pthread_mutex_unlock(&pool->poolMutex);
			break;
		}
		pthread_mutex_unlock(&pool->poolMutex);
	}
	return NULL;
}


void threadPoolInit(thread_pool_t* pool,int thread_num,int maxThreadN){
	pthread_cond_init(&pool->queueNotEmpty,NULL);
	pthread_cond_init(&pool->nothreadWorking,NULL);
	pthread_mutex_init(&pool->poolMutex,NULL);

	pool->queueHead=NULL;
	pool->queueTail=NULL;

	pool->freeThread=0;
	pool->totalThread=thread_num;
	pool->maxThread=maxThreadN;

	pool->shutdown=0;

	for(int i=0;i<thread_num;i++){
		pthread_t tid;
		pthread_create(&tid,NULL,thread_runtine,(void*)pool);
		pthread_detach(tid);
		printf("%lu thread creating\n",tid);
	}

}

void threadPoolAddTask(thread_pool_t* pool,void*(*cb)(void*),void* arg){
	if(pool->shutdown){
		printf("thread pool is closing\n");
		return;
	}
	task_t* newTask=(task_t*)malloc(sizeof(task_t));
	newTask->thread_cb=cb;
	newTask->arg=arg;
	
	pthread_mutex_lock(&pool->poolMutex);
	if(pool->queueHead==NULL){
		pool->queueHead=newTask;
	}
	else{
		pool->queueTail->next=newTask;
	}
	pool->queueTail=newTask;

	if(pool->freeThread>0){
		pthread_mutex_unlock(&pool->poolMutex);
		pthread_cond_signal(&pool->queueNotEmpty);
	}
	else if(pool->totalThread<pool->maxThread){
		pthread_t tid;
		pthread_create(&tid,NULL,thread_runtine,(void*)pool);
		printf("no thread is waiting task,server is busy, create a new thread%lu\n",tid);	
		pthread_detach(tid);
		pool->freeThread++;
		pool->totalThread++;
		pthread_mutex_unlock(&pool->poolMutex);
		pthread_cond_signal(&pool->queueNotEmpty);
	}
	else {
		pthread_mutex_unlock(&pool->poolMutex);
		pthread_cond_signal(&pool->queueNotEmpty);
	
	}
}

void threadPoolDestroy(thread_pool_t* pool){
	if(pool->shutdown){
		return;
	}

	pthread_mutex_lock(&pool->poolMutex);
	pool->shutdown=1;
	pthread_cond_broadcast(&pool->queueNotEmpty);
	while(pool->totalThread>0){
		pthread_cond_wait(&pool->nothreadWorking,&pool->poolMutex);
	}
	pthread_cond_destroy(&pool->queueNotEmpty);
	pthread_cond_destroy(&pool->nothreadWorking);
	pthread_mutex_destroy(&pool->poolMutex);

	pthread_mutex_unlock(&pool->poolMutex);
}
