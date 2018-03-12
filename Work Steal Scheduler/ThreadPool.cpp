//Reference : https://13abyknight.wordpress.com/2013/03/20/a-simple-thread-pool-c-implementation-on-linux/
#include <iostream>
#include <queue>
#include <pthread.h>
#include "ThreadPool.h"
 
using namespace std;
 



thread_pool::thread_pool(){
    numOfThreads = 2;
    for (int i = 0; i < numOfThreads; ++i)
    {
        threads.push_back(new worker_thread(i, this));
    }
}

thread_pool::thread_pool(int num){
    numOfThreads = num;
    for (int i = 0; i < numOfThreads; ++i)
    {
        threads.push_back(new worker_thread(i, this));
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

job* thread_pool::StealTask(worker_thread* p){

    //Implement your steal algo
    //dont look for own queue here else u will get a deadlock

    for (int i = 0; i < numOfThreads; ++i)
    {
        
        if (!threads[i]->jobDequeue.empty())
        {
            cout << "--------------Thread " << p->tid << " trying to steal Task from------" << i << endl;
            pthread_mutex_lock(&threads[i]->jobDequeue_lock);
            job* j = threads[i]->jobDequeue.back();
            threads[i]->jobDequeue.pop_back();
            pthread_mutex_unlock(&threads[i]->jobDequeue_lock);
            cout << "--------------Thread " << p->tid << " steal Task from " << i << " success------ jobid " << j->jobID << endl;
            return j;
        }
        // pthread_mutex_unlock(&threads[i]->jobDequeue_lock);
    }
    return NULL;
}



worker_thread::worker_thread(int _tid, thread_pool* _parentPool):tid(_tid), parentPool(_parentPool){
    thread = new pthread_t;
    // jobDequeue_lock = PTHREAD_MUTEX_INITIALIZER;
    // jobDequeue_cond = PTHREAD_COND_INITIALIZER;
}

worker_thread::~worker_thread(){
    delete thread;
    thread = NULL;
}

void worker_thread::assignJob(job *_job_){
    pthread_mutex_lock(&jobDequeue_lock);
    jobDequeue.push_back(_job_);
    pthread_mutex_unlock(&jobDequeue_lock);
    pthread_cond_signal(&jobDequeue_cond);   
}

bool worker_thread::loadJob(job*& _job_, worker_thread* p)
{
    pthread_mutex_lock(&jobDequeue_lock);

    while(jobDequeue.empty() && (_job_ = StealTask(p)) && !_job_){
        pthread_cond_wait(&jobDequeue_cond, &jobDequeue_lock);
    }


    //first look for own job queue
    if (!jobDequeue.empty())
    {
        _job_ = jobDequeue.front();
        jobDequeue.pop_front();
        pthread_mutex_unlock(&jobDequeue_lock);
        return true;
    }
    

    //if own job queue is empty look for other threads job queue and extract from back
    // _job_ = StealTask(p);
    pthread_mutex_unlock(&jobDequeue_lock);
    return true;
}

void worker_thread::start(){

    pthread_create(thread, NULL, &worker_thread::threadExecute, (void *)this);
    // cout << "Thread:" << tid << " is alive now!\n";

}

void *worker_thread::threadExecute(void *param)
{
    worker_thread *p = (worker_thread *)param;
    job *oneJob = NULL;
    while(p->loadJob(oneJob, p))
    {
        if(oneJob)
            oneJob->working((void*)&p->tid);
        delete oneJob;
        oneJob = NULL;
    }
}

job* worker_thread::StealTask(worker_thread* p){
    return parentPool->StealTask(p);
}