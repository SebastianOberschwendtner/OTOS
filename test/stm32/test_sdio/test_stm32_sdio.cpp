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
 * @file    test_stm32_sdio.cpp
 * @author  SO
 * @version v5.0.0
 * @date    29-December-2021
 * @brief   Unit tests for testing the sdio driver for stm32 microcontrollers.
 ==============================================================================
 */

/* === Includes === */
#include <unity.h>
#include <mock.h>
#include <array>
#include "stm32/sdio_stm32.h"

/* === Test List ===
* ▢ error codes:
*   ✓ error -130: Timeout during transfer
*   ✓ error -131: Bus busy during start of transfer
*/

/* === Tests === */
void setUp() {
    /* set stuff up here */
    RCC->registers_to_default();
    SDIO->registers_to_default();
};

void tearDown() {
/* clean stuff up here */
};

/** 
 * @brief Test the initialization of the controller
 */
void test_rcc_clock_enable()
{
    setUp();
    /* Create Controller -> SDIO */
    auto UUT = sdio::Controller::create(1'000'000);

    /* Test the side effects */
    TEST_ASSERT_BIT_HIGH(RCC_APB2ENR_SDIOEN_Pos, RCC->APB2ENR);
};

/** 
 * @brief Test setting the interface clock speed
 */
void test_set_clock()
{
    setUp();
    /* Create Controller -> SDIO */
    auto UUT = sdio::Controller::create(1'000'000);

    /* Test the side effects */
    TEST_ASSERT_BITS(SDIO_CLKCR_CLKDIV_Msk, 46, SDIO->CLKCR);
    TEST_ASSERT_BIT_HIGH(SDIO_CLKCR_CLKEN_Pos, SDIO->CLKCR);

    /* Change clock rate again */
    SDIO->CLKCR &= ~SDIO_CLKCR_CLKEN;
    UUT.set_clock(400'000);
    TEST_ASSERT_BITS(SDIO_CLKCR_CLKDIV_Msk, 118, SDIO->CLKCR);
    TEST_ASSERT_BIT_LOW(SDIO_CLKCR_CLKEN_Pos, SDIO->CLKCR);

    /* Change the powersave option */
    SDIO->CLKCR |= SDIO_CLKCR_CLKEN;
    UUT.set_clock(400'000, true);
    TEST_ASSERT_BIT_HIGH(SDIO_CLKCR_PWRSAV_Pos, SDIO->CLKCR);
    TEST_ASSERT_BIT_HIGH(SDIO_CLKCR_CLKEN_Pos, SDIO->CLKCR);
};

/** 
 * @brief Test setting the width of the bus
 */
void test_set_width()
{
    setUp();
    /* Create Controller -> SDIO */
    auto UUT = sdio::Controller::create(1'000'000);

    /* Test the side effects */
    UUT.set_bus_width( sdio::Width::Default );
    TEST_ASSERT_BITS_LOW( SDIO_CLKCR_WIDBUS_Msk, SDIO->CLKCR);
    UUT.set_bus_width( sdio::Width::_4Bit );
    TEST_ASSERT_BIT_HIGH( SDIO_CLKCR_WIDBUS_Pos, SDIO->CLKCR);
    UUT.set_bus_width( sdio::Width::_8Bit );
    TEST_ASSERT_BIT_HIGH( SDIO_CLKCR_WIDBUS_Pos + 1, SDIO->CLKCR);
};

/** 
 * @brief Test enabling the peripheral
 */
void test_enable()
{
    setUp();
    /* Create Controller -> SDIO */
    auto UUT = sdio::Controller::create(1'000'000);

    /* Test the side effects */
    UUT.enable();
    TEST_ASSERT_BIT_HIGH(SDIO_CLKCR_CLKEN_Pos, SDIO->CLKCR);
    TEST_ASSERT_EQUAL(0b11, SDIO->POWER);
};

/** 
 * @brief Test setting the hardware timeout
 */
void test_set_timeout()
{
    setUp();
    /* Create Controller -> SDIO */
    auto UUT = sdio::Controller::create(1'000'000);

    /* Test the side effects */
    UUT.set_hardware_timeout(0xFF);
    TEST_ASSERT_EQUAL(0xFF, SDIO->DTIMER);
};

/** 
 * @brief Test setting the block transfer byte length
 */
void test_set_block_length()
{
    setUp();
    /* Create Controller -> SDIO */
    auto UUT = sdio::Controller::create(1'000'000);

    /* Test the side effects */
    UUT.set_data_length(512);
    TEST_ASSERT_EQUAL(512, SDIO->DLEN);
};

/** 
 * @brief Test getting the peripheral status
 */
void test_get_status()
{
    setUp();
    /* Create Controller -> SDIO */
    auto UUT = sdio::Controller::create(1'000'000);

    /* Test whether a command was sent */
    SDIO->STA = 0;
    TEST_ASSERT_FALSE( UUT.command_sent() );
    SDIO->STA = SDIO_STA_CMDSENT;
    TEST_ASSERT_TRUE( UUT.command_sent() );

    /* Test whether a command response was received */
    SDIO->STA = 0;
    TEST_ASSERT_FALSE( UUT.command_response_received() );
    SDIO->STA = SDIO_STA_CMDREND;
    TEST_ASSERT_TRUE( UUT.command_response_received() );

    /* Test whether the bus is busy */
    SDIO->STA = 0;
    TEST_ASSERT_FALSE( UUT.is_busy() );
    SDIO->STA = SDIO_STA_CMDACT;
    TEST_ASSERT_TRUE( UUT.is_busy() );
    SDIO->STA = SDIO_STA_RXACT;
    TEST_ASSERT_TRUE( UUT.is_busy() );
    SDIO->STA = SDIO_STA_TXACT;
    TEST_ASSERT_TRUE( UUT.is_busy() );

    /* Test hardware timeouts */
    SDIO->STA = 0;
    TEST_ASSERT_FALSE( UUT.hardware_timeout() );
    SDIO->STA = SDIO_STA_DTIMEOUT;
    TEST_ASSERT_TRUE( UUT.hardware_timeout() );
    SDIO->STA = SDIO_STA_CTIMEOUT;
    TEST_ASSERT_TRUE( UUT.hardware_timeout() );

    /* Test the command response CRC fail */
    SDIO->STA = 0;
    TEST_ASSERT_FALSE( UUT.command_response_crc_fail() );
    SDIO->STA = SDIO_STA_CCRCFAIL;
    TEST_ASSERT_TRUE( UUT.command_response_crc_fail() );

    /* Test the end of a data block transfer */
    SDIO->STA = 0;
    TEST_ASSERT_FALSE( UUT.data_block_transfer_finished() );
    SDIO->STA = SDIO_STA_DBCKEND;
    TEST_ASSERT_TRUE( UUT.data_block_transfer_finished() );

    /* Test checking RX FIFO for valid data */
    SDIO->STA = 0;
    TEST_ASSERT_FALSE( UUT.data_RX_available() );
    SDIO->STA = SDIO_STA_RXDAVL;
    TEST_ASSERT_TRUE( UUT.data_RX_available() );

    /* Test checking TX FIFO empty */
    SDIO->STA = 0;
    TEST_ASSERT_FALSE( UUT.data_TX_empty() );
    SDIO->STA = SDIO_STA_TXFIFOE;
    TEST_ASSERT_TRUE( UUT.data_TX_empty() );
};

/** 
 * @brief Test clearing all flags
 */
void test_clear_flags()
{
    setUp();
    /* Create Controller -> SDIO */
    auto UUT = sdio::Controller::create(1'000'000);

    /* Test clearing all command flags */
    UUT.clear_command_flags();
    TEST_ASSERT_BIT_HIGH(SDIO_ICR_CMDSENTC_Pos, SDIO->ICR);
    TEST_ASSERT_BIT_HIGH(SDIO_ICR_CMDRENDC_Pos, SDIO->ICR);
    TEST_ASSERT_BIT_HIGH(SDIO_ICR_CCRCFAILC_Pos, SDIO->ICR);
};

/** 
 * @brief Test sending a command without expected response
 */
void test_command_no_response()
{
    setUp();
    uint32_t Expected = 0;

    /* Create Controller -> SDIO */
    auto UUT = sdio::Controller::create(1'000'000);

    /* Test a successful transfer */
    SDIO->registers_to_default();
    SDIO->STA = SDIO_STA_CMDSENT;
    TEST_ASSERT_TRUE( UUT.send_command_no_response(0x12, 0x34) );
    Expected = SDIO_CMD_CPSMEN | SDIO_CMD_ENCMDCOMPL | (0x12 & 0b111111);
    TEST_ASSERT_EQUAL(Expected, SDIO->CMD);
    TEST_ASSERT_EQUAL( 0x34, SDIO->ARG);
    TEST_ASSERT_BIT_HIGH(SDIO_ICR_CMDSENTC_Pos, SDIO->ICR);
    TEST_ASSERT_EQUAL( error::Code::None, UUT.get_error() );

    /* Test a timeout during transfer */
    SDIO->registers_to_default();
    SDIO->STA = SDIO_STA_CTIMEOUT;
    TEST_ASSERT_FALSE( UUT.send_command_no_response(0x12, 0x34) );
    TEST_ASSERT_EQUAL(0, SDIO->ICR);
    TEST_ASSERT_EQUAL(error::Code::SDIO_Timeout, UUT.get_error() );

    /* Test transmitting when bus is busy */
    SDIO->registers_to_default();
    SDIO->STA = SDIO_STA_CMDACT;
    TEST_ASSERT_FALSE( UUT.send_command_no_response(0x12, 0x34) );
    TEST_ASSERT_EQUAL(0, SDIO->ICR);
    TEST_ASSERT_EQUAL(error::Code::SDIO_BUS_Busy_Error, UUT.get_error() );
};

/** 
 * @brief Test sending a command with a expected R1 response
 */
void test_command_R1_response()
{
    setUp();
    uint32_t Expected = 0;

    /* Create Controller -> SDIO */
    auto UUT = sdio::Controller::create(1'000'000);

    /* Test a successful transfer */
    SDIO->registers_to_default();
    SDIO->STA = SDIO_STA_CMDREND;
    SDIO->RESP1 = 0x55;
    auto response = UUT.send_command_R1_response(0x12, 0x34);
    TEST_ASSERT_TRUE(response);
    TEST_ASSERT_EQUAL(0x55, response.value() );
    Expected = SDIO_CMD_CPSMEN | SDIO_CMD_ENCMDCOMPL | SDIO_CMD_WAITRESP_0 | (0x12 & 0b111111);
    TEST_ASSERT_EQUAL(Expected, SDIO->CMD);
    TEST_ASSERT_EQUAL( 0x34, SDIO->ARG);
    TEST_ASSERT_BIT_HIGH(SDIO_ICR_CMDRENDC_Pos, SDIO->ICR);
    TEST_ASSERT_EQUAL( error::Code::None, UUT.get_error() );

    /* Test a timeout during transfer */
    SDIO->registers_to_default();
    SDIO->STA = SDIO_STA_CTIMEOUT;
    SDIO->RESP1 = 0x55;
    response = UUT.send_command_R1_response(0x12, 0x34);
    TEST_ASSERT_FALSE(response);
    TEST_ASSERT_EQUAL(0, SDIO->ICR);
    TEST_ASSERT_EQUAL(error::Code::SDIO_Timeout, UUT.get_error() );

    /* Test transmitting when bus is busy */
    SDIO->registers_to_default();
    SDIO->STA = SDIO_STA_CMDACT;
    SDIO->RESP1 = 0x55;
    response = UUT.send_command_R1_response(0x12, 0x34);
    TEST_ASSERT_FALSE(response);
    TEST_ASSERT_EQUAL(0, SDIO->ICR);
    TEST_ASSERT_EQUAL(error::Code::SDIO_BUS_Busy_Error, UUT.get_error() );
};

/** 
 * @brief Test sending a command with a expected R2 response
 */
void test_command_R2_response()
{
    setUp();
    uint32_t Expected = 0;

    /* Create Controller -> SDIO */
    auto UUT = sdio::Controller::create(1'000'000);

    /* Test a successful transfer */
    SDIO->registers_to_default();
    SDIO->STA = SDIO_STA_CMDREND;
    SDIO->RESP1 = 0x55;
    auto response = UUT.send_command_R2_response(0x12, 0x34);
    TEST_ASSERT_TRUE(response);
    TEST_ASSERT_EQUAL(0x55, response.value() );
    Expected = SDIO_CMD_CPSMEN | SDIO_CMD_ENCMDCOMPL  | SDIO_CMD_WAITRESP_1 | SDIO_CMD_WAITRESP_0 | (0x12 & 0b111111);
    TEST_ASSERT_EQUAL(Expected, SDIO->CMD);
    TEST_ASSERT_EQUAL( 0x34, SDIO->ARG);
    TEST_ASSERT_BIT_HIGH(SDIO_ICR_CMDRENDC_Pos, SDIO->ICR);
    TEST_ASSERT_EQUAL( error::Code::None, UUT.get_error() );

    /* Test a timeout during transfer */
    SDIO->registers_to_default();
    SDIO->STA = SDIO_STA_CTIMEOUT;
    SDIO->RESP1 = 0x55;
    response = UUT.send_command_R2_response(0x12, 0x34);
    TEST_ASSERT_FALSE(response);
    TEST_ASSERT_EQUAL(0, SDIO->ICR);
    TEST_ASSERT_EQUAL(error::Code::SDIO_Timeout, UUT.get_error() );

    /* Test transmitting when bus is busy */
    SDIO->registers_to_default();
    SDIO->STA = SDIO_STA_CMDACT;
    SDIO->RESP1 = 0x55;
    response = UUT.send_command_R2_response(0x12, 0x34);
    TEST_ASSERT_FALSE(response);
    TEST_ASSERT_EQUAL(0, SDIO->ICR);
    TEST_ASSERT_EQUAL(error::Code::SDIO_BUS_Busy_Error, UUT.get_error() );
};

/** 
 * @brief Test sending a command with a expected R3 response
 */
void test_command_R3_response()
{
    setUp();
    uint32_t Expected = 0;

    /* Create Controller -> SDIO */
    auto UUT = sdio::Controller::create(1'000'000);

    /* Test a successful transfer when CRC did not match (which is allowed for R3 responses) */
    SDIO->registers_to_default();
    SDIO->STA = SDIO_STA_CCRCFAIL;
    SDIO->RESP1 = 0x55;
    auto response = UUT.send_command_R3_response(0x12, 0x34);
    TEST_ASSERT_TRUE(response);
    TEST_ASSERT_EQUAL(0x55, response.value() );
    Expected = SDIO_CMD_CPSMEN | SDIO_CMD_ENCMDCOMPL  | SDIO_CMD_WAITRESP_0 | (0x12 & 0b111111);
    TEST_ASSERT_EQUAL(Expected, SDIO->CMD);
    TEST_ASSERT_EQUAL( 0x34, SDIO->ARG);
    TEST_ASSERT_BIT_HIGH(SDIO_ICR_CMDRENDC_Pos, SDIO->ICR);
    TEST_ASSERT_BIT_HIGH(SDIO_ICR_CCRCFAILC_Pos, SDIO->ICR);
    TEST_ASSERT_EQUAL( error::Code::None, UUT.get_error() );

    /* Test a successful transfer when CRC match */
    SDIO->registers_to_default();
    SDIO->STA = SDIO_STA_CMDREND;
    SDIO->RESP1 = 0x55;
    response = UUT.send_command_R3_response(0x12, 0x34);
    TEST_ASSERT_TRUE(response);
    TEST_ASSERT_EQUAL(0x55, response.value() );
    Expected = SDIO_CMD_CPSMEN | SDIO_CMD_ENCMDCOMPL  | SDIO_CMD_WAITRESP_0 | (0x12 & 0b111111);
    TEST_ASSERT_EQUAL(Expected, SDIO->CMD);
    TEST_ASSERT_EQUAL( 0x34, SDIO->ARG);
    TEST_ASSERT_BIT_HIGH(SDIO_ICR_CMDRENDC_Pos, SDIO->ICR);
    TEST_ASSERT_BIT_HIGH(SDIO_ICR_CCRCFAILC_Pos, SDIO->ICR);
    TEST_ASSERT_EQUAL( error::Code::None, UUT.get_error() );

    /* Test a timeout during transfer */
    SDIO->registers_to_default();
    SDIO->STA = SDIO_STA_CTIMEOUT;
    SDIO->RESP1 = 0x55;
    response = UUT.send_command_R3_response(0x12, 0x34);
    TEST_ASSERT_FALSE(response);
    TEST_ASSERT_EQUAL(0, SDIO->ICR);
    TEST_ASSERT_EQUAL(error::Code::SDIO_Timeout, UUT.get_error() );

    /* Test transmitting when bus is busy */
    SDIO->registers_to_default();
    SDIO->STA = SDIO_STA_CMDACT;
    SDIO->RESP1 = 0x55;
    response = UUT.send_command_R3_response(0x12, 0x34);
    TEST_ASSERT_FALSE(response);
    TEST_ASSERT_EQUAL(0, SDIO->ICR);
    TEST_ASSERT_EQUAL(error::Code::SDIO_BUS_Busy_Error, UUT.get_error() );
};

/** 
 * @brief Test reading the data from a long response
 */
void test_get_long_response()
{
    setUp();

    /* Create Controller -> SDIO */
    auto UUT = sdio::Controller::create(1'000'000);
    SDIO->RESP1 = 0x12;
    SDIO->RESP2 = 0x13;
    SDIO->RESP3 = 0x14;
    SDIO->RESP4 = 0x15;

    /* Test reading the registers */
    TEST_ASSERT_EQUAL(0x12, UUT.get_long_response<0>() );
    TEST_ASSERT_EQUAL(0x13, UUT.get_long_response<1>() );
    TEST_ASSERT_EQUAL(0x14, UUT.get_long_response<2>() );
    TEST_ASSERT_EQUAL(0x15, UUT.get_long_response<3>() );
};

/** 
 * @brief Test reading block data
 */
void test_read_block()
{
    setUp();

    /* Create Controller -> SDIO */
    auto UUT = sdio::Controller::create(1'000'000);
    std::array<uint32_t, 128> buffer{0};

    /* Test reading a block with no errors */
    SDIO->STA = SDIO_STA_DBCKEND;
    TEST_ASSERT_TRUE( UUT.read_single_block(buffer.begin(), buffer.end()) );
    TEST_ASSERT_EQUAL( 512, SDIO->DLEN );
    TEST_ASSERT_EQUAL( (9 << 4) | SDIO_DCTRL_DTDIR | SDIO_DCTRL_DTEN, SDIO->DCTRL);
    TEST_ASSERT_EQUAL( SDIO_ICR_DBCKENDC | SDIO_ICR_DATAENDC, SDIO->ICR);
    TEST_ASSERT_EQUAL(error::Code::None, UUT.get_error() );

    /* Test reading a block when bus is busy */
    SDIO->STA = SDIO_STA_RXACT;
    TEST_ASSERT_FALSE( UUT.read_single_block(buffer.begin(), buffer.end()) );
    TEST_ASSERT_EQUAL(error::Code::SDIO_BUS_Busy_Error, UUT.get_error() );

    /* Test reading a block when a timeout occurs */
    SDIO->STA = SDIO_STA_DTIMEOUT;
    TEST_ASSERT_FALSE( UUT.read_single_block(buffer.begin(), buffer.end()) );
    TEST_ASSERT_EQUAL( 512, SDIO->DLEN );
    TEST_ASSERT_EQUAL( (9 << 4) | SDIO_DCTRL_DTDIR | SDIO_DCTRL_DTEN, SDIO->DCTRL);
    TEST_ASSERT_EQUAL( SDIO_ICR_DBCKENDC | SDIO_ICR_DATAENDC, SDIO->ICR);
    TEST_ASSERT_EQUAL(error::Code::SDIO_Timeout, UUT.get_error() );
};

/** 
 * @brief Test write block data
 */
void test_write_block()
{
    setUp();

    /* Create Controller -> SDIO */
    auto UUT = sdio::Controller::create(1'000'000);
    std::array<uint32_t, 128> buffer{0x11};

    /* Test reading a block with no errors */
    SDIO->STA = SDIO_STA_DBCKEND | SDIO_STA_TXFIFOE;
    TEST_ASSERT_TRUE( UUT.write_single_block(buffer.begin(), buffer.end()) );
    TEST_ASSERT_EQUAL( 512, SDIO->DLEN );
    TEST_ASSERT_EQUAL( (9 << 4) | SDIO_DCTRL_DTEN, SDIO->DCTRL);
    TEST_ASSERT_EQUAL( SDIO_ICR_DBCKENDC | SDIO_ICR_DATAENDC, SDIO->ICR);
    TEST_ASSERT_EQUAL(error::Code::None, UUT.get_error() );

    /* Test reading a block when bus is busy */
    SDIO->STA = SDIO_STA_TXACT;
    TEST_ASSERT_FALSE( UUT.write_single_block(buffer.begin(), buffer.end()) );
    TEST_ASSERT_EQUAL(error::Code::SDIO_BUS_Busy_Error, UUT.get_error() );

    /* Test reading a block when a timeout occurs */
    SDIO->STA = SDIO_STA_DTIMEOUT;
    TEST_ASSERT_FALSE( UUT.write_single_block(buffer.begin(), buffer.end()) );
    TEST_ASSERT_EQUAL( 512, SDIO->DLEN );
    TEST_ASSERT_EQUAL( (9 << 4) | SDIO_DCTRL_DTEN, SDIO->DCTRL);
    TEST_ASSERT_EQUAL( SDIO_ICR_DBCKENDC | SDIO_ICR_DATAENDC, SDIO->ICR);
    TEST_ASSERT_EQUAL(error::Code::SDIO_Timeout, UUT.get_error() );
};

/* === Main === */
int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_rcc_clock_enable);
    RUN_TEST(test_set_clock);
    RUN_TEST(test_set_width);
    RUN_TEST(test_set_timeout);
    RUN_TEST(test_set_block_length);
    RUN_TEST(test_enable);
    RUN_TEST(test_get_status);
    RUN_TEST(test_clear_flags);
    RUN_TEST(test_command_no_response);
    RUN_TEST(test_command_R1_response);
    RUN_TEST(test_command_R2_response);
    RUN_TEST(test_command_R3_response);
    RUN_TEST(test_get_long_response); 
    RUN_TEST(test_read_block);
    RUN_TEST(test_write_block);
    return UNITY_END();
};