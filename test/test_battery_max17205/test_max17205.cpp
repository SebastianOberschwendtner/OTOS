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
 * @file    test_max17205.cpp
 * @author  SO
 * @version v2.0.0
 * @date    14-November-2021
 * @brief   Unit tests to test the driver for battery balancer and coulomb counter.
 ******************************************************************************
 */

// === Includes ===
#include <unity.h>
#include <mock.h>
#include <array>
#include <optional>
#include "interface.h"

// === Fixtures ===

// Mock the i2c driver
struct I2C_Mock { };
Mock::Callable<bool> set_target_address;
Mock::Callable<bool> send_word;
Mock::Callable<bool> send_data;
Mock::Callable<bool> send_array;
Mock::Callable<bool> send_array_leader;
Mock::Callable<bool> read_array;
Mock::Callable<bool> read_word;
std::array<unsigned char, 66> rx_buffer{0};

namespace Bus {
    void change_address(I2C_Mock& bus, const unsigned char address)
    {
        ::set_target_address(address);
        return;
    };
    bool send_bytes(
        I2C_Mock& bus,
        const unsigned char first_byte,
        const unsigned char second_byte,
        const unsigned char third_byte
        )
    {
        // set the payload data
        Bus::Data_t temp{};
        temp.byte[2] = first_byte;
        temp.byte[1] = second_byte;
        temp.byte[0] = third_byte;

        // send the data
        return ::send_data(temp.value);
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
        ::send_array_leader(byte, data, n_bytes); 
    return (rx_buffer[0] << 8) | rx_buffer[1];
    };
    std::optional<unsigned int> read_word(I2C_Mock& bus, const unsigned char reg)
    {
        ::read_word(reg);
        return (rx_buffer[1] << 8) | rx_buffer[0];
    };
    bool read_array(I2C_Mock &bus, const unsigned char reg, unsigned char* dest, const unsigned char n_bytes)
    {
        std::copy(rx_buffer.begin(), rx_buffer.begin() + n_bytes, dest);
        return ::read_array(reg);
    };
};

// Include the UUT
#include "battery/max17205.h"
#include "battery/max17205.cpp"
template class MAX17205::Controller<I2C_Mock>;

/** === Test List ===
 * ✓ controller has properties:
 *      ✓ Battery voltage in mV
 *      ✓ Battery current in mA
 *      ✓ Cell 1 voltage in mA
 *      ✓ Cell 2 voltage in mA
 *      ✓ Total battery capacity in mAh
 *      ✓ Remaining battery capacity in mAh
 *      ✓ Number of cycles
 *      ✓ Age of battery
 *      ✓ Series Resistance of battery in mΩ
 *      ✓ Temperature
 * ✓ controller can write registers
 * ✓ controller can read registers
 * ✓ controller can set correct initialization data
 * ▢ controller read and convert correctly:
 *      ▢ Capacity
 *      ▢ Percentage
 *      ✓ Voltage
 *      ✓ Cell Voltage
 *      ▢ Average Cell Voltage
 *      ✓ Current
 *      ✓ Average Current
 *      ▢ Temperature
 *      ▢ Resistance
 *      ▢ Time
 * ▢ controller can enable/disable balancing
 * ▢ controller can enable/disable sleep mode
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
    MAX17205::Controller UUT(i2c);

    // Test the properties
    TEST_ASSERT_EQUAL(0, UUT.get_battery_voltage());
    TEST_ASSERT_EQUAL(0, UUT.get_battery_current());
    TEST_ASSERT_EQUAL(0, UUT.get_cell_voltage(1));
    TEST_ASSERT_EQUAL(0, UUT.get_cell_voltage(2));
    TEST_ASSERT_EQUAL(0, UUT.get_total_capacity());
    TEST_ASSERT_EQUAL(0, UUT.get_remaining_capacity());
    TEST_ASSERT_EQUAL(0, UUT.get_number_cycles());
    TEST_ASSERT_EQUAL(0, UUT.get_age());
    TEST_ASSERT_EQUAL(0, UUT.get_ESR());
    TEST_ASSERT_EQUAL(0, UUT.get_temperature());
    
    // initialization
    TEST_ASSERT_TRUE(UUT.initialize());
    ::read_word.assert_called_once_with(0xBD);
};

/// @brief Test writting of registers
void test_write_register(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    MAX17205::Controller UUT(i2c);

    // perform test
    // send low address
    ::set_target_address.reset();
    ::send_data.reset();
    TEST_ASSERT_TRUE(UUT.write_register(MAX17205::Register::Cell_1, 0x1234));
    ::set_target_address.assert_called_once_with(MAX17205::i2c_address_low);
    ::send_data.assert_called_once_with(0xD83412);

    // send high address
    TEST_ASSERT_TRUE(UUT.write_register(MAX17205::Register::nConfig, 0x1234));
    ::set_target_address.assert_called_once_with(MAX17205::i2c_address_high);
    ::send_data.assert_called_once_with(0xB03412);
};

/// @brief Test reading of registers
void test_read_register(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    MAX17205::Controller UUT(i2c);

    // perform test
    // send low address
    TEST_ASSERT_TRUE(UUT.read_register(MAX17205::Register::Cell_1));
    ::set_target_address.assert_called_once_with(MAX17205::i2c_address_low);
    ::read_word.assert_called_once_with(0xD8);

    // send high address
    TEST_ASSERT_TRUE(UUT.read_register(MAX17205::Register::nConfig));
    ::set_target_address.assert_called_once_with(MAX17205::i2c_address_high);
    ::read_word.assert_called_once_with(0xB0);
};

/// @brief test reading the battery voltage
void test_read_battery_voltage(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    MAX17205::Controller UUT(i2c);

    // perform test
    rx_buffer[3] = 0x00;
    rx_buffer[2] = 0x00;
    rx_buffer[1] = 0xA0;
    rx_buffer[0] = 0x0F;
    TEST_ASSERT_TRUE(UUT.read_battery_voltage());
    TEST_ASSERT_EQUAL(5000, UUT.get_battery_voltage());
    ::read_word.assert_called_once_with(0xDA);

    rx_buffer[3] = 0x00;
    rx_buffer[2] = 0x00;
    rx_buffer[1] = 0xC0;
    rx_buffer[0] = 0x12;
    TEST_ASSERT_TRUE(UUT.read_battery_voltage());
    TEST_ASSERT_EQUAL(6000, UUT.get_battery_voltage());
};

/// @brief test reading the battery current
void test_read_battery_current(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    MAX17205::Controller UUT(i2c);

    // perform test
    rx_buffer[3] = 0x00;
    rx_buffer[2] = 0x00;
    rx_buffer[1] = 0x80;
    rx_buffer[0] = 0x0C;
    ::read_word.reset();
    TEST_ASSERT_TRUE(UUT.read_battery_current());
    TEST_ASSERT_EQUAL(1000, UUT.get_battery_current());
    ::read_word.assert_called_once_with(0x0A);

    rx_buffer[3] = 0x00;
    rx_buffer[2] = 0x00;
    rx_buffer[1] = 0x40;
    rx_buffer[0] = 0x06;
    TEST_ASSERT_TRUE(UUT.read_battery_current());
    TEST_ASSERT_EQUAL(500, UUT.get_battery_current());
};

/// @brief test reading the cell voltage
void test_read_cell_voltage(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    MAX17205::Controller UUT(i2c);

    // perform test
    rx_buffer[3] = 0x00;
    rx_buffer[2] = 0xA5;
    rx_buffer[1] = 0x00;
    rx_buffer[0] = 0xD2;
    TEST_ASSERT_TRUE(UUT.read_cell_voltage());
    TEST_ASSERT_EQUAL( 3300, UUT.get_cell_voltage(1));
    TEST_ASSERT_EQUAL( 4200, UUT.get_cell_voltage(2));
    ::read_array.assert_called_once_with(0xD7);
};

/// @brief test reading the battery current
void test_read_battery_current_avg(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    MAX17205::Controller UUT(i2c);

    // perform test
    rx_buffer[3] = 0x00;
    rx_buffer[2] = 0x00;
    rx_buffer[1] = 0x80;
    rx_buffer[0] = 0x0C;
    ::read_word.reset();
    TEST_ASSERT_TRUE(UUT.read_battery_current_avg());
    TEST_ASSERT_EQUAL(1000, UUT.get_battery_current());
    ::read_word.assert_called_once_with(0x0B);

    rx_buffer[3] = 0x00;
    rx_buffer[2] = 0x00;
    rx_buffer[1] = 0x40;
    rx_buffer[0] = 0x06;
    TEST_ASSERT_TRUE(UUT.read_battery_current_avg());
    TEST_ASSERT_EQUAL(500, UUT.get_battery_current());
};

/// @brief test reading the cell voltage
void test_read_cell_voltage_avg(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    MAX17205::Controller UUT(i2c);

    // perform test
    rx_buffer[3] = 0x00;
    rx_buffer[2] = 0xA5;
    rx_buffer[1] = 0x00;
    rx_buffer[0] = 0xD2;
    TEST_ASSERT_TRUE(UUT.read_cell_voltage_avg());
    TEST_ASSERT_EQUAL( 3300, UUT.get_cell_voltage(1));
    TEST_ASSERT_EQUAL( 4200, UUT.get_cell_voltage(2));
    ::read_array.assert_called_once_with(0xD3);
};

/// === Run Tests ===
int main(int argc, char** argv)
{
    UNITY_BEGIN();
    test_init();
    test_write_register();
    test_read_register();
    test_read_battery_voltage();
    test_read_battery_current();
    test_read_cell_voltage();
    test_read_battery_current_avg();
    test_read_cell_voltage_avg();
    UNITY_END();
    return EXIT_SUCCESS;
};
