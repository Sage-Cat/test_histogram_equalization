#pragma once
#include <memory>

struct ScopeTimer
{
public:
    explicit ScopeTimer(const char *title = nullptr);
    ~ScopeTimer();

private:
    struct pImpl;
    std::unique_ptr<pImpl> _pImpl; // Using smart pointers for automatic memory management
};
