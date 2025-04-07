#pragma once

#include <string>
#include <vector>
#include <map>
#include <mutex>

namespace tangue
{
    class ArgumentManager
    {
    public:
        static ArgumentManager &getInstance(const int argc, const char *argv[]);
        static ArgumentManager &getInstance();

        std::string get(const std::string &key, const std::string &defaultValue = "") const;
        bool has(const std::string &key) const;
        const std::map<std::string, std::string> &getAll() const;

    private:
        ArgumentManager(const int argc, const char *argv[]);
        ~ArgumentManager() = default;

        ArgumentManager(const ArgumentManager &) = delete;
        ArgumentManager &operator=(const ArgumentManager &) = delete;

        static inline ArgumentManager *instancePtr = nullptr;
        static inline bool initialized = false;

        std::map<std::string, std::string> arguments;
        mutable std::mutex mutex_;
    };
} // namespace tangue
