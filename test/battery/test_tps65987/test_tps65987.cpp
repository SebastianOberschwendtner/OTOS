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
 * @version v4.1.0
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
Mock::Callable<bool> set_target_address;
Mock::Callable<bool> send_word;
Mock::Callable<bool> send_array;
Mock::Callable<bool> send_array_leader;
Mock::Callable<bool> read_array;
std::array<unsigned char, 66> rx_buffer{0};

namespace Bus {
    void change_address(I2C_Mock& bus, const unsigned char address)
    {
        ::set_target_address(address);
        return;
    };
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
 * ▢ controller can handle the following registers:
 *     ▢ 0x1A - Status Register
 *     ▢ 0x26 - Power Path Status ?
 *     ▢ 0x27 - Global System Configuration ?
 *     ▢ 0x28 - Port Configuration ?
 *     ▢ 0x29 - Port Control ?
 *     ▢ 0x30 - RX Source Capabilities
 *     ▢ 0x31 - RX Sink Capabilities
 *     ▢ 0x32 - TX Source Capabilities
 *     ▢ 0x33 - TX Sink Capabilities
 *     ▢ 0x34 - Active Contract PDO
 *     ▢ 0x35 - Active Contract RDO
 *     ▢ 0x36 - Sink Request RDO
 *     ▢ 0x37 - Auto Negotiate Sink
 *     ▢ 0x3F - Power Status
 *     ✓ 0x40 - PD Status
 *     ▢ 0x41 - PD3.0 Status ?
 *     ▢ 0x70 - Sleep Configuration
 */

void setUp(void) {
    set_target_address.reset();
    send_word.reset();
    send_array.reset();
    send_array_leader.reset();
    read_array.reset();
};

void tearDown(void) {
// clean stuff up here
};

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
    ::set_target_address.assert_called_once_with(TPS65987::i2c_address);
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

/// @brief Test the reading of the status
void test_read_status(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    TPS65987::Controller UUT(i2c);

    // Response with all zeros
    rx_buffer[0] = 0x88;
    rx_buffer[1] = 0x77;
    rx_buffer[2] = 0x66;
    rx_buffer[3] = 0x55;
    rx_buffer[4] = 0x44;
    rx_buffer[5] = 0x33;
    rx_buffer[6] = 0x22;
    rx_buffer[7] = 0x11;
    auto response = UUT.read_status();
    TEST_ASSERT_TRUE(response);
    TEST_ASSERT_EQUAL(0x44332211, response.value());
    ::read_array.assert_called_once_with(8+1);
};

/// @brief Test the PDO class for USB PD contracts
void test_PDO_class(void)
{
    // Create a PDO object
    TPS65987::PDO pdo;

    // Test the default constructor
    TEST_ASSERT_EQUAL(0, pdo.get_voltage());
    TEST_ASSERT_EQUAL(0, pdo.get_current());
    TEST_ASSERT_EQUAL(0, pdo.get_data());
    TEST_ASSERT_TRUE(pdo.is_fixed_supply());

    // Test setting the voltage
    pdo.set_voltage(5000);
    TEST_ASSERT_EQUAL(0x19000, pdo.get_data());
    TEST_ASSERT_EQUAL(5000, pdo.get_voltage());

    // Test setting the current
    pdo.set_current(3000);
    TEST_ASSERT_EQUAL(0x1912C, pdo.get_data());
    TEST_ASSERT_EQUAL(3000, pdo.get_current());

    // Test constructor with data
    TPS65987::PDO pdo2{(0b11 << 30) | 0x1912CUL};
    TEST_ASSERT_EQUAL(5000, pdo2.get_voltage());
    TEST_ASSERT_EQUAL(3000, pdo2.get_current());
    TEST_ASSERT_FALSE(pdo2.is_fixed_supply());

    // Test copy constructor
    TPS65987::PDO pdo3{pdo2};
    TEST_ASSERT_EQUAL(5000, pdo3.get_voltage());
    TEST_ASSERT_EQUAL(3000, pdo3.get_current());

    // Test assignment operator
    TPS65987::PDO pdo4;
    pdo4 = 0x1912C;
    TEST_ASSERT_EQUAL(5000, pdo4.get_voltage());
    TEST_ASSERT_EQUAL(3000, pdo4.get_current());
};

/// === Run Tests ===
int main(int argc, char** argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_init);
    RUN_TEST(test_read_register);
    RUN_TEST(test_write_register);
    RUN_TEST(test_read_active_command);
    RUN_TEST(test_write_command);
    RUN_TEST(test_read_mode);
    RUN_TEST(test_initialization);
    RUN_TEST(test_read_mode);
    RUN_TEST(test_read_PD_status);
    RUN_TEST(test_read_status);
    RUN_TEST(test_PDO_class);
    return UNITY_END();
};
