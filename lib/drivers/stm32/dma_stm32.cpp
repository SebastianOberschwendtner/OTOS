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
 ==============================================================================
 * @file    dma_stm32.cpp
 * @author  SO
 * @version v2.9.0
 * @date    18-July-2022
 * @brief   DMA driver for STM32 microcontrollers.
 ==============================================================================
 */

// *** Includes ***
#include "dma_stm32.h"

// *** Methods ***
DMA::Stream::Stream(const Stream_t &stream)
{
    // Enable the DMA clock
#if defined(STM32F4)
    if (stream.DMA == 1)
        RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
    else
        RCC->AHB2ENR |= RCC_AHB1ENR_DMA2EN;
#elif defined(STM32L0)
    RCC->AHBENR |= RCC_AHBENR_DMAEN;
#endif

    // Set the instance
    switch (stream.Stream)
    {
#if defined(STM32F4)
    case 0: // Stream 0
        this->Instance = (stream.DMA == 1) ? DMA1_Stream0 : DMA2_Stream0;
        this->Flags = reinterpret_cast<volatile std::uintptr_t*>((stream.DMA == 1) ? &DMA1->LISR : &DMA2->LISR);
        this->ClearFlags = reinterpret_cast<volatile std::uintptr_t*>((stream.DMA == 1) ? &DMA1->LIFCR : &DMA2->LIFCR);
        this->flag_offset = 0;
        break;
    case 1: // Stream 1
        this->Instance = (stream.DMA == 1) ? DMA1_Stream1 : DMA2_Stream1;
        this->Flags = reinterpret_cast<volatile std::uintptr_t*>((stream.DMA == 1) ? &DMA1->LISR : &DMA2->LISR);
        this->ClearFlags = reinterpret_cast<volatile std::uintptr_t*>((stream.DMA == 1) ? &DMA1->LIFCR : &DMA2->LIFCR);
        this->flag_offset = 6;
        break;
    case 2: // Stream 2
        this->Instance = (stream.DMA == 1) ? DMA1_Stream2 : DMA2_Stream2;
        this->Flags = reinterpret_cast<volatile std::uintptr_t*>((stream.DMA == 1) ? &DMA1->LISR : &DMA2->LISR);
        this->ClearFlags = reinterpret_cast<volatile std::uintptr_t*>((stream.DMA == 1) ? &DMA1->LIFCR : &DMA2->LIFCR);
        this->flag_offset = 16;
        break;
    case 3: // Stream 3
        this->Instance = (stream.DMA == 1) ? DMA1_Stream3 : DMA2_Stream3;
        this->Flags = reinterpret_cast<volatile std::uintptr_t*>((stream.DMA == 1) ? &DMA1->LISR : &DMA2->LISR);
        this->ClearFlags = reinterpret_cast<volatile std::uintptr_t*>((stream.DMA == 1) ? &DMA1->LIFCR : &DMA2->LIFCR);
        this->flag_offset = 22;
        break;
    case 4: // Stream 4
        this->Instance = (stream.DMA == 1) ? DMA1_Stream4 : DMA2_Stream4;
        this->Flags = reinterpret_cast<volatile std::uintptr_t*>((stream.DMA == 1) ? &DMA1->HISR : &DMA2->HISR);
        this->ClearFlags = reinterpret_cast<volatile std::uintptr_t*>((stream.DMA == 1) ? &DMA1->HIFCR : &DMA2->HIFCR);
        this->flag_offset = 0;
        break;
    case 5: // Stream 5
        this->Instance = (stream.DMA == 1) ? DMA1_Stream5 : DMA2_Stream5;
        this->Flags = reinterpret_cast<volatile std::uintptr_t*>((stream.DMA == 1) ? &DMA1->HISR : &DMA2->HISR);
        this->ClearFlags = reinterpret_cast<volatile std::uintptr_t*>((stream.DMA == 1) ? &DMA1->HIFCR : &DMA2->HIFCR);
        this->flag_offset = 6;
        break;
    case 6: // Stream 6
        this->Instance = (stream.DMA == 1) ? DMA1_Stream6 : DMA2_Stream6;
        this->Flags = reinterpret_cast<volatile std::uintptr_t*>((stream.DMA == 1) ? &DMA1->HISR : &DMA2->HISR);
        this->ClearFlags = reinterpret_cast<volatile std::uintptr_t*>((stream.DMA == 1) ? &DMA1->HIFCR : &DMA2->HIFCR);
        this->flag_offset = 16;
        break;
    case 7: // Stream 7
        this->Instance = (stream.DMA == 1) ? DMA1_Stream7 : DMA2_Stream7;
        this->Flags = reinterpret_cast<volatile std::uintptr_t*>((stream.DMA == 1) ? &DMA1->HISR : &DMA2->HISR);
        this->ClearFlags = reinterpret_cast<volatile std::uintptr_t*>((stream.DMA == 1) ? &DMA1->HIFCR : &DMA2->HIFCR);
        this->flag_offset = 22;
        break;
#elif defined(STM32L0)
    case 1: // Channel 1
        this->Instance = reinterpret_cast<volatile DMA::DMA_Stream_TypeDef*>(DMA1_Channel1_BASE);
        break;
    case 2: // Channel 2
        this->Instance = reinterpret_cast<volatile DMA::DMA_Stream_TypeDef*>(DMA1_Channel2_BASE);
        break;
    case 3: // Channel 3
        this->Instance = reinterpret_cast<volatile DMA::DMA_Stream_TypeDef*>(DMA1_Channel3_BASE);
        break;
    case 4: // Channel 4
        this->Instance = reinterpret_cast<volatile DMA::DMA_Stream_TypeDef*>(DMA1_Channel4_BASE);
        break;
    case 5: // Channel 5
        this->Instance = reinterpret_cast<volatile DMA::DMA_Stream_TypeDef*>(DMA1_Channel5_BASE);
        break;
    case 6: // Channel 6
        this->Instance = reinterpret_cast<volatile DMA::DMA_Stream_TypeDef*>(DMA1_Channel6_BASE);
        break;
    case 7: // Channel 7
        this->Instance = reinterpret_cast<volatile DMA::DMA_Stream_TypeDef*>(DMA1_Channel7_BASE);
        break;
#endif
    default:
        break;
    }

    // Set the channel
#if defined(STM32F4)
    this->Instance->CR &= ~DMA_SxCR_CHSEL_Msk;
    this->Instance->CR |= (stream.Channel << DMA_SxCR_CHSEL_Pos) & DMA_SxCR_CHSEL_Msk;
#elif defined(STM32L0)
    // L0 has only one interrupt register
    this->Flags = reinterpret_cast<volatile std::uintptr_t*>(&DMA1->ISR);
    this->ClearFlags = reinterpret_cast<volatile std::uintptr_t*>(&DMA1->IFCR);
    this->flag_offset = (stream.Stream - 1) * 4;

    // Set the Channel or Request mapping for the selected Stream/Channel
    DMA1_CSELR->CSELR &= ~(DMA_CSELR_C1S_Msk << ((stream.Stream - 1) * 4));
    DMA1_CSELR->CSELR |= (stream.Channel << ((stream.Stream - 1) * 4));
#endif
};

/**
 * @brief Set the data direction of the DMA stream.
 *
 * @param direction The data direction.
 */
void DMA::Stream::set_direction(const Direction &direction)
{
#if defined(STM32F4)
    // clear old direction
    this->Instance->CR &= ~DMA_SxCR_DIR_Msk;

    // set new direction
    this->Instance->CR |= (static_cast<unsigned char>(direction) << DMA_SxCR_DIR_Pos) & DMA_SxCR_DIR_Msk;
#elif defined(STM32L0)
    // clear old direction
    this->Instance->CCR &= ~(DMA_CCR_MEM2MEM_Msk | DMA_CCR_DIR_Msk);

    // set new direction
    if (direction == Direction::memory_to_memory)
        this->Instance->CCR |= DMA_CCR_MEM2MEM | DMA_CCR_DIR;
    else if (direction == Direction::memory_to_peripheral)
        this->Instance->CCR |= DMA_CCR_DIR;
#endif
};

/**
 * @brief Clear the interrupt flags of the DMA stream.
 * Currently ALL flags are cleared by default!
 * 
 * @param flag The flag to clear.
 */
void DMA::Stream::clear_interrupt_flag(const Flag &flag)
{
#if defined(STM32F4)
    *this->ClearFlags = ( 0b111101 << this->flag_offset);
#elif defined(STM32L0)
    *this->ClearFlags = ( 0b1111 << this->flag_offset);
#endif
};

/**
 * @brief Enable the DMA stream.
 * This function automatically resets the interrupt flags and
 * does not disturb ongoing transfers on the DMA Stream.
 * 
 * @return Returns True when the Stream was successfully enabled.
 */ 
bool DMA::Stream::enable(void)
{
    // Only when Stream is not already enabled
    if (not this->is_enabled())
        {
        // Reset interrupt flags
        this->clear_interrupt_flag(Flag::All);

        // Enable DMA stream
#if defined(STM32F4)
        this->Instance->CR |= DMA_SxCR_EN;
#elif defined(STM32L0)
        this->Instance->CCR |= DMA_CCR_EN;
#endif

        // Stream was successfully enabled
        return true;
    }
    else // Stream was already enabled
        return false;
};

/**
 * @brief Check if the DMA stream is enabled.
 *
 * @return Whether the DMA stream is enabled.
 */
bool DMA::Stream::is_enabled(void) const
{
#if defined(STM32F4)
    return this->Instance->CR & DMA_SxCR_EN;
#elif defined(STM32L0)
    return this->Instance->CCR & DMA_CCR_EN;
#endif
};

/**
 * @brief Check whether the current transfer of the stream is complete.
 * @return Returns True when the current transfer of the stream is complete.
 */
bool DMA::Stream::is_transfer_complete(void) const
{
#if defined(STM32F4)
    /* 
     * The interrupt flags are derived from the definition of Stream 0
     * to make the intention of the offset clearer.
     */
    return *this->Flags & (1 << (this->flag_offset + DMA_LISR_TCIF0_Pos));
#elif defined(STM32L0)
    return *this->Flags & (1 << (this->flag_offset + DMA_ISR_TCIF1_Pos));
#endif
};