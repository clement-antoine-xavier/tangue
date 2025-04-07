/**
 * @file environment_manager.hpp
 * @brief Provides the EnvironmentManager class for managing environment variables.
 * 
 * This header defines the EnvironmentManager class, which is a singleton
 * responsible for managing environment variables in a thread-safe manner.
 * It allows retrieving, setting, and unsetting environment variables.
 */

#pragma once

#include <mutex>
#include <map>
#include <string>

namespace tangue
{
    /**
     * @class EnvironmentManager
     * @brief Singleton class for managing environment variables.
     * 
     * The EnvironmentManager class provides methods to access and modify
     * environment variables. It ensures thread safety and supports initialization
     * with environment variables passed as an array of strings.
     */
    class EnvironmentManager
    {
    public:
        /**
         * @brief Retrieves the singleton instance of EnvironmentManager.
         * 
         * This method initializes the EnvironmentManager with the provided
         * environment variables if it has not been initialized yet.
         * 
         * @param envp Array of environment variables (key=value format).
         * @return Reference to the singleton instance of EnvironmentManager.
         */
        static EnvironmentManager &getInstance(const char *envp[]);

        /**
         * @brief Retrieves the singleton instance of EnvironmentManager.
         * 
         * This method returns the already initialized EnvironmentManager instance.
         * If the instance has not been initialized, behavior is undefined.
         * 
         * @return Reference to the singleton instance of EnvironmentManager.
         */
        static EnvironmentManager &getInstance(void);

        /**
         * @brief Retrieves the value of an environment variable.
         * 
         * This method fetches the value associated with the given key. If the key
         * does not exist, the provided default value is returned.
         * 
         * @param key The name of the environment variable to retrieve.
         * @param defaultValue The value to return if the key does not exist.
         * @return The value of the environment variable or the default value.
         */
        std::string get(const std::string &key, const std::string &defaultValue = "") const;

        /**
         * @brief Sets the value of an environment variable.
         * 
         * This method associates the given key with the specified value. If the key
         * already exists, its value is updated.
         * 
         * @param key The name of the environment variable to set.
         * @param value The value to associate with the key.
         */
        void set(const std::string &key, const std::string &value);

        /**
         * @brief Unsets an environment variable.
         * 
         * This method removes the environment variable associated with the given key.
         * If the key does not exist, no action is taken.
         * 
         * @param key The name of the environment variable to unset.
         */
        void unset(const std::string &key);

    private:
        /**
         * @brief Constructs an EnvironmentManager instance.
         * 
         * This constructor initializes the EnvironmentManager with the provided
         * environment variables.
         * 
         * @param envp Array of environment variables (key=value format).
         */
        EnvironmentManager(const char *envp[]);

        /**
         * @brief Destructor for EnvironmentManager.
         * 
         * Cleans up resources used by the EnvironmentManager instance.
         */
        ~EnvironmentManager() = default;

        /**
         * @brief Deleted copy constructor.
         * 
         * Prevents copying of the EnvironmentManager instance.
         */
        EnvironmentManager(const EnvironmentManager &) = delete;

        /**
         * @brief Deleted copy assignment operator.
         * 
         * Prevents assignment to the EnvironmentManager instance.
         */
        EnvironmentManager &operator=(const EnvironmentManager &) = delete;

        /**
         * @brief Pointer to the singleton instance of EnvironmentManager.
         */
        static inline EnvironmentManager *instancePtr = nullptr;

        /**
         * @brief Indicates whether the EnvironmentManager has been initialized.
         */
        static inline bool initialized = false;

        /**
         * @brief Stores the environment variables as key-value pairs.
         */
        std::map<std::string, std::string> envVars;

        /**
         * @brief Mutex for ensuring thread-safe access to environment variables.
         */
        mutable std::mutex mutex_;
    };
} // namespace tangue