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
 * @file    test_tps65987.cpp
 * @author  SO
 * @version v2.0.0
 * @date    14-November-2021
 * @brief   Unit tests to test the driver for USB-3 PD controller.
 ******************************************************************************
 */

// === Includes ===
#include <unity.h>
#include <mock.h>
#include <array>

// === Fixtures ===

// Mock the i2c driver
struct I2C_Mock { };
Mock::Callable<bool> send_word;
Mock::Callable<bool> send_array;
Mock::Callable<bool> send_array_leader;
Mock::Callable<bool> read_array;
std::array<unsigned char, 66> rx_buffer{0};

namespace Bus {
    bool send_word(I2C_Mock& bus, unsigned int word)
    {
       return ::send_word(word);
    };
    bool send_array(I2C_Mock& bus, const unsigned char* data, const unsigned char n_bytes)
    { 
        std::copy(data, data + n_bytes, rx_buffer.begin());
        return ::send_array(n_bytes); 
    };
    bool send_array_leader(I2C_Mock& bus, const unsigned char byte, const unsigned char* data, const unsigned char n_bytes)
    { 
        return ::send_array_leader(byte, data, n_bytes); 
    };
    bool read_array(I2C_Mock &bus, const unsigned char reg, unsigned char* dest, const unsigned char n_bytes)
    {
        std::copy(rx_buffer.begin(), rx_buffer.begin() + n_bytes, dest);
        return ::read_array(n_bytes);
    };
};

// Include the UUT
#include "battery/tps65987.h"
#include "battery/tps65987.cpp"
template class TPS65987::Controller<I2C_Mock>;

/** === Test List ===
 * ▢ Controller has properties:
 *      ✓ current operating mode
 *      ✓ active command
 *      ✓ Current power contract:
 *          ✓ Sink/Source
 *          ✓ USB type (1,2,3)
 *          ✓ Voltage
 *          ✓ Current
 * ✓ controller can read registers with variable length
 * ✓ controller can write registers with variable length
 * ✓ controller can send commands
 * ✓ controller can read the mode the PD IC is in
 * ✓ controller initializes the PD IC based on its mode
 * ▢ controller can check whether command was finished
 * ▢ controller can read the return code of a finished command
 * ▢ controller can read the PD status
 */

// static void setUp(void) {
// // set stuff up here
// };

// static void tearDown(void) {
// // clean stuff up here
// };

// === Define Tests ===
/// @brief Test the constructor
void test_init(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    TPS65987::Controller UUT(i2c);

    // perform testing
    TEST_ASSERT_EQUAL(0, static_cast<unsigned char>(UUT.get_mode()));
    char Expected_Command[] = {0,0,0,0};
    TEST_ASSERT_EQUAL_CHAR_ARRAY(Expected_Command, UUT.get_active_command(), 4);
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().role);
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().USB_type);
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().voltage);
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().current);
};

/// @brief Test reading a register with variable length
void test_read_register(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    TPS65987::Controller UUT(i2c);

    // perform testing
    TEST_ASSERT_TRUE(UUT.read_register(TPS65987::Register::Data1));
    ::read_array.assert_called_once_with(65);

    TEST_ASSERT_TRUE(UUT.read_register(TPS65987::Register::Cmd1));
    ::read_array.assert_called_once_with(5);
};

/// @brief Test writing a register with variable length
void test_write_register(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    TPS65987::Controller UUT(i2c);

    // perform testing
    TEST_ASSERT_TRUE(UUT.write_register(TPS65987::Register::Data1));
    ::send_array.assert_called_once_with(66);

    TEST_ASSERT_TRUE(UUT.write_register(TPS65987::Register::Cmd1));
    ::send_array.assert_called_once_with(6);
};

/// @brief Test reading a command status
void test_read_active_command(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    TPS65987::Controller UUT(i2c);

    // perform testing
    rx_buffer[0] = '!';
    rx_buffer[1] = 'D';
    rx_buffer[2] = 'M';
    rx_buffer[3] = 'C';
    TEST_ASSERT_TRUE(UUT.read_active_command());
    ::read_array.assert_called_once_with(5);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("CMD!", UUT.get_active_command(), 4);
};

/// @brief Test writting a command
void test_write_command(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    TPS65987::Controller UUT(i2c);

    // perform testing
    TEST_ASSERT_TRUE(UUT.write_command("PTCc"));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("PTCc", &rx_buffer[2], 4);
};

/// @brief Test reading the current mode of the controller
void test_read_mode(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    TPS65987::Controller UUT(i2c);

    // perform testing
    rx_buffer[0] = 'H';
    rx_buffer[1] = 'C';
    rx_buffer[2] = 'T';
    rx_buffer[3] = 'P';
    TEST_ASSERT_TRUE(UUT.read_mode());
    ::read_array.assert_called_once_with(5);
    TEST_ASSERT_EQUAL(1, static_cast<unsigned char>(UUT.get_mode()));
};

/// @brief Test the correct initialization of the controller
void test_initialization(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    TPS65987::Controller UUT(i2c);

    // perform testing
    // when controller is in patch mode -> exit by sending "PTCc" command
    rx_buffer[0] = 'H';
    rx_buffer[1] = 'C';
    rx_buffer[2] = 'T';
    rx_buffer[3] = 'P';
    TEST_ASSERT_TRUE(UUT.initialize());
    ::read_array.assert_called_once_with(5);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("PTCc", &rx_buffer[2], 4);
};

/// @brief Test the reading of the PD status
void test_read_PD_status(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    TPS65987::Controller UUT(i2c);

    // Response with all zeros
    rx_buffer[0] = 0x00;
    rx_buffer[1] = 0x00;
    rx_buffer[2] = 0x00;
    rx_buffer[3] = 0x00;
    TEST_ASSERT_TRUE(UUT.read_PD_status());
    ::read_array.assert_called_once_with(5);
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().role);
    TEST_ASSERT_EQUAL(3, UUT.get_active_contract().USB_type);
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().voltage);
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().current);

    // Plug type is USB2.0
    rx_buffer[0] = 0x00;
    rx_buffer[1] = 0x00;
    rx_buffer[2] = 0x00;
    rx_buffer[3] = TPS65987::PlugDetails_0;
    TEST_ASSERT_TRUE(UUT.read_PD_status());
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().role);
    TEST_ASSERT_EQUAL(2, UUT.get_active_contract().USB_type);
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().voltage);
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().current);

    // USB default current
    rx_buffer[0] = 0x00;
    rx_buffer[1] = 0x00;
    rx_buffer[2] = 0x00;
    rx_buffer[3] = TPS65987::CCPullUp_0;
    TEST_ASSERT_TRUE(UUT.read_PD_status());
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().role);
    TEST_ASSERT_EQUAL(3, UUT.get_active_contract().USB_type);
    TEST_ASSERT_EQUAL(5000, UUT.get_active_contract().voltage);
    TEST_ASSERT_EQUAL(900, UUT.get_active_contract().current);

    // USB 1.5A
    rx_buffer[0] = 0x00;
    rx_buffer[1] = 0x00;
    rx_buffer[2] = 0x00;
    rx_buffer[3] = TPS65987::CCPullUp_1;
    TEST_ASSERT_TRUE(UUT.read_PD_status());
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().role);
    TEST_ASSERT_EQUAL(3, UUT.get_active_contract().USB_type);
    TEST_ASSERT_EQUAL(5000, UUT.get_active_contract().voltage);
    TEST_ASSERT_EQUAL(1500, UUT.get_active_contract().current);

    // USB 3.0A
    rx_buffer[0] = 0x00;
    rx_buffer[1] = 0x00;
    rx_buffer[2] = 0x00;
    rx_buffer[3] = TPS65987::CCPullUp_1 | TPS65987::CCPullUp_0;
    TEST_ASSERT_TRUE(UUT.read_PD_status());
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().role);
    TEST_ASSERT_EQUAL(3, UUT.get_active_contract().USB_type);
    TEST_ASSERT_EQUAL(5000, UUT.get_active_contract().voltage);
    TEST_ASSERT_EQUAL(3000, UUT.get_active_contract().current);

    // Role source
    rx_buffer[0] = 0x00;
    rx_buffer[1] = 0x00;
    rx_buffer[2] = 0x00;
    rx_buffer[3] = TPS65987::PresentRole;
    TEST_ASSERT_TRUE(UUT.read_PD_status());
    TEST_ASSERT_EQUAL(1, UUT.get_active_contract().role);
    TEST_ASSERT_EQUAL(3, UUT.get_active_contract().USB_type);
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().voltage);
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().current);
};

/// === Run Tests ===
int main(int argc, char** argv)
{
    UNITY_BEGIN();
    test_init();
    test_read_register();
    test_write_register();
    test_read_active_command();
    test_write_command();
    test_read_mode();
    test_initialization();
    test_read_mode();
    test_read_PD_status();
    UNITY_END();
    return EXIT_SUCCESS;
};
