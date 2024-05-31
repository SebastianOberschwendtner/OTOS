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
 * @file    test_task.cpp
 * @author  SO
 * @version v5.0.0
 * @date    26-Dezember-2021
 * @brief   Unit tests to test the task functionality.
 ==============================================================================
 */

/* === Includes === */
#include <unity.h>
#include <mock.h>
#include "task.h"

/** === Test List ===
 */

/* === Fixtures === */
Mock::Callable<bool> call_timer;
std::uint32_t tick_ms = 0;
std::uint32_t mock_handle_constant()
{
    call_timer.add_call(0);
    return tick_ms;
};
std::uint32_t mock_handle_increment()
{
    call_timer.add_call(0);
    return tick_ms++;
};

void setUp(){
    /* set stuff up here */
    tick_ms = 0;
    call_timer.reset();
};

void tearDown(){
    /* clean stuff up here */
};

/* === Define Tests === */
/** 
 * @brief Test the constructor of the display controller
 */
void test_constructor()
{
    setUp();
    /* Create task */
    OTOS::TimedTask UUT(&mock_handle_constant);
};

/** 
 * @brief Test the time elapsed function
 */
void test_time_elapsed()
{
    setUp();
    /* Create task */
    OTOS::TimedTask UUT(&mock_handle_constant);

    /* Elapsed time after construction */
    TEST_ASSERT_EQUAL(0, UUT.time_elapsed_ms());

    /* Set ticks to 10 */
    tick_ms = 10;
    TEST_ASSERT_EQUAL(10, UUT.time_elapsed_ms());

    /* Update last tick */
    UUT.tic();
    TEST_ASSERT_EQUAL(0, UUT.time_elapsed_ms());
    TEST_ASSERT_EQUAL(10, UUT.toc() );
};

/** 
 * @brief Test the waiting functions
 */
void test_waiting()
{
    setUp();
    /* Create task */
    OTOS::TimedTask UUT(&mock_handle_increment);
    UUT.wait_ms(10);

    /* Test side effects */
    TEST_ASSERT_EQUAL(11, call_timer.call_count);
};

/** 
 * @brief Test blocking the execution of the task
 */
void test_blocking()
{
    setUp();
    /* Create task */
    OTOS::TimedTask UUT(&mock_handle_increment);
    UUT.block_ms(10);

    /* Test side effects */
    TEST_ASSERT_EQUAL(11, call_timer.call_count);
};

/** 
 * === Run Tests ===
 */
int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_constructor);
    RUN_TEST(test_time_elapsed);
    RUN_TEST(test_waiting);
    RUN_TEST(test_blocking);
    UNITY_END();
    return EXIT_SUCCESS;
};
