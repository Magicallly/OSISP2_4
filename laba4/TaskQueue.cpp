#include "TaskQueue.h"

TaskQueue::TaskQueue()
{
	InitializeCriticalSectionAndSpinCount(&criticalSection, SPINS);
	first = NULL;
	last = NULL;
}

Task* TaskQueue::Dequeue()
{
	EnterCriticalSection(&criticalSection);
	if (Empty())
	{
		LeaveCriticalSection(&criticalSection);
		return NULL;
	}
	if (first == last)
	{
		Node* tmpReturn = first;
		first = NULL;
		last = NULL;
		LeaveCriticalSection(&criticalSection);
		return tmpReturn->data;
	}
	Node* tmpLast = last;
	last = last->prev;
	last->next = NULL;
	LeaveCriticalSection(&criticalSection);
	return tmpLast->data;
}

void TaskQueue::Enqueue(Task* indata)
{
	EnterCriticalSection(&criticalSection);
	Node* tmp = new Node();
	tmp->data = indata;
	tmp->next = NULL;
	tmp->prev = NULL;

	if (Empty())
	{
		first = tmp;
		last = tmp;
	}
	else
	{
		Node* tmpFirst = first;
		first = tmp;
		first->next = tmpFirst;
		tmpFirst->prev = first;
	}
	LeaveCriticalSection(&criticalSection);
}



bool TaskQueue::Empty()
{
	return ((first == NULL) && (last == NULL));
}

TaskQueue::~TaskQueue()
{
	DeleteCriticalSection(&criticalSection);
}
