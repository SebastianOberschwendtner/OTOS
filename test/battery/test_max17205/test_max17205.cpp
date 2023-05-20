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
 * @version v4.2.0
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
 *      ✓ Capacity
 *      ✓ Percentage
 *      ✓ Voltage
 *      ✓ Cell Voltage
 *      ▢ Average Cell Voltage
 *      ✓ Current
 *      ✓ Average Current
 *      ▢ Temperature
 *      ▢ Resistance
 *      ✓ Time
 *      ✓ SOC
 * ▢ controller can enable/disable balancing
 * ▢ controller can enable/disable sleep mode
 */

void setUp(void) {
    // set stuff up here
    set_target_address.reset();
    send_word.reset();
    send_data.reset();
    send_array.reset();
    send_array_leader.reset();
    read_array.reset();
    read_word.reset();
};

void tearDown(void) {
// clean stuff up here
};

// === Define Tests ===
/// @brief Test the unit types
void test_unit_capacity()
{
    // Create a capacity value
    MAX17205::mAh capacity{};

    // Default construction should be zero
    TEST_ASSERT_EQUAL(0, capacity);
    
    // When assigning a integral value, it should be converted to mAh
    capacity = 1000;
    TEST_ASSERT_EQUAL(1000, capacity);

    // Construct with integral value should get converted
    MAX17205::mAh capacity2{2000};
    TEST_ASSERT_EQUAL(2000, capacity2);

    // Test copy assignment
    capacity = capacity2;
    TEST_ASSERT_EQUAL(2000, capacity);
}
void test_unit_percentage()
{
    // Create a percentage value
    MAX17205::percent percentage{};

    // Default construction should be zero
    TEST_ASSERT_EQUAL(0, percentage);

    // When assigning a integral value, it should be converted to %
    percentage = 256;
    TEST_ASSERT_EQUAL(1, percentage);
}
void test_unit_current()
{
    // Create a current value
    MAX17205::mA current{};

    // Default construction should be zero
    TEST_ASSERT_EQUAL(0, current);

    // When assigning a integral value, it should be converted to mA
    current = 5*640;
    TEST_ASSERT_EQUAL(1000, current);
    current = -5*640;
    TEST_ASSERT_EQUAL(-1000, current);
    std::uint16_t temp_unsigned = 65532U;
    current = temp_unsigned;    
    TEST_ASSERT_EQUAL(-1, current);
}
void test_unit_voltage()
{
    // Create a voltage value
    MAX17205::mV voltage{};

    // Default construction should be zero
    TEST_ASSERT_EQUAL(0, voltage);

    // When assigning a integral value, it should be converted to mV
    voltage = 12800;
    TEST_ASSERT_EQUAL(1000, voltage);
}
void test_unit_seconds()
{
    // Create a seconds value
    MAX17205::seconds seconds{};

    // Default construction should be zero
    TEST_ASSERT_EQUAL(0, seconds);

    // When assigning a integral value, it should be converted to seconds
    seconds = 1;
    TEST_ASSERT_EQUAL(5, seconds);

    // maximum value should be 65535
    seconds = 65536;
    TEST_ASSERT_EQUAL(368640, seconds);
}

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
    TEST_ASSERT_EQUAL(0, UUT.get_SOC());
    TEST_ASSERT_EQUAL(0, UUT.get_number_cycles());
    TEST_ASSERT_EQUAL(0, UUT.get_age());
    TEST_ASSERT_EQUAL(0, UUT.get_ESR());
    TEST_ASSERT_EQUAL(0, UUT.get_temperature());
    TEST_ASSERT_EQUAL(0, UUT.get_TTE());
    TEST_ASSERT_EQUAL(0, UUT.get_TTF());
    
    // initialization
    TEST_ASSERT_TRUE(UUT.initialize());
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
    rx_buffer[2] = 0xD2;
    rx_buffer[1] = 0x00;
    rx_buffer[0] = 0xA5;
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
    rx_buffer[2] = 0xD2;
    rx_buffer[1] = 0x00;
    rx_buffer[0] = 0xA5;
    TEST_ASSERT_TRUE(UUT.read_cell_voltage_avg());
    TEST_ASSERT_EQUAL( 3300, UUT.get_cell_voltage(1));
    TEST_ASSERT_EQUAL( 4200, UUT.get_cell_voltage(2));
    ::read_array.assert_called_once_with(0xD3);
};

/// @brief test reading the remaining capacity
void test_read_remaining_capacity(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    MAX17205::Controller UUT(i2c);

    // perform test
    rx_buffer[3] = 0x00;
    rx_buffer[2] = 0x00;
    rx_buffer[1] = 0x0A;
    rx_buffer[0] = 0x00;
    TEST_ASSERT_TRUE(UUT.read_remaining_capacity());
    TEST_ASSERT_EQUAL( 10, UUT.get_remaining_capacity());
    ::read_word.assert_called_once_with(0x05);
};

/// @brief test reading the soc
void test_read_soc(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    MAX17205::Controller UUT(i2c);

    // perform test
    rx_buffer[3] = 0x00;
    rx_buffer[2] = 0x00;
    rx_buffer[1] = 0x00;
    rx_buffer[0] = 0x0B;
    TEST_ASSERT_TRUE(UUT.read_soc());
    TEST_ASSERT_EQUAL( 11, UUT.get_SOC());
    ::read_word.assert_called_once_with(0x06);
};

/// @brief test reading the time to empty
void test_read_TTE(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    MAX17205::Controller UUT(i2c);

    // perform test
    rx_buffer[3] = 0x00;
    rx_buffer[2] = 0x00;
    rx_buffer[1] = 0x03;
    rx_buffer[0] = 0x00;
    TEST_ASSERT_TRUE(UUT.read_TTE());
    TEST_ASSERT_EQUAL( 16, UUT.get_TTE());
    ::read_word.assert_called_once_with(0x11);
};

/// @brief test reading the time to full
void test_read_TTF(void)
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    MAX17205::Controller UUT(i2c);

    // perform test
    rx_buffer[3] = 0x00;
    rx_buffer[2] = 0x00;
    rx_buffer[1] = 0x03;
    rx_buffer[0] = 0x00;
    TEST_ASSERT_TRUE(UUT.read_TTF());
    TEST_ASSERT_EQUAL( 16, UUT.get_TTF());
    ::read_word.assert_called_once_with(0x20);
}

/// @brief Test the generic register read
void test_generic_read_register()
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    MAX17205::Controller UUT(i2c);

    // Read a register
    rx_buffer[3] = 0x00;
    rx_buffer[2] = 0x00;
    rx_buffer[1] = 0x0A;
    rx_buffer[0] = 0x00;
    MAX17205::percent value = UUT.read_register(0x0FF).value();

    // perform test
    TEST_ASSERT_EQUAL( 10, value);
    ::read_word.assert_called_once_with(0x0FF);
}

/// @brief Test the register classes
void test_register_classes()
{
    // *** PackCfg Class ***
    MAX17205::PackCfg pack_cfg;
    TEST_ASSERT_EQUAL_HEX16(MAX17205::Register::PackCfg, pack_cfg.address);
    TEST_ASSERT_EQUAL(0, pack_cfg.value);
    // Test setting and getting the NCELLS field
    TEST_ASSERT_EQUAL(0, pack_cfg.NCELLS());
    pack_cfg.set_NCELLS(2);
    TEST_ASSERT_EQUAL(2, pack_cfg.NCELLS());
    TEST_ASSERT_BITS(0b1111, 2, pack_cfg.value);
    // Test setting and getting the BALCFG field
    TEST_ASSERT_EQUAL(0, pack_cfg.BALCFG());
    pack_cfg.set_BALCFG(0b101);
    TEST_ASSERT_EQUAL(0b101, pack_cfg.BALCFG());
    TEST_ASSERT_BITS(0b11100000, 0b101 << 5, pack_cfg.value);
    // Other bits
    pack_cfg.value |= ( 1<< 8); // Set the CXEN bit
    TEST_ASSERT_TRUE(pack_cfg.CxEn());
    TEST_ASSERT_FALSE(pack_cfg.BtEn());
    TEST_ASSERT_FALSE(pack_cfg.ChEn());
    TEST_ASSERT_FALSE(pack_cfg.TdEn());
    TEST_ASSERT_FALSE(pack_cfg.A1En());
    TEST_ASSERT_FALSE(pack_cfg.A2En());
    TEST_ASSERT_FALSE(pack_cfg.FGT());

    // *** Config Class ***
    MAX17205::Config config;
    TEST_ASSERT_EQUAL_HEX16(MAX17205::Register::Config, config.address);
    // Test setting and getting the Aen bit
    TEST_ASSERT_FALSE(config.Aen());
    config.set_Aen(true);
    TEST_ASSERT_TRUE(config.Aen());
    TEST_ASSERT_BITS(0b100, 0b100, config.value);
    // Test setting and getting the ALRTp bit
    TEST_ASSERT_FALSE(config.ALRTp());
    config.set_ALRTp(true);
    TEST_ASSERT_TRUE(config.ALRTp());
    TEST_ASSERT_BIT_HIGH((1 << 11), config.value);

    // *** SAlrtTh Class ***
    MAX17205::SAlrtTh salrtth;
    TEST_ASSERT_EQUAL_HEX16(MAX17205::Register::SAlrtTh, salrtth.address);
    // Test setting and getting the minimum SoC threshold
    TEST_ASSERT_EQUAL(0, salrtth.SMIN());
    salrtth.set_SMIN(20);
    TEST_ASSERT_EQUAL(20, salrtth.SMIN());
    TEST_ASSERT_EQUAL(20, salrtth.value);
    // Test setting and getting the maximum SoC threshold
    TEST_ASSERT_EQUAL(0, salrtth.SMAX());
    salrtth.set_SMAX(80);
    TEST_ASSERT_EQUAL(80, salrtth.SMAX());
    TEST_ASSERT_BITS((0xFF << 8), (80 << 8), salrtth.value);
}

/// @brief Test reading register classes
void test_reading_register_classes()
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object and register
    MAX17205::Controller UUT(i2c);
    MAX17205::PackCfg pack_cfg;


    // Read a register
    rx_buffer[3] = 0x00;
    rx_buffer[2] = 0x00;
    rx_buffer[1] = 0x03;
    rx_buffer[0] = 0x00;
    TEST_ASSERT_TRUE(UUT.read(pack_cfg));
    TEST_ASSERT_EQUAL(3, pack_cfg.NCELLS());
    ::read_word.assert_called_once_with(pack_cfg.address);
}

/// @brief Test writing register classes
void test_writing_register_classes()
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object and register
    MAX17205::Controller UUT(i2c);
    MAX17205::PackCfg pack_cfg;

    // Write a register
    pack_cfg.set_NCELLS(2);
    ::set_target_address.reset();
    ::send_data.reset();
    TEST_ASSERT_TRUE(UUT.write(pack_cfg));
    ::set_target_address.assert_called_once_with(MAX17205::i2c_address_low);
    ::send_data.assert_called_once_with(0xBD0200);
}

/// === Run Tests ===
int main(int argc, char** argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_unit_capacity);
    RUN_TEST(test_unit_percentage);
    RUN_TEST(test_unit_current);
    RUN_TEST(test_unit_voltage);
    RUN_TEST(test_unit_seconds);
    RUN_TEST(test_init);
    RUN_TEST(test_write_register);
    RUN_TEST(test_read_register);
    RUN_TEST(test_read_battery_voltage);
    RUN_TEST(test_read_battery_current);
    RUN_TEST(test_read_cell_voltage);
    RUN_TEST(test_read_battery_current_avg);
    RUN_TEST(test_read_cell_voltage_avg);
    RUN_TEST(test_read_remaining_capacity);
    RUN_TEST(test_read_soc);
    RUN_TEST(test_read_TTE);
    RUN_TEST(test_read_TTF);
    RUN_TEST(test_generic_read_register);
    RUN_TEST(test_register_classes);
    RUN_TEST(test_reading_register_classes);
    RUN_TEST(test_writing_register_classes);
    return UNITY_END();
};
