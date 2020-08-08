#ifndef MUTEX_H_
#define MUTEX_H_
#include <pthread.h>


class Mutex{
public:
	Mutex();
	~Mutex();
	bool lock();
	bool unlock();
	inline pthread_mutex_t* getPthreadMutex(){
		return &mutex_;
	}
private:
	pthread_mutex_t mutex_;
};

#endif
