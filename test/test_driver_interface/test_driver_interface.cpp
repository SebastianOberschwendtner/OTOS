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
 * ▢ base class is initalized with no error
 */

// === Tests ===
void setUp(void) {
// set stuff up here
};

void tearDown(void) {
// clean stuff up here
};

/// @brief Test the initialization of the controller
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
    Driver::Base UUT;
    UUT.set_error(Error::I2C_Address_Error);
    TEST_ASSERT_EQUAL(Error::I2C_Address_Error, UUT.get_error());
}

// === Main ===
int main(int argc, char** argv)
{
    UNITY_BEGIN();
    test_init();
    test_set_error();
    UNITY_END();
    return 0;
};