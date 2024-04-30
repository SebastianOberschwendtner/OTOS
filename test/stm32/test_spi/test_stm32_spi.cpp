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
 * @file    test_stm32_spi.cpp
 * @author  SO
 * @version v5.0.0
 * @date    22-December-2021
 * @brief   Unit tests for testing the spi driver for stm32 microcontrollers.
 ==============================================================================
 */

/* === Includes === */
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

/* === Fixtures === */
using stm32::Peripheral;

/* Mock DMA Stream */
struct DMA_Stream
{
    Mock::Callable<> assign_peripheral;
    Mock::Callable<> set_peripheral_size;
    Mock::Callable<dma::Direction> set_direction;
};

/* === Tests === */
void setUp()
{
    /* set stuff up here */
    RCC->registers_to_default();
    SPI1->registers_to_default();
};

void tearDown(){
    /* clean stuff up here */
};

/** 
 * @brief Test the initialization of the controller
 */
void test_rcc_clock_enable()
{
    /* Create Controller -> SPI1 */
    RCC->registers_to_default();
    auto UUT1 = spi::Controller::create<Peripheral::SPI_1>(1'000'000);
    TEST_ASSERT_BIT_HIGH(RCC_APB2ENR_SPI1EN_Pos, RCC->APB2ENR);

    /* Create Controller -> SPI2 */
    RCC->registers_to_default();
    auto UUT2 = spi::Controller::create<Peripheral::SPI_2>(1'000'000);
    TEST_ASSERT_BIT_HIGH(RCC_APB1ENR_SPI2EN_Pos, RCC->APB1ENR);

    /* Create Controller -> SPI3 */
    RCC->registers_to_default();
    auto UUT3 = spi::Controller::create<Peripheral::SPI_3>(1'000'000);
    TEST_ASSERT_BIT_HIGH(RCC_APB1ENR_SPI3EN_Pos, RCC->APB1ENR);

    /* Create Controller -> SPI4 */
    RCC->registers_to_default();
    auto UUT4 = spi::Controller::create<Peripheral::SPI_4>(1'000'000);
    TEST_ASSERT_BIT_HIGH(RCC_APB2ENR_SPI4EN_Pos, RCC->APB2ENR);

    /* Create Controller -> SPI5 */
    RCC->registers_to_default();
    auto UUT5 = spi::Controller::create<Peripheral::SPI_5>(1'000'000);
    TEST_ASSERT_BIT_HIGH(RCC_APB2ENR_SPI5EN_Pos, RCC->APB2ENR);

    /* Create Controller -> SPI6 */
    RCC->registers_to_default();
    auto UUT6 = spi::Controller::create<Peripheral::SPI_6>(1'000'000);
    TEST_ASSERT_BIT_HIGH(RCC_APB2ENR_SPI6EN_Pos, RCC->APB2ENR);
};

/** 
 * @brief Test the configuration of the peripheral during construction
 */
void test_constructor()
{
    setUp();

    /* Create Object */
    auto spi1 = spi::Controller::create<Peripheral::SPI_1>(1'000'000);

    /* Test side effects */
    /* in Master Mode */
    TEST_ASSERT_BIT_HIGH(SPI_CR1_MSTR_Pos, SPI1->CR1);

    /* Peripheral is not enabled */
    TEST_ASSERT_BIT_LOW(SPI_CR1_SPE_Pos, SPI1->CR1);
};

/** 
 * @brief Test the prescaler configuration
 */
void test_set_prescaler()
{
    setUp();

    /* *** Assume F_APBx = 4'000'000 *** */

    /* Test baudrate of 2'000'000 */
    auto spi = spi::Controller::create<Peripheral::SPI_1>(2'000'000);
    TEST_ASSERT_BITS(SPI_CR1_BR_Msk, (0 << SPI_CR1_BR_Pos), SPI1->CR1);

    /* Test baudrate of 1'000'000 */
    spi = spi::Controller::create<Peripheral::SPI_1>(1'000'000);
    TEST_ASSERT_BITS(SPI_CR1_BR_Msk, (1 << SPI_CR1_BR_Pos), SPI1->CR1);

    /* Test baudrate of 500'000 */
    spi = spi::Controller::create<Peripheral::SPI_1>(500'000);
    TEST_ASSERT_BITS(SPI_CR1_BR_Msk, (2 << SPI_CR1_BR_Pos), SPI1->CR1);

    /* Test baudrate of 250'000 */
    spi = spi::Controller::create<Peripheral::SPI_1>(250'000);
    TEST_ASSERT_BITS(SPI_CR1_BR_Msk, (3 << SPI_CR1_BR_Pos), SPI1->CR1);

    /* Test baudrate of 125'000 */
    spi = spi::Controller::create<Peripheral::SPI_1>(125'000);
    TEST_ASSERT_BITS(SPI_CR1_BR_Msk, (4 << SPI_CR1_BR_Pos), SPI1->CR1);

    /* Test baudrate of 62'500 */
    spi = spi::Controller::create<Peripheral::SPI_1>(62'500);
    TEST_ASSERT_BITS(SPI_CR1_BR_Msk, (5 << SPI_CR1_BR_Pos), SPI1->CR1);

    /* Test baudrate of 31'250 */
    spi = spi::Controller::create<Peripheral::SPI_1>(31'250);
    TEST_ASSERT_BITS(SPI_CR1_BR_Msk, (6 << SPI_CR1_BR_Pos), SPI1->CR1);

    /* Test baudrate of 15'625 */
    spi = spi::Controller::create<Peripheral::SPI_1>(15'625);
    TEST_ASSERT_BITS(SPI_CR1_BR_Msk, (7 << SPI_CR1_BR_Pos), SPI1->CR1);
};

/** 
 * @brief Test the correct setting of the clocking timing
 */
void test_set_clock_properties()
{
    setUp();
    auto UUT = spi::Controller::create<Peripheral::SPI_1>(1'000'000);

    /* test all four possible configurations */
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

/** 
 * @brief Test the target select pin management
 */
void test_set_target_selection()
{
    setUp();
    auto UUT = spi::Controller::create<Peripheral::SPI_1>(1'000'000);

    /* Select target selection using a hardware pin */
    UUT.set_use_hardware_chip_select(true);
    TEST_ASSERT_BIT_LOW(SPI_CR1_SSM_Pos, SPI1->CR1);
    TEST_ASSERT_BIT_LOW(SPI_CR1_SSI_Pos, SPI1->CR1);
    TEST_ASSERT_BIT_HIGH(SPI_CR2_SSOE_Pos, SPI1->CR2);

    /* Disable hardware selection */
    UUT.set_use_hardware_chip_select(false);
    TEST_ASSERT_BIT_HIGH(SPI_CR1_SSM_Pos, SPI1->CR1);
    TEST_ASSERT_BIT_HIGH(SPI_CR1_SSI_Pos, SPI1->CR1);
    TEST_ASSERT_BIT_LOW(SPI_CR2_SSOE_Pos, SPI1->CR2);

    /* Repeat target selection using a hardware pin */
    UUT.set_use_hardware_chip_select(true);
    TEST_ASSERT_BIT_LOW(SPI_CR1_SSM_Pos, SPI1->CR1);
    TEST_ASSERT_BIT_LOW(SPI_CR1_SSI_Pos, SPI1->CR1);
    TEST_ASSERT_BIT_HIGH(SPI_CR2_SSOE_Pos, SPI1->CR2);
};

/** 
 * @brief Test enabling and disabling the peripheral
 */
void test_enable()
{
    setUp();
    auto UUT = spi::Controller::create<Peripheral::SPI_1>(1'000'000);

    /* Enable bus */
    UUT.enable();
    TEST_ASSERT_BIT_HIGH(SPI_CR1_SPE_Pos, SPI1->CR1);

    /* Disable bus again */
    UUT.disable();
    TEST_ASSERT_BIT_LOW(SPI_CR1_SPE_Pos, SPI1->CR1);
};

/** 
 * @brief Test status of TX register
 */
void test_last_transmit_finished()
{
    setUp();
    auto UUT = spi::Controller::create<Peripheral::SPI_1>(1'000'000);

    /* Test register empty */
    SPI1->SR = SPI_SR_TXE;
    TEST_ASSERT_TRUE(UUT.last_transmit_finished());

    /* Test register not empty */
    SPI1->SR = 0;
    TEST_ASSERT_FALSE(UUT.last_transmit_finished());
};

/** 
 * @brief Test status of RX register
 */
void test_rx_data_valid()
{
    setUp();
    auto UUT = spi::Controller::create<Peripheral::SPI_1>(1'000'000);

    /* Test register empty */
    SPI1->SR = SPI_SR_RXNE;
    TEST_ASSERT_TRUE(UUT.RX_data_valid());

    /* Test register not empty */
    SPI1->SR = 0;
    TEST_ASSERT_FALSE(UUT.RX_data_valid());
};

/** 
 * @brief Test status bus busy
 */
void test_bus_busy()
{
    setUp();
    auto UUT = spi::Controller::create<Peripheral::SPI_1>(1'000'000);

    /* Test register empty */
    SPI1->SR = SPI_SR_BSY;
    TEST_ASSERT_TRUE(UUT.is_busy());

    /* Test register not empty */
    SPI1->SR = 0;
    TEST_ASSERT_FALSE(UUT.is_busy());
};

/** 
 * @brief Test sending data
 */
void test_send_data()
{
    setUp();
    auto UUT = spi::Controller::create<Peripheral::SPI_1>(1'000'000);

    /* Test sending a byte, when TX buffer is empty */
    SPI1->SR = SPI_SR_TXE;
    UUT.set_error(error::Code::None);
    bus::Data_t payload{0xAA};
    TEST_ASSERT_TRUE(UUT.send_data(payload, 1));
    TEST_ASSERT_EQUAL(error::Code::None, UUT.get_error());
    TEST_ASSERT_EQUAL(payload.value, SPI1->DR);

    /* Test sending a byte, when TX buffer is not empty */
    SPI1->SR = 0;
    SPI1->DR = 0;
    UUT.set_error(error::Code::None);
    TEST_ASSERT_FALSE(UUT.send_data(payload, 1));
    TEST_ASSERT_EQUAL(error::Code::SPI_Timeout, UUT.get_error());
    TEST_ASSERT_EQUAL(0, SPI1->DR);

    /* Test sending a byte, when the bus is busy */
    SPI1->SR = SPI_SR_BSY;
    SPI1->DR = 0;
    UUT.set_error(error::Code::None);
    TEST_ASSERT_FALSE(UUT.send_data(payload, 1));
    TEST_ASSERT_EQUAL(error::Code::SPI_BUS_Busy_Error, UUT.get_error());
    TEST_ASSERT_EQUAL(0, SPI1->DR);

    /* Test sending a word, when TX buffer is empty */
    SPI1->SR = SPI_SR_TXE;
    payload.value = 0xAABB;
    UUT.set_error(error::Code::None);
    TEST_ASSERT_TRUE(UUT.send_data(payload, 2));
    TEST_ASSERT_EQUAL(error::Code::None, UUT.get_error());
    TEST_ASSERT_EQUAL(payload.byte[0], SPI1->DR);
};

/** 
 * @brief Test read data
 */
void test_read_data()
{
    setUp();
    auto UUT = spi::Controller::create<Peripheral::SPI_1>(1'000'000);

    /* Test reading a byte, when RX buffer is not empty */
    SPI1->SR = SPI_SR_RXNE | SPI_SR_TXE;
    SPI1->DR = 0x12;
    TEST_ASSERT_TRUE(UUT.read_data(0x12, 1));
    TEST_ASSERT_EQUAL(error::Code::None, UUT.get_error());
    TEST_ASSERT_EQUAL(0x00, UUT.get_rx_data().byte[0]);

    /* Test read a byte, when RX buffer is empty */
    SPI1->SR = SPI_SR_TXE;
    SPI1->DR = 0x12;
    TEST_ASSERT_FALSE(UUT.read_data(0x12, 1));
    TEST_ASSERT_EQUAL(error::Code::SPI_Timeout, UUT.get_error());
};

/** 
 * @brief Test sending an array
 */
void test_send_array()
{
    setUp();
    std::array<uint8_t, 10> buffer{0};
    auto UUT = spi::Controller::create<Peripheral::SPI_1>(1'000'000);

    /* fill array with consecutive numbers */
    std::iota(buffer.begin(), buffer.end(), 0);

    /* Test sending the array */
    SPI1->SR = SPI_SR_TXE;
    UUT.set_error(error::Code::None);
    TEST_ASSERT_TRUE(UUT.send_array(buffer.data(), 6));
    TEST_ASSERT_EQUAL(error::Code::None, UUT.get_error());
    TEST_ASSERT_EQUAL(5, SPI1->DR);

    /* Test sending an array, when the bus is busy */
    SPI1->SR = SPI_SR_BSY;
    SPI1->DR = 0;
    UUT.set_error(error::Code::None);
    TEST_ASSERT_FALSE(UUT.send_array(buffer.data(), 6));
    TEST_ASSERT_EQUAL(error::Code::SPI_BUS_Busy_Error, UUT.get_error());
    TEST_ASSERT_EQUAL(0, SPI1->DR);
};

/** 
 * @brief Test reading an array
 */
void test_read_array()
{
    setUp();
    std::array<uint8_t, 10> buffer{0};
    auto UUT = spi::Controller::create<Peripheral::SPI_1>(1'000'000);

    /* fill array with consecutive numbers */
    std::iota(buffer.begin(), buffer.end(), 1U);

    /* Test reading the array */
    SPI1->SR = SPI_SR_RXNE | SPI_SR_TXE;
    TEST_ASSERT_TRUE(UUT.read_array(buffer.data(), 6));
    TEST_ASSERT_EQUAL(error::Code::None, UUT.get_error());
    TEST_ASSERT_EQUAL(0x00, buffer[0]);
    TEST_ASSERT_EQUAL(0x00, buffer[5]);
    TEST_ASSERT_EQUAL(0x07, buffer[6]);

    /* Test reading an array, when the bus is busy */
    SPI1->SR = SPI_SR_BSY;
    TEST_ASSERT_FALSE(UUT.read_array(buffer.data(), 6));
    TEST_ASSERT_EQUAL(error::Code::SPI_BUS_Busy_Error, UUT.get_error());
};

/** 
 * @brief Test changing the data width of the SPI
 */
void test_set_data_width()
{
    /* Create SPI Controller */
    auto UUT = spi::Controller::create<Peripheral::SPI_1>(1'000'000);

    /* Test setting the data width to 16 bits */
    UUT.set_data_to_16bit();
    TEST_ASSERT_BIT_HIGH(SPI_CR1_DFF_Pos, SPI1->CR1);
    TEST_ASSERT_EQUAL(error::Code::None, UUT.get_error());
};

/** 
 * @brief Test creating a DMA Stream with the SPI as target/source
 */
void test_create_dma_stream()
{
    /* Create SPI Controller */
    auto UUT = spi::Controller::create<Peripheral::SPI_1>(1'000'000);

    /* Create a DMA stream */
    auto stream = UUT.create_dma_stream(DMA_Stream{}, dma::Direction::memory_to_peripheral);

    /* Test side effects */
    TEST_ASSERT_BIT_HIGH(SPI_CR2_TXDMAEN_Pos, SPI1->CR2);
    stream.assign_peripheral.assert_called_once();
    stream.set_direction.assert_called_once_with(static_cast<int>(dma::Direction::memory_to_peripheral));
    TEST_ASSERT_EQUAL(0, stream.set_peripheral_size.call_count);

    /* Create DMA stream when SPI is in 16bit mode */
    UUT.set_data_to_16bit();
    stream = UUT.create_dma_stream(DMA_Stream{}, dma::Direction::memory_to_peripheral);
    stream.set_peripheral_size.assert_called_once_with(static_cast<int>(dma::Width::_16bit));

    /* Create a DMA stream with a different direction */
    stream = UUT.create_dma_stream(DMA_Stream{}, dma::Direction::peripheral_to_memory);
    TEST_ASSERT_BIT_HIGH(SPI_CR2_RXDMAEN_Pos, SPI1->CR2);
};

/* === Main === */
int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_rcc_clock_enable);
    RUN_TEST(test_constructor);
    RUN_TEST(test_set_prescaler);
    RUN_TEST(test_set_clock_properties);
    RUN_TEST(test_set_target_selection);
    RUN_TEST(test_enable);
    RUN_TEST(test_last_transmit_finished);
    RUN_TEST(test_rx_data_valid);
    RUN_TEST(test_bus_busy);
    RUN_TEST(test_send_data);
    RUN_TEST(test_read_data);
    RUN_TEST(test_send_array);
    RUN_TEST(test_read_array);
    RUN_TEST(test_create_dma_stream);
    RUN_TEST(test_set_data_width);
    return UNITY_END();
};