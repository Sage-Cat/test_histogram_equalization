#include "Timer.h"

#include <chrono>
#include <iostream>

struct ScopeTimer::pImpl
{
public:
    explicit pImpl(const char *title)
        : _title(title),
          _start(std::chrono::high_resolution_clock::now())
    { // Initialize start time on construction
    }

    ~pImpl()
    {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - _start);
        std::cout << (_title ? _title : "Time elapsed") << ": " << duration.count() << " ms\n";
    }

private:
    std::chrono::high_resolution_clock::time_point _start;
    const char *_title;
};

ScopeTimer::ScopeTimer(const char *title)
    : _pImpl(std::make_unique<pImpl>(title))
{ // Using make_unique for safer and more concise memory allocation
}

ScopeTimer::~ScopeTimer() = default; // Destructor is simplified due to smart pointer usage
