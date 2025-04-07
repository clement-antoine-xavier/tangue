#include <stdexcept>

#include "argument_manager.hpp"

tangue::ArgumentManager &tangue::ArgumentManager::getInstance(const int argc, const char *argv[])
{
    static ArgumentManager instance(argc, argv);
    return instance;
}

tangue::ArgumentManager &tangue::ArgumentManager::getInstance()
{
    if (!initialized)
    {
        throw std::runtime_error("ArgumentManager not initialized. Call getInstance(argc, argv) first.");
    }
    return *instancePtr;
}

tangue::ArgumentManager::ArgumentManager(const int argc, const char *argv[])
{
    initialized = true;
    instancePtr = this;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg(argv[i]);

        if (arg.starts_with("--"))
        {
            auto eqPos = arg.find('=');
            if (eqPos != std::string::npos)
            {
                std::string key = arg.substr(2, eqPos - 2);
                std::string value = arg.substr(eqPos + 1);
                arguments[key] = value;
            }
            else
            {
                std::string key = arg.substr(2);
                arguments[key] = "true"; // flag with no value
            }
        }
    }
}

std::string tangue::ArgumentManager::get(const std::string &key, const std::string &defaultValue) const
{
    auto it = arguments.find(key);
    return it != arguments.end() ? it->second : defaultValue;
}

bool tangue::ArgumentManager::has(const std::string &key) const
{
    return arguments.find(key) != arguments.end();
}

const std::map<std::string, std::string> &tangue::ArgumentManager::getAll() const
{
    return arguments;
}