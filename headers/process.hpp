/**
 * @file Process.hpp
 * @brief Defines the Process class for managing system processes.
 */

#pragma once

#include <string>
#include <vector>
#include <sys/types.h>

namespace tangue
{
    /**
     * @class Process
     * @brief A class to encapsulate operations on system processes.
     */
    class Process
    {
    public:
        /**
         * @brief Default constructor for the Process class.
         */
        Process() = default;

        /**
         * @brief Constructs a Process object with a specific process ID (PID).
         * @param pid The process ID to associate with this Process object.
         */
        explicit Process(pid_t pid);

        /**
         * @brief Retrieves the process ID (PID) associated with this Process object.
         * @return The process ID.
         */
        pid_t getPID(void) const;

        /**
         * @brief Checks if the process is currently running.
         * @return True if the process is running, false otherwise.
         */
        bool isRunning(void) const;

        /**
         * @brief Sends a signal to terminate the process.
         * @param signal The signal to send to the process (default is SIGTERM).
         * @return True if the signal was successfully sent, false otherwise.
         */
        bool kill(int signal = SIGTERM) const;

        /**
         * @brief Waits for the process to terminate.
         * @param noHang If true, the function will not block and will return immediately.
         * @return The exit status of the process, or -1 if an error occurred.
         */
        int wait(bool noHang = false) const;

    private:
        /**
         * @brief The process ID (PID) associated with this Process object.
         *        Defaults to -1, indicating no process is associated.
         */
        pid_t pid_ = -1;
    };
} // namespace tangue
