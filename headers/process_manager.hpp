/**
 * @file process_manager.hpp
 * @brief Defines the ProcessManager class for managing and tracking processes.
 */

#pragma once

#include <string>
#include <vector>
#include <set>
#include <mutex>

#include "process.hpp"

namespace tangue
{
    /**
     * @class ProcessManager
     * @brief Singleton class responsible for managing and tracking processes.
     *
     * The ProcessManager class provides functionality to spawn, track, and untrack
     * processes. It ensures thread-safe access to the set of tracked process IDs.
     */
    class ProcessManager
    {
    public:
        /**
         * @brief Retrieves the singleton instance of the ProcessManager.
         * @return Reference to the singleton ProcessManager instance.
         */
        static ProcessManager &getInstance(void);

        /**
         * @brief Spawns a new process with the specified program and arguments.
         * @param program The path to the program to execute.
         * @param args A vector of arguments to pass to the program.
         * @return A Process object representing the spawned process.
         */
        Process spawn(const std::string &program, const std::vector<std::string> &args);

        /**
         * @brief Tracks a process by adding its PID to the tracked set.
         * @param process The Process object to track.
         * @return True if the process was successfully tracked, false otherwise.
         */
        bool track(const Process &process);

        /**
         * @brief Untracks a process by removing its PID from the tracked set.
         * @param pid The PID of the process to untrack.
         * @return True if the process was successfully untracked, false otherwise.
         */
        bool untrack(pid_t pid);

        /**
         * @brief Checks if a process with the specified PID is being tracked.
         * @param pid The PID of the process to check.
         * @return True if the process is being tracked, false otherwise.
         */
        bool isTracked(pid_t pid) const;

        /**
         * @brief Retrieves the set of currently tracked process IDs.
         * @return A const reference to the set of tracked PIDs.
         */
        const std::set<pid_t> &getTrackedPIDs(void) const;

    private:
        /**
         * @brief Default constructor. Private to enforce singleton pattern.
         */
        ProcessManager(void) = default;

        /**
         * @brief Destructor. Private to enforce singleton pattern.
         */
        ~ProcessManager(void) = default;

        /**
         * @brief Deleted copy constructor to prevent copying.
         */
        ProcessManager(const ProcessManager &) = delete;

        /**
         * @brief Deleted copy assignment operator to prevent copying.
         */
        ProcessManager &operator=(const ProcessManager &) = delete;

        /**
         * @brief Set of tracked process IDs.
         */
        std::set<pid_t> trackedPIDs;

        /**
         * @brief Mutex for thread-safe access to the trackedPIDs set.
         */
        mutable std::mutex mutex_;
    };
} // namespace tangue
