#include <iostream>
#include "ThreadPool.h"
#include <unistd.h>
using namespace std;
// #include "ThreadPool.h"

class myjob : public job{
public:
	myjob(int id):job(id){}
	void working(){

		cout << "Printing from thread " << jobID << endl;
		sleep(5);
	}
};

int main(int argc, char const *argv[])
{

	thread_pool tp(4);
	for (int i = 0; i < 10; ++i)
	{
		myjob* j = new myjob(i);
		tp.assignJob(j);
	}

	tp.start();


	int x;
	cin >> x;

	return 0;
}