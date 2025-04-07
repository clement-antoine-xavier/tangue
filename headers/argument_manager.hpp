/**
 * @file argument_manager.hpp
 * @brief Declaration of the ArgumentManager class for managing command-line arguments.
 * 
 * This file contains the definition of the ArgumentManager class, which provides
 * a singleton interface for parsing and accessing command-line arguments. It allows
 * retrieval of arguments by key, checking for the existence of keys, and accessing
 * all parsed arguments.
 */

#pragma once

#include <string>
#include <vector>
#include <map>
#include <mutex>

namespace tangue
{
    /**
     * @class ArgumentManager
     * @brief A singleton class for managing command-line arguments.
     * 
     * The ArgumentManager class provides a thread-safe interface for parsing and
     * accessing command-line arguments. It supports retrieving arguments by key,
     * checking for the existence of keys, and accessing all parsed arguments.
     */
    class ArgumentManager
    {
    public:
        /**
         * @brief Retrieves the singleton instance of ArgumentManager, initializing it with arguments.
         * 
         * This method initializes the ArgumentManager instance with the provided
         * command-line arguments if it has not been initialized yet.
         * 
         * @param argc The number of command-line arguments.
         * @param argv The array of command-line arguments.
         * @return Reference to the singleton instance of ArgumentManager.
         */
        static ArgumentManager &getInstance(const int argc, const char *argv[]);

        /**
         * @brief Retrieves the singleton instance of ArgumentManager.
         * 
         * This method returns the already initialized ArgumentManager instance.
         * If the instance has not been initialized, behavior is undefined.
         * 
         * @return Reference to the singleton instance of ArgumentManager.
         */
        static ArgumentManager &getInstance();

        /**
         * @brief Retrieves the value of a command-line argument by key.
         * 
         * This method retrieves the value associated with the specified key.
         * If the key does not exist, the provided default value is returned.
         * 
         * @param key The key of the argument to retrieve.
         * @param defaultValue The value to return if the key does not exist (default is an empty string).
         * @return The value associated with the key, or the default value if the key does not exist.
         */
        std::string get(const std::string &key, const std::string &defaultValue = "") const;

        /**
         * @brief Checks if a command-line argument with the specified key exists.
         * 
         * This method checks whether an argument with the given key exists in the parsed arguments.
         * 
         * @param key The key to check for existence.
         * @return True if the key exists, false otherwise.
         */
        bool has(const std::string &key) const;

        /**
         * @brief Retrieves all parsed command-line arguments.
         * 
         * This method returns a constant reference to the map containing all parsed
         * command-line arguments as key-value pairs.
         * 
         * @return A constant reference to the map of all parsed arguments.
         */
        const std::map<std::string, std::string> &getAll() const;

    private:
        /**
         * @brief Constructs an ArgumentManager instance and parses the command-line arguments.
         * 
         * This constructor initializes the ArgumentManager instance and parses the
         * provided command-line arguments into key-value pairs.
         * 
         * @param argc The number of command-line arguments.
         * @param argv The array of command-line arguments.
         */
        ArgumentManager(const int argc, const char *argv[]);

        /**
         * @brief Default destructor for ArgumentManager.
         */
        ~ArgumentManager() = default;

        /**
         * @brief Deleted copy constructor to enforce singleton pattern.
         */
        ArgumentManager(const ArgumentManager &) = delete;

        /**
         * @brief Deleted copy assignment operator to enforce singleton pattern.
         */
        ArgumentManager &operator=(const ArgumentManager &) = delete;

        static inline ArgumentManager *instancePtr = nullptr; ///< Pointer to the singleton instance.
        static inline bool initialized = false; ///< Flag indicating whether the instance has been initialized.

        std::map<std::string, std::string> arguments; ///< Map of parsed command-line arguments.
        mutable std::mutex mutex_; ///< Mutex for thread-safe access to arguments.
    };
} // namespace tangue
