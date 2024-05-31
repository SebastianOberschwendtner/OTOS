/**
 * OTOS - Open Tec Operating System
 * Copyright (c) 2021 Sebastian Oberschwendtner, sebastian.oberschwendtner@gmail.com
 *
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#ifndef MOCK_H
#define MOCK_H

// *** Includes ***
#include <stdint.h>
#include <string>
#include <vector>
#include <unity.h>

namespace Mock
{
    
    // === Class defines ===

    // Class for mocking functions and methods
    template<typename return_type = bool>
    class Callable
    {
    public:
        // *** Properties ***
        int32_t call_count{0};
        int32_t last_called_with{0};

        // *** Constructors ***
        // Callable(): call_count(0), last_called_with(0){};
        // Callable(const int32_t count, const arg_type args): call_count(count), last_called_with(args){};

        // *** Operators ***
        /**
         * @brief Call operator which returns a specified value.
         * @return Returns a value defined by the template parameter.
         */
        template<typename T1, typename... Ts>
        return_type operator() (T1 arg, Ts... args) 
        { 
            this->call_count++;
            if constexpr (std::is_pointer<T1>())
                this->last_called_with = static_cast<int>(*arg);
            else
                this->last_called_with = static_cast<int>(arg);
            return return_type{1}; 
        };

        // Call Operator with no arguments
        return_type operator() () 
        { 
            this->call_count++;
            return return_type{1}; 
        };

        // *** Methods ***
        /**
         * @brief Remember the function call
         * @param args The argument the function was called with.
         */
        void add_call(const int32_t args)
        {
            this->call_count++;
            this->last_called_with = args;
        };

        /**
         * @brief Reset the function calls
         */
        void reset(void)
        {
            this->call_count = 0;
            this->last_called_with = 0;
        };

        /**
         * @brief Assert wether the methods was called once.
         * After calling this function, the callable state is resetted!
         * @details Calls the unit TEST_ macros.
         */
        void assert_called_once(void) { 
            // Prepare errors message
            const char *fmt = "Expected to be called once, but was called %d times.";
            int32_t sz = std::snprintf(nullptr, 0, fmt, this->call_count);
            std::vector<char> buf(sz + 1); // note +1 for null terminator
            std::snprintf(&buf[0], buf.size(), fmt, this->call_count);

            // Test the call count
            TEST_ASSERT_TRUE_MESSAGE(this->call_count==1, &buf[0]); 
            // Reset the call count again
            this->call_count = 0;
        };

        /**
         * @brief Assert whether the function was last called with
         * the expected argument.
         * @param Expected The last expected argument
         * @details Calls the unit TEST_ macros.
         */
        void assert_called_last_with(const int32_t Expected) {
            // Prepare errors message
            const char *fmt = "Expected to be called last with ((int) %d), but was called with ((int) %d).";
            int32_t sz = std::snprintf(nullptr, 0, fmt, Expected, this->last_called_with);
            std::vector<char> buf(sz + 1); // note +1 for null terminator
            std::snprintf(&buf[0], buf.size(), fmt, Expected, this->last_called_with);

            // Test the call args
            TEST_ASSERT_TRUE_MESSAGE(Expected == this->last_called_with, &buf[0]); 
            TEST_ASSERT_TRUE_MESSAGE(this->call_count > 0, "Expected to be called at least once, but was called 0 times.");

            // Reset the call arg again
            this->last_called_with = 0;
        };

        /**
         * @brief Assert whether the function was called once with
         * the expected argument.
         * @param Expected The last expected argument
         * @details Calls the unit TEST_ macros.
         */
        void assert_called_once_with(const int32_t Expected) { 
            // Call the corresponding tests.
            this->assert_called_last_with(Expected);
            this->assert_called_once();
        };
    };

    // Base class for mocking registers
    class Peripheral
    {
    public:
        Peripheral(){};
    };
}
#endif