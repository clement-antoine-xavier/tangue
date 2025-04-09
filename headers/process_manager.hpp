#pragma once

#include <string>
#include <vector>
#include <set>
#include <mutex>

#include "process.hpp"

namespace tangue
{
    class ProcessManager
    {
    public:
        static ProcessManager &getInstance(void);

        Process spawn(const std::string &program, const std::vector<std::string> &args);
        bool track(const Process &process);
        bool untrack(pid_t pid);
        bool isTracked(pid_t pid) const;
        const std::set<pid_t> &getTrackedPIDs(void) const;

    private:
        ProcessManager(void) = default;
        ~ProcessManager(void) = default;

        ProcessManager(const ProcessManager &) = delete;
        ProcessManager &operator=(const ProcessManager &) = delete;

        std::set<pid_t> trackedPIDs;
        mutable std::mutex mutex_;
    };
} // namespace tangue
