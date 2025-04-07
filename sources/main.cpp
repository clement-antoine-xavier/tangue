#include <iostream>
#include <memory>

#include "tangue.hpp"

int main(const int argc, const char const* argv[], const char const* envp[])
{
    std::unique_ptr<tangue::Tangue> tangue = nullptr;

    try
    {
        tangue = std::make_unique<tangue::Tangue>(argc, argv, envp);
    }
    catch (const std::exception &exception)
    {
        std::cerr << "Error: " << exception.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
