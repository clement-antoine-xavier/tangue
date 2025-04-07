#include <cstdlib>
#include <stdexcept>

#include "environment_manager.hpp"

tangue::EnvironmentManager &tangue::EnvironmentManager::getInstance(const char *envp[])
{
    static EnvironmentManager instance(envp);
    return instance;
}

tangue::EnvironmentManager &tangue::EnvironmentManager::getInstance()
{
    if (!initialized)
    {
        throw std::runtime_error("EnvironmentManager not initialized. Call getInstance(envp) first.");
    }
    return *instancePtr;
}

tangue::EnvironmentManager::EnvironmentManager(const char *envp[])
{
    initialized = true;
    instancePtr = this;

    for (int i = 0; envp[i] != nullptr; ++i)
    {
        std::string entry(envp[i]);
        auto pos = entry.find('=');
        if (pos != std::string::npos)
        {
            std::string key = entry.substr(0, pos);
            std::string value = entry.substr(pos + 1);
            envVars[key] = value;
        }
    }
}

std::string tangue::EnvironmentManager::get(const std::string &key, const std::string &defaultValue) const
{
    auto it = envVars.find(key);
    return it != envVars.end() ? it->second : defaultValue;
}

void tangue::EnvironmentManager::set(const std::string &key, const std::string &value)
{
    std::lock_guard<std::mutex> lock(mutex_);
    envVars[key] = value;
    setenv(key.c_str(), value.c_str(), 1);
}

void tangue::EnvironmentManager::unset(const std::string &key)
{
    std::lock_guard<std::mutex> lock(mutex_);
    envVars.erase(key);
    unsetenv(key.c_str());
}