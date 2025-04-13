#pragma once
#include "Core.h"
#include "UUID.h"

#include <atomic>
#include <condition_variable>
#include <queue>
#include <mutex>

namespace Stulu {
	class STULU_API CpuDispatcher;
	using NativeEventHandle = void*;

	class Task {
	public:
		Task() : m_id(0) {}
		Task(uint64_t id) : m_id(id) {}

		virtual ~Task() = default;

		void Assign(uint64_t id = Stulu::UUID()) {
			m_id = id;
		}
		void Release() {
			// delete this; actually has defined behavior
			if (this)
				delete this;
		}
		bool Valid() const {
			return m_id != 0;
		}

		virtual void Run() = 0;

		virtual const char* GetName() const {
			return "Unnamed Task";
		}

		operator bool() const {
			return Valid();
		}
	private:
		uint64_t m_id;
	};

	class WrappedTask : public Task {
	public:
		WrappedTask(std::function<void()>&& func)
			: Task(Stulu::UUID()), m_func(std::move(func)){}

		virtual void Run() override {
			if (m_func)
				m_func();
		}
	private:
		std::function<void()> m_func;
	};


	class STULU_API WorkerThread {
	public:
		WorkerThread(CpuDispatcher* dispatcher)
			: m_dispatcher(dispatcher) { }

		~WorkerThread() {
			if (m_thread.joinable())
				m_thread.join();
		}

		void Start() {
			if (m_dispatcher) {
				m_thread = std::thread(&WorkerThread::Execute, this);
			}
			else {
				CORE_ERROR("CPU Worker Thread has no disptacher assigned!");
			}
		}

	private:
		void Execute();

		std::thread m_thread;
		CpuDispatcher* m_dispatcher;
	};

	class STULU_API CpuDispatcher {
	public:
		CpuDispatcher(uint64_t threadCount = std::thread::hardware_concurrency());
		~CpuDispatcher();

		void Dispatch(Task* task);
		void Dispatch(std::function<void()> function) {
			Dispatch(new WrappedTask(std::move(function)));
		}
		template<class T, typename ... Args>
		void DispatchTask(Args&& ... args) {
			Dispatch(new T(std::forward<Args>(args)...));
		}

		void ResetWakeup() const;
		void Wakeup() const;
		void Sleep() const;

		bool FetchTask(Task*& outTask);
		static void ExecuteTask(Task* task);

		size_t ThreadCount() const { return m_workers.size(); }
		std::queue<Task*>& GetTasks() { return m_tasks; }
		std::mutex& GetTaskMutex() { return m_taskMutex; }
		NativeEventHandle GetWakeEvent() const { return m_wakeupEvent; }
		bool IsAlive() const { return m_running.load(); }
	private:
		std::vector<WorkerThread*> m_workers;
		std::queue<Task*> m_tasks;
		std::mutex m_taskMutex;
		std::atomic<bool> m_running;
		NativeEventHandle m_wakeupEvent;

		void PushTask(Task* task);
	};
}