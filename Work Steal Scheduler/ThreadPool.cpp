//Reference : https://13abyknight.wordpress.com/2013/03/20/a-simple-thread-pool-c-implementation-on-linux/
#include <iostream>
#include <queue>
#include <pthread.h>
#include "ThreadPool.h"
 
using namespace std;
  
int job::finished_jobs = 0;
 
pthread_mutex_t worker_thread::jobQueue_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t worker_thread::jobQueue_cond = PTHREAD_COND_INITIALIZER;
 
// void thread_pool::initThreads(pthread_t *threads)
// {

//     for(int i = 0; i < numOfThreads; i++)
//     {
//         pthread_create(&threads[i], NULL, &thread_pool::threadExecute, (void *)this);
//         cout << "Thread:" << i << " is alive now!\n";
//     }
// }

thread_pool::thread_pool(){
    numOfThreads = 2;
    for (int i = 0; i < numOfThreads; ++i)
    {
        threads.push_back(new worker_thread(i));
    }
}

thread_pool::thread_pool(int num){
    numOfThreads = num;
    for (int i = 0; i < numOfThreads; ++i)
    {
        threads.push_back(new worker_thread(i));
    }
}


void thread_pool::start()
{
    for(int i = 0; i < numOfThreads; i++)
    {
        threads[i]->start();
    }
}

void thread_pool::assignJob(job* _job_)
{
    static int threadid = 0;
    //some huerestic now trying round robin
    threads[threadid++]->assignJob(_job_);
    if (threadid >= numOfThreads)
    {
        threadid = 0;
    }
}





worker_thread::worker_thread(int _tid):tid(_tid){
    thread = new pthread_t;
}

worker_thread::~worker_thread(){
    delete thread;
    thread = NULL;
}

void worker_thread::assignJob(job *_job_){
    pthread_mutex_lock(&jobQueue_lock);
    jobQueue.push(_job_);
    pthread_mutex_unlock(&jobQueue_lock);
    pthread_cond_signal(&jobQueue_cond);   
}

bool worker_thread::loadJob(job*& _job_)
{
    pthread_mutex_lock(&jobQueue_lock);
    while(jobQueue.empty())
        pthread_cond_wait(&jobQueue_cond, &jobQueue_lock);
    _job_ = jobQueue.front();
    jobQueue.pop();
    pthread_mutex_unlock(&jobQueue_lock);
    return true;
}

void worker_thread::start(){

    pthread_create(thread, NULL, &worker_thread::threadExecute, (void *)this);
    cout << "Thread:" << tid << " is alive now!\n";

}

void *worker_thread::threadExecute(void *param)
{
    worker_thread *p = (worker_thread *)param;
    job *oneJob = NULL;
    while(p->loadJob(oneJob))
    {
        if(oneJob)
            oneJob->working((void*)&p->tid);
        delete oneJob;
        oneJob = NULL;
    }
}