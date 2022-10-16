/**
 * OTOS - Open Tec Operating System
 * Copyright (c) 2022 Sebastian Oberschwendtner, sebastian.oberschwendtner@gmail.com
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
 * @file    test_stn32_dma.cpp
 * @author  SO
 * @version v3.2.0
 * @date    18-July-2022
 * @brief   Unit tests for testing the DMA driver for stm32 microcontrollers.
 ******************************************************************************
 */

// ****** Includes ******
#include <unity.h>
#include <mock.h>
#include "stm32/dma_stm32.h"

/** === Test List ===
* ▢ Initialization:
*   ✓ Clocks of DMA1 and DMA2 are enabled.
*   ✓ Stream peripherals are selected.
*   ✓ Stream channel is assigned.
* ▢ Settings:
*   ✓ Peripheral can be assigned.
*   ✓ Memory can be assigned.
*   ▢ Peripheral size can be set.
*   ✓ Memory size is deduced from integral data type.
*   ✓ Memory size and number of transfers is deduced when assigning an array.
*   ✓ Data direction can be assigned.
*   ▢ Priority can be assigned.
*   ✓ Memory increment can be turned on.
*   ✓ Peripheral increment can be turned on.
* ▢ Enable and disable Stream.
* ▢ Status:
*   ✓ Read whether the stream is enabled.
*   ▢ Read interrupt flags:
*     ✓ Transfer Finished
*     ▢ Half Transfer Finished
*     ▢ Transfer Error
*     ▢ Direct Mode Error
*   ▢ Clear interrupt flags:
*     ✓ All flags
*     ▢ Transfer Finished.
*     ▢ Half Transfer Finished
*     ▢ Transfer Error
*     ▢ Direct Mode Error
* ▢ FIFO Control:
*/

// === Mocks ===

// === Tests ===
void setUp(void)
{
    // set stuff up here
    RCC->registers_to_default();
    DMA1->registers_to_default();
    DMA1_Stream0->registers_to_default();
    DMA1_Stream1->registers_to_default();
};

void tearDown(void){
    // clean stuff up here
};

/// @brief Test the initialization of the DMA Stream
void test_init(void)
{
    // Create Stream with DMA1
    // => The init struct can be directly given, see initialization of DMA2
    DMA::Stream_t stream;
    stream.DMA = 1;
    stream.Stream = 0;
    stream.Channel = 0;
    DMA::Stream DMA1{stream};

    // perform tests
    TEST_ASSERT_BIT_HIGH(RCC_AHB1ENR_DMA1EN_Pos, RCC->AHB1ENR);
    TEST_ASSERT_BIT_LOW(RCC_AHB1ENR_DMA2EN_Pos, RCC->AHB2ENR);

    // Create Stream with DMA2
    DMA::Stream DMA2{{2,0,0}};

    // perform tests
    TEST_ASSERT_BIT_HIGH(RCC_AHB1ENR_DMA1EN_Pos, RCC->AHB1ENR);
    TEST_ASSERT_BIT_HIGH(RCC_AHB1ENR_DMA2EN_Pos, RCC->AHB2ENR);
};

/// @brief Test the channel selection when initializing the DMA Stream
void test_channel_selection(void)
{
    // test Stream0 Channel0
    DMA::Stream DMA1{{1,0,0}};
    TEST_ASSERT_BITS(DMA_SxCR_CHSEL_Msk, 0 << DMA_SxCR_CHSEL_Pos, DMA1_Stream0->CR);

    // test Stream0 Channel1
    DMA1 = DMA::Stream{{1,0,1}};
    TEST_ASSERT_BITS(DMA_SxCR_CHSEL_Msk, 1 << DMA_SxCR_CHSEL_Pos, DMA1_Stream0->CR);

    // test Stream0 Channel2
    DMA1 = DMA::Stream{{1,0,2}};
    TEST_ASSERT_BITS(DMA_SxCR_CHSEL_Msk, 2 << DMA_SxCR_CHSEL_Pos, DMA1_Stream0->CR);

    // test Stream1 Channel3
    DMA1 = DMA::Stream{{1,1,3}};
    TEST_ASSERT_BITS(DMA_SxCR_CHSEL_Msk, 3 << DMA_SxCR_CHSEL_Pos, DMA1_Stream1->CR);
};

/// @brief Test assigning a peripheral to a DMA Stream
void test_assign_peripheral_address(void)
{
    // Create DMA Controller
    DMA::Stream UUT{{1,0,1}};
    
    // Assign peripheral address
    UUT.assign_peripheral(I2C1->DR);

    // perform tests
    // => Only check the 32bits since the peripheral address is only 32bits long
    TEST_ASSERT_EQUAL_HEX32(reinterpret_cast<std::uintptr_t>(&I2C1->DR), DMA1_Stream0->PAR);

    // Assign peripheral address and turn increment on
    UUT.assign_peripheral(I2C1->DR, true);
    TEST_ASSERT_BIT_HIGH(DMA_SxCR_PINC_Pos, DMA1_Stream0->CR);

    // Assign peripheral address and turn increment off
    UUT.assign_peripheral(I2C1->DR, false);
    TEST_ASSERT_BIT_LOW(DMA_SxCR_PINC_Pos, DMA1_Stream0->CR);
};

/// @brief Test setting the peripheral size
void test_set_peripheral_size(void)
{
    // Create DMA Controller
    DMA::Stream UUT{{1,0,1}};

    // Set peripheral size to 8bit
    UUT.set_peripheral_size(DMA::Width::_8bit);
    TEST_ASSERT_BITS(DMA_SxCR_PSIZE_Msk, 0 << DMA_SxCR_PSIZE_Pos, DMA1_Stream0->CR);

    // Set peripheral size to 16bit
    UUT.set_peripheral_size(DMA::Width::_16bit);
    TEST_ASSERT_BITS(DMA_SxCR_PSIZE_Msk, 1 << DMA_SxCR_PSIZE_Pos, DMA1_Stream0->CR);

    // Set peripheral size to 32bit
    UUT.set_peripheral_size(DMA::Width::_32bit);
    TEST_ASSERT_BITS(DMA_SxCR_PSIZE_Msk, 2 << DMA_SxCR_PSIZE_Pos, DMA1_Stream0->CR);
};

/// @brief Assign a memory address to a DMA Stream
void test_assign_memory_address(void)
{
    // Create DMA Controller and memory
    DMA::Stream UUT{{1,0,1}};
    unsigned int mem = 0;
    
    // Assign memory address
    UUT.assign_memory(mem);
    
    // perform tests
    // => Only check the 32bits since the memory address is only 32bits long
    TEST_ASSERT_EQUAL_HEX32(reinterpret_cast<std::uintptr_t>(&mem), DMA1_Stream0->M0AR);

    // Assign memory and set the increment mode
    UUT.assign_memory(mem, true);
    TEST_ASSERT_BIT_HIGH(DMA_SxCR_MINC_Pos, DMA1_Stream0->CR);

    // Disabling the increment mode clears the flag
    UUT.assign_memory(mem, false);
    TEST_ASSERT_BIT_LOW(DMA_SxCR_MINC_Pos, DMA1_Stream0->CR);

    // Assign 8bit memory and test the memory size
    unsigned char mem8 = 0;
    UUT.assign_memory(mem8);
    TEST_ASSERT_BITS(DMA_SxCR_MSIZE_Msk, 0 << DMA_SxCR_MSIZE_Pos, DMA1_Stream0->CR);

    // Assign 16bit memory and test the memory size
    std::uint16_t mem16 = 0;
    UUT.assign_memory(mem16);
    TEST_ASSERT_BITS(DMA_SxCR_MSIZE_Msk, 1 << DMA_SxCR_MSIZE_Pos, DMA1_Stream0->CR);

    // Assign 32bit memory and test the memory size
    std::uint32_t mem32 = 0;
    UUT.assign_memory(mem32);
    TEST_ASSERT_BITS(DMA_SxCR_MSIZE_Msk, 2 << DMA_SxCR_MSIZE_Pos, DMA1_Stream0->CR);
};

/// @brief Test setting the number of data items to transfer
void test_set_number_of_transfers(void)
{
    // Create DMA Controller
    DMA::Stream UUT{{1,0,1}};
    
    // Set number of data items to 1
    UUT.set_number_of_transfers(1);
    TEST_ASSERT_BITS(DMA_SxNDT_Msk, 1 << DMA_SxNDT_Pos, DMA1_Stream0->NDTR);

    // Set number of data items to 2
    UUT.set_number_of_transfers(2);
    TEST_ASSERT_BITS(DMA_SxNDT_Msk, 2 << DMA_SxNDT_Pos, DMA1_Stream0->NDTR);

    // Set number of data items to 3
    UUT.set_number_of_transfers(3);
    TEST_ASSERT_BITS(DMA_SxNDT_Msk, 3 << DMA_SxNDT_Pos, DMA1_Stream0->NDTR);
};

/// @brief Test assigning an array to a DMA Stream
void test_assign_array(void)
{
    // Create DMA Stream and memory array
    DMA::Stream UUT{{1,0,1}};
    std::array<std::uint8_t, 5> array8 = {0,1,2,3,4};
    std::array<std::uint16_t, 5> array16 = {0,1,2,3,4};

    // Assign array and test the stream config
    UUT.assign_memory(array8);
    TEST_ASSERT_EQUAL_HEX32(reinterpret_cast<std::uintptr_t>(&array8[0]), DMA1_Stream0->M0AR);
    TEST_ASSERT_BIT_HIGH(DMA_SxCR_MINC_Pos, DMA1_Stream0->CR);
    TEST_ASSERT_EQUAL( array8.size(), DMA1_Stream0->NDTR);

    // Assign array and test the stream config
    UUT.assign_memory(array16, false);
    TEST_ASSERT_EQUAL_HEX32(reinterpret_cast<std::uintptr_t>(&array16[0]), DMA1_Stream0->M0AR);
    TEST_ASSERT_BIT_LOW(DMA_SxCR_MINC_Pos, DMA1_Stream0->CR);
    TEST_ASSERT_BITS(DMA_SxCR_MSIZE_Msk, 1 << DMA_SxCR_MSIZE_Pos, DMA1_Stream0->CR);
    TEST_ASSERT_EQUAL( array16.size(), DMA1_Stream0->NDTR);
};

/// @brief Test reading of status flags
void test_reading_status(void)
{
    // Create DMA Stream
    DMA::Stream UUT{{1,1,1}};

    // perform tests
    TEST_ASSERT_FALSE(UUT.is_enabled());
    DMA1_Stream1->CR = DMA_SxCR_EN;
    TEST_ASSERT_TRUE(UUT.is_enabled());
};

/// @brief Test setting the transfer direction
void test_set_transfer_direction(void)
{
    // Create DMA Stream
    DMA::Stream UUT{{1,1,1}};
    
    // perform tests
    UUT.set_direction(DMA::Direction::peripheral_to_memory);
    TEST_ASSERT_BITS(DMA_SxCR_DIR_Msk, 0, DMA1_Stream1->CR);
    UUT.set_direction(DMA::Direction::memory_to_peripheral);
    TEST_ASSERT_BITS(DMA_SxCR_DIR_Msk, DMA_SxCR_DIR_0, DMA1_Stream1->CR);
    UUT.set_direction(DMA::Direction::memory_to_memory);
    TEST_ASSERT_BITS(DMA_SxCR_DIR_Msk, DMA_SxCR_DIR_1, DMA1_Stream1->CR);
};

/// @brief Test reading the TCIF flag
void test_read_transfer_complete_flag(void)
{
    // Create DMA Stream0
    DMA::Stream UUT{{1,0,0}};
    // Read flag from Stream0
    TEST_ASSERT_FALSE(UUT.is_transfer_complete());
    DMA1->LISR = DMA_LISR_TCIF0;
    TEST_ASSERT_TRUE(UUT.is_transfer_complete());

    // Create DMA Stream1
    UUT = DMA::Stream{{1,1,0}};
    // Read flag from Stream1
    TEST_ASSERT_FALSE(UUT.is_transfer_complete());
    DMA1->LISR = DMA_LISR_TCIF1;
    TEST_ASSERT_TRUE(UUT.is_transfer_complete());

    // Create DMA Stream2
    UUT = DMA::Stream{{1,2,0}};
    // Read flag from Stream2
    TEST_ASSERT_FALSE(UUT.is_transfer_complete());
    DMA1->LISR = DMA_LISR_TCIF2;
    TEST_ASSERT_TRUE(UUT.is_transfer_complete());

    // Create DMA Stream3
    UUT = DMA::Stream{{1,3,0}};
    // Read flag from Stream3
    TEST_ASSERT_FALSE(UUT.is_transfer_complete());
    DMA1->LISR = DMA_LISR_TCIF3;
    TEST_ASSERT_TRUE(UUT.is_transfer_complete());

    // Reset the flag register
    DMA1->registers_to_default();

    // Create DMA Stream4
    UUT = DMA::Stream{{1,4,0}};
    // Read flag from Stream4
    TEST_ASSERT_FALSE(UUT.is_transfer_complete());
    DMA1->HISR = DMA_HISR_TCIF4;
    TEST_ASSERT_TRUE(UUT.is_transfer_complete());
};

/// @brief Test clearing the interrupt flags
void test_clear_interrupt_flags(void)
{
    // Create DMA Stream0
    DMA::Stream UUT{{1,0,0}};
    
    // Clear all flags
    UUT.clear_interrupt_flag(DMA::Flag::All);
    TEST_ASSERT_BITS_HIGH(
        DMA_LIFCR_CTCIF0 | DMA_LIFCR_CHTIF0 | DMA_LIFCR_CTEIF0 | DMA_LIFCR_CDMEIF0 | DMA_LIFCR_CFEIF0,
        DMA1->LIFCR
    );

    // Same test for Stream1
    UUT = DMA::Stream{{1,1,0}};
    UUT.clear_interrupt_flag(DMA::Flag::All);
    TEST_ASSERT_BITS_HIGH(
        DMA_LIFCR_CTCIF1 | DMA_LIFCR_CHTIF1 | DMA_LIFCR_CTEIF1 | DMA_LIFCR_CDMEIF1 | DMA_LIFCR_CFEIF1,
        DMA1->LIFCR
    );

    // Same test for Stream2
    UUT = DMA::Stream{{1,2,0}};
    UUT.clear_interrupt_flag(DMA::Flag::All);
    TEST_ASSERT_BITS_HIGH(
        DMA_LIFCR_CTCIF2 | DMA_LIFCR_CHTIF2 | DMA_LIFCR_CTEIF2 | DMA_LIFCR_CDMEIF2 | DMA_LIFCR_CFEIF2,
        DMA1->LIFCR
    );

    // Same test for Stream4
    UUT = DMA::Stream{{1,4,0}};
    UUT.clear_interrupt_flag(DMA::Flag::All);
    TEST_ASSERT_BITS_HIGH(
        DMA_HIFCR_CTCIF4 | DMA_HIFCR_CHTIF4 | DMA_HIFCR_CTEIF4 | DMA_HIFCR_CDMEIF4 | DMA_HIFCR_CFEIF4,
        DMA1->HIFCR
    );
};

/// @brief Test enabling the dma stream
void test_enable_dma_stream(void)
{
    // Create DMA Stream0
    DMA::Stream UUT{{1,0,0}};
    
    // Enable Stream0
    TEST_ASSERT_TRUE(UUT.enable());
    TEST_ASSERT_BIT_HIGH(DMA_SxCR_EN_Pos, DMA1_Stream0->CR);

    // This should reset all interrupt flags
    TEST_ASSERT_BITS_HIGH(
        DMA_LIFCR_CTCIF0 | DMA_LIFCR_CHTIF0 | DMA_LIFCR_CTEIF0 | DMA_LIFCR_CDMEIF0 | DMA_LIFCR_CFEIF0,
        DMA1->LIFCR
    );

    // Stream should only be enable when the EN flag is 0
    DMA1->registers_to_default();
    DMA1_Stream0->registers_to_default();
    DMA1_Stream0->CR = DMA_SxCR_EN;

    TEST_ASSERT_FALSE(UUT.enable());
    TEST_ASSERT_BIT_HIGH(DMA_SxCR_EN_Pos, DMA1_Stream0->CR);
    TEST_ASSERT_BITS_LOW(
        DMA_LIFCR_CTCIF0 | DMA_LIFCR_CHTIF0 | DMA_LIFCR_CTEIF0 | DMA_LIFCR_CDMEIF0 | DMA_LIFCR_CFEIF0,
        DMA1->LIFCR
    );
};

// === Main ===
int main(int argc, char **argv)
{
    UNITY_BEGIN();
    RUN_TEST(test_init);
    RUN_TEST(test_channel_selection);
    RUN_TEST(test_assign_peripheral_address);
    RUN_TEST(test_set_peripheral_size);
    RUN_TEST(test_assign_memory_address);
    RUN_TEST(test_set_number_of_transfers);
    RUN_TEST(test_assign_array);
    RUN_TEST(test_reading_status);
    RUN_TEST(test_set_transfer_direction);
    RUN_TEST(test_read_transfer_complete_flag);
    RUN_TEST(test_clear_interrupt_flags);
    RUN_TEST(test_enable_dma_stream);
    return UNITY_END();
};
