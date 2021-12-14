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
 ==============================================================================
 * @file    test_kernel.c
 * @author  SO
 * @version v1.6.0
 * @date    16-March-2021
 * @brief   Unit tests for the OTOS kernel to be executed on the host.
 ==============================================================================
 */

// *** Includes ***
#include <unity.h>
#include <kernel.h>

// Create UUT
OTOS::Kernel UUT;

void setUp(void) {
// set stuff up here
};

void tearDown(void) {
// clean stuff up here
};

// *** Define Tests ***

/**
 * @brief Test the constructor.
 */
void test_Constructor(void)
{
    // There should be no allocated stack
    TEST_ASSERT_EQUAL( 0, UUT.get_allocated_stacksize() );
};

/**
 * @brief Test the correct allocation of the stack of one thread.
 */
void test_scheduleThread(void)
{
    // Schedule one thread
    UUT.schedule_thread(0, OTOS::Check::StackSize<256>(), OTOS::Priority::Normal);

    // Test the new stack size
    TEST_ASSERT_EQUAL(256, UUT.get_allocated_stacksize());

    // Schedule another thread
    UUT.schedule_thread(0, OTOS::Check::StackSize<256>(), OTOS::Priority::Normal);

    // Test the new stack size
    TEST_ASSERT_EQUAL(2*256, UUT.get_allocated_stacksize());
};

/**
 * @brief Test the ms timer of the kernel.
 */
void test_Time_ms(void)
{
    //The initial time value should be 0
    TEST_ASSERT_EQUAL( 0, UUT.get_time_ms());
    TEST_ASSERT_EQUAL( 0, OTOS::get_time_ms());

    //Increase the count by 1 ms
    UUT.count_time_ms();
    TEST_ASSERT_EQUAL( 1, UUT.get_time_ms());
    TEST_ASSERT_EQUAL( 1, OTOS::get_time_ms());
};

// *** Perform the tests ***
int main(int argc, char** argv)
{
    UNITY_BEGIN();
    test_Constructor();
    test_scheduleThread();
    test_Time_ms();
    UNITY_END();
    return 0;
}
