#include "thread_pool.h"

void* taskforThread(void* arg){
	int number=*((int*)arg);
	printf("this is %lu , i am handling %d task\n",pthread_self(),number);
	free(arg);
	usleep(1000);
	return NULL;
}

int main(int argc,char* argv[])
{
	thread_pool_t pool;
	threadPoolInit(&pool,3,10);
	for(int i=0;i<10000;i++){
		int* arg=(int*)malloc(sizeof(int));
		if(arg==NULL){
			perror("malloc failed: ");
			exit(1);
		}
		*arg=i+1;
		threadPoolAddTask(&pool,taskforThread,(void*)arg);
	}	
	threadPoolDestroy(&pool);
	return 0;
}
