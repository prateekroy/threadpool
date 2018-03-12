//Reference: https://13abyknight.wordpress.com/2013/03/20/a-simple-thread-pool-c-implementation-on-linux/
#ifndef __THREADPOOL_H_
#define __THREADPOOL_H_

#include <iostream>
#include <queue>
#include <pthread.h>

using namespace std;

class job
{
public:
    job(int id) : jobID(id) {}
    virtual ~job(){}
    //will be overrided by specific JOB
    void virtual working(void* param) = 0;

protected:
    int jobID;

private:
    static int finished_jobs;
    static pthread_mutex_t jobLock;
};

class worker_thread{
    pthread_t* thread;
    queue<job*> jobQueue;
    int tid;
    bool loadJob(job*& _job_);
    static void *threadExecute(void *);
    static pthread_mutex_t jobQueue_lock;
    static pthread_cond_t jobQueue_cond;

public:
    worker_thread(int _tid);
    ~worker_thread();
    void assignJob(job *_job_);
    void start();
};
 
class thread_pool
{
public:
    thread_pool();
    thread_pool(int num);
    virtual ~thread_pool() {};
    void assignJob(job *_job_);   //some huerestic to assign job to which thread 
    void start();

private:
    vector<worker_thread*> threads;
    int numOfThreads;
};

#endif