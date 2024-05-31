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
 * @file    test_driver_interface.cpp
 * @author  SO
 * @version v5.0.0
 * @date    30-August-2021
 * @brief   Unit tests for testing driver interfaces for OTOS.
 ==============================================================================
 */

// ****** Includes ******
#include "unity.h"
#include "mock.h"
#include "interface.h"
#include <array>

/** === Test List ===
 * ✓ base class is initalized with no error
 * ✓ has method to get most recent error
 * ▢ has methods which counts timeouts
 * ▢ timeout methods sets timeout error
 */

/* === Mocks === */
enum class IO {
   SYSTEM_ = 0, 
   I2C_1 
};

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
    driver::Base<IO> UUT;
    driver::Base IO{IO::I2C_1};

    // perform testing
    TEST_ASSERT_EQUAL(error::Code::None, UUT.get_error());
    TEST_ASSERT_TRUE(UUT.timed_out());
    TEST_ASSERT_EQUAL(IO::SYSTEM_, UUT.instance);
    TEST_ASSERT_EQUAL(IO::I2C_1, IO.instance);
};

/// @brief Test whether errors can be set
void test_set_error(void)
{
    // create object
    driver::Base<IO> UUT;
    UUT.set_error(error::Code::I2C_Address_Error);

    // perform testing
    TEST_ASSERT_EQUAL(error::Code::I2C_Address_Error, UUT.get_error());
};

/// @brief test the timeout methods
void test_timeout(void)
{
    // create object
    driver::Base<IO> UUT;
    UUT.set_timeout(5); // set timeout to 5 calls

    // perform testing
    for (int count = 0; count < 5; count++)
        TEST_ASSERT_FALSE(UUT.timed_out());
    TEST_ASSERT_TRUE(UUT.timed_out());
    UUT.reset_timeout();
    TEST_ASSERT_FALSE(UUT.timed_out());

    // perform testing for larger timeoutvalues
    UUT.set_timeout(65000);
    UUT.reset_timeout();
    for (uint32_t count = 0; count < 65000; count++)
        TEST_ASSERT_FALSE(UUT.timed_out());
    TEST_ASSERT_TRUE(UUT.timed_out());
    UUT.reset_timeout();
    TEST_ASSERT_FALSE(UUT.timed_out());
};

/// @brief test the interface to GPIOs
void test_gpio_interface(void)
{
    // Setup mocked GPIO Pin
    struct Mock_Pin
    {
        Mock::Callable<bool> set_alternate_function;
    };
    Mock_Pin mypin{};
    driver::Base controller{IO::I2C_1};

    // Test assigning alternate function
    gpio::assign(mypin, controller);
    mypin.set_alternate_function.assert_called_once_with(
        static_cast<int>(IO::I2C_1)
    );
};

/// @brief test the bus driver interface
void test_bus_interface(void)
{
    struct Mock_Bus
    {
        bus::Data_t buffer{0};
        Mock::Callable<bool> set_target_address;
        Mock::Callable<bool> send_data;
        Mock::Callable<bool> send_array;
        Mock::Callable<bool> send_array_leader;
        Mock::Callable<bool> read_array;
        Mock::Callable<bool> read_data;
        bus::Data_t get_rx_data(void) { return this->buffer; };
    };
    Mock_Bus mybus{};

    // Test address setting
    bus::change_address(mybus, 0x12);
    mybus.set_target_address.assert_called_once_with(0x12);

    // Test send byte(s)
    TEST_ASSERT_TRUE(bus::send_byte(mybus, 0x34));
    mybus.send_data.assert_called_once_with(0x34);
    TEST_ASSERT_TRUE(bus::send_bytes(mybus, 0x12, 0x34));
    mybus.send_data.assert_called_once_with(0x1234);
    TEST_ASSERT_TRUE(bus::send_bytes(mybus, 0x12, 0x34, 0x56));
    mybus.send_data.assert_called_once_with(0x123456);

    // Test send word
    TEST_ASSERT_TRUE(bus::send_word(mybus, 0x4312));
    mybus.send_data.assert_called_once_with(0x4312);

    // Test send_array
    uint8_t temp = 69;
    TEST_ASSERT_TRUE(bus::send_array(mybus, &temp, 1));
    mybus.send_array.assert_called_once_with(69);

    // Test send array with leading byte
    TEST_ASSERT_TRUE(bus::send_array_leader(mybus, 0x34, &temp, 1));
    mybus.send_array_leader.assert_called_once_with(0x34);

    // test read array
    TEST_ASSERT_TRUE(bus::read_array(mybus, 0x56, &temp, 1));
    mybus.read_array.assert_called_once_with(0x56);

    // test read array without sending register first
    mybus.read_array.reset();
    TEST_ASSERT_TRUE(bus::read_array(mybus, &temp, 1));
    mybus.read_array.assert_called_once();

    // test read word
    mybus.buffer.value = 0x43;
    auto response = bus::read_word(mybus, 0x20);
    mybus.read_data.assert_called_once_with(0x20);
    TEST_ASSERT_TRUE(response);
    TEST_ASSERT_EQUAL(0x43, response.value());

    // test read byte without sending address
    mybus.buffer.value = 0x44;
    mybus.read_data.reset();
    response = bus::read_byte(mybus);
    mybus.read_data.assert_called_once();
    TEST_ASSERT_TRUE(response);
    TEST_ASSERT_EQUAL(0x44, response.value());

    // Test sending an std::array
    mybus.send_array.reset();
    std::array<uint8_t, 6> std_array{0};
    TEST_ASSERT_TRUE(bus::send_array(mybus, std_array));
    mybus.send_array.assert_called_once();

    // Test reading an std::array
    mybus.read_array.reset();
    TEST_ASSERT_TRUE(bus::read_array(mybus, std_array));
    mybus.read_array.assert_called_once();
};

/// @brief Test and define the interface for timers
void test_timer_interface(void)
{
    // Timer fixture
    struct Mock_Timer
    {
        Mock::Callable<bool> start;
        Mock::Callable<bool> stop;
        Mock::Callable<uint32_t> get_count;
    };
    Mock_Timer mytime{};

    // Call interface
    timer::start(mytime);
    mytime.start.assert_called_once();
    timer::stop(mytime);
    mytime.stop.assert_called_once();
    TEST_ASSERT_EQUAL(1, timer::get_count(mytime));
    mytime.get_count.assert_called_once();
};

// === Main ===
int main(int argc, char** argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_init);
    RUN_TEST(test_set_error);
    RUN_TEST(test_timeout);
    RUN_TEST(test_gpio_interface);
    RUN_TEST(test_bus_interface);
    RUN_TEST(test_timer_interface);
    return UNITY_END();
};