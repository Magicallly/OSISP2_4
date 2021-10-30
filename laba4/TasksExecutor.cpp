#include "TasksExecutor.h"


TasksExecutor::TasksExecutor(int threadsCount)
{
	hThreads = (HANDLE*)malloc(sizeof(HANDLE) * threadsCount);
	this->threadsAmount = threadsCount;
}

void TasksExecutor::StartProcessing(int threadsCount, TaskQueue* taskQueue)
{
	for (int i = 0; i < threadsCount; i++)
	{
		hThreads[i] = CreateThread(NULL, 0, &ThreadFunction, (LPVOID)taskQueue, 0, NULL);
	}
}

void TasksExecutor::WaitAll()
{
	WaitForMultipleObjects(threadsAmount, hThreads, TRUE, INFINITE);
}


DWORD WINAPI TasksExecutor::ThreadFunction(LPVOID obj)
{
	TaskQueue* tasks = (TaskQueue*)obj;
	Task* tmp = tasks->Dequeue();
	tmp->func(tmp->params);

	return 0;
}


TasksExecutor::~TasksExecutor()
{
	for (int i = 0; i < threadsAmount; i++)
	{
		CloseHandle(hThreads[i]);
	}
}

