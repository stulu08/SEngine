#pragma once
#include <Stulu.h>
template<typename Fn>
class Timer {
public:
	Timer(const char* name, Fn&& f)
		: m_name(name), m_stopped(false), m_f(f) {
		m_startTimepoint = std::chrono::high_resolution_clock::now();
	}
	~Timer() {
		if (!m_stopped)
			stop();
	}
	void stop() {
		auto endTimePoint = std::chrono::high_resolution_clock::now();
		long long start = std::chrono::time_point_cast<std::chrono::microseconds>(m_startTimepoint).time_since_epoch().count();
		long long end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch().count();
		m_stopped = true;
		float duration = (end - start) * 0.001;
		m_f({ m_name, duration });
	}
private:
	const char* m_name;
	std::chrono::time_point<std::chrono::steady_clock> m_startTimepoint;
	bool m_stopped;
	Fn m_f;
};

class TestProject : public Stulu::Application {
public:
	TestProject();
};

