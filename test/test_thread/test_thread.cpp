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
 * @file    test_thread.c
 * @author  SO
 * @version v1.0.0
 * @date    16-March-2021
 * @brief   Unit tests for the thread handler of OTOS.
 ==============================================================================
 */

// *** Includes ***
#include <array>
#include <unity.h>
#include <thread.h>

// Create UUT
OTOS::Thread UUT;

// Mock Stack Memory
std::array<u_base_t, 256> LocalStack;

void setUp(void) {
// set stuff up here
};

void tearDown(void) {
// clean stuff up here
};

// *** Define Tests ***

/**
 * @brief Test the correct behavior of the cosntructor
 */
void test_Constructor(void)
{
    // Test the constructor
    TEST_ASSERT_EQUAL( 0, UUT.getStackSize() );
    TEST_ASSERT_EQUAL( 0, UUT.StackPointer );
};

/**
 * @brief Test setting the stack data
 */
void test_SetStack(void)
{
    // Set the stack to the local stack with size 50
    UUT.setStack(LocalStack.end(), 50);
    TEST_ASSERT_EQUAL( LocalStack.end(), UUT.StackPointer);
    TEST_ASSERT_EQUAL( 50, UUT.getStackSize() );
    TEST_ASSERT_FALSE( UUT.getStackOverflow() );
};

/**
 * @brief Test the stack overflow warning.
 */
void test_StackOverflow(void)
{
    // Set the stack to the local stack with size 50
    UUT.setStack(LocalStack.end(), 50);
    
    // After initializing the stack, no overflow should occur
    TEST_ASSERT_FALSE( UUT.getStackOverflow() );

    // Increase the stack pointer of the thread just before its limit
    // -> No overflow should occur now.
    UUT.StackPointer = LocalStack.end() - 49;
    TEST_ASSERT_FALSE( UUT.getStackOverflow() );

    // Increase the stack pointer of the thread exactly to its limit
    // -> An overflow should occur now.
    UUT.StackPointer = LocalStack.end() - 50;
    TEST_ASSERT_TRUE( UUT.getStackOverflow() );

    // Increase the stack pointer of the thread beyond its limit
    // -> An overflow should occur now.
    UUT.StackPointer = LocalStack.end() - 51;
    TEST_ASSERT_TRUE( UUT.getStackOverflow() );

};

// *** Perform the tests *** 
int main(int argc, char** argv)
{
    UNITY_BEGIN();
    test_Constructor();
    test_SetStack();
    test_StackOverflow();
    UNITY_END();
    return 0;
};
