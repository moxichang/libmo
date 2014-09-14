#include <iostream>

#include <IPC/Semaphore.h>

using namespace std;

int main()
{

	Semaphore *sem = new Semaphore("/tmp.test.trigger");

	sem->Post();

	cout<<"trigger on."<<endl;

	delete sem;

	return 0;
}

