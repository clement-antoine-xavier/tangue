#pragma once

#include <string>
#include <map>
#include <mutex>

namespace tangue
{
    class EnvironmentManager
    {
    public:
        static EnvironmentManager &getInstance(const char *envp[]);
        static EnvironmentManager &getInstance(void);

        std::string get(const std::string &key, const std::string &defaultValue = "") const;
        void set(const std::string &key, const std::string &value);
        void unset(const std::string &key);

    private:
        EnvironmentManager(const char *envp[]);
        ~EnvironmentManager() = default;

        EnvironmentManager(const EnvironmentManager &) = delete;
        EnvironmentManager &operator=(const EnvironmentManager &) = delete;

        static inline EnvironmentManager *instancePtr = nullptr;
        static inline bool initialized = false;

        std::map<std::string, std::string> envVars;
        mutable std::mutex mutex_;
    };
} // namespace tangue