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
 * @file    test_stm32_usart.cpp
 * @author  SO
 * @version v2.7.3
 * @date    23-December-2021
 * @brief   Unit tests for testing the usart driver for stm32 microcontrollers.
 ******************************************************************************
 */

// === Includes ===
#include <unity.h>
#include <mock.h>
#include <array>
#include <numeric>
#include "stm32/usart_stm32.h"

/** === Test List ===
* ▢ error codes:
*   ✓ error -120: Timeout during transfer
*   ✓ error -121: Bus busy during start of transfer
*/



// === Tests ===
void setUp(void) {
// set stuff up here
RCC->registers_to_default();
USART1->registers_to_default();
};

void tearDown(void) {
// clean stuff up here
};

/// @brief Test the initialization of the controller
void test_rcc_clock_enable(void)
{
    setUp();

    // Create Controller -> USART1
    RCC->registers_to_default();
    USART::Controller<IO::USART_1> UUT1(9600);
    TEST_ASSERT_BIT_HIGH(RCC_APB2ENR_USART1EN_Pos, RCC->APB2ENR);

    // Create Controller -> USART2
    RCC->registers_to_default();
    USART::Controller<IO::USART_2> UUT2(9600);
    TEST_ASSERT_BIT_HIGH(RCC_APB1ENR_USART2EN_Pos, RCC->APB1ENR);

    // Create Controller -> USART3
    RCC->registers_to_default();
    USART::Controller<IO::USART_3> UUT3(9600);
    TEST_ASSERT_BIT_HIGH(RCC_APB1ENR_USART3EN_Pos, RCC->APB1ENR);

    // Create Controller -> USART4
    RCC->registers_to_default();
    USART::Controller<IO::USART_4> UUT4(9600);
    TEST_ASSERT_BIT_HIGH(RCC_APB1ENR_UART4EN_Pos, RCC->APB1ENR);

    // Create Controller -> USART5
    RCC->registers_to_default();
    USART::Controller<IO::USART_5> UUT5(9600);
    TEST_ASSERT_BIT_HIGH(RCC_APB1ENR_UART5EN_Pos, RCC->APB1ENR);

    // Create Controller -> USART6
    RCC->registers_to_default();
    USART::Controller<IO::USART_6> UUT6(9600);
    TEST_ASSERT_BIT_HIGH(RCC_APB2ENR_USART6EN_Pos, RCC->APB2ENR);

    // Create Controller -> USART7
    RCC->registers_to_default();
    USART::Controller<IO::USART_7> UUT7(9600);
    TEST_ASSERT_BIT_HIGH(RCC_APB1ENR_UART7EN_Pos, RCC->APB1ENR);

    // Create Controller -> USART8
    RCC->registers_to_default();
    USART::Controller<IO::USART_8> UUT8(9600);
    TEST_ASSERT_BIT_HIGH(RCC_APB1ENR_UART8EN_Pos, RCC->APB1ENR);
};

/// @brief Test the configuration of the peripheral during construction
void test_constructor(void)
{
    setUp();

    // Create Object
    USART::Controller<IO::USART_1> UUT(9'600, 8, USART::StopBits::_1_0);

    // Test side effects
    TEST_ASSERT_BIT_LOW(USART_CR1_M_Pos, USART1->CR1);
    TEST_ASSERT_BIT_HIGH(USART_CR1_TE_Pos, USART1->CR1);
    TEST_ASSERT_BIT_HIGH(USART_CR1_RE_Pos, USART1->CR1);
    TEST_ASSERT_BIT_LOW(USART_CR1_UE_Pos, USART1->CR1);
    TEST_ASSERT_BIT_LOW(USART_CR2_STOP_Pos, USART1->CR2);
    TEST_ASSERT_BIT_LOW(USART_CR2_STOP_Pos + 1, USART1->CR2);
    TEST_ASSERT_EQUAL((26 << 4), USART1->BRR);

    // Create Object
    USART::Controller<IO::USART_1> UUT1(115'200, 9, USART::StopBits::_2_0);

    // Test side effects
    TEST_ASSERT_BIT_HIGH(USART_CR1_M_Pos, USART1->CR1);
    TEST_ASSERT_BIT_HIGH(USART_CR1_TE_Pos, USART1->CR1);
    TEST_ASSERT_BIT_HIGH(USART_CR1_RE_Pos, USART1->CR1);
    TEST_ASSERT_BIT_LOW(USART_CR1_UE_Pos, USART1->CR1);
    TEST_ASSERT_BIT_LOW(USART_CR2_STOP_Pos, USART1->CR2);
    TEST_ASSERT_BIT_HIGH(USART_CR2_STOP_Pos + 1, USART1->CR2);
    TEST_ASSERT_EQUAL((2 << 4) | 2, USART1->BRR);
};

/// @brief Test enabling the USART peripheral
void test_enable(void)
{
    setUp();
    USART::Controller<IO::USART_1> UUT(9'600, 8, USART::StopBits::_1_0);

    // Test the enabling
    UUT.enable();
    TEST_ASSERT_BIT_HIGH(USART_CR1_UE_Pos, USART1->CR1);

    // Test dissabling the bus again
    UUT.disable();
    TEST_ASSERT_BIT_LOW(USART_CR1_UE_Pos, USART1->CR1);
};

/// @brief Test checking the transfer state
void test_last_transmit_finished(void)
{
    setUp();
    USART::Controller<IO::USART_1> UUT(9'600, 8, USART::StopBits::_1_0);

    // When the transmit is not finished
    USART1->SR = 0;
    TEST_ASSERT_FALSE( UUT.last_transmit_finished() );

    // When the transmit is finished
    USART1->SR = USART_SR_TXE;
    TEST_ASSERT_TRUE( UUT.last_transmit_finished() );
};

/// @brief Test checking whether the bus is busy
void test_bus_busy(void)
{
    setUp();
    USART::Controller<IO::USART_1> UUT(9'600, 8, USART::StopBits::_1_0);

    // Check when bus is busy
    USART1->SR = 0;
    TEST_ASSERT_TRUE( UUT.is_busy() );

    // Check when bus is not busy
    USART1->SR = USART_SR_TC;
    TEST_ASSERT_FALSE( UUT.is_busy() );
};

/// @brief Test sending data
void test_send_data(void)
{
    setUp();
    USART::Controller<IO::USART_1> UUT(9'600, 8, USART::StopBits::_1_0);

    // Test sending data when no error is present
    Bus::Data_t payload{0xAB};
    USART1->SR = USART_SR_TC | USART_SR_TXE;
    USART1->DR = 0;
    UUT.set_error(Error::Code::None);
    TEST_ASSERT_TRUE( UUT.send_data(payload, 1) );
    TEST_ASSERT_EQUAL( Error::Code::None, UUT.get_error() );
    TEST_ASSERT_EQUAL( payload.byte[0], USART1->DR);

    // Test sending data when the TX buffer is not empty
    USART1->SR = USART_SR_TC;
    USART1->DR = 0;
    UUT.set_error(Error::Code::None);
    TEST_ASSERT_FALSE( UUT.send_data(payload, 1) );
    TEST_ASSERT_EQUAL( Error::Code::USART_Timeout, UUT.get_error() );
    TEST_ASSERT_EQUAL( 0, USART1->DR);

    // Test sending data when the peripheral is busy
    USART1->SR = USART_SR_TXE;
    USART1->DR = 0;
    UUT.set_error(Error::Code::None);
    TEST_ASSERT_FALSE( UUT.send_data(payload, 1) );
    TEST_ASSERT_EQUAL( Error::Code::USART_BUS_Busy_Error, UUT.get_error() );
    TEST_ASSERT_EQUAL( 0, USART1->DR);

    // Test sending data when no error is present
    payload.value = 0xCCDD;
    USART1->SR = USART_SR_TC | USART_SR_TXE;
    USART1->DR = 0;
    UUT.set_error(Error::Code::None);
    TEST_ASSERT_TRUE( UUT.send_data(payload, 2) );
    TEST_ASSERT_EQUAL( Error::Code::None, UUT.get_error() );
    TEST_ASSERT_EQUAL( payload.byte[0], USART1->DR);
};

/// @brief Test sending an array
void test_send_array(void)
{
    setUp();
    std::array<unsigned char, 10> buffer{0};
    USART::Controller<IO::USART_1> UUT(1'000'000);

    // fill array with consecutive numbers
    std::iota( buffer.begin(), buffer.end(), 0);

    // Test sending the array
    USART1->SR = USART_SR_TC | USART_SR_TXE;
    UUT.set_error(Error::Code::None);
    TEST_ASSERT_TRUE( UUT.send_array(buffer.data(), 6) );
    TEST_ASSERT_EQUAL(Error::Code::None, UUT.get_error());
    TEST_ASSERT_EQUAL( 5, USART1->DR);

    // Test sending an array, when the bus is busy
    USART1->SR = USART_SR_TXE;
    USART1->DR = 0;
    UUT.set_error(Error::Code::None);
    TEST_ASSERT_FALSE( UUT.send_array(buffer.data(), 6) );
    TEST_ASSERT_EQUAL(Error::Code::USART_BUS_Busy_Error, UUT.get_error());
    TEST_ASSERT_EQUAL( 0, USART1->DR);
};

// === Main ===
int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_rcc_clock_enable);
    RUN_TEST(test_constructor);
    RUN_TEST(test_enable);
    RUN_TEST(test_last_transmit_finished);
    RUN_TEST(test_bus_busy);
    RUN_TEST(test_send_data);
    RUN_TEST(test_send_array);
    return UNITY_END();
};