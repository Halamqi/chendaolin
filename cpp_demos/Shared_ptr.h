template <typename T>
class Shared_ptr{
	public:
		Shared_ptr(T* rawPtr=nullptr);
		Shared_ptr(const Shared_ptr& other);
		Shared_ptr& operator=(const Shared_ptr& other);
	//	Shared_ptr(Shared_ptr&& other);
	//	Shared_ptr& operator=(Shared_ptr&& other);
		~Shared_ptr();

		int user_count() 
		{
			if(userCount_)
				return *userCount_;
			else return 0;
		}
		bool unique() {return user_count()==1;}
		
		T* get() {return raw_;}
		
		operator bool() const {return raw_!=nullptr;}

		T& operator*(){return *raw_;}
		T* operator->(){return raw_;}
		
		void reset(T* newPtr);
		void swap(Shared_ptr& other);

	private:
		T* raw_;
		int* userCount_;

};

template <typename T>
Shared_ptr<T>::Shared_ptr(T* rawPtr)
	:raw_(rawPtr),
	userCount_(rawPtr==nullptr?nullptr:new int(1))
{
}

template <typename T>
Shared_ptr<T>::Shared_ptr(const Shared_ptr<T>& other)
	:raw_(other.raw_),
	userCount_(other.userCount_)
{
	if(raw_&&userCount_)
		++(*userCount_);
}

template <typename T>
Shared_ptr<T>& Shared_ptr<T>::operator=(const Shared_ptr<T>& other)
{
	if(this==&other) return *this;
	if(other)
	{
		if(raw_)
		{
			if(--(*userCount_)==0)
			{
				delete raw_;
				delete userCount_;
			}
		}
		
		raw_=other.raw_;
		userCount_=other.userCount_;
		++(*userCount_);
	}
	else
	{
		if(raw_)
		{
			if(--(*userCount_)==0)
			{
				delete raw_;
				delete userCount_;
			}
		}
		raw_=nullptr;
		userCount_=nullptr;
	}
	return *this;
}

template <typename T>
Shared_ptr<T>::~Shared_ptr()
{
	if(raw_)
	{
		if(--(*userCount_)==0)
		{
			delete raw_;
			delete userCount_;
		}
	}
}

template <typename T>
void Shared_ptr<T>::swap(Shared_ptr<T>& other)
{
	T* temp=nullptr;
	int* tempuser=nullptr;

	temp=raw_;
	raw_=other.raw_;
	other.raw_=temp;

	tempuser=userCount_;
	userCount_=other.userCount_;
	other.userCount_=tempuser;
}

template <typename T>
void Shared_ptr<T>::reset(T* newRaw)
{
	if(newRaw==nullptr)
	{
		if(raw_)
		{
			if(--(*userCount_)==0)
			{
				delete raw_;
				delete userCount_;
			}
		}
		raw_=nullptr;
		userCount_=nullptr;
	}

	else
	{
		if(raw_)
		{
			if(--(*userCount_)==0)
			{
				delete raw_;
				delete userCount_;
			}
		}
		raw_=newRaw;
		userCount_=new int(1);
	}
}
