#include "st_pch.h"
#include "CpuDispatcher.h"

namespace Stulu {
	CpuDispatcher::CpuDispatcher(uint64_t threadCount)
		: m_running(true) {

		if (threadCount > 1) {
			m_wakeupEvent = CreateEventA(NULL, TRUE, FALSE, NULL);

			for (uint64_t i = 0; i < threadCount; ++i) {
				m_workers.push_back(new WorkerThread(this));
				m_workers[i]->Start();
			}
		}
	}
	CpuDispatcher::~CpuDispatcher() {
		m_running = false;
		ResetWakeup();

		for (WorkerThread* worker : m_workers) {
			delete worker;
		}

		CloseHandle(m_wakeupEvent);
	}

	void CpuDispatcher::Dispatch(Task* task) {
		if (!task)
			return;

		if (ThreadCount() > 1) {
			PushTask(task);
			Wakeup();
		}
		else {
			ExecuteTask(task);
		}
	}

	void CpuDispatcher::ResetWakeup() const {
		ResetEvent(m_wakeupEvent);

		if (!m_running)
			Wakeup();
	}
	void CpuDispatcher::Wakeup() const {
		SetEvent(m_wakeupEvent);
	}
	void CpuDispatcher::Sleep() const {
		WaitForSingleObject(m_wakeupEvent, INFINITE);
	}

	bool CpuDispatcher::FetchTask(Task*& outTask) {
		std::lock_guard<std::mutex> lock(m_taskMutex);

		if (!m_tasks.empty()) {
			outTask = m_tasks.front();
			m_tasks.pop();
			return true;
		}
	
		return false;
	}

	void CpuDispatcher::ExecuteTask(Task* task) {
		if (task && task->Valid())
			task->Run();

		task->Release();
	}

	void CpuDispatcher::PushTask(Task* task) {
		std::lock_guard<std::mutex> lock(m_taskMutex);
		m_tasks.push(std::move(task));
	}

	void WorkerThread::Execute() {
		while (m_dispatcher->IsAlive()) {
			m_dispatcher->ResetWakeup();

			Task* task;
			if (m_dispatcher->FetchTask(task)) {
				CpuDispatcher::ExecuteTask(task);
			}
			else {
				m_dispatcher->Sleep();
			}
		}
	}
}