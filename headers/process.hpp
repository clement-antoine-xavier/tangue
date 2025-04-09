#pragma once

#include <string>
#include <vector>
#include <sys/types.h>

namespace tangue
{
    class Process
    {
    public:
        Process() = default;
        explicit Process(pid_t pid);

        pid_t getPID(void) const;
        bool isRunning(void) const;
        bool kill(int signal = SIGTERM) const;
        int wait(bool noHang = false) const;

    private:
        pid_t pid_ = -1;
    };
} // namespace tangue
