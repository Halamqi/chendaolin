#ifndef MUTEXGUARD_H_
#define MUTEXGUARD_H_
#include "Mutex.h"

class MutexGuard{
	public:
		MutexGuard(Mutex& m)
			:mutex(m){
			mutex.lock();
			}

		~MutexGuard(){
			mutex.unlock();
		}
	private:
		Mutex& mutex;
};

#endif
