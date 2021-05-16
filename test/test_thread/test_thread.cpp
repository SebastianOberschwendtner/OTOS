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
 * @file    test_thread.c
 * @author  SO
 * @version v1.0.0
 * @date    16-March-2021
 * @brief   Unit tests for the thread handler of OTOS.
 ******************************************************************************
 */

// ****** Includes ******
#include <unity.h>
#include <thread.h>

// Create UUT
OTOS::Thread UUT;

void setUp(void) {
// set stuff up here
};

void tearDown(void) {
// clean stuff up here
};

// ****** Define Tests ******

/**
 * @brief Test the correct behavior of the cosntructor
 */
void test_Constructor(void)
{
    // Test constructor of super class
    TEST_ASSERT_EQUAL( OTOS::PrioLow, UUT.ThreadPriority );
    TEST_ASSERT_EQUAL( 0, UUT.TickSchedule );
    TEST_ASSERT_EQUAL( 0, UUT.TickCounter );

    // Test the constructor of sub class
    TEST_ASSERT_EQUAL( 0, UUT.StackSize);
    TEST_ASSERT_NULL( UUT.StackPointer );
    TEST_ASSERT_NULL( UUT.StackTop );
};

/**
 * @brief Test the stack overflow warning.
 */
void test_StackOverflow(void)
{
    TEST_ASSERT_FALSE( UUT.StackOverflow() );
};

int main(int argc, char** argv)
{
    UNITY_BEGIN();
    test_Constructor();
    test_StackOverflow();
    UNITY_END();
    return 0;
};
