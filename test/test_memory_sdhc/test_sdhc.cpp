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
 * @file    test_sdhc.cpp
 * @author  SO
 * @version v2.4.0
 * @date    29-December-2021
 * @brief   Unit tests for testing the SDHC interface.
 ******************************************************************************
 */

// === Includes ===
#include <unity.h>
#include <mock.h>
#include <array>
#include "memory/sdhc.h"

/** === Test List ===

*/

// === Fixtures ===
struct Mock_SDIO : public SDHC::interface
{
    Mock::Callable<bool> call_command_no_response;
    Mock::Callable<bool> call_command_R1_response;
    Mock::Callable<bool> call_command_R2_response;
    Mock::Callable<bool> call_command_R3_response;
    Mock::Callable<bool> call_command_R6_response;
    Mock::Callable<bool> call_command_R7_response;
    Mock::Callable<bool> call_read_single_block;
    Mock::Callable<bool> call_write_single_block;
    unsigned long last_argument = 0;
    unsigned long R1_response = 0;
    unsigned long R2_response = 0;
    unsigned long R3_response = 0;
    unsigned long R6_response = 0;
    unsigned long R7_response = 0;
    bool send_command_no_response(const unsigned char command, const unsigned long argument) override
    {
        last_argument = argument;
        return call_command_no_response(command);
    };
    std::optional<unsigned long> send_command_R1_response(const unsigned char command, const unsigned long argument) override
    {
        last_argument = argument;
        call_command_R1_response.add_call(command);
        return this->R1_response;
    };
    std::optional<unsigned long> send_command_R2_response(const unsigned char command, const unsigned long argument) override
    {
        last_argument = argument;
        call_command_R2_response.add_call(command);
        return this->R2_response;
    };
    std::optional<unsigned long> send_command_R3_response(const unsigned char command, const unsigned long argument) override
    {
        last_argument = argument;
        call_command_R3_response.add_call(command);
        return this->R3_response;
    };
    std::optional<unsigned long> send_command_R6_response(const unsigned char command, const unsigned long argument) override
    {
        last_argument = argument;
        call_command_R6_response.add_call(command);
        return this->R6_response;
    };
    std::optional<unsigned long> send_command_R7_response(const unsigned char command, const unsigned long argument) override
    {
        last_argument = argument;
        call_command_R7_response.add_call(command);
        return this->R7_response;
    };
    bool read_single_block(const unsigned long* buffer_begin, const unsigned long* buffer_end) override
    {
        return call_read_single_block(reinterpret_cast<int>(buffer_begin));
    };
    bool write_single_block(const unsigned long* buffer_begin, const unsigned long* buffer_end) override
    {
        return call_write_single_block(reinterpret_cast<int>(buffer_begin));
    };
};
Mock_SDIO sdio;

// === Tests ===
void setUp(void) {
    // set stuff up here
    sdio.last_argument = 0;
    sdio.R1_response = 0;
    sdio.R2_response = 0;
    sdio.R3_response = 0;
    sdio.R6_response = 0;
    sdio.R7_response = 0;
    sdio.call_command_no_response.reset();
    sdio.call_command_R1_response.reset();
    sdio.call_command_R2_response.reset();
    sdio.call_command_R3_response.reset();
    sdio.call_command_R6_response.reset();
    sdio.call_command_R7_response.reset();
    sdio.call_read_single_block.reset();
    sdio.call_write_single_block.reset();
};

void tearDown(void) {
    // clean stuff up here
};

/// @brief Test constructing a new SD_Card
void test_constructor(void)
{
    // Create Card
    setUp();
    SDHC::Card UUT(sdio);

    // Test members
    TEST_ASSERT_EQUAL(SDHC::State::Identification, UUT.state );
    TEST_ASSERT_EQUAL(0, UUT.RCA );
    TEST_ASSERT_TRUE( UUT.is_SDSC() );
};

/// @brief Test resetting the card
void test_reset(void)
{
    // Create Card
    setUp();
    SDHC::Card UUT(sdio);

    // Test the reset
    TEST_ASSERT_TRUE( UUT.reset() );
    TEST_ASSERT_EQUAL(0, sdio.last_argument);
    sdio.call_command_no_response.assert_called_once_with(SDHC::CMD<0>());
};

/// @brief Test setting the power supply voltage
void test_set_supply_voltage(void)
{
    // Create Card
    setUp();
    SDHC::Card UUT(sdio);
    sdio.R7_response = SDHC::CHECK_PATTERN;

    // Test setting the supply voltage when card does respond
    TEST_ASSERT_TRUE( UUT.set_supply_voltage() );
    TEST_ASSERT_EQUAL(SDHC::CMD8::Voltage_0 | SDHC::CHECK_PATTERN, sdio.last_argument);
    sdio.call_command_R7_response.assert_called_once_with(SDHC::CMD<8>());

    // Test setting the supply voltage when card does not respond
    sdio.R7_response = 0;
    TEST_ASSERT_FALSE( UUT.set_supply_voltage() );
    TEST_ASSERT_EQUAL(SDHC::CMD8::Voltage_0 | SDHC::CHECK_PATTERN, sdio.last_argument);
    sdio.call_command_R7_response.assert_called_once_with(SDHC::CMD<8>());
    TEST_ASSERT_EQUAL(SDHC::State::Disconnected, UUT.state );
};

/// @brief Test the card initialization commnad
void test_initialize_card(void)
{
    // Create Card
    setUp();
    SDHC::Card UUT(sdio);

    // Test the successfull initialization of SDSC card
    sdio.R1_response = SDHC::R1::APP_CMD;
    sdio.R3_response = 0;
    TEST_ASSERT_TRUE( UUT.initialize_card() );
    sdio.call_command_R1_response.assert_called_once_with(SDHC::CMD<55>() );
    sdio.call_command_R3_response.assert_called_once_with(SDHC::ACMD<41>() );
    TEST_ASSERT_EQUAL( SDHC::ACMD41::HCS | SDHC::ACMD41::XPC | SDHC::OCR::_3_0V, sdio.last_argument);
    TEST_ASSERT_EQUAL( SDHC::State::Identification, UUT.state );
    TEST_ASSERT_TRUE( UUT.is_SDSC() );

    // Test the successfull initialization of SDSC card
    setUp();
    sdio.R1_response = SDHC::R1::APP_CMD;
    sdio.R3_response = SDHC::R3::CCS;
    TEST_ASSERT_TRUE( UUT.initialize_card() );
    sdio.call_command_R1_response.assert_called_once_with(SDHC::CMD<55>() );
    sdio.call_command_R3_response.assert_called_once_with(SDHC::ACMD<41>() );
    TEST_ASSERT_EQUAL( SDHC::ACMD41::HCS | SDHC::ACMD41::XPC | SDHC::OCR::_3_0V, sdio.last_argument);
    TEST_ASSERT_EQUAL( SDHC::State::Identification, UUT.state );
    TEST_ASSERT_FALSE( UUT.is_SDSC() );

    // Test sending the command while card is busy
    setUp();
    sdio.R1_response = SDHC::R1::APP_CMD;
    sdio.R3_response = SDHC::R3::BUSY;
    TEST_ASSERT_FALSE( UUT.initialize_card() );
    sdio.call_command_R1_response.assert_called_once_with(SDHC::CMD<55>() );
    sdio.call_command_R3_response.assert_called_once_with(SDHC::ACMD<41>() );
    TEST_ASSERT_EQUAL( SDHC::ACMD41::HCS | SDHC::ACMD41::XPC | SDHC::OCR::_3_0V, sdio.last_argument);
    TEST_ASSERT_EQUAL( SDHC::State::Identification, UUT.state );

    // Test sending the command when card does not accept the command
    setUp();
    sdio.R1_response = 0;
    sdio.R3_response = 0;
    TEST_ASSERT_FALSE( UUT.initialize_card() );
    sdio.call_command_R1_response.assert_called_once_with(SDHC::CMD<55>() );
    TEST_ASSERT_EQUAL(0 ,sdio.call_command_R3_response.call_count);
    TEST_ASSERT_EQUAL( 0 , sdio.last_argument);
    TEST_ASSERT_EQUAL( SDHC::State::Disconnected, UUT.state );
};

/// @brief Test getting the RCA (Relative Card Address)
void test_get_RCA(void)
{
    // Create Card
    setUp();
    SDHC::Card UUT(sdio);

    // Test successfull read of RCA
    sdio.R6_response = (0xAB << 16);
    TEST_ASSERT_TRUE( UUT.get_RCA() );
    sdio.call_command_R2_response.assert_called_once_with(SDHC::CMD<2>() );
    sdio.call_command_R6_response.assert_called_once_with(SDHC::CMD<3>() );
    TEST_ASSERT_EQUAL(0xAB, UUT.RCA );
    TEST_ASSERT_EQUAL( SDHC::State::StandBy, UUT.state );
};

/// @brief Test selecting a card using its RCA
void test_select_card(void)
{
    // Create Card
    setUp();
    SDHC::Card UUT(sdio);

    // Test successfull selecting card
    sdio.R1_response = 0;
    UUT.RCA = 0xAB;
    TEST_ASSERT_TRUE( UUT.select() );
    sdio.call_command_R1_response.assert_called_once_with(SDHC::CMD<7>() );
    TEST_ASSERT_EQUAL( 0xAB << 16, sdio.last_argument);
    TEST_ASSERT_EQUAL( SDHC::State::Transfering, UUT.state );

    // Test selecting card with error
    sdio.R1_response = SDHC::R1::ERROR;
    UUT.RCA = 0xAB;
    UUT.state = SDHC::State::StandBy;
    TEST_ASSERT_FALSE( UUT.select() );
    sdio.call_command_R1_response.assert_called_once_with(SDHC::CMD<7>() );
    TEST_ASSERT_EQUAL( 0xAB << 16, sdio.last_argument);
    TEST_ASSERT_EQUAL( SDHC::State::StandBy, UUT.state );
};

/// @brief Test changing the bus width to 4bits
void test_change_bus_width(void)
{
    // Create Card
    setUp();
    SDHC::Card UUT(sdio);

    // Test the successfull change of the bus width
    sdio.R1_response = SDHC::R1::APP_CMD;
    TEST_ASSERT_TRUE( UUT.set_bus_width_4bits() );
    sdio.call_command_R1_response.assert_called_last_with(SDHC::ACMD<6>() );
    TEST_ASSERT_EQUAL( 0b10, sdio.last_argument);

    // Test the successfull change of the bus width
    sdio.R1_response = SDHC::R1::APP_CMD | SDHC::R1::ERROR;
    TEST_ASSERT_FALSE( UUT.set_bus_width_4bits() );
    sdio.call_command_R1_response.assert_called_last_with(SDHC::ACMD<6>() );
    TEST_ASSERT_EQUAL( 0b10, sdio.last_argument);
};

/// @brief Test ejecting the card
void test_eject(void)
{
    // Create Card
    setUp();
    SDHC::Card UUT(sdio);

    // Test ejecting the card when it can be ejected
    UUT.state = SDHC::State::Transfering;
    UUT.RCA = 0xDE;
    UUT.eject();
    sdio.call_command_no_response.assert_called_once_with( SDHC::CMD<15>() );
    TEST_ASSERT_EQUAL( 0xDE << 16, sdio.last_argument );
    TEST_ASSERT_EQUAL( SDHC::State::Disconnected, UUT.state );

    // Test ejecting the card when it is already ejected
    setUp();
    UUT.state = SDHC::State::Disconnected;
    UUT.RCA = 0xDE;
    UUT.eject();
    TEST_ASSERT_EQUAL(0, sdio.call_command_no_response.call_count);
    TEST_ASSERT_EQUAL(0, sdio.last_argument );
    TEST_ASSERT_EQUAL( SDHC::State::Disconnected, UUT.state );
};

/// @brief Test reading a single block
void test_read_single_block(void)
{
    // Create Card
    setUp();
    SDHC::Card UUT(sdio);
    auto buffer = SDHC::create_block_buffer<1>();

    // Test a successful read of one block for SDSC cards
    TEST_ASSERT_TRUE( UUT.read_single_block(buffer.begin(), 1) );
    sdio.call_command_R1_response.assert_called_once_with(SDHC::CMD<17>());
    sdio.call_read_single_block.assert_called_once_with(reinterpret_cast<int>(buffer.begin()));
    TEST_ASSERT_EQUAL(1*SDHC::BLOCKLENGTH, sdio.last_argument);

    // Test a successful read of one block for SDHC cards
    setUp();
    UUT.type_sdsc = false;
    TEST_ASSERT_TRUE( UUT.read_single_block(buffer.begin(), 1) );
    sdio.call_command_R1_response.assert_called_once_with(SDHC::CMD<17>());
    sdio.call_read_single_block.assert_called_once_with(reinterpret_cast<int>(buffer.begin()));
    TEST_ASSERT_EQUAL(1, sdio.last_argument);
};

/// @brief Test writing a single block
void test_write_single_block(void)
{
    // Create Card
    setUp();
    SDHC::Card UUT(sdio);
    auto buffer = SDHC::create_block_buffer<1>();

    // Test a successful read of one block for SDSC cards
    TEST_ASSERT_TRUE( UUT.write_single_block(buffer.begin(), 1) );
    sdio.call_command_R1_response.assert_called_once_with(SDHC::CMD<24>());
    sdio.call_write_single_block.assert_called_once_with(reinterpret_cast<int>(buffer.begin()));
    TEST_ASSERT_EQUAL(1*SDHC::BLOCKLENGTH, sdio.last_argument);

    // Test a successful read of one block for SDHC cards
    setUp();
    UUT.type_sdsc = false;
    TEST_ASSERT_TRUE( UUT.write_single_block(buffer.begin(), 1) );
    sdio.call_command_R1_response.assert_called_once_with(SDHC::CMD<24>());
    sdio.call_write_single_block.assert_called_once_with(reinterpret_cast<int>(buffer.begin()));
    TEST_ASSERT_EQUAL(1, sdio.last_argument);
};

// === Main ===
int main(int argc, char **argv)
{
    UNITY_BEGIN();
    test_constructor();
    test_reset();
    test_set_supply_voltage();
    test_initialize_card();
    test_get_RCA();
    test_select_card();
    test_change_bus_width();
    test_eject();
    test_read_single_block();
    test_write_single_block();
    UNITY_END();
    return EXIT_SUCCESS;
};