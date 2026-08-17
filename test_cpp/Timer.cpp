#include "Timer.h"

#include <iostream>
#include <utility>

ScopeTimer::ScopeTimer(std::string title)
    : _title(std::move(title)), _start(std::chrono::steady_clock::now())
{
}

ScopeTimer::~ScopeTimer()
{
    const auto elapsed = std::chrono::steady_clock::now() - _start;
    const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);
    std::cout << _title << ": " << milliseconds.count() << " ms\n";
}
