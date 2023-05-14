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
 * @file    test_bq25700.cpp
 * @author  SO
 * @version v2.7.4
 * @date    14-November-2021
 * @brief   Unit tests to test the driver for battery charger.
 ******************************************************************************
 */

// === Includes ===
#include <unity.h>
#include <mock.h>
#include "battery/bq25700.h"

// === Fixtures ===

#include <array>
#include <optional>
#include "interface.h"

// === Fixtures ===

// Mock the i2c driver
struct I2C_Mock {
    unsigned char buffer_position{0};
};
Mock::Callable<bool> set_target_address;
Mock::Callable<bool> send_word;
Mock::Callable<bool> send_bytes;
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
        return ::send_bytes(temp.value);
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
        unsigned char pos = bus.buffer_position++;
        return (rx_buffer[pos + 1] << 8) | rx_buffer[pos];
    };
    bool read_array(I2C_Mock &bus, const unsigned char reg, unsigned char* dest, const unsigned char n_bytes)
    {
        std::copy(rx_buffer.begin(), rx_buffer.begin() + n_bytes, dest);
        return ::read_array(reg);
    };
};

// Include the UUT
#include "battery/bq25700.h"
#include "battery/bq25700.cpp"
template class BQ25700::Controller<I2C_Mock>;

/** === Test List ===
 * ▢ controller has the properties:
 *      ✓ measured system voltage in xx.x V
 *      ✓ measured input voltage in xx.x V
 *      ✓ measured input current (+/-) in x.xxx A
 *      ✓ OTG voltage (setpoint) in xx.x V
 *      ✓ OTG current (setpoint) in x.xxx A
 *      ✓ Charge current (setpoint) in x.xxx A
 * ✓ Controller checks whether target is responding during initialization of the controller.
 * ▢ Controller has states:
 *      ▢ Init
 *      ▢ Idle
 *      ▢ Charging
 *      ▢ OTG
 *      ▢ Error
 * ▢ controller can be assigned to the pins:
 *      ▢ EN_OTG (output)
 *      ▢ CHRG_OK (input)
 *      ▢ PROCHOT (input)
 * ▢ controller can set options:
 *      ▢ options registers
 *      ✓ Charge current
 *      ✓ OTG current
 *      ✓ OTG voltage
 * ▢ Controller can read the battery status
 *      ▢ whether it is charging
 *      ▢ whether OTG is enabled
 *      ▢ faults occurred
 * ▢ controller can read the adc values
 * ▢ controller can enable
 *      ▢ battery charging
 *      ▢ OTG
 *      ▢ Sleep mode
 */

void setUp() {
// set stuff up here
};

void tearDown() {
// clean stuff up here
};

// === Define Tests ===
/// @brief Test the register classes
void test_register_ChargeOption0()
{
    // Create register object
    BQ25700::ChargeOption0 reg;

    // Assert address and initial value
    TEST_ASSERT_EQUAL(BQ25700::Register::Charge_Option_0, reg.address);
    TEST_ASSERT_EQUAL(0x0000, reg.value);
    TEST_ASSERT_FALSE(reg.EN_OOA());

    // Assert setting the EN_OOA bit
    reg.set_EN_OOA(true);
    TEST_ASSERT_TRUE(reg.EN_OOA());
    TEST_ASSERT_BITS_HIGH((1<<10), reg.value);
}

/// @brief Test the constructor
void test_constructor()
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    BQ25700::Controller UUT(i2c);

    // perform testing
    TEST_ASSERT_EQUAL(0, UUT.get_system_voltage());
    TEST_ASSERT_EQUAL(0, UUT.get_input_voltage());
    TEST_ASSERT_EQUAL(0, UUT.get_input_current());
    TEST_ASSERT_EQUAL(4480, UUT.get_OTG_voltage()); // OTG reset voltage is 4.480 V
    TEST_ASSERT_EQUAL(0, UUT.get_OTG_current());
    TEST_ASSERT_EQUAL(0, UUT.get_charge_current());
};

/// @brief Test initializing the controller
void test_init()
{
    ::set_target_address.reset();
    // Setup the mocked i2c driver
    I2C_Mock i2c;
    rx_buffer[0] = BQ25700::manufacturer_id;
    rx_buffer[2] = BQ25700::device_id;

    // create the controller object
    BQ25700::Controller UUT(i2c);

    // perform testing
    TEST_ASSERT_TRUE(UUT.initialize());
    ::set_target_address.assert_called_once_with(BQ25700::i2c_address);
    TEST_ASSERT_EQUAL(2, ::read_word.call_count);
    TEST_ASSERT_EQUAL(0, static_cast<unsigned char>(UUT.get_state()));
};

/// @brief Test sending options
void test_set_options()
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object
    BQ25700::Controller UUT(i2c);

    // Test writting a register
    TEST_ASSERT_TRUE(UUT.write_register(BQ25700::Register::Charge_Option_0, 0x1234));
    ::send_bytes.assert_called_once_with(0x123412); // BQ25700 expects MSB last

    // Test setting the charge current
    TEST_ASSERT_TRUE(UUT.set_charge_current(1000));
    TEST_ASSERT_EQUAL(960U, UUT.get_charge_current());
    ::send_bytes.assert_called_once_with(0x14C003); // BQ25700 expects MSB last

    // Test setting the OTG voltage
    TEST_ASSERT_TRUE(UUT.set_OTG_voltage(5000));
    TEST_ASSERT_EQUAL(4992U, UUT.get_OTG_voltage());
    ::send_bytes.assert_called_once_with(0x3B0002); // BQ25700 expects MSB last

    // Test setting the OTG current
    TEST_ASSERT_TRUE(UUT.set_OTG_current(3300));
    TEST_ASSERT_EQUAL(0x4200U, UUT.get_OTG_current());
    ::send_bytes.assert_called_once_with(0x3C0042); // BQ25700 expects MSB last

    // Test enabling the OTG voltage
    TEST_ASSERT_TRUE(UUT.enable_OTG(true));
    TEST_ASSERT_EQUAL(3, static_cast<unsigned char>(UUT.get_state()));
    ::send_bytes.assert_called_once_with(0x320010); // BQ25700 expects MSB last
    // Test disabling the OTG voltage again
    TEST_ASSERT_TRUE(UUT.enable_OTG(false));
    TEST_ASSERT_EQUAL(1, static_cast<unsigned char>(UUT.get_state()));
    ::send_bytes.assert_called_once_with(0x320000); // BQ25700 expects MSB last
};
/// @brief Test reading register classes
void test_read_register()
{
    ::set_target_address.reset();
    ::read_word.reset();
    // Setup the mocked i2c driver
    I2C_Mock i2c;
    rx_buffer[0] = 0x22;
    rx_buffer[1] = 0x11;

    // create the controller object and register
    BQ25700::Controller UUT(i2c);
    BQ25700::ChargeOption0 reg;

    // perform testing
    TEST_ASSERT_TRUE(UUT.read(reg));
    // ::set_target_address.assert_called_once_with(BQ25700::i2c_address);
    TEST_ASSERT_EQUAL(1, ::read_word.call_count);
    TEST_ASSERT_EQUAL_HEX16(0x2211, reg.value);
}

/// @brief Test writing register classes
void test_write_register()
{
    // Setup the mocked i2c driver
    I2C_Mock i2c;

    // create the controller object and register
    BQ25700::Controller UUT(i2c);
    BQ25700::ChargeOption0 reg;
    reg.value = 0x1234;

    // Test writting a register
    TEST_ASSERT_TRUE(UUT.write(reg));
    ::send_bytes.assert_called_once_with(0x123412); // BQ25700 expects MSB last
}

/// @brief Main test function for BQ25700
int main(int argc, char** argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_register_ChargeOption0);
    RUN_TEST(test_constructor);
    RUN_TEST(test_init);
    RUN_TEST(test_set_options);
    RUN_TEST(test_read_register);
    RUN_TEST(test_write_register);
    return UNITY_END();
};