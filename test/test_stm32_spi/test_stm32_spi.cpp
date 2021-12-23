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
 * @file    test_stm32_spi.cpp
 * @author  SO
 * @version v2.1.0
 * @date    22-December-2021
 * @brief   Unit tests for testing the spi driver for stm32 microcontrollers.
 ******************************************************************************
 */

// === Includes ===
#include <unity.h>
#include <mock.h>
#include <array>
#include <numeric>
#include "stm32/spi_stm32.h"

/** === Test List ===
* ▢ error codes:
*   ✓ error -110: Timeout during transfer
*   ✓ error -111: Bus busy during start of transfer
*/



// === Tests ===
void setUp(void) {
// set stuff up here
RCC->registers_to_default();
SPI1->registers_to_default();
};

// void tearDown(void) {
// // clean stuff up here
// };

/// @brief Test the initialization of the controller
void test_rcc_clock_enable(void)
{
    // Create Controller -> SPI1
    RCC->registers_to_default();

    SPI::Controller<IO::SPI_1> UUT1(1);
    TEST_ASSERT_BIT_HIGH(RCC_APB2ENR_SPI1EN_Pos, RCC->APB2ENR);

    // Create Controller -> SPI2
    RCC->registers_to_default();
    SPI::Controller<IO::SPI_2> UUT2(1);
    TEST_ASSERT_BIT_HIGH(RCC_APB1ENR_SPI2EN_Pos, RCC->APB1ENR);

    // Create Controller -> SPI3
    RCC->registers_to_default();
    SPI::Controller<IO::SPI_3> UUT3(1);
    TEST_ASSERT_BIT_HIGH(RCC_APB1ENR_SPI3EN_Pos, RCC->APB1ENR);

    // Create Controller -> SPI4
    RCC->registers_to_default();
    SPI::Controller<IO::SPI_4> UUT4(1);
    TEST_ASSERT_BIT_HIGH(RCC_APB2ENR_SPI4EN_Pos, RCC->APB2ENR);

    // Create Controller -> SPI5
    RCC->registers_to_default();
    SPI::Controller<IO::SPI_5> UUT5(1);
    TEST_ASSERT_BIT_HIGH(RCC_APB2ENR_SPI5EN_Pos, RCC->APB2ENR);

    // Create Controller -> SPI6
    RCC->registers_to_default();
    SPI::Controller<IO::SPI_6> UUT6(1);
    TEST_ASSERT_BIT_HIGH(RCC_APB2ENR_SPI6EN_Pos, RCC->APB2ENR);
};

/// @brief Test the configuration of the peripheral during construction
void test_constructor(void)
{
    setUp();

    // Create Object
    SPI::Controller<IO::SPI_1> UUT(1'000'000);

    // Test side effects
    // Prescaler for baudrate
    constexpr unsigned char Expected = F_CPU / 1'000'000;
    TEST_ASSERT_BITS(0b111000, (Expected<<1), SPI1->CR1);

    // in Master Mode
    TEST_ASSERT_BIT_HIGH(SPI_CR1_MSTR_Pos, SPI1->CR1);

    // Peripheral is not enabled
    TEST_ASSERT_BIT_LOW(SPI_CR1_SPE_Pos, SPI1->CR1);
};

/// @brief Test the correct setting of the clocking timing
void test_set_clock_properties(void)
{
    setUp();
    SPI::Controller<IO::SPI_1> UUT(1'000'000);

    // test all four possible configurations
    UUT.set_clock_timing(Level::High, Edge::Rising);
    TEST_ASSERT_BIT_HIGH(SPI_CR1_CPHA_Pos, SPI1->CR1);
    TEST_ASSERT_BIT_HIGH(SPI_CR1_CPOL_Pos, SPI1->CR1);

    UUT.set_clock_timing(Level::Low, Edge::Falling);
    TEST_ASSERT_BIT_HIGH(SPI_CR1_CPHA_Pos, SPI1->CR1);
    TEST_ASSERT_BIT_LOW(SPI_CR1_CPOL_Pos, SPI1->CR1);

    UUT.set_clock_timing(Level::High, Edge::Falling);
    TEST_ASSERT_BIT_LOW(SPI_CR1_CPHA_Pos, SPI1->CR1);
    TEST_ASSERT_BIT_HIGH(SPI_CR1_CPOL_Pos, SPI1->CR1);

    UUT.set_clock_timing(Level::Low, Edge::Rising);
    TEST_ASSERT_BIT_LOW(SPI_CR1_CPHA_Pos, SPI1->CR1);
    TEST_ASSERT_BIT_LOW(SPI_CR1_CPOL_Pos, SPI1->CR1);
};

/// @brief Test the target select pin management
void test_set_target_selection(void)
{
    setUp();
    SPI::Controller<IO::SPI_1> UUT(1'000'000);

    // Select target selection using a hardware pin
    UUT.set_use_hardware_chip_select(true);
    TEST_ASSERT_BIT_LOW(SPI_CR1_SSM_Pos, SPI1->CR1);
    TEST_ASSERT_BIT_LOW(SPI_CR1_SSI_Pos, SPI1->CR1);
    TEST_ASSERT_BIT_HIGH(SPI_CR2_SSOE_Pos, SPI1->CR2);

    // Disable hardware selection
    UUT.set_use_hardware_chip_select(false);
    TEST_ASSERT_BIT_HIGH(SPI_CR1_SSM_Pos, SPI1->CR1);
    TEST_ASSERT_BIT_HIGH(SPI_CR1_SSI_Pos, SPI1->CR1);
    TEST_ASSERT_BIT_LOW(SPI_CR2_SSOE_Pos, SPI1->CR2);

    // Repeat target selection using a hardware pin
    UUT.set_use_hardware_chip_select(true);
    TEST_ASSERT_BIT_LOW(SPI_CR1_SSM_Pos, SPI1->CR1);
    TEST_ASSERT_BIT_LOW(SPI_CR1_SSI_Pos, SPI1->CR1);
    TEST_ASSERT_BIT_HIGH(SPI_CR2_SSOE_Pos, SPI1->CR2);
};

/// @brief Test enabling and disabling the peripheral
void test_enable(void)
{
    setUp();
    SPI::Controller<IO::SPI_1> UUT(1'000'000);

    // Enable bus
    UUT.enable();
    TEST_ASSERT_BIT_HIGH(SPI_CR1_SPE_Pos, SPI1->CR1);
    
    // Disable bus again
    UUT.disable();
    TEST_ASSERT_BIT_LOW(SPI_CR1_SPE_Pos, SPI1->CR1);
};

/// @brief Test status of TX register
void test_last_transmit_finished(void)
{
    setUp();
    SPI::Controller<IO::SPI_1> UUT(1'000'000);

    // Test register empty
    SPI1->SR = SPI_SR_TXE;
    TEST_ASSERT_TRUE( UUT.last_transmit_finished() );

    // Test register not empty
    SPI1->SR = 0;
    TEST_ASSERT_FALSE( UUT.last_transmit_finished() );
};

/// @brief Test status bus busy
void test_bus_busy(void)
{
    setUp();
    SPI::Controller<IO::SPI_1> UUT(1'000'000);

    // Test register empty
    SPI1->SR = SPI_SR_BSY;
    TEST_ASSERT_TRUE( UUT.is_busy() );

    // Test register not empty
    SPI1->SR = 0;
    TEST_ASSERT_FALSE( UUT.is_busy() );
};

/// @brief Test sending data
void test_send_data(void)
{
    setUp();
    SPI::Controller<IO::SPI_1> UUT(1'000'000);

    // Test sending a byte, when TX buffer is empty
    SPI1->SR = SPI_SR_TXE;
    UUT.set_error(Error::Code::None);
    Bus::Data_t payload{0xAA};
    TEST_ASSERT_TRUE(UUT.send_data(payload, 1));
    TEST_ASSERT_EQUAL(Error::Code::None, UUT.get_error());
    TEST_ASSERT_EQUAL(payload.value, SPI1->DR);

    // Test sending a byte, when TX buffer is not empty
    SPI1->SR = 0;
    SPI1->DR = 0;
    UUT.set_error(Error::Code::None);
    TEST_ASSERT_FALSE(UUT.send_data(payload, 1));
    TEST_ASSERT_EQUAL(Error::Code::SPI_Timeout, UUT.get_error());
    TEST_ASSERT_EQUAL(0, SPI1->DR);

    // Test sending a byte, when the bus is busy
    SPI1->SR = SPI_SR_BSY;
    SPI1->DR = 0;
    UUT.set_error(Error::Code::None);
    TEST_ASSERT_FALSE(UUT.send_data(payload, 1));
    TEST_ASSERT_EQUAL(Error::Code::SPI_BUS_Busy_Error, UUT.get_error());
    TEST_ASSERT_EQUAL(0, SPI1->DR);

    // Test sending a word, when TX buffer is empty
    SPI1->SR = SPI_SR_TXE;
    payload.value = 0xAABB;
    UUT.set_error(Error::Code::None);
    TEST_ASSERT_TRUE(UUT.send_data(payload, 2));
    TEST_ASSERT_EQUAL(Error::Code::None, UUT.get_error());
    TEST_ASSERT_EQUAL(payload.byte[0], SPI1->DR);
};

/// @brief Test sending an array
void test_send_array(void)
{
    setUp();
    std::array<unsigned char, 10> buffer{0};
    SPI::Controller<IO::SPI_1> UUT(1'000'000);

    // fill array with consecutive numbers
    std::iota( buffer.begin(), buffer.end(), 0);

    // Test sending the array
    SPI1->SR = SPI_SR_TXE;
    UUT.set_error(Error::Code::None);
    TEST_ASSERT_TRUE( UUT.send_array(buffer.data(), 6) );
    TEST_ASSERT_EQUAL(Error::Code::None, UUT.get_error());
    TEST_ASSERT_EQUAL( 5, SPI1->DR);

    // Test sending an array, when the bus is busy
    SPI1->SR = SPI_SR_BSY;
    SPI1->DR = 0;
    UUT.set_error(Error::Code::None);
    TEST_ASSERT_FALSE( UUT.send_array(buffer.data(), 6) );
    TEST_ASSERT_EQUAL(Error::Code::SPI_BUS_Busy_Error, UUT.get_error());
    TEST_ASSERT_EQUAL( 0, SPI1->DR);
};

// === Main ===
int main(int argc, char **argv)
{
    UNITY_BEGIN();
    test_rcc_clock_enable();
    test_constructor();
    test_set_clock_properties();
    test_set_target_selection();
    test_enable();
    test_last_transmit_finished();
    test_bus_busy();
    test_send_data();
    test_send_array();
    UNITY_END();
    return EXIT_SUCCESS;
};