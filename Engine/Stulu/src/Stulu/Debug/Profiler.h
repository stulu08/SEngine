#pragma once
#include "Stulu/Core/Core.h"
#include <chrono>
#include <string>
#include <unordered_map>

class STULU_API Profiler {
public:
    static Profiler& GetInstance();

    inline void SaveTime(const char* name, std::chrono::high_resolution_clock::time_point startPoint) {
        auto endTime = std::chrono::high_resolution_clock::now();
        frameTimes[name] = std::chrono::duration<double, std::milli>(endTime - startPoint).count();
    }

    auto begin() { return frameTimes.begin(); }
    auto end() { return frameTimes.end(); }
    auto begin() const { return frameTimes.begin(); }
    auto end() const { return frameTimes.end(); }

    inline void Refresh() {
        frameTimes.clear();
    }
private:
    std::unordered_map<const char*, double> frameTimes;
    static Profiler s_instance;
    Profiler() = default;
};

class ProfilerScope {
public:
    inline ProfilerScope(const char* name) : m_name(name) {
        m_start = std::chrono::high_resolution_clock::now();
    }

    ~ProfilerScope() {
        Profiler::GetInstance().SaveTime(m_name, m_start);
    }

private:
    std::chrono::high_resolution_clock::time_point m_start;
    const char* m_name;
};

#define ST_PROFILING_SCOPE(name) ProfilerScope ST_CONCAT(st_profiler_timer,__LINE__)(name)