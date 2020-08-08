#ifndef SINGLETON_H_
#define SINGLETON_H_
#include <pthread.h>

template <typename T>
class Singleton{
	public:
		static T& getInstance(){
			pthread_once(&once_t,Singleton::get);
			return *instance_;
		}

	private:
		Singleton();
		~Singleton();
		
		static void get(){
			instance_=new T();
		}
	private:
		static T* instance_;
		static pthread_once_t once_t;
};

template <typename T>
T* Singleton<T>::instance_=NULL;

template <typename T>
pthread_once_t Singleton<T>::once_t=PTHREAD_ONCE_INIT;
#endif
