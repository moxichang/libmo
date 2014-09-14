#include <iostream>

#include <IPC/Semaphore.h>

using namespace std;

int main()
{

	Semaphore *sem = new Semaphore("/tmp.test.trigger");

	while(1){

		sem->Wait();
		cout<<"got trigger."<<endl;
	}

	delete sem;

	return 0;
}

