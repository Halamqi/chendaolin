#include <iostream>
#include "Singleton_5.h"
#include <vector>
#include <unistd.h>
#include <pthread.h>
#include "MutexGuard.h"
using namespace std;

Mutex mutex;

void* threadfunc(void* arg){
	{
		MutexGuard lock(mutex);
		cout<<Singleton<int>::getInstance()<<endl;
	}
	return nullptr;
}

int main(int argc,char* argv[])
{
	int& a=Singleton<int>::getInstance();
	{
		MutexGuard lock(mutex);
		a=10;
		cout<<Singleton<int>::getInstance()<<endl;
	}
	pthread_t tid;
	pthread_create(&tid,NULL,threadfunc,NULL);
	sleep(5);
	return 0;
}
