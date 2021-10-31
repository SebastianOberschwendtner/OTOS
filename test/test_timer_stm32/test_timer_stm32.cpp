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
 * @file    test_timer_stm32.cpp
 * @author  SO
 * @version v1.4.0
 * @date    31-October-2021
 * @brief   Unit tests for testing the timer driver for stm32 microcontrollers.
 ******************************************************************************
 */

// ****** Includes ******
#include "unity.h"
#include "mock.h"
#include "stm32/timer_stm32.h"

/** === Test List ===
 * ▢ Timer instance can be created:
 *      ▢ clock for peripheral gets enabled
 * ✓ Timer count can be read
 * ▢ Timer can be started
 * ▢ Timer can be stopped
*/

// === Mocks ===

// === Tests ===
void setUp(void) {
// set stuff up here
TIM1->registers_to_default();
RCC->registers_to_default();
};

void tearDown(void) {
// clean stuff up here
};

/// @brief Test the initialization of the controller
void test_init(void)
{
    // Create timer
    Timer::Timer UUT(Timer::Instance::TIM_1);

    // perform tests
};

/// @brief Test the reading of the counter value
void test_get_count(void)
{
    // Set stuff up
    setUp();

    // Create timer
    Timer::Timer UUT(Timer::Instance::TIM_1);

    // First the count should be 0
    TEST_ASSERT_EQUAL(0, UUT.get_count());

    // Increase the actual counter and test whether result is correct
    TIM1->CNT++;
    TEST_ASSERT_EQUAL(1, UUT.get_count());

}

// === Main ===
int main(int argc, char** argv)
{
    UNITY_BEGIN();
    test_init();
    test_get_count();
    UNITY_END();
    return 0;
};