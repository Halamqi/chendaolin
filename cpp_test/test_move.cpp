#include <iostream>
#include <utility>
#include <string>

using namespace std;

int main(){
	int&& a=10;
	cout<<a<<endl;
	a++;
	cout<<a<<endl;
	return 0;
}
