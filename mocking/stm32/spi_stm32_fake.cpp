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
 * @file    spi_stm32_fake.cpp
 * @author  SO
 * @version v2.1.0
 * @date    22-Dezember-2021
 * @brief   Fakes the spi interface for STM32 microcontrollers.
 ==============================================================================
 */

// *** Includes ***
#include "spi_stm32_fake.h"

// *** Fakes ***
// Fake Peripheral
static SPI_TypeDef SPI1_Fake;
static SPI_TypeDef SPI2_Fake;
static SPI_TypeDef SPI3_Fake;
static SPI_TypeDef SPI4_Fake;
static SPI_TypeDef SPI5_Fake;
static SPI_TypeDef SPI6_Fake;

// Public Pointer to fake which mimics peripheral behaviour.
SPI_TypeDef *SPI1 = &SPI1_Fake;
SPI_TypeDef *SPI2 = &SPI2_Fake;
SPI_TypeDef *SPI3 = &SPI3_Fake;
SPI_TypeDef *SPI4 = &SPI4_Fake;
SPI_TypeDef *SPI5 = &SPI5_Fake;
SPI_TypeDef *SPI6 = &SPI6_Fake;

// Fake register address
std::uintptr_t SPI1_BASE = reinterpret_cast<std::uintptr_t>(SPI1);
std::uintptr_t SPI2_BASE = reinterpret_cast<std::uintptr_t>(SPI2);
std::uintptr_t SPI3_BASE = reinterpret_cast<std::uintptr_t>(SPI3);
std::uintptr_t SPI4_BASE = reinterpret_cast<std::uintptr_t>(SPI4);
std::uintptr_t SPI5_BASE = reinterpret_cast<std::uintptr_t>(SPI5);
std::uintptr_t SPI6_BASE = reinterpret_cast<std::uintptr_t>(SPI6);

/**
 * @brief Constructor for the fake SPI peripheral. Initializes the
 * register values to the default values as defined in the datasheet
 * of the STM32.
 * @details There should only be one instance of this object during testing.
 * This static instance is made available by the pointer to the object, which
 * fakes the peripheral during testing.
 */
SPI_TypeDef::SPI_TypeDef()
{
    // Reinit to default
    registers_to_default();
};

/**
 * @brief Reset all the registers to the default values
 */
void SPI_TypeDef::registers_to_default(void)
{
    this->CR1 = 0;
    this->CR2 = 0;
    this->SR = 0;
    this->DR = 0;
    this->CRCPR = 0;
    this->RXCRCR = 0;
    this->TXCRCR = 0;
    this->I2SCFGR = 0;
    this->I2SPR = 0;
};