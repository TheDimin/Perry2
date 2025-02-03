#include "EnginePCH.h"
#pragma hdrstop

#include <windows.h>
//#define FIBER_ENABLED

inline void SwitchTrackedFiber(void* fiber)
{
	void* oldFiber = GetCurrentFiber();
	PROFILER_FIBER_SWITCH(oldFiber,fiber);
	::SwitchToFiber(fiber);
	PROFILER_FIBER_RETURN(oldFiber);
}

void Perry::Fiber::Execute()
{
#ifdef FIBER_ENABLED
	this->ParentFiber = GetCurrentFiber();
	GetThreadPool()->taskQueue.push(this);
	GetThreadPool()->RunJob();
#else
	FiberContext fc;
	function(fc);
#endif

}

void Perry::Fiber::Schedule()
{
#ifdef FIBER_ENABLED
	this->ParentFiber = nullptr;
	GetThreadPool()->taskQueue.push(this);
#else
	FiberContext fc;
	function(fc);
#endif
}

void Perry::Fiber::SwitchToFiber()
{
#ifdef FIBER_ENABLED
	ASSERT(MyFiber)
	::SwitchToFiber(MyFiber);
#else

#endif
}

void Perry::Fiber::SwitchBack()
{
#ifdef FIBER_ENABLED
	if (ParentFiber)
		::SwitchToFiber(ParentFiber);

	if (ThreadFiber)
		::SwitchToFiber(ThreadFiber);

	ASSERT(LOG_FIBER, "Failed to switch back ?");
#endif

}
Perry::Fiber::Fiber()
{
	MyFiber= nullptr;
	ParentFiber = nullptr;
	Context = nullptr;
}

Perry::Fiber::Fiber(const std::function<void(FiberContext&)>&& func, void* context) :
	Context(context), function(std::move(func))
{
	ParentFiber = nullptr;
	MyFiber = CreateFiber(0, Fiber::FiberExec, this);
	ASSERT_MSG(LOG_FIBER, MyFiber, "Failed to create a fiber...");
	PROFILER_FIBER_REGISTER(MyFiber, "UNKOWN");
}
Perry::Fiber::Fiber(void* fiber) : MyFiber(fiber)
{
	ParentFiber = nullptr;
	Context = nullptr;
	PROFILER_FIBER_REGISTER(MyFiber, "UNKOWN");
}
Perry::Fiber::~Fiber()
{
	//TODO these objects are trivially copyable, we need to detect when a fiber actually gets deleted
}

void Perry::Fiber::FiberExec(void* data)
{
	Fiber* f = static_cast<Fiber*>(data);

	FiberContext context = { f };

	if(!f->function)
		f->SwitchBack();

	f->function(context);
	f->Complete = true;
	f->SwitchBack();
}

Perry::ThreadPool::ThreadPool()
{
	for (int i = 0; i < 2/*std::thread::hardware_concurrency()*/; ++i)
	{
		threads.emplace_back(&ThreadPool::FiberJobThread, this);
	}
}

bool Perry::ThreadPool::RunJob()
{
	Fiber* jobFiber{ nullptr };

	if (!taskQueue.try_pop(jobFiber))
	{
		return false;
	}
	jobFiber->ThreadFiber = GetCurrentFiber();
	jobFiber->SwitchToFiber();
	return true;
}

void Perry::ThreadPool::FiberJobThread()
{
	Perry::Fiber rootFiber = Perry::ThreadToFiber(nullptr);

	while (!stop)
	{
		if (!RunJob())
		{
			Sleep(1000); // giver other system threads a chance ... this shouldn't happen to often tho..

			LOG(LOG_FIBER, "no fiber work found");
			continue;
		}
	}
}

Perry::ThreadPool* Perry::GetThreadPool()
{
	static ThreadPool pool{};

	return &pool;
}
Perry::Fiber Perry::ThreadToFiber(void* context)
{
	return Fiber(::ConvertThreadToFiber(context));
}