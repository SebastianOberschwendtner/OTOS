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
 * @file    dma_stm32_fake.cpp
 * @author  SO
 * @version v2.9.0
 * @date    18-July-2022
 * @brief   Fakes the DMA interface for STM32 microcontrollers.
 ==============================================================================
 */

// *** Includes ***
#include "dma_stm32_fake.h"

// *** Fakes ***
// Fake Peripheral
static DMA_TypeDef DMA1_Fake;
static DMA_Stream_TypeDef DMA1_Stream0_Fake;
static DMA_Stream_TypeDef DMA1_Stream1_Fake;
static DMA_Stream_TypeDef DMA1_Stream2_Fake;
static DMA_Stream_TypeDef DMA1_Stream3_Fake;
static DMA_Stream_TypeDef DMA1_Stream4_Fake;
static DMA_Stream_TypeDef DMA1_Stream5_Fake;
static DMA_Stream_TypeDef DMA1_Stream6_Fake;
static DMA_Stream_TypeDef DMA1_Stream7_Fake;
static DMA_TypeDef DMA2_Fake;
static DMA_Stream_TypeDef DMA2_Stream0_Fake;
static DMA_Stream_TypeDef DMA2_Stream1_Fake;
static DMA_Stream_TypeDef DMA2_Stream2_Fake;
static DMA_Stream_TypeDef DMA2_Stream3_Fake;
static DMA_Stream_TypeDef DMA2_Stream4_Fake;
static DMA_Stream_TypeDef DMA2_Stream5_Fake;
static DMA_Stream_TypeDef DMA2_Stream6_Fake;
static DMA_Stream_TypeDef DMA2_Stream7_Fake;

// Public Pointer to fake which mimics peripheral behaviour.
DMA_TypeDef *DMA1 = &DMA1_Fake;
DMA_Stream_TypeDef *DMA1_Stream0 = &DMA1_Stream0_Fake;
DMA_Stream_TypeDef *DMA1_Stream1 = &DMA1_Stream1_Fake;
DMA_Stream_TypeDef *DMA1_Stream2 = &DMA1_Stream2_Fake;
DMA_Stream_TypeDef *DMA1_Stream3 = &DMA1_Stream3_Fake;
DMA_Stream_TypeDef *DMA1_Stream4 = &DMA1_Stream4_Fake;
DMA_Stream_TypeDef *DMA1_Stream5 = &DMA1_Stream5_Fake;
DMA_Stream_TypeDef *DMA1_Stream6 = &DMA1_Stream6_Fake;
DMA_Stream_TypeDef *DMA1_Stream7 = &DMA1_Stream7_Fake;
DMA_TypeDef *DMA2 = &DMA2_Fake;
DMA_Stream_TypeDef *DMA2_Stream0 = &DMA2_Stream0_Fake;
DMA_Stream_TypeDef *DMA2_Stream1 = &DMA2_Stream1_Fake;
DMA_Stream_TypeDef *DMA2_Stream2 = &DMA2_Stream2_Fake;
DMA_Stream_TypeDef *DMA2_Stream3 = &DMA2_Stream3_Fake;
DMA_Stream_TypeDef *DMA2_Stream4 = &DMA2_Stream4_Fake;
DMA_Stream_TypeDef *DMA2_Stream5 = &DMA2_Stream5_Fake;
DMA_Stream_TypeDef *DMA2_Stream6 = &DMA2_Stream6_Fake;
DMA_Stream_TypeDef *DMA2_Stream7 = &DMA2_Stream7_Fake;

// Fake register address
std::uintptr_t DMA1_BASE = reinterpret_cast<std::uintptr_t>(DMA1);
std::uintptr_t DMA1_Stream0_BASE = reinterpret_cast<std::uintptr_t>(DMA1_Stream0);
std::uintptr_t DMA1_Stream1_BASE = reinterpret_cast<std::uintptr_t>(DMA1_Stream1);
std::uintptr_t DMA1_Stream2_BASE = reinterpret_cast<std::uintptr_t>(DMA1_Stream2);
std::uintptr_t DMA1_Stream3_BASE = reinterpret_cast<std::uintptr_t>(DMA1_Stream3);
std::uintptr_t DMA1_Stream4_BASE = reinterpret_cast<std::uintptr_t>(DMA1_Stream4);
std::uintptr_t DMA1_Stream5_BASE = reinterpret_cast<std::uintptr_t>(DMA1_Stream5);
std::uintptr_t DMA1_Stream6_BASE = reinterpret_cast<std::uintptr_t>(DMA1_Stream6);
std::uintptr_t DMA1_Stream7_BASE = reinterpret_cast<std::uintptr_t>(DMA1_Stream7);
std::uintptr_t DMA2_BASE = reinterpret_cast<std::uintptr_t>(DMA2);
std::uintptr_t DMA2_Stream0_BASE = reinterpret_cast<std::uintptr_t>(DMA2_Stream0);
std::uintptr_t DMA2_Stream1_BASE = reinterpret_cast<std::uintptr_t>(DMA2_Stream1);
std::uintptr_t DMA2_Stream2_BASE = reinterpret_cast<std::uintptr_t>(DMA2_Stream2);
std::uintptr_t DMA2_Stream3_BASE = reinterpret_cast<std::uintptr_t>(DMA2_Stream3);
std::uintptr_t DMA2_Stream4_BASE = reinterpret_cast<std::uintptr_t>(DMA2_Stream4);
std::uintptr_t DMA2_Stream5_BASE = reinterpret_cast<std::uintptr_t>(DMA2_Stream5);
std::uintptr_t DMA2_Stream6_BASE = reinterpret_cast<std::uintptr_t>(DMA2_Stream6);
std::uintptr_t DMA2_Stream7_BASE = reinterpret_cast<std::uintptr_t>(DMA2_Stream7);

// *** Methods ***

/**
 * @brief Reset all the registers to the default values
 */
void DMA_TypeDef::registers_to_default(void)
{
    this->HIFCR = 0;
    this->LIFCR = 0;
    this->HISR = 0;
    this->LISR = 0;
};

/**
 * @brief Reset all the registers to the default values
 */
void DMA_Stream_TypeDef::registers_to_default(void)
{
    this->CR = 0;
    this->NDTR = 0;
    this->PAR = 0;
    this->M0AR = 0;
    this->M1AR = 0;
    this->FCR = 0;
};