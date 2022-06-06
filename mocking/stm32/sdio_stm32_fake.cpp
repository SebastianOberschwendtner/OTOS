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
 * @file    sdio_stm32_fake.cpp
 * @author  SO
 * @version v2.7.3
 * @date    29-Dezember-2021
 * @brief   Fakes the SDIO interface for STM32 microcontrollers.
 ==============================================================================
 */

// *** Includes ***
#include "sdio_stm32_fake.h"

// *** Fakes ***
// Fake Peripheral
static SDIO_TypeDef SDIO_Fake;

// Public Pointer to fake which mimics peripheral behaviour.
SDIO_TypeDef *SDIO = &SDIO_Fake;

// Fake register address
std::uintptr_t SDIO_BASE = reinterpret_cast<std::uintptr_t>(SDIO);

/**
 * @brief Constructor for the fake SPI peripheral. Initializes the
 * register values to the default values as defined in the datasheet
 * of the STM32.
 * @details There should only be one instance of this object during testing.
 * This static instance is made available by the pointer to the object, which
 * fakes the peripheral during testing.
 */
SDIO_TypeDef::SDIO_TypeDef()
{
    // Reinit to default
    registers_to_default();
};

/**
 * @brief Reset all the registers to the default values
 */
void SDIO_TypeDef::registers_to_default(void)
{
    this->POWER = 0;
    this->CLKCR = 0;
    this->ARG = 0;
    this->CMD = 0;
    this->RESPCMD = 0;
    this->RESP1 = 0;
    this->RESP2 = 0;
    this->RESP3 = 0;
    this->RESP4 = 0;
    this->DTIMER = 0;
    this->DLEN = 0;
    this->DCTRL = 0;
    this->DCOUNT = 0;
    this->STA = 0;
    this->ICR = 0;
    this->MASK = 0;
    this->FIFOCNT = 0;
    this->FIFO = 0;
};