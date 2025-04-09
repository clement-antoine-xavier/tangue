#include <csignal>

#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

#include "process.hpp"

tangue::Process::Process(pid_t pid) : pid_(pid) {}

pid_t tangue::Process::getPID() const
{
    return pid_;
}

bool tangue::Process::isRunning() const
{
    if (pid_ <= 0)
        return false;
    return kill(pid_) == 0;
}

bool tangue::Process::kill(int signal) const
{
    if (pid_ <= 0)
        return false;
    return ::kill(pid_, signal) == 0;
}

int tangue::Process::wait(bool noHang) const
{
    int status = 0;
    int options = noHang ? WNOHANG : 0;

    if (pid_ <= 0)
        return -1;
    return waitpid(pid_, &status, options);
}