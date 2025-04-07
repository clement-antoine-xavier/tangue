#include <iostream>
#include <memory>

#include <argument_manager.hpp>
#include "environment_manager.hpp"
#include "tangue.hpp"

int main(const int argc, const char *argv[], const char *envp[])
{
    tangue::ArgumentManager::getInstance(argc, argv);
    tangue::EnvironmentManager::getInstance(envp);
    std::unique_ptr<tangue::Tangue> tangue = nullptr;

    try
    {
        tangue = std::make_unique<tangue::Tangue>();
    }
    catch (const std::exception &exception)
    {
        std::cerr << "Error: " << exception.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
