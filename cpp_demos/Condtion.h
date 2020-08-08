#include "Mutex.h"

class Condition{
	public:
		Condition(Mutex& m)
			:mutex_(m)
		{
			pthread_cond_init(&cond_,NULL);
		}

		~Condition(){
			pthread_cond_destroy(&cond_);
		}

		void wait(){
			pthread_cond_wait(&cond_,mutex_.getPthreadMutex());
		}

		void notify(){
			pthread_cond_signal(&cond_);
		}

		void notifyAll(){
			pthread_cond_broadcast(&cond_);
		}

	private:
		pthread_cond_t cond_;
		Mutex& mutex_;
};
