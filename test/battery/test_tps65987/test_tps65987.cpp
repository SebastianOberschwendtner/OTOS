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
 * @file    test_tps65987.cpp
 * @author  SO
 * @version v5.0.0
 * @date    14-November-2021
 * @brief   Unit tests to test the driver for USB-3 PD controller.
 ==============================================================================
 */

/* === Includes === */
#include <unity.h>
#include <mock.h>
#include <array>
#include <unordered_map>
#include "interface.h" /* NOLINT */

/* === Fixtures === */
enum class IO {
   SYSTEM_ = 0, 
   I2C_1 
};

/* Mock the i2c driver */
/* NOLINTBEGIN */
struct I2C_Mock : public driver::Base<IO>
{
};
Mock::Callable<bool> set_target_address;
Mock::Callable<bool> send_word;
Mock::Callable<bool> send_array;
Mock::Callable<bool> send_array_leader;
Mock::Callable<bool> read_array;
std::array<std::uint8_t, 66> rx_buffer{0};

namespace bus
{
    /**
     * @attention The rx_buffer represents the written buffer verbatim!
     * This means that the first byte of the buffer is the register address
     * when sending a word or array. When reading an array, the first byte
     * is the first received byte with the register address!
     */
    void change_address(I2C_Mock bus, const std::uint8_t address)
    {
        ::set_target_address(address);
        return;
    }
    bool send_word(I2C_Mock bus, uint16_t word)
    {
        return ::send_word(word);
    }
    bool send_array(I2C_Mock bus, std::uint8_t *const data, const std::uint8_t n_bytes)
    {
        std::copy(data, data + n_bytes, rx_buffer.begin());
        return ::send_array(n_bytes);
    }
    bool send_array_leader(
        I2C_Mock bus,
        const std::uint8_t byte,
        std::uint8_t *const data,
        const std::uint8_t n_bytes)
    {
        return ::send_array_leader(byte, data, n_bytes);
    }
    bool read_array(I2C_Mock bus, const std::uint8_t reg, std::uint8_t *dest, const std::uint8_t n_bytes)
    {
        std::copy(rx_buffer.begin(), rx_buffer.begin() + n_bytes, dest);
        return ::read_array(n_bytes);
    }
}; /* namespace Bus */
/* NOLINTEND */

/* Include the UUT */
#include "battery/tps65987.h"
#include "battery/tps65987.cpp" /* NOLINT */
template class tps65987::Controller<I2C_Mock>;

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

void setUp()
{
    set_target_address.reset();
    send_word.reset();
    send_array.reset();
    send_array_leader.reset();
    read_array.reset();
}

void tearDown()
{
    /* clean stuff up here */
}

/* === Define Tests === */
/** 
 * @brief Test the constructor
 */
void test_init()
{
    /* Setup the mocked i2c driver */
    const I2C_Mock i2c;

    /* create the controller object */
    const tps65987::Controller UUT(i2c);

    /* perform testing */
    TEST_ASSERT_EQUAL(0, static_cast<std::uint8_t>(UUT.get_mode()));
    char Expected_Command[] = {0, 0, 0, 0}; /* NOLINT */
    TEST_ASSERT_EQUAL_CHAR_ARRAY(Expected_Command, UUT.get_active_command(), 4);
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().role);
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().USB_type);
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().voltage);
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().current);
}

/** 
 * @brief Test reading a register with variable length
 */
void test_read_register()
{
    /* Setup the mocked i2c driver */
    const I2C_Mock i2c;

    /* create the controller object */
    tps65987::Controller UUT(i2c);

    /* perform testing */
    TEST_ASSERT_TRUE(UUT.read_register(tps65987::registers::Data1));
    ::read_array.assert_called_once_with(65);

    TEST_ASSERT_TRUE(UUT.read_register(tps65987::registers::Cmd1));
    ::read_array.assert_called_once_with(5);
}

/** 
 * @brief Test writing a register with variable length
 */
void test_write_register()
{
    /* Setup the mocked i2c driver */
    const I2C_Mock i2c;

    /* create the controller object */
    tps65987::Controller UUT(i2c);

    /* perform testing */
    TEST_ASSERT_TRUE(UUT.write_register(tps65987::registers::Data1));
    ::send_array.assert_called_once_with(66);

    TEST_ASSERT_TRUE(UUT.write_register(tps65987::registers::Cmd1));
    ::send_array.assert_called_once_with(6);
}

/** 
 * @brief Test reading a command status
 */
void test_read_active_command()
{
    /* Setup the mocked i2c driver */
    const I2C_Mock i2c;

    /* create the controller object */
    tps65987::Controller UUT(i2c);

    /* perform testing */
    rx_buffer[0] = '!';
    rx_buffer[1] = 'D';
    rx_buffer[2] = 'M';
    rx_buffer[3] = 'C';
    TEST_ASSERT_TRUE(UUT.read_active_command());
    ::read_array.assert_called_once_with(5);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("CMD!", UUT.get_active_command(), 4);
}

/** 
 * @brief Test writting a command
 */
void test_write_command()
{
    /* Setup the mocked i2c driver */
    const I2C_Mock i2c;

    /* create the controller object */
    tps65987::Controller UUT(i2c);

    /* perform testing */
    TEST_ASSERT_TRUE(UUT.write_command("PTCc"));
    TEST_ASSERT_EQUAL_CHAR_ARRAY("PTCc", &rx_buffer[2], 4);
}

/** 
 * @brief Test reading the current mode of the controller
 */
void test_read_mode()
{
    /* Setup the mocked i2c driver */
    const I2C_Mock i2c;

    /* create the controller object */
    tps65987::Controller UUT(i2c);

    /* perform testing */
    rx_buffer[0] = 'H';
    rx_buffer[1] = 'C';
    rx_buffer[2] = 'T';
    rx_buffer[3] = 'P';
    TEST_ASSERT_TRUE(UUT.read_mode());
    ::read_array.assert_called_once_with(5);
    TEST_ASSERT_EQUAL(1, static_cast<std::uint8_t>(UUT.get_mode()));
}

/** 
 * @brief Test the correct initialization of the controller
 */
void test_initialization()
{
    /* Setup the mocked i2c driver */
    const I2C_Mock i2c;

    /* create the controller object */
    tps65987::Controller UUT(i2c);

    /* perform testing */
    /* when controller is in patch mode -> exit by sending "PTCc" command */
    rx_buffer[0] = 'H';
    rx_buffer[1] = 'C';
    rx_buffer[2] = 'T';
    rx_buffer[3] = 'P';
    TEST_ASSERT_TRUE(UUT.initialize());
    ::set_target_address.assert_called_once_with(tps65987::i2c_address);
    ::read_array.assert_called_once_with(5);
    TEST_ASSERT_EQUAL_CHAR_ARRAY("PTCc", &rx_buffer[2], 4);
}

/** 
 * @brief Test the reading of the PD status
 */
void test_read_PD_status()
{
    /* Setup the mocked i2c driver */
    const I2C_Mock i2c;

    /* create the controller object */
    tps65987::Controller UUT(i2c);

    /* Response with all zeros */
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

    /* Plug type is USB2.0 */
    rx_buffer[0] = 0x00;
    rx_buffer[1] = 0x00;
    rx_buffer[2] = 0x00;
    rx_buffer[3] = tps65987::PlugDetails_0;
    TEST_ASSERT_TRUE(UUT.read_PD_status());
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().role);
    TEST_ASSERT_EQUAL(2, UUT.get_active_contract().USB_type);
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().voltage);
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().current);

    /* USB default current */
    rx_buffer[0] = 0x00;
    rx_buffer[1] = 0x00;
    rx_buffer[2] = 0x00;
    rx_buffer[3] = tps65987::CCPullUp_0;
    TEST_ASSERT_TRUE(UUT.read_PD_status());
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().role);
    TEST_ASSERT_EQUAL(3, UUT.get_active_contract().USB_type);
    TEST_ASSERT_EQUAL(5000, UUT.get_active_contract().voltage);
    TEST_ASSERT_EQUAL(900, UUT.get_active_contract().current);

    /* USB 1.5A */
    rx_buffer[0] = 0x00;
    rx_buffer[1] = 0x00;
    rx_buffer[2] = 0x00;
    rx_buffer[3] = tps65987::CCPullUp_1;
    TEST_ASSERT_TRUE(UUT.read_PD_status());
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().role);
    TEST_ASSERT_EQUAL(3, UUT.get_active_contract().USB_type);
    TEST_ASSERT_EQUAL(5000, UUT.get_active_contract().voltage);
    TEST_ASSERT_EQUAL(1500, UUT.get_active_contract().current);

    /* USB 3.0A */
    rx_buffer[0] = 0x00;
    rx_buffer[1] = 0x00;
    rx_buffer[2] = 0x00;
    rx_buffer[3] = tps65987::CCPullUp_1 | tps65987::CCPullUp_0;
    TEST_ASSERT_TRUE(UUT.read_PD_status());
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().role);
    TEST_ASSERT_EQUAL(3, UUT.get_active_contract().USB_type);
    TEST_ASSERT_EQUAL(5000, UUT.get_active_contract().voltage);
    TEST_ASSERT_EQUAL(3000, UUT.get_active_contract().current);

    /* Role source */
    rx_buffer[0] = 0x00;
    rx_buffer[1] = 0x00;
    rx_buffer[2] = 0x00;
    rx_buffer[3] = tps65987::PresentRole;
    TEST_ASSERT_TRUE(UUT.read_PD_status());
    TEST_ASSERT_EQUAL(1, UUT.get_active_contract().role);
    TEST_ASSERT_EQUAL(3, UUT.get_active_contract().USB_type);
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().voltage);
    TEST_ASSERT_EQUAL(0, UUT.get_active_contract().current);
}

/** 
 * @brief Test the reading of the status
 */
void test_read_status()
{
    /* Setup the mocked i2c driver */
    const I2C_Mock i2c;

    /* create the controller object */
    tps65987::Controller UUT(i2c);

    /* Response with all zeros */
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
    ::read_array.assert_called_once_with(8 + 1);
}

/** 
 * @brief Test the PDO class for USB PD contracts
 */
void test_PDO_class()
{
    /* Create a PDO object */
    tps65987::PDO pdo;

    /* Test the default constructor */
    TEST_ASSERT_EQUAL(0, pdo.voltage());
    TEST_ASSERT_EQUAL(0, pdo.current());
    TEST_ASSERT_EQUAL(0, pdo.get_data());
    TEST_ASSERT_EQUAL(tps65987::PDO::Fixed_Supply, pdo.type());

    /* Test setting the voltage */
    pdo.set_voltage(5000);
    TEST_ASSERT_EQUAL(0x19000, pdo.get_data());
    TEST_ASSERT_EQUAL(5000, pdo.voltage());

    /* Test setting the current */
    pdo.set_current(3000);
    TEST_ASSERT_EQUAL(0x1912C, pdo.get_data());
    TEST_ASSERT_EQUAL(3000, pdo.current());

    /* Test constructor with data */
    tps65987::PDO pdo2{(0b11UL << 30) | 0x1912CUL};
    TEST_ASSERT_EQUAL(tps65987::PDO::APDO, pdo2.type());

    /* Test copy constructor */
    const tps65987::PDO pdo3{pdo};
    TEST_ASSERT_EQUAL(5000, pdo3.voltage());
    TEST_ASSERT_EQUAL(3000, pdo3.current());

    /* Test assignment operator */
    tps65987::PDO pdo4;
    pdo4 = 0x1912CUL;
    TEST_ASSERT_EQUAL(5000, pdo4.voltage());
    TEST_ASSERT_EQUAL(3000, pdo4.current());

    /* Test getting the voltage of a variable supply */
    pdo2 = (0b10UL << 30) | (0x190UL << 20);
    TEST_ASSERT_EQUAL(tps65987::PDO::Variable_Supply, pdo2.type());
    TEST_ASSERT_EQUAL(20000, pdo2.voltage());
}

/** 
 * @brief Test the reading of the active contract PDO
 */
void test_read_active_pdo()
{
    /* Setup the mocked i2c driver */
    const I2C_Mock i2c;

    /* create the controller object */
    tps65987::Controller UUT(i2c);

    /* Response with all zeros */
    rx_buffer[0] = 0x66;
    rx_buffer[1] = 0x55;
    rx_buffer[2] = 0x44;
    rx_buffer[3] = 0x33;
    rx_buffer[4] = 0x22;
    rx_buffer[5] = 0x11;
    auto response = UUT.read_active_pdo();
    TEST_ASSERT_TRUE(response);
    TEST_ASSERT_EQUAL_HEX(0x44332211, response.value().get_data());
    ::read_array.assert_called_once_with(6 + 1);
}

/** 
 * @brief Test the reading of the TX sink capabilities PDOs
 */
void test_read_TX_sink_capability()
{
    /* Setup the mocked i2c driver */
    const I2C_Mock i2c;

    /* create the controller object */
    tps65987::Controller UUT(i2c);

    /* Set up response */
    rx_buffer[52] = 0x55;
    rx_buffer[53] = 0x44;
    rx_buffer[54] = 0x33;
    rx_buffer[55] = 0x22;
    rx_buffer[56] = 0x11;
    auto response = UUT.read_TX_sink_pdo(0);
    TEST_ASSERT_TRUE(response);
    TEST_ASSERT_EQUAL_HEX(0x55443322, response.value().get_data());
    ::read_array.assert_called_once_with(57 + 1);
}

/** 
 * @brief Test the status register class
 */
void test_class_status()
{
    /* Create empty register */
    tps65987::Status status;

    /* Test getting the underlying data array */
    auto &data = status.get_data();
    TEST_ASSERT_EQUAL(tps65987::registers::Status.length, data.size());

    /* Test reading the register options */
    TEST_ASSERT_EQUAL(tps65987::registers::Status.address, status.address);
    TEST_ASSERT_EQUAL(tps65987::registers::Status.length, status.length);

    /* Test reading the plug present bit */
    TEST_ASSERT_FALSE(status.PlugPresent());
    data[0] = 0x01;
    TEST_ASSERT_TRUE(status.PlugPresent());

    /* Test reading the ConnState property */
    TEST_ASSERT_EQUAL(0, status.ConnState());
    data[0] |= 0x02 << 1;
    TEST_ASSERT_EQUAL(2, status.ConnState());

    /* Test reading the port role */
    TEST_ASSERT_FALSE(status.PortRole());
    data[0] |= 0x01 << 5;
    TEST_ASSERT_TRUE(status.PortRole());

    /* Test reading the Vbus status */
    TEST_ASSERT_EQUAL(0, status.VbusStatus());
    data[2] |= 0b10 << 4;
    TEST_ASSERT_EQUAL(0b10, status.VbusStatus());
}

/** 
 * @brief Test the power path status register class
 */
void test_class_power_path_status()
{
    /* Create empty register */
    tps65987::PowerPathStatus power_path_status;

    /* Test getting the underlying data array */
    auto &data = power_path_status.get_data();

    /* Test reading the register options */
    TEST_ASSERT_EQUAL(tps65987::registers::Power_Path_Status.length, data.size());

    /* Test reading the PP1_CABLEswitch status */
    TEST_ASSERT_EQUAL(0, power_path_status.PP1_CABLEswitch());
    data[0] = 0b10;
    TEST_ASSERT_EQUAL(2, power_path_status.PP1_CABLEswitch());

    /* Test reading the PP2_CABLEswitch status */
    TEST_ASSERT_EQUAL(0, power_path_status.PP2_CABLEswitch());
    data[0] |= 0b0100;
    TEST_ASSERT_EQUAL(1, power_path_status.PP2_CABLEswitch());

    /* Test reading the PP1switch status */
    TEST_ASSERT_EQUAL(0, power_path_status.PP1switch());
    data[0] |= (1 << 6);
    TEST_ASSERT_EQUAL(1, power_path_status.PP1switch());

    /* Test reading the PP2switch status */
    TEST_ASSERT_EQUAL(0, power_path_status.PP2switch());
    data[1] |= (1 << 1);
    TEST_ASSERT_EQUAL(1, power_path_status.PP2switch());
}

/** 
 * @brief Test the specialized register classes
 */
void test_class_global_system_configuration()
{
    /* Create empty register */
    tps65987::GlobalConfiguration global_config;

    /* Test getting the underlying data array */
    auto data = global_config.get_data();
    TEST_ASSERT_EQUAL(tps65987::registers::Global_SysConfig.length, data.size());

    /* Test reading the register options */
    TEST_ASSERT_EQUAL(tps65987::registers::Global_SysConfig.address, global_config.address);
    TEST_ASSERT_EQUAL(tps65987::registers::Global_SysConfig.length, global_config.length);
    TEST_ASSERT_EQUAL(0, global_config.PP1_CABLEconfig());
    TEST_ASSERT_EQUAL(0, global_config.PP2_CABLEconfig());
    TEST_ASSERT_EQUAL(0, global_config.PP1config());
    TEST_ASSERT_EQUAL(0, global_config.PP2config());

    /* Test setting the register options */
    global_config.set_PP1_CABLEconfig(1);
    global_config.set_PP2_CABLEconfig(1);
    global_config.set_PP1config(1);
    global_config.set_PP2config(1);
    TEST_ASSERT_EQUAL(1, global_config.PP1_CABLEconfig());
    TEST_ASSERT_EQUAL(1, global_config.PP2_CABLEconfig());
    TEST_ASSERT_EQUAL(1, global_config.PP1config());
    TEST_ASSERT_EQUAL(1, global_config.PP2config());
    TEST_ASSERT_EQUAL(0b1001, global_config.get_data().at(2));

    /* Test copying an array into the register */
    std::array<uint8_t, 14> raw{0};
    raw.fill(0x01);
    std::copy(raw.begin(), raw.end(), global_config.get_data().begin());
    TEST_ASSERT_EQUAL(0x01, global_config.get_data().at(0));
    TEST_ASSERT_EQUAL(0x01, global_config.get_data().at(13));
}

/** 
 * @brief Test reading the Global System Configuration Register
 */
void test_reading_global_system_configuration()
{
    /* Setup the mocked i2c driver */
    const I2C_Mock i2c;

    /* create the controller object */
    tps65987::Controller UUT(i2c);

    /* Set up response */
    rx_buffer[11] = 0x01; /* Byte 2 */
    rx_buffer[12] = 0x00; /* Byte 1 */
    rx_buffer[13] = 0x00; /* Byte 0 */
    rx_buffer[14] = 14;   /* Length */

    /* Read the register */
    tps65987::GlobalConfiguration global_config;
    auto response = UUT.read(global_config);

    /* Test the register options */
    TEST_ASSERT_TRUE(response);
    TEST_ASSERT_EQUAL(1, global_config.PP1config());
    TEST_ASSERT_EQUAL(0, global_config.PP2config());
}

/** 
 * @brief Test writing the Global System Configuration Register
 */
void test_writing_global_system_configuration()
{
    /* Setup the mocked i2c driver */
    const I2C_Mock i2c;

    /* create the controller object */
    tps65987::Controller UUT(i2c);

    /* Set and write the register content */
    tps65987::GlobalConfiguration global_config;
    global_config.set_PP2config(3);
    auto response = UUT.write(global_config);

    /* Test the content of the send data */
    TEST_ASSERT_TRUE(response);
    TEST_ASSERT_EQUAL(0x27, rx_buffer.at(0));
    TEST_ASSERT_EQUAL(14, rx_buffer.at(1));
    TEST_ASSERT_EQUAL(0, rx_buffer.at(2));
    TEST_ASSERT_EQUAL(0, rx_buffer.at(3));
    TEST_ASSERT_EQUAL((3 << 3), rx_buffer.at(4));
}

/** 
 * @brief Test the port configuration register class
 */
void test_class_port_configuration()
{
    /* Create empty register */
    tps65987::PortConfiguration port_config;

    /* Test getting the underlying data array */
    auto data = port_config.get_data();
    TEST_ASSERT_EQUAL(tps65987::registers::Port_Config.length, data.size());

    /* Test reading the register options */
    TEST_ASSERT_EQUAL(tps65987::registers::Port_Config.address, port_config.address);
    TEST_ASSERT_EQUAL(tps65987::registers::Port_Config.length, port_config.length);
    TEST_ASSERT_EQUAL(0, port_config.TypeCStateMachine());
    TEST_ASSERT_EQUAL(0, port_config.ReceptacleType());
    TEST_ASSERT_EQUAL(0, port_config.VCONNsupported());

    /* Test setting the register options */
    port_config.set_TypeCStateMachine(1);
    port_config.set_ReceptacleType(1);
    port_config.set_VCONNsupported(1);
    TEST_ASSERT_EQUAL(1, port_config.TypeCStateMachine());
    TEST_ASSERT_EQUAL(1, port_config.ReceptacleType());
    TEST_ASSERT_EQUAL(1, port_config.VCONNsupported());
}

/** 
 * @brief Test the port control register class
 */
void test_class_port_control()
{
    /* Create empty register */
    tps65987::PortControl port_control;

    /* Test getting the underlying data array */
    auto data = port_control.get_data();
    TEST_ASSERT_EQUAL(tps65987::registers::Port_Ctrl.length, data.size());
    TEST_ASSERT_EQUAL(0, port_control.TypeCCurrent());
    TEST_ASSERT_EQUAL(0, port_control.ChargerAdvertiseEnable());
    TEST_ASSERT_EQUAL(0, port_control.ChargerDetectEnable());

    /* Test reading the register options */
    TEST_ASSERT_EQUAL(tps65987::registers::Port_Ctrl.address, port_control.address);
    TEST_ASSERT_EQUAL(tps65987::registers::Port_Ctrl.length, port_control.length);

    /* Test setting the register options */
    port_control.set_TypeCCurrent(1);
    TEST_ASSERT_EQUAL(1, port_control.TypeCCurrent());
    port_control.set_ChargerAdvertiseEnable(0b010);
    TEST_ASSERT_EQUAL(0b010, port_control.ChargerAdvertiseEnable());
    TEST_ASSERT_BITS(0b11100, (0b010 << 2), port_control.get_data().at(3));
    port_control.set_ChargerDetectEnable(1);
    TEST_ASSERT_EQUAL(1, port_control.ChargerDetectEnable());
    TEST_ASSERT_BITS(0b11000000, 0b01000000, port_control.get_data().at(3));
}

/** 
 * @brief Test setting the source capabilities PDOs
 */
void test_write_TX_source_PDOs()
{
    /* Create an array with the PDOs */
    tps65987::Capability capability{{}, 1};
    capability.first.set_voltage(5000);
    capability.second = 2;

    /* Setup the mocked i2c driver */
    const I2C_Mock i2c;

    /* create the controller object */
    tps65987::Controller UUT(i2c);

    /* Register the first PDO */
    UUT.register_TX_source_capability(capability);

    /* Test the correct data in the outgoing buffer */
    UUT.write_register(tps65987::registers::TX_Source_Cap);
    TEST_ASSERT_EQUAL(1, rx_buffer[2]);
    TEST_ASSERT_EQUAL(2, rx_buffer[6]);

    /* Get the PDO back from the buffer */
    const tps65987::PDO pdo{static_cast<uint32_t>(
        (rx_buffer[13] << 24) |
        (rx_buffer[12] << 16) |
        (rx_buffer[11] << 8) |
        rx_buffer[10])};
    TEST_ASSERT_EQUAL(5000, pdo.voltage());

    /* Register the same PDO again with different voltage */
    capability.first.set_voltage(10000);
    UUT.register_TX_source_capability(capability);

    /* Test the correct data in the outgoing buffer */
    UUT.write_register(tps65987::registers::TX_Source_Cap);
    TEST_ASSERT_EQUAL(2, rx_buffer[2]);
    TEST_ASSERT_EQUAL(0b1010, rx_buffer[6]);

    /* Get the PDO back from the buffer */
    const tps65987::PDO pdo2{static_cast<uint32_t>(
        (rx_buffer[17] << 24) |
        (rx_buffer[16] << 16) |
        (rx_buffer[15] << 8) |
        rx_buffer[14])};
    TEST_ASSERT_EQUAL(10000, pdo2.voltage());
}

/** 
 * === Run Tests ===
 */
int main()
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
    RUN_TEST(test_read_active_pdo);
    RUN_TEST(test_read_TX_sink_capability);
    RUN_TEST(test_class_status);
    RUN_TEST(test_class_power_path_status);
    RUN_TEST(test_class_global_system_configuration);
    RUN_TEST(test_reading_global_system_configuration);
    RUN_TEST(test_writing_global_system_configuration);
    RUN_TEST(test_class_port_configuration);
    RUN_TEST(test_class_port_control);
    RUN_TEST(test_write_TX_source_PDOs);
    return UNITY_END();
}
