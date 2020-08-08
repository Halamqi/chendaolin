#include "Mutex.h"
#include <exception>

Mutex::Mutex(){
	if(pthread_mutex_init(&mutex_,NULL)!=0)
		throw std::exception();
}

Mutex::~Mutex(){
	pthread_mutex_destroy(&mutex_);
}

bool Mutex::lock(){
	return pthread_mutex_lock(&mutex_)==0;
}

bool Mutex::unlock(){
  	return pthread_mutex_unlock(&mutex_)==0;
}
