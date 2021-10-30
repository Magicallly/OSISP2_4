#ifndef TASKQUEUE_H
#define TASKQUEUE_H

#include <windows.h>
#include <queue>

#include "Task.h"

#define SPINS 4000

class Node
{
public:
	Task* data;
	Node* prev;
	Node* next;
};

class TaskQueue
{
private:
	Node* first, * last;
	CRITICAL_SECTION criticalSection;


public:
	TaskQueue();
	Task* Dequeue();
	void Enqueue(Task* indata);
	bool Empty();
	~TaskQueue();
};

#endif


