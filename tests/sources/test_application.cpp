#include "test_application.hpp"

namespace tangue
{
    namespace tests
    {

        TEST_F(TestApplication, DefaultConstructor)
        {
            Application application;
            SUCCEED();
        }

    } // namespace tests
} // namespace tangue