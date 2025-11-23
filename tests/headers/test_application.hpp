#pragma once

#include <gtest/gtest.h>

#include "application.hpp"

namespace tangue
{
    namespace tests
    {

        class TestApplication : public testing::Test
        {
        protected:
            void SetUp() override
            {
            }

            void TearDown() override
            {
            }
        };
    } // namespace tests
} // namespace tangue