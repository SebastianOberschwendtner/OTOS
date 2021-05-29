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
 * @file    test_schedule.c
 * @author  SO
 * @version v1.0.0
 * @date    16-March-2021
 * @brief   Test the basic functions of the scheduler of OTSO.
 ==============================================================================
 */

// *** Includes ***
#include <unity.h>
#include <schedule.h>

// Create UUT
OTOS::Schedule UUT;

void setUp(void) {
// set stuff up here
};

void tearDown(void) {
// clean stuff up here
};

// *** Define Tests ***

/**
 * @brief Test the correct behavior of the constructor
 */
void test_Constructor(void)
{
    // The thread should be runable after initialization
    TEST_ASSERT_TRUE( UUT.Runable() );
};


/**
 * @brief Test whether thread is runable.
 */
void test_Runable(void)
{
    // After initializing the thread should be runable
    TEST_ASSERT_TRUE( UUT.Runable() )

    // Set the thread schedule
    UUT.SetSchedule(1, OTOS::PrioNormal);

    // Thread should now not be runable
    TEST_ASSERT_FALSE( UUT.Runable() );

    // Increment tick counter
    UUT.CountTick();

    // Thread should now be runable
    TEST_ASSERT_TRUE( UUT.Runable() );
};

// *** Perform the tests ***
int main(int argc, char** argv)
{
    UNITY_BEGIN();
    test_Constructor();
    test_Runable();
    UNITY_END();
    return 0;
};
