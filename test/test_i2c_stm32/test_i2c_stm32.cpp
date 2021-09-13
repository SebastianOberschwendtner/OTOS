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
 * @version v1.0.8
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
* ▢ Transmitting:
*   ✓ controller can send start condition
*   ✓ controller can check whether start condition was generated
*   ✓ controller can check whether peripheral is in controller mode
*   ▢ controller can start communication by transmitting target address
*   ✓ controller can check whether target address is sent
*   ▢ controller can check whether ACK or NACK was received
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
*   ▢ error -1: Target address not acknowledged
*/

// === Mocks ===
class Mock_Pin: public GPIO::PIN_Base, public Mock::Peripheral
{
public:
    // === Functions to watch ===
    Mock::Callable set_mode;
    Mock::Callable set_type;
    Mock::Callable set_alternate;

    // === Mocked interface ===
    Mock_Pin() {};

    void setMode                (const GPIO::Mode NewMode)          override { set_mode.add_call((int)NewMode); };
    void setType                (const GPIO::Type NewType)          override { set_type.add_call((int)NewType); };
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
    TEST_ASSERT_EQUAL((1<<15) | (1<<14) | 2, I2C1->CCR);
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
    SCL.set_alternate.assert_called_once_with((int)GPIO::I2C_1);
    SCL.set_type.assert_called_once_with((int) GPIO::OPEN_DRAIN);
    SDA.set_mode.assert_called_once_with((int)GPIO::AF_Mode);
    SDA.set_alternate.assert_called_once_with((int)GPIO::I2C_1);
    SCL.set_type.assert_called_once_with((int) GPIO::OPEN_DRAIN);
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

/// @brief Test the start of an I2C communication
void test_start_communication(void)
{
    // Create Object
    I2C::Controller UUT(I2C::I2C_1, 400000);

    // Start communication
    UUT.set_target_address(0xEE);
    UUT.write_address();

    // Perform testing
    TEST_ASSERT_EQUAL(0xEE, I2C1->DR);
};

/// @brief Test whether controller can read its events
void test_events(void)
{
    setUp();

    // Create Object
    I2C::Controller UUT(I2C::I2C_1, 100000);

    // Check the events after init
    TEST_ASSERT_FALSE(UUT.in_controller_mode());
    TEST_ASSERT_FALSE(UUT.start_sent());
    TEST_ASSERT_FALSE(UUT.address_sent());

    // Test whether the return values change according to the events
    // controller in controller mode
    I2C1->SR2 = I2C_SR2_MSL;
    TEST_ASSERT_TRUE(UUT.in_controller_mode());
    TEST_ASSERT_FALSE(UUT.start_sent());
    TEST_ASSERT_FALSE(UUT.address_sent());

    // start condition generated
    I2C1->SR1 = I2C_SR1_SB;
    TEST_ASSERT_TRUE(UUT.in_controller_mode());
    TEST_ASSERT_TRUE(UUT.start_sent());
    TEST_ASSERT_FALSE(UUT.address_sent());

    // address was sent
    I2C1->SR1 = I2C_SR1_ADDR;
    TEST_ASSERT_TRUE(UUT.in_controller_mode());
    TEST_ASSERT_FALSE(UUT.start_sent());
    TEST_ASSERT_TRUE(UUT.address_sent());

    // Start generation
    UUT.generate_start();
    TEST_ASSERT_BIT_HIGH(I2C_CR1_START_Pos, I2C1->CR1);

    // Acknowledge received
    TEST_ASSERT_TRUE(UUT.ack_received());
    I2C1->SR1 = I2C_SR1_AF;
    TEST_ASSERT_FALSE(UUT.ack_received());

    // TX shift register is empty and ready to receive data
    TEST_ASSERT_FALSE(UUT.TX_register_empty());
    I2C1->SR1 = I2C_SR1_TXE;
    TEST_ASSERT_TRUE(UUT.TX_register_empty());

    // Stop generation
    UUT.generate_stop();
    TEST_ASSERT_BIT_HIGH(I2C_CR1_STOP_Pos, I2C1->CR1);

    // Transfer of last byte is finished
    TEST_ASSERT_FALSE(UUT.transfer_finished());
    I2C1->SR1 = I2C_SR1_BTF;
    TEST_ASSERT_TRUE(UUT.transfer_finished());

    // Detect when bus is busy
    TEST_ASSERT_FALSE(UUT.bus_busy());
    I2C1->SR2 = I2C_SR2_BUSY;
    TEST_ASSERT_TRUE(UUT.bus_busy());
};

/// @brief Test the i2c address transmission
void test_address_transmission(void)
{
    setUp();
    // Set the flags for a successfull data transmission
    I2C1->SR1 = I2C_SR1_ADDR | I2C_SR1_SB;
    I2C1->SR2 = I2C_SR2_MSL;

    // Create object
    I2C::Controller UUT(I2C::I2C_1, 100000);
    UUT.set_target_address(0xEE);

    // perform testing
    TEST_ASSERT_TRUE(UUT.send_address());
    TEST_ASSERT_EQUAL(0xEE, I2C1->DR);

    // test when target is not responding
    I2C1->SR1 = I2C_SR1_AF | I2C_SR1_SB;
    TEST_ASSERT_FALSE(UUT.send_address());
    TEST_ASSERT_EQUAL(Error::I2C_Address_Error, UUT.get_error());
    TEST_ASSERT_EQUAL(0xEE, I2C1->DR);

    // test a timeout of peripheral
    UUT.set_timeout(5);
    I2C1->SR1 = I2C_SR1_AF;
    TEST_ASSERT_FALSE(UUT.send_address());
    TEST_ASSERT_EQUAL(Error::I2C_Timeout, UUT.get_error());
};

/// @brief Test sending a byte via the i2c bus
void test_send_byte(void)
{
    setUp();
    // Set the flags for a successfull data transmission
    I2C1->SR1 = I2C_SR1_BTF | I2C_SR1_TXE | I2C_SR1_ADDR | I2C_SR1_SB;
    I2C1->SR2 = I2C_SR2_MSL;

    // Create object
    I2C::Controller UUT(I2C::I2C_1, 100000);
    UUT.set_target_address(0xEE);

    // Perform testing
    TEST_ASSERT_TRUE(UUT.send_byte(0xAA));
    TEST_ASSERT_EQUAL(0xAA, I2C1->DR); 
    
    // Test the timeout
    I2C1->SR1 = I2C_SR1_TXE | I2C_SR1_ADDR | I2C_SR1_SB;
    TEST_ASSERT_FALSE(UUT.send_byte(0xAA));
    TEST_ASSERT_EQUAL(Error::I2C_Timeout, UUT.get_error());

    // Test an acknowledge error
    I2C1->SR1 = I2C_SR1_TXE | I2C_SR1_ADDR | I2C_SR1_SB | I2C_SR1_AF;
    TEST_ASSERT_FALSE(UUT.send_byte(0xAA));
    TEST_ASSERT_EQUAL(Error::I2C_Data_ACK_Error, UUT.get_error());

    // Test sending when bus is busy
    I2C1->SR2 |= I2C_SR2_BUSY;
    TEST_ASSERT_FALSE(UUT.send_byte(0xAA));
    TEST_ASSERT_EQUAL(Error::I2C_BUS_Busy_Error, UUT.get_error());
};

/// @brief Test sending a word via the i2c bus
void test_send_word(void)
{
    setUp();
    // Set the flags for a successfull data transmission
    I2C1->SR1 = I2C_SR1_BTF | I2C_SR1_TXE | I2C_SR1_ADDR | I2C_SR1_SB;
    I2C1->SR2 = I2C_SR2_MSL;

    // Create object
    I2C::Controller UUT(I2C::I2C_1, 100000);
    UUT.set_target_address(0xEE);

    // Perform testing
    TEST_ASSERT_TRUE(UUT.send_word(0xAAEE));
    TEST_ASSERT_EQUAL(0xEE, I2C1->DR); 
    
    // Test the timeout
    I2C1->SR1 = I2C_SR1_TXE | I2C_SR1_ADDR | I2C_SR1_SB;
    TEST_ASSERT_FALSE(UUT.send_word(0xAAEE));
    TEST_ASSERT_EQUAL(Error::I2C_Timeout, UUT.get_error());

    // Test an acknowledge error
    I2C1->SR1 = I2C_SR1_TXE | I2C_SR1_ADDR | I2C_SR1_SB | I2C_SR1_AF;
    TEST_ASSERT_FALSE(UUT.send_word(0xAAEE));
    TEST_ASSERT_EQUAL(Error::I2C_Data_ACK_Error, UUT.get_error());
};

// === Main ===
int main(int argc, char** argv)
{
    UNITY_BEGIN();
    test_init();
    test_target_address();
    test_output_assignment();
    test_enable();
    test_start_communication();
    test_events();
    test_address_transmission();
    test_send_byte();
    test_send_word();
    UNITY_END();
    return 0;
};