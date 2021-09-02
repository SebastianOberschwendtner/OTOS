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
#include "stm32/i2c_stm32.h"

/** === Test List ===
* ▢ i2c controller can be created defining the clock speed:
*   ▢ controller does not change peripheral, when it is busy
*   ▢ controller does not change a pre-existing setup
* ▢ controller can set the CLK pin
* ▢ controller can set the DATA pin
* ▢ i2c controller stores:
*   ▢ target address
*   ▢ return data
*   ▢ error messages
* ▢ controller has a method which gives the current peripheral status (busy, ready, ...)
* ▢ controller has a non-blocking send function:
*   ▢ for ( 8 bits) 1 byte
*   ▢ for (16 bist) 2 bytes
*   ▢ for (24 bits) 3 bytes
*   ▢ for (32 bits) 4 bytes
* ▢ controller has a non-blocking receive function:
*   ▢ for ( 8 bits) 1 byte
*   ▢ for (16 bist) 2 bytes
*   ▢ for (24 bits) 3 bytes
*   ▢ for (32 bits) 4 bytes
* ✗ Multiple controllers with the same peripheral assigned to them do not interfere with each other
*/

void setUp(void) {
// set stuff up here
};

void tearDown(void) {
// clean stuff up here
};

// ****** Define Tests ******
void test_init(void)
{
    // Create object
    I2C::Controller UUT(1, 400000);

    // Perform testing
};

int main(int argc, char** argv)
{
    UNITY_BEGIN();
    test_init();
    UNITY_END();
    return 0;
};