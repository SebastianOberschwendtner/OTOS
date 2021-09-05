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
#include "mock.h"
#include "stm32/i2c_stm32.h"

/** === Test List ===
* ▢ i2c controller can be created defining the clock speed:
*   ✓ controller enables the clock of the peripheral
*   ✓ error is 0
*   ✓ target address is 0
*   ✓ return data is 0
*   ✓ controller sets the configuration of the peripheral correct
*   ✓ controller sets the clk configuration correct based on F_APB
*   ▢ controller does not change peripheral, when it is busy
* ✓ controller can set and store target address
* ✓ controller can set the CLK pin
* ✓ controller can set the DATA pin
* ✓ controller can be enabled
* ✓ controller can be disabled
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
* ▢ error codes:
*   ▢ error -1: 
*/

// === Mocks ===
class Mock_Pin: public GPIO::PIN_Base, public Mock::Peripheral
{
public:
    // === Functions to watch ===
    Mock::Callable set_mode;
    Mock::Callable set_alternate;

    // === Mocked interface ===
    Mock_Pin() {};

    void setMode                (const GPIO::Mode NewMode)          override { set_mode.add_call((int)NewMode); };
    void setType                (const GPIO::Type NewType)          override {};
    void setSpeed               (const GPIO::Speed NewSpeed)        override {};
    void setPull                (const GPIO::Pull NewPull)          override {};
    void set_alternate_function (const GPIO::Alternate function)    override { set_alternate.add_call((int)function); };
    void set                    (const bool NewState)               override {};
    void setHigh                (void)                              override {};
    void setLow                 (void)                              override {};
    void toggle                 (void)                              override {};
    bool get                    (void) const                        override {return false;};
};

// === Tests ===
void setUp(void) {
// set stuff up here
I2C1->registers_to_default();
};

void tearDown(void) {
// clean stuff up here
};

/// @brief Test the initialization of the controller
void test_init(void)
{
    setUp();
    // Assume other clocks in RCC are already set
    RCC->APB1ENR = (1<<23) | (1<<20);

    // Create object
    I2C::Controller UUT(I2C::I2C_1, 400000);

    // Perform testing
    TEST_ASSERT_EQUAL((1<<23) | (1<<21) | (1<<20), RCC->APB1ENR); // Enable the peripheral clock
    TEST_ASSERT_EQUAL( 0, I2C1->CR1);
    TEST_ASSERT_EQUAL(20, I2C1->CR2);
    TEST_ASSERT_EQUAL((1<<15) | (1<<14) | 50, I2C1->CCR);
    TEST_ASSERT_EQUAL(11, I2C1->TRISE);
    TEST_ASSERT_EQUAL(0, I2C1->FLTR);
    TEST_ASSERT_EQUAL(0, UUT.get_target_address());  // Test that the target is address is 0
    TEST_ASSERT_EQUAL(0, UUT.get_rx_data().value); // The return data is initialized with 0
    TEST_ASSERT_EQUAL(0, UUT.get_error());  // Test that no errors exist after creating  the controller
};

/// @brief Test the setting of the target address
void test_target_address(void)
{
    // Create object
    I2C::Controller UUT(I2C::I2C_1, 400000);
    UUT.set_target_address(0xEE);

    // test whether the address was set
    TEST_ASSERT_EQUAL(0xEE, UUT.get_target_address());

    // Test whether the last bit is always zero when setting the address
    UUT.set_target_address(0xFF);
    TEST_ASSERT_EQUAL(0xFE, UUT.get_target_address());
};

/// @brief Test the assignment of the output pins.
void test_output_assignment(void)
{
    // Create object
    I2C::Controller UUT(I2C::I2C_1, 400000);

    // Assign the outputs
    Mock_Pin SCL;
    Mock_Pin SDA;
    UUT.assign_pin(SCL);
    UUT.assign_pin(SDA);

    // Perform testing
    SCL.set_mode.assert_called_once_with((int)GPIO::AF_Mode);
    SCL.set_alternate.assert_called_once_with((int)GPIO::I2C1);
    SDA.set_mode.assert_called_once_with((int)GPIO::AF_Mode);
    SDA.set_alternate.assert_called_once_with((int)GPIO::I2C1);
};

/// @brief Test the enabling and disabling of the peripheral.
void test_enable(void)
{
    // Create Object
    I2C::Controller UUT(I2C::I2C_1, 400000);

    // Perform testing
    TEST_ASSERT_BIT_LOW(0, I2C1->CR1);
    UUT.enable();
    TEST_ASSERT_BIT_HIGH(0, I2C1->CR1);
    UUT.disable();
    TEST_ASSERT_BIT_LOW(0, I2C1->CR1);
};

// === Main ===
int main(int argc, char** argv)
{
    UNITY_BEGIN();
    test_init();
    test_target_address();
    test_output_assignment();
    test_enable();
    UNITY_END();
    return 0;
};