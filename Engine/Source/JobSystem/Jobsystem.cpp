#include "EnginePCH.h"
#pragma hdrstop

using namespace Perry;

JobSystem::JobSystem()
{
}

void JobSystem::RegisterTask(Task* task)
{
	Tasks[task->Type].push(task);
}

Task* JobSystem::ReceiveTask()
{
	//TODO make smart

	if (!Tasks[TaskType::Game].empty())
	{
		Task* t;
		if (Tasks[TaskType::Game].try_pop(t))
			return t;
	}

	if (!Tasks[TaskType::Graphics].empty())
	{
		Task* t;
		if (Tasks[TaskType::Graphics].try_pop(t))
			return t;
	}

	ASSERT(LOG_GENERIC, false);
	return nullptr;
}

JobSystem* GetJobSystem()
{
	static JobSystem instance = {};

	return &instance;
}

void Job::CompleteTask(Task* task) const
{
	task->IsComplete = true;
	OnComplete();

}


JobThread::JobThread()
{
	//thread = std::thread(&AwaitTaskLoop,this);
}
JobThread::~JobThread()
{
	active = false;
}

void JobThread::AwaitTaskLoop()
{
	while (active)
	{
		if (localJobs.empty())
		{
			//Our jobsqueu is empty at some point we want to steal from global queue
		}

		//GetJobSystem()->
	}
}