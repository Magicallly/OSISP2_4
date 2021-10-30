#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "TaskQueue.h"

#define SPINS 4000

class TasksExecutor
{
public:
	TasksExecutor(int threadsCount);
	void WaitAll();
	void StartProcessing(int threadsCount, TaskQueue* taskQueue);
	~TasksExecutor();
private:
	HANDLE* hThreads;
	int threadsAmount;
	static DWORD WINAPI ThreadFunction(LPVOID obj);
};

#endif

