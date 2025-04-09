#include <iostream>
#include <cstring>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "process_manager.hpp"

tangue::ProcessManager& tangue::ProcessManager::getInstance() {
    static tangue::ProcessManager instance;
    return instance;
}

tangue::Process tangue::ProcessManager::spawn(const std::string& program, const std::vector<std::string>& args) {
    pid_t pid = fork();

    if (pid == 0) {
        // In child process
        std::vector<char*> execArgs;
        execArgs.push_back(const_cast<char*>(program.c_str()));
        for (const auto& arg : args) {
            execArgs.push_back(const_cast<char*>(arg.c_str()));
        }
        execArgs.push_back(nullptr);

        execvp(program.c_str(), execArgs.data());
        std::cerr << "Failed to exec " << program << ": " << strerror(errno) << std::endl;
        _exit(EXIT_FAILURE);
    } else if (pid > 0) {
        // In parent process
        std::lock_guard<std::mutex> lock(mutex_);
        trackedPIDs.insert(pid);
        return Process(pid);
    } else {
        // Fork failed
        std::cerr << "Fork failed: " << strerror(errno) << std::endl;
        return Process(); // Invalid process
    }
}

bool tangue::ProcessManager::track(const Process& process) {
    if (process.getPID() <= 0) return false;

    std::lock_guard<std::mutex> lock(mutex_);
    return trackedPIDs.insert(process.getPID()).second;
}

bool tangue::ProcessManager::untrack(pid_t pid) {
    std::lock_guard<std::mutex> lock(mutex_);
    return trackedPIDs.erase(pid) > 0;
}

bool tangue::ProcessManager::isTracked(pid_t pid) const {
    std::lock_guard<std::mutex> lock(mutex_);
    return trackedPIDs.contains(pid);
}

const std::set<pid_t>& tangue::ProcessManager::getTrackedPIDs() const {
    return trackedPIDs;
}