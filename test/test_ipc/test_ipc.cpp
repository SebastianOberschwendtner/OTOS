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
/**
 ******************************************************************************
 * @file    test_ipc.cpp
 * @author  SO
 * @version v2.7.3
 * @date    16-September-2021
 * @brief   Unit tests for testing the inter process communication within OTOS.
 ******************************************************************************
 */

// === Includes ===
#include "unity.h"
#include "ipc.h"

/** === Test List ===
 * ✓ global manager for ipc
 * ✓ there exists a maximum number of PIDs
 * ✓ task can register object pointers
 * ▢ task can unregister PID data
 * ▢ manager responds whether a specific PID is available or not.
 * ✓ task can get registered pointer:
 *      ✓ normal pointer when PID is registered
 *      ✓ returns false when PID is not registered yet
*/

// === Mocks ===
constexpr unsigned char PID_1 = 1;
constexpr unsigned char PID_2 = 2;

// === Tests ===
void setUp(void) {
// set stuff up here
};

void tearDown(void) {
// clean stuff up here
};

// /// @brief Test the initialization of the manager
// void test_init(void)
// {
//     // Get the manager object
//     IPC::Manager UUT(IPC::Check::PID<PID_1>());

//     // perform tests
//     TEST_ASSERT_FALSE(UUT.get_error());
// };

/// @brief Test registration of data
void test_register_data(void)
{
    // Get the manager object
    IPC::Manager UUT(IPC::Check::PID<PID_1>());

    // the data to be shared
    unsigned int data = 0;

    // Perform tests
    TEST_ASSERT_EQUAL(Error::Code::None, UUT.register_data(&data));

    // re-registering data should yield in an error
    TEST_ASSERT_EQUAL(Error::Code::IPC_Multiple_Registrations, UUT.register_data(&data));

    // Deregistered PIDs can be assigned again
    UUT.deregister_data();
    TEST_ASSERT_EQUAL(Error::Code::None, UUT.register_data(&data));

    // Registering too much PIDs should result in an error => this is handled when constructing the manager
};

/// @brief Test the use of IPC data
void test_get_data(void)
{
    // Set data to be shared and register it
    unsigned int data = 5;
    IPC::Manager UUT(IPC::Check::PID<PID_1>());
    UUT.deregister_data();
    UUT.register_data(&data);

    // Get the IPC data
    unsigned int* p_data = reinterpret_cast<unsigned int*>(*IPC::Manager::get_data(PID_1));

    // Addresses should match
    TEST_ASSERT_EQUAL(&data, p_data);

    // Test changing the value via the pointer
    *p_data = 42;
    TEST_ASSERT_EQUAL(42, data);

    // Getting unregistered PIDs should return an empty value
    TEST_ASSERT_FALSE(UUT.get_data(PID_2));

    // Other PID should be nullptr
    TEST_ASSERT_EQUAL(0, UUT.get_data(PID_2).value_or<void*>(0));
};

// === Main ===
int main(int argc, char** argv)
{
    UNITY_BEGIN();
    // test_init();
    RUN_TEST(test_register_data);
    RUN_TEST(test_get_data);
    return UNITY_END();
};
