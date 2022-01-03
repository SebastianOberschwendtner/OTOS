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
 * @file    system_stm32_fake.cpp
 * @author  SO
 * @version v2.5.0
 * @date    02-January-2022
 * @brief   Fakes the system interface for STM32 microcontrollers.
 ==============================================================================
 */

// *** Includes ***
#include "system_stm32_fake.h"

// *** Fakes ***
// Fake Peripheral
static FLASH_TypeDef FLASH_Fake;

// Public Pointer to fake which mimics peripheral behaviour.
FLASH_TypeDef* FLASH = &FLASH_Fake;

// Fake register address
unsigned long FLASH_BASE = reinterpret_cast<unsigned long>(FLASH);

// *** Methods ***

/**
 * @brief Constructor for the fake FLASH peripheral. Initializes the
 * register values to the default values as defined in the datasheet
 * of the STM32.
 * @details There should only be one instance of this object during testing.
 * This static instance is made available by the pointer to the object, which
 * fakes the peripheral during testing.
 */
FLASH_TypeDef::FLASH_TypeDef()
{
    // Reinit to default
    registers_to_default();
};

/**
 * @brief Reset all the registers to the default values
 */
void FLASH_TypeDef::registers_to_default(void)
{
    this->ACR = 0;
    this->KEYR = 0;
    this->OPTKEYR = 0;
    this->SR = 0;
    this->CR = 0;  
    this->OPTCR = 0;
    this->OPTCR1 = 0;
};