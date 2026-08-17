#pragma once

#include <chrono>
#include <string>

class ScopeTimer
{
public:
    explicit ScopeTimer(std::string title);
    ~ScopeTimer();

    ScopeTimer(const ScopeTimer &) = delete;
    ScopeTimer &operator=(const ScopeTimer &) = delete;

private:
    std::string _title;
    std::chrono::steady_clock::time_point _start;
};
