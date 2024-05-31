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
 * @file    test_kernel.cpp
 * @author  SO
 * @version v5.0.0
 * @date    16-March-2021
 * @brief   Unit tests for the OTOS kernel to be executed on the host.
 ==============================================================================
 */

/* === Includes === */
#include <unity.h>
#include <mock.h>
#include <kernel.h>

/* === Text Fixtures === */
extern Mock::Callable<uint32_t> otos_switch;

void setUp() {
/* set stuff up here */
};

void tearDown() {
/* clean stuff up here */
};

/* === Define Tests === */

/**
 * @brief Test the constructor.
 */
void test_Constructor()
{
    /* Create UUT */
    OTOS::Kernel UUT;

    /* There should be no allocated stack */
    TEST_ASSERT_EQUAL( 0, UUT.get_allocated_stacksize() );
};

/**
 * @brief Test the correct allocation of the stack of one thread.
 */
void test_scheduleThread()
{
    /* Create UUT */
    OTOS::Kernel UUT;

    /* Schedule one thread */
    UUT.schedule_thread<256>(0, OTOS::Priority::Normal);

    /* Test the new stack size */
    TEST_ASSERT_EQUAL(256, UUT.get_allocated_stacksize());

    /* Schedule another thread */
    UUT.schedule_thread<256>(0, OTOS::Priority::Normal);

    /* Test the new stack size */
    TEST_ASSERT_EQUAL(2*256, UUT.get_allocated_stacksize());
};

/**
 * @brief Test switching the threads 
 */
void test_switch_to_thread()
{
    /* Create UUT */
    OTOS::Kernel UUT;
    UUT.schedule_thread<256>(0, OTOS::Priority::Normal);
    UUT.schedule_thread<256>(0, OTOS::Priority::Normal);

    /* Reset assembly function calls */
    otos_switch.reset();

    /* Switch the context */
    UUT.switch_to_thread(0);
    otos_switch.assert_called_once();
};

/**
 * @brief Test getting the next thread without timing or priority requirements.
 */
void test_scheduling_no_timing_no_priority()
{
    /* Create UUT */
    OTOS::Kernel UUT;
    UUT.schedule_thread<256>(0, OTOS::Priority::Normal);
    UUT.schedule_thread<256>(0, OTOS::Priority::Normal);
    UUT.schedule_thread<256>(0, OTOS::Priority::Normal);

    /* Next thread is 0 */
    TEST_ASSERT_EQUAL(0, UUT.get_next_thread().value_or(-1));
    UUT.switch_to_thread(0);
    /* Next thread is 1 */
    TEST_ASSERT_EQUAL(1, UUT.get_next_thread().value_or(-1));
    UUT.switch_to_thread(1);
    /* Next thread is 2 */
    TEST_ASSERT_EQUAL(2, UUT.get_next_thread().value_or(-1));
    UUT.switch_to_thread(2);
    /* Next thread is 0 */
    TEST_ASSERT_EQUAL(0, UUT.get_next_thread().value_or(-1));
};

/**
 * @brief Test getting the next thread with timing but no priority requirements.
 */
void test_scheduling_with_timing_no_priority()
{
    /* Create UUT */
    OTOS::Kernel UUT;
    UUT.schedule_thread<256>(0, OTOS::Priority::Normal);
    UUT.schedule_thread<256>(0, OTOS::Priority::Normal, 500);

    /* Next thread is 0 */
    TEST_ASSERT_EQUAL(0, UUT.get_next_thread().value_or(-1));
    UUT.switch_to_thread(0);

    /* Next thread is still 0 */
    TEST_ASSERT_EQUAL(0, UUT.get_next_thread().value_or(-1));
    UUT.switch_to_thread(0);

    /* Next thread is 1 when schedule was updated */
    UUT.update_schedule();
    TEST_ASSERT_EQUAL(0, UUT.get_next_thread().value_or(-1));
    UUT.switch_to_thread(0);
    UUT.update_schedule();
    TEST_ASSERT_EQUAL(1, UUT.get_next_thread().value_or(-1));
    UUT.switch_to_thread(1);

    /* Next thread is 0 after updating the schedule */
    UUT.update_schedule();
    TEST_ASSERT_EQUAL(0, UUT.get_next_thread().value_or(-1));
    UUT.switch_to_thread(0);

    /* Next thread is 1 after updating the schedule */
    UUT.update_schedule();
    TEST_ASSERT_EQUAL(1, UUT.get_next_thread().value_or(-1));
    UUT.switch_to_thread(1);
};

/**
 * @brief Test getting the next thread with timing and priority requirements.
 */
void test_scheduling_with_timing_with_priority()
{
    /* Create UUT */
    OTOS::Kernel UUT;
    UUT.schedule_thread<256>(0, OTOS::Priority::Normal);
    UUT.schedule_thread<256>(0, OTOS::Priority::Normal, 500);
    UUT.schedule_thread<256>(0, OTOS::Priority::High, 500);

    /* Next thread is 0 */
    TEST_ASSERT_EQUAL(0, UUT.get_next_thread().value_or(-1));
    UUT.switch_to_thread(0);

    /* Next thread is still 0 */
    TEST_ASSERT_EQUAL(0, UUT.get_next_thread().value_or(-1));
    UUT.switch_to_thread(0);

    /* Next thread is 2 when schedule was updated */
    UUT.update_schedule();
    TEST_ASSERT_EQUAL(0, UUT.get_next_thread().value_or(-1));
    UUT.switch_to_thread(0);
    UUT.update_schedule();
    TEST_ASSERT_EQUAL(2, UUT.get_next_thread().value_or(-1));
    UUT.switch_to_thread(2);

    /* Next thread is 1 after updating the schedule */
    UUT.update_schedule();
    TEST_ASSERT_EQUAL(1, UUT.get_next_thread().value_or(-1));
    UUT.switch_to_thread(1);
    TEST_ASSERT_EQUAL(0, UUT.get_next_thread().value_or(-1));
    UUT.switch_to_thread(0);

    /* Next thread is 1 after updating the schedule */
    UUT.update_schedule();
    TEST_ASSERT_EQUAL(2, UUT.get_next_thread().value_or(-1));
    UUT.switch_to_thread(2);
};

/**
 * @brief Test the ms timer of the kernel.
 */
void test_Time_ms()
{
    /* Create UUT */
    OTOS::Kernel UUT;

    /*The initial time value should be 0 */
    TEST_ASSERT_EQUAL( 0, UUT.get_time_ms());
    TEST_ASSERT_EQUAL( 0, OTOS::get_time_ms());

    /*Increase the count by 1 ms */
    UUT.count_time_ms();
    TEST_ASSERT_EQUAL( 1, UUT.get_time_ms());
    TEST_ASSERT_EQUAL( 1, OTOS::get_time_ms());
};

/* === Perform the tests === */
int main(int argc, char** argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_Constructor);
    RUN_TEST(test_scheduleThread);
    RUN_TEST(test_switch_to_thread);
    RUN_TEST(test_scheduling_no_timing_no_priority);
    RUN_TEST(test_scheduling_with_timing_no_priority);
    RUN_TEST(test_scheduling_with_timing_with_priority);
    RUN_TEST(test_Time_ms);
    return UNITY_END();
}
