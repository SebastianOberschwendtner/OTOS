/**
 * OTOS - Open Tec Operating System
 * Copyright (c) 2021 - 2024 Sebastian Oberschwendtner, sebastian.oberschwendtner@gmail.com
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
 * @file    test_thread.cpp
 * @author  SO
 * @version v5.0.0
 * @date    16-March-2021
 * @brief   Unit tests for the thread handler of OTOS.
 ==============================================================================
 */

/* === Includes === */
#include <array>
#include <unity.h>
#include <mock.h>
#include <thread.h>

/* Mock Stack Memory */
std::array<u_base_t, 256> LocalStack;

void setUp() {
/* set stuff up here */
};

void tearDown() {
/* clean stuff up here */
};

/* === Define Tests === */

/**
 * @brief Test the correct behavior of the constructor.
 */
void test_Constructor()
{
    /* Create UUT */
    OTOS::Thread UUT;

    /* Test the constructor */
    TEST_ASSERT_EQUAL( 0, UUT.get_stacksize() );
    TEST_ASSERT_EQUAL( 0, UUT.Stack_pointer );
};

/**
 * @brief Test setting the stack data.
 */
void test_SetStack()
{
    /* Create UUT */
    OTOS::Thread UUT;

    /* Set the stack to the local stack with size 50 */
    UUT.set_stack(LocalStack.end(), 50);
    TEST_ASSERT_EQUAL( LocalStack.end(), UUT.Stack_pointer);
    TEST_ASSERT_EQUAL( 50, UUT.get_stacksize() );
    TEST_ASSERT_FALSE( UUT.get_stackoverflow() );
};

/**
 * @brief Test the stack overflow warning.
 */
void test_StackOverflow()
{
    /* Create UUT */
    OTOS::Thread UUT;

    /* Set the stack to the local stack with size 50 */
    UUT.set_stack(LocalStack.end(), 50);
    
    /* After initializing the stack, no overflow should occur */
    TEST_ASSERT_FALSE( UUT.get_stackoverflow() );

    /* Increase the stack pointer of the thread just before its limit */
    /* -> No overflow should occur now. */
    UUT.Stack_pointer = LocalStack.end() - 49;
    TEST_ASSERT_FALSE( UUT.get_stackoverflow() );

    /* Increase the stack pointer of the thread exactly to its limit */
    /* -> An overflow should occur now. */
    UUT.Stack_pointer = LocalStack.end() - 50;
    TEST_ASSERT_TRUE( UUT.get_stackoverflow() );

    /* Increase the stack pointer of the thread beyond its limit */
    /* -> An overflow should occur now. */
    UUT.Stack_pointer = LocalStack.end() - 51;
    TEST_ASSERT_TRUE( UUT.get_stackoverflow() );

};

/**
 * @brief Test the state change of a thread which is scheduled
 * to run always. 
 */
void test_is_runnable_execute_always()
{
    /* Create UUT */
    OTOS::Thread UUT;

    /* After initializing the thread should not be runnable */
    TEST_ASSERT_FALSE( UUT.is_runnable() );

    /* Set the thread schedule to always be runnable */
    UUT.set_schedule(0, OTOS::Priority::Normal);

    /* Thread should now be runnable */
    TEST_ASSERT_TRUE( UUT.is_runnable() );

    /* When thread is running it should not be runnable */
    UUT.set_running();
    TEST_ASSERT_FALSE( UUT.is_runnable() );
    UUT.count_tick();
    TEST_ASSERT_FALSE( UUT.is_runnable() );

    /* When the thread finishes execution, it should be runnable after a tick count */
    UUT.set_blocked();
    TEST_ASSERT_TRUE( UUT.is_runnable() );
    UUT.count_tick();
    TEST_ASSERT_TRUE( UUT.is_runnable() );
};

/**
 * @brief Test the state change of a schedule when it reaches the runnable state.
 */
void test_is_runnable_with_schedule()
{
    /* Create UUT */
    OTOS::Thread UUT;

    /* After initializing the thread should not be runnable */
    TEST_ASSERT_FALSE( UUT.is_runnable() );

    /* Set the thread schedule to run every second tick  */
    UUT.set_schedule(1, OTOS::Priority::Normal);

    /* Thread should now not be runnable */
    TEST_ASSERT_FALSE( UUT.is_runnable() );

    /* Increment tick counter */
    UUT.count_tick();

    /* Thread should now be runnable */
    TEST_ASSERT_TRUE( UUT.is_runnable() );

    /* Set thread running */
    UUT.set_running();
    TEST_ASSERT_FALSE( UUT.is_runnable() );

    /* A tick count does not affect running threads */
    UUT.count_tick();
    TEST_ASSERT_FALSE( UUT.is_runnable() );

    /* After execution the thread should be blocked */
    UUT.set_blocked();
    TEST_ASSERT_FALSE( UUT.is_runnable() );

    /* A tick count should now make the thread runnable */
    UUT.count_tick();
    TEST_ASSERT_TRUE( UUT.is_runnable() );
};

void test_priority()
{
    /* Create UUT */
    OTOS::Thread UUT;

    /* Test initial Priority */
    UUT.set_schedule(1, OTOS::Priority::High);
    TEST_ASSERT_EQUAL(OTOS::Priority::High, UUT.get_priority());

    /* Test changing the priority */
    UUT.set_schedule(1, OTOS::Priority::Low);
    TEST_ASSERT_EQUAL(OTOS::Priority::Low, UUT.get_priority());
};

/* === Perform the tests ===  */
int main(int argc, char** argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_Constructor);
    RUN_TEST(test_SetStack);
    RUN_TEST(test_StackOverflow);
    RUN_TEST(test_is_runnable_execute_always);
    RUN_TEST(test_is_runnable_with_schedule);
    RUN_TEST(test_priority);
    UNITY_END();
    return 0;
};
