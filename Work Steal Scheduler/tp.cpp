#include <iostream>
#include "ThreadPool.h"
#include <unistd.h>
using namespace std;
// #include "ThreadPool.h"

class myjob : public job{
public:
	myjob(int id):job(id){}
	void working(void* param){
		int tid = *((int *)param);
		cout << "Printing from thread " << tid << " Job Id " << jobID << endl;
		if (jobID % 2)
		{
			cout << "Thread " << tid << " going to sleep on jobid " << jobID << endl;
			sleep(5);
			cout << "Thread " << tid << " wokeup! Ready to schedule " << endl;
		}
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