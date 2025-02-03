#pragma once

namespace Perry
{
	enum class TaskType //Orderd on priority !! higher means it will have priority
	{
		Game,
		Graphics
	};

	//typedef void (*Jobsignature)();

	//TODO: Task system2.0
	//void ExampleUsage()
	//{
	//	Job engineInit = Job("EngineInit");
	//	engineinit.Task([]() { /*Do something*/ }, JobType::Graphics);
	//	engineinit.Task([]() { /*Do something*/ });
	//	engineinit.Task([]() { /*Do something*/ });
	//	engineinit.Task([]() { /*Do something*/ })
	//		.ContinueWith([]() {});

	//	SchedulaJob(engineInit);
	//}

	class Task
	{
	public:
		Task(const std::string& name, const std::function<void()>&& task, TaskType type = TaskType::Game) :
			Name(name), TaskToExecute(std::move(task)), Type(type)
		{
		}


		Task& OnComplete(const Task&& task);

		const std::string& GetName() const { return Name; }
	private:
		friend class Job;
		friend class JobSystem;
		std::string Name;
		TaskType Type = TaskType::Game;
		std::function<void()> TaskToExecute;
		class Job* invoker;
		bool IsComplete = false;
	};

	class Job
	{
	public:
		Job(const std::string& name) : Name(name) {}
		Job(const Task& task) : Name(task.GetName()) {};

		void Assign(const Task&& task);
		void Execute();

	private:
		void CompleteTask(Task*)const;

		std::function<void()> OnComplete;
		std::string Name;
		std::vector<Task*> Tasks;
	};

	/*class JobBuilder
	{
	public:
		void ExecuteTask(Task&& t);
		void AttachTask(Task&& t);

		void AwaitTasks();

		std::vector<Task> TaskList;

	private:
		friend class Task;
		void TaskComplete(Task* task);
	};*/

	struct JobThread
	{
		JobThread();
		~JobThread();
		void AwaitTaskLoop();

		std::thread thread;
		std::vector<Concurrency::concurrent_queue<Task*>> localJobs;
		bool active = true;
	};

	class JobSystem
	{
	public:
		JobSystem();

		void RegisterTask(Task* task);
	private:
		friend JobSystem* GetJobSystem();
		Task* ReceiveTask();

		std::map<TaskType, Concurrency::concurrent_queue<Task*>> Tasks{};
		std::vector<JobThread> Threads;
	};
}

static Perry::JobSystem* GetJobSystem();