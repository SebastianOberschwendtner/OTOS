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
 * @file    test_i2c_stm32.cpp
 * @author  SO
 * @version v1.0.7
 * @date    30-August-2021
 * @brief   Unit tests for testing the i2c driver for stm32 microcontrollers.
 ******************************************************************************
 */

// ****** Includes ******
#include "unity.h"
#include "interface.h"

/** === Test List ===
 * ✓ base class is initalized with no error
 * ✓ has method to get most recent error
 * ▢ has methods which counts timeouts
 * ▢ timeout methods sets timeout error
 */

// === Tests ===
void setUp(void) {
// set stuff up here
};

void tearDown(void) {
// clean stuff up here
};

/// @brief Test the initialization of the driver object
void test_init(void)
{
    // create object
    Driver::Base UUT;

    // perform testing
    TEST_ASSERT_EQUAL(0, UUT.get_error());
};

/// @brief Test whether errors can be set
void test_set_error(void)
{
    // create object
    Driver::Base UUT;
    UUT.set_error(Error::Code::I2C_Address_Error);

    // perform testing
    TEST_ASSERT_EQUAL(Error::Code::I2C_Address_Error, UUT.get_error());
};

/// @brief test the timeout methods
void test_timeout(void)
{
    // create object
    Driver::Base UUT;
    UUT.set_timeout(5); // set timeout to 5 calls

    // perform testing
    for (int count = 0; count < 5; count++)
        TEST_ASSERT_FALSE(UUT.timed_out());
    TEST_ASSERT_TRUE(UUT.timed_out())
    UUT.reset_timeout();
    TEST_ASSERT_FALSE(UUT.timed_out());
};

// === Main ===
int main(int argc, char** argv)
{
    UNITY_BEGIN();
    test_init();
    test_set_error();
    test_timeout();
    UNITY_END();
    return 0;
};