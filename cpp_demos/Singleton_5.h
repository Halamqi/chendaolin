#ifndef SINGLETON_H_
#define SINGLETON_H_
template <typename T>
class Singleton{
	public:
		static T& getInstance(){
			static T instance_;
			return instance_;
		}

	private:
		Singleton();
		~Singleton();
};
#endif
