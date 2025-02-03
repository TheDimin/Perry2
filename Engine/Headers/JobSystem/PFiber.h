#pragma once

#define SLEEP_FIBER()... //TODO fiber should be put to sleep

namespace Perry
{
	class Fiber;
	//TODO do we realy want to pass this stuff around like this ? we have to consider if we have enough if we pass the Fiber itsellf along
	struct FiberContext
	{
		Fiber* fiber;
	};

	class Fiber
	{
	public:
		//Run this fiber (at some point) and then return to the caller
		void Execute();
		//Run this, but don't await the result
		void Schedule();

		//Switch current fiber to execute this fiber
		void SwitchToFiber();
		void SwitchBack();

		Fiber();
		Fiber(const std::function<void(FiberContext&)>&& func, void* context);
		Fiber(void* fiber);
		~Fiber();

		void* MyFiber = nullptr;// THe fiber we represent
		void* ParentFiber = nullptr;
		void* ThreadFiber = nullptr;
		void* Context = nullptr;
		bool Complete = false;

		std::function<void(FiberContext&)> function = nullptr;

	private:
		friend class ThreadPool;
		static void FiberExec(void* data);

	};


	class ThreadPool
	{
		friend class Fiber;
	public:
		ThreadPool();
		std::vector<std::thread> threads;
		Concurrency::concurrent_queue<Fiber*> taskQueue;
		concurrency::concurrent_queue<Fiber*> RenderFibers;
		bool stop = false;

	private:
		//Return false if there was no job to run
		bool RunJob();
		void FiberJobThread();
	};

	extern ThreadPool* GetThreadPool();

	//Platform function wrappers
	extern Fiber ThreadToFiber(void* context);
} // namespace Perry
