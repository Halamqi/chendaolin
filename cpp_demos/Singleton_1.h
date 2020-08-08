#ifndef SINGLETON_H_
#define SINGLETON_H_

template <typename T>
class Singleton{
	private:
		Singleton(){}
		~Singleton(){};
		static T* instance;

	public:
		static T& getInstance(){
			if(!instance){
				instance=new T();
			}
			return *instance;
		}
};

template <typename T>
T* Singleton<T>::instance=NULL;
#endif
