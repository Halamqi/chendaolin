#ifndef SINGLETON_H_
#define SINGLETON_H_

#include "MutexGuard.h"

template <typename T>
class Singleton{
	public:
		static T& getInstance(){
			{
				MutexGuard lock(mutex_);
				if(!instance_){
					instance_=new T();
				}
				return *instance_;
			}
		}

	private:
		Singleton();
		~Singleton();
		static T* instance_;
		static Mutex mutex_;
};
template <typename T>
T* Singleton<T>::instance_=NULL;

template <typename T>
Mutex Singleton<T>::mutex_=Mutex();
#endif
