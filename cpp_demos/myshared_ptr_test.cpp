#include <iostream>

#include "Shared_ptr.h"

using namespace std;

struct A{
	int a;
	short b;
	void print()
	{
		cout<<"a = "<<a<<" b = "<<b<<endl;
	}

	A(int x=0,int y=0):a(x),b(y){}
};

int main()
{
	Shared_ptr<int> p1;
	int count1=p1.user_count();
	void* rawp1=p1.get();
	cout<<"count1 = "<<count1<<" rawp1 = "<<rawp1<<endl;

	cout<<"after reset Shared_ptr<int> p1:\n";
	int* pi=new int(10);
	p1.reset(pi);
	cout<<"count1 = "<<p1.user_count()<<" rawp1 = "<<p1.get()
		<<" value = "<<*p1<<endl;

	Shared_ptr<int> p3(new int(9));
	cout<<"count3 = "<<p3.user_count()<<" rawp3 = "<<p3.get()
		<<" value = "<<*p3<<endl;
	
/*	cout<<"after assignment:\n";
	p3=p1;
	cout<<"count1 = "<<p1.user_count()<<" rawp1 = "<<p1.get()
		<<" value = "<<*p1<<endl;
	cout<<"count3 = "<<p3.user_count()<<" rawp3 = "<<p3.get()
		<<" value = "<<*p3<<endl;
*/
	cout<<"after swap:\n";
	p1.swap(p3);
	cout<<"count1 = "<<p1.user_count()<<" rawp1 = "<<p1.get()
		<<" value = "<<*p1<<endl;
	cout<<"count3 = "<<p3.user_count()<<" rawp3 = "<<p3.get()
		<<" value = "<<*p3<<endl;

	Shared_ptr<double> p2(new double(12131.21313));
	int count2=p2.user_count();
	void* rawp2=p2.get();
	cout<<"count2 = "<<count2<<" rawp2 = "<<rawp2
		<<" value = "<<*p2<<endl;
	
	Shared_ptr<A> pA1(new A(1,1));
	cout<<"count4 = "<<pA1.user_count()<<" rawpA1 = "<<pA1.get()
		<<" value:A.a = "<<(*pA1).a
		<<" value:A.b = "<<(*pA1).b
		<<" value:A->a = "<<pA1->a
		<<" value:A->b = "<<pA1->b
		<<endl;
		cout<<" A->print() ";
		pA1->print();

	Shared_ptr<A> pA2(pA1);
	pA2->a=100;
	pA2->b=99;
	cout<<"count5 = "<<pA2.user_count()<<" rawpA2 = "<<pA2.get()
		<<" value:A.a = "<<(*pA2).a
		<<" value:A.b = "<<(*pA2).b
		<<" value:A->a = "<<pA2->a
		<<" value:A->b = "<<pA2->b
		<<endl;

		cout<<" A->print() ";
		pA2->print();
	return 0;
}
