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
 ==============================================================================
 * @file    test_sdhc.cpp
 * @author  SO
 * @version v2.7.3
 * @date    29-December-2021
 * @brief   Unit tests for testing the SDHC interface.
 ==============================================================================
 */

// === Includes ===
#include <unity.h>
#include <mock.h>
#include <array>
#include "memory/sdhc.h"

/** === Test List ===

*/

// === Fixtures ===
struct Mock_SDIO : public sdhc::interface
{
    Mock::Callable<bool> call_command_no_response;
    Mock::Callable<bool> call_command_R1_response;
    Mock::Callable<bool> call_command_R2_response;
    Mock::Callable<bool> call_command_R3_response;
    Mock::Callable<bool> call_command_R6_response;
    Mock::Callable<bool> call_command_R7_response;
    Mock::Callable<bool> call_read_single_block;
    Mock::Callable<bool> call_write_single_block;
    uint32_t last_argument = 0;
    uint32_t R1_response = 0;
    uint32_t R2_response = 0;
    uint32_t R3_response = 0;
    uint32_t R6_response = 0;
    uint32_t R7_response = 0;
    bool send_command_no_response(const uint8_t command, const uint32_t argument) override
    {
        last_argument = argument;
        return call_command_no_response(command);
    };
    std::optional<uint32_t> send_command_R1_response(const uint8_t command, const uint32_t argument) override
    {
        last_argument = argument;
        call_command_R1_response.add_call(command);
        return this->R1_response;
    };
    std::optional<uint32_t> send_command_R2_response(const uint8_t command, const uint32_t argument) override
    {
        last_argument = argument;
        call_command_R2_response.add_call(command);
        return this->R2_response;
    };
    std::optional<uint32_t> send_command_R3_response(const uint8_t command, const uint32_t argument) override
    {
        last_argument = argument;
        call_command_R3_response.add_call(command);
        return this->R3_response;
    };
    std::optional<uint32_t> send_command_R6_response(const uint8_t command, const uint32_t argument) override
    {
        last_argument = argument;
        call_command_R6_response.add_call(command);
        return this->R6_response;
    };
    std::optional<uint32_t> send_command_R7_response(const uint8_t command, const uint32_t argument) override
    {
        last_argument = argument;
        call_command_R7_response.add_call(command);
        return this->R7_response;
    };
    bool read_single_block(const uint32_t* buffer_begin, const uint32_t* buffer_end) override
    {
        return call_read_single_block();
    };
    bool write_single_block(const uint32_t* buffer_begin, const uint32_t* buffer_end) override
    {
        return call_write_single_block();
    };
};
Mock_SDIO mock_sdio;

// === Tests ===
void setUp(void) {
    // set stuff up here
    mock_sdio.last_argument = 0;
    mock_sdio.R1_response = 0;
    mock_sdio.R2_response = 0;
    mock_sdio.R3_response = 0;
    mock_sdio.R6_response = 0;
    mock_sdio.R7_response = 0;
    mock_sdio.call_command_no_response.reset();
    mock_sdio.call_command_R1_response.reset();
    mock_sdio.call_command_R2_response.reset();
    mock_sdio.call_command_R3_response.reset();
    mock_sdio.call_command_R6_response.reset();
    mock_sdio.call_command_R7_response.reset();
    mock_sdio.call_read_single_block.reset();
    mock_sdio.call_write_single_block.reset();
};

void tearDown(void) {
    // clean stuff up here
};

/// @brief Test constructing a new SD_Card
void test_constructor(void)
{
    // Create Card
    setUp();
    sdhc::Card UUT(mock_sdio);

    // Test members
    TEST_ASSERT_EQUAL(sdhc::State::Identification, UUT.state );
    TEST_ASSERT_EQUAL(0, UUT.RCA );
    TEST_ASSERT_TRUE( UUT.is_SDSC() );
};

/// @brief Test resetting the card
void test_reset(void)
{
    // Create Card
    setUp();
    sdhc::Card UUT(mock_sdio);

    // Test the reset
    TEST_ASSERT_TRUE( UUT.reset() );
    TEST_ASSERT_EQUAL(0, mock_sdio.last_argument);
    mock_sdio.call_command_no_response.assert_called_once_with(sdhc::CMD<0>());
};

/// @brief Test setting the power supply voltage
void test_set_supply_voltage(void)
{
    // Create Card
    setUp();
    sdhc::Card UUT(mock_sdio);
    mock_sdio.R7_response = sdhc::CHECK_PATTERN;

    // Test setting the supply voltage when card does respond
    TEST_ASSERT_TRUE( UUT.set_supply_voltage() );
    TEST_ASSERT_EQUAL(sdhc::CMD8::Voltage_0 | sdhc::CHECK_PATTERN, mock_sdio.last_argument);
    mock_sdio.call_command_R7_response.assert_called_once_with(sdhc::CMD<8>());

    // Test setting the supply voltage when card does not respond
    mock_sdio.R7_response = 0;
    TEST_ASSERT_FALSE( UUT.set_supply_voltage() );
    TEST_ASSERT_EQUAL(sdhc::CMD8::Voltage_0 | sdhc::CHECK_PATTERN, mock_sdio.last_argument);
    mock_sdio.call_command_R7_response.assert_called_once_with(sdhc::CMD<8>());
    TEST_ASSERT_EQUAL(sdhc::State::Disconnected, UUT.state );
};

/// @brief Test the card initialization commnad
void test_initialize_card(void)
{
    // Create Card
    setUp();
    sdhc::Card UUT(mock_sdio);

    // Test the successfull initialization of SDSC card
    mock_sdio.R1_response = sdhc::R1::APP_CMD;
    mock_sdio.R3_response = sdhc::R3::NOT_BUSY;
    TEST_ASSERT_TRUE( UUT.initialize_card() );
    mock_sdio.call_command_R1_response.assert_called_once_with(sdhc::CMD<55>() );
    mock_sdio.call_command_R3_response.assert_called_once_with(sdhc::ACMD<41>() );
    TEST_ASSERT_EQUAL( sdhc::ACMD41::HCS | sdhc::ACMD41::XPC | sdhc::OCR::_3_0V, mock_sdio.last_argument);
    TEST_ASSERT_EQUAL( sdhc::State::Identification, UUT.state );
    TEST_ASSERT_TRUE( UUT.is_SDSC() );

    // Test the successfull initialization of SDSC card
    setUp();
    mock_sdio.R1_response = sdhc::R1::APP_CMD;
    mock_sdio.R3_response = sdhc::R3::CCS | sdhc::R3::NOT_BUSY;
    TEST_ASSERT_TRUE( UUT.initialize_card() );
    mock_sdio.call_command_R1_response.assert_called_once_with(sdhc::CMD<55>() );
    mock_sdio.call_command_R3_response.assert_called_once_with(sdhc::ACMD<41>() );
    TEST_ASSERT_EQUAL( sdhc::ACMD41::HCS | sdhc::ACMD41::XPC | sdhc::OCR::_3_0V, mock_sdio.last_argument);
    TEST_ASSERT_EQUAL( sdhc::State::Identification, UUT.state );
    TEST_ASSERT_FALSE( UUT.is_SDSC() );

    // Test sending the command while card is busy
    setUp();
    mock_sdio.R1_response = sdhc::R1::APP_CMD;
    mock_sdio.R3_response = 0;
    TEST_ASSERT_FALSE( UUT.initialize_card() );
    mock_sdio.call_command_R1_response.assert_called_once_with(sdhc::CMD<55>() );
    mock_sdio.call_command_R3_response.assert_called_once_with(sdhc::ACMD<41>() );
    TEST_ASSERT_EQUAL( sdhc::ACMD41::HCS | sdhc::ACMD41::XPC | sdhc::OCR::_3_0V, mock_sdio.last_argument);
    TEST_ASSERT_EQUAL( sdhc::State::Identification, UUT.state );

    // Test sending the command when card does not accept the command
    setUp();
    mock_sdio.R1_response = 0;
    mock_sdio.R3_response = sdhc::R3::NOT_BUSY;
    TEST_ASSERT_FALSE( UUT.initialize_card() );
    mock_sdio.call_command_R1_response.assert_called_once_with(sdhc::CMD<55>() );
    TEST_ASSERT_EQUAL(0 ,mock_sdio.call_command_R3_response.call_count);
    TEST_ASSERT_EQUAL( 0 , mock_sdio.last_argument);
    TEST_ASSERT_EQUAL( sdhc::State::Disconnected, UUT.state );
};

/// @brief Test getting the RCA (Relative Card Address)
void test_get_RCA(void)
{
    // Create Card
    setUp();
    sdhc::Card UUT(mock_sdio);

    // Test successfull read of RCA
    mock_sdio.R6_response = (0xAB << 16);
    TEST_ASSERT_TRUE( UUT.get_RCA() );
    mock_sdio.call_command_R2_response.assert_called_once_with(sdhc::CMD<2>() );
    mock_sdio.call_command_R6_response.assert_called_once_with(sdhc::CMD<3>() );
    TEST_ASSERT_EQUAL(0xAB, UUT.RCA );
    TEST_ASSERT_EQUAL( sdhc::State::StandBy, UUT.state );
};

/// @brief Test selecting a card using its RCA
void test_select_card(void)
{
    // Create Card
    setUp();
    sdhc::Card UUT(mock_sdio);

    // Test successfull selecting card
    mock_sdio.R1_response = 0;
    UUT.RCA = 0xAB;
    TEST_ASSERT_TRUE( UUT.select() );
    mock_sdio.call_command_R1_response.assert_called_once_with(sdhc::CMD<7>() );
    TEST_ASSERT_EQUAL( 0xAB << 16, mock_sdio.last_argument);
    TEST_ASSERT_EQUAL( sdhc::State::Transfering, UUT.state );

    // Test selecting card with error
    mock_sdio.R1_response = sdhc::R1::ERROR;
    UUT.RCA = 0xAB;
    UUT.state = sdhc::State::StandBy;
    TEST_ASSERT_FALSE( UUT.select() );
    mock_sdio.call_command_R1_response.assert_called_once_with(sdhc::CMD<7>() );
    TEST_ASSERT_EQUAL( 0xAB << 16, mock_sdio.last_argument);
    TEST_ASSERT_EQUAL( sdhc::State::StandBy, UUT.state );
};

/// @brief Test changing the bus width to 4bits
void test_change_bus_width(void)
{
    // Create Card
    setUp();
    sdhc::Card UUT(mock_sdio);

    // Test the successfull change of the bus width
    mock_sdio.R1_response = sdhc::R1::APP_CMD;
    TEST_ASSERT_TRUE( UUT.set_bus_width_4bits() );
    mock_sdio.call_command_R1_response.assert_called_last_with(sdhc::ACMD<6>() );
    TEST_ASSERT_EQUAL( 0b10, mock_sdio.last_argument);

    // Test the successfull change of the bus width
    mock_sdio.R1_response = sdhc::R1::APP_CMD | sdhc::R1::ERROR;
    TEST_ASSERT_FALSE( UUT.set_bus_width_4bits() );
    mock_sdio.call_command_R1_response.assert_called_last_with(sdhc::ACMD<6>() );
    TEST_ASSERT_EQUAL( 0b10, mock_sdio.last_argument);
};

/// @brief Test ejecting the card
void test_eject(void)
{
    // Create Card
    setUp();
    sdhc::Card UUT(mock_sdio);

    // Test ejecting the card when it can be ejected
    UUT.state = sdhc::State::Transfering;
    UUT.RCA = 0xDE;
    UUT.eject();
    mock_sdio.call_command_no_response.assert_called_once_with( sdhc::CMD<15>() );
    TEST_ASSERT_EQUAL( 0xDE << 16, mock_sdio.last_argument );
    TEST_ASSERT_EQUAL( sdhc::State::Disconnected, UUT.state );

    // Test ejecting the card when it is already ejected
    setUp();
    UUT.state = sdhc::State::Disconnected;
    UUT.RCA = 0xDE;
    UUT.eject();
    TEST_ASSERT_EQUAL(0, mock_sdio.call_command_no_response.call_count);
    TEST_ASSERT_EQUAL(0, mock_sdio.last_argument );
    TEST_ASSERT_EQUAL( sdhc::State::Disconnected, UUT.state );
};

/// @brief Test reading a single block
void test_read_single_block(void)
{
    // Create Card
    setUp();
    sdhc::Card UUT(mock_sdio);
    auto buffer = sdhc::create_block_buffer<1>();

    // Test a successful read of one block for SDSC cards
    TEST_ASSERT_TRUE( UUT.read_single_block(buffer.begin(), 1) );
    mock_sdio.call_command_R1_response.assert_called_once_with(sdhc::CMD<17>());
    mock_sdio.call_read_single_block.assert_called_once();
    TEST_ASSERT_EQUAL(1*sdhc::BLOCKLENGTH, mock_sdio.last_argument);

    // Test a successful read of one block for SDHC cards
    setUp();
    UUT.type_sdsc = false;
    TEST_ASSERT_TRUE( UUT.read_single_block(buffer.begin(), 1) );
    mock_sdio.call_command_R1_response.assert_called_once_with(sdhc::CMD<17>());
    mock_sdio.call_read_single_block.assert_called_once();
    TEST_ASSERT_EQUAL(1, mock_sdio.last_argument);
};

/// @brief Test writing a single block
void test_write_single_block(void)
{
    // Create Card
    setUp();
    sdhc::Card UUT(mock_sdio);
    auto buffer = sdhc::create_block_buffer<1>();

    // Test a successful read of one block for SDSC cards
    TEST_ASSERT_TRUE( UUT.write_single_block(buffer.begin(), 1) );
    mock_sdio.call_command_R1_response.assert_called_once_with(sdhc::CMD<24>());
    mock_sdio.call_write_single_block.assert_called_once();
    TEST_ASSERT_EQUAL(1*sdhc::BLOCKLENGTH, mock_sdio.last_argument);

    // Test a successful read of one block for SDHC cards
    setUp();
    UUT.type_sdsc = false;
    TEST_ASSERT_TRUE( UUT.write_single_block(buffer.begin(), 1) );
    mock_sdio.call_command_R1_response.assert_called_once_with(sdhc::CMD<24>());
    mock_sdio.call_write_single_block.assert_called_once();
    TEST_ASSERT_EQUAL(1, mock_sdio.last_argument);
};

void test_data_access(void)
{
    // auto buffer = SDHC::create_block_buffer<1>();
    std::array<uint32_t, 128> buffer{0};
    buffer[0] = 0x03020100;
    buffer[1] = 0x07060504;

    uint8_t* char_pointer = reinterpret_cast<uint8_t*>(buffer.begin());

    TEST_ASSERT_EQUAL( 0x00, *(char_pointer + 0));
    TEST_ASSERT_EQUAL( 0x01, *(char_pointer + 1));
    TEST_ASSERT_EQUAL( 0x02, *(char_pointer + 2));
    TEST_ASSERT_EQUAL( 0x03, *(char_pointer + 3));
    TEST_ASSERT_EQUAL( 0x04, *(char_pointer + 4));
}

// === Main ===
int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_constructor);
    RUN_TEST(test_reset);
    RUN_TEST(test_set_supply_voltage);
    RUN_TEST(test_initialize_card);
    RUN_TEST(test_get_RCA);
    RUN_TEST(test_select_card);
    RUN_TEST(test_change_bus_width);
    RUN_TEST(test_eject);
    RUN_TEST(test_read_single_block);
    RUN_TEST(test_write_single_block);
    RUN_TEST(test_data_access);
    return UNITY_END();
};