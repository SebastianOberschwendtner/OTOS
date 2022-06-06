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
 * @file    clock_stm32_fake.cpp
 * @author  SO
 * @version v2.7.3
 * @date    27-August-2021
 * @brief   Fakes the clock interface for STM32 microcontrollers.
 ==============================================================================
 */

// *** Includes ***
#include "clock_stm32_fake.h"

// *** Fakes ***
// Fake Peripheral
static RCC_TypeDef RCC_Fake;

// Public Pointer to fake which mimics peripheral behaviour.
RCC_TypeDef* RCC = &RCC_Fake;

// Fake register address
std::uintptr_t RCC_BASE = reinterpret_cast<std::uintptr_t>(RCC);

// *** Methods ***

/**
 * @brief Constructor for the fake RCC peripheral. Initializes the
 * register values to the default values as defined in the datasheet
 * of the STM32.
 * @details There should only be one instance of this object during testing.
 * This static instance is made available by the pointer to the object, which
 * fakes the peripheral during testing.
 */
RCC_TypeDef::RCC_TypeDef()
{
    // Reinit to default
    registers_to_default();
};

/**
 * @brief Reset all the registers to the default values
 */
void RCC_TypeDef::registers_to_default(void)
{
    this->CR        = 0x83;
    this->PLLCFGR   = 0x24003010;
    this->CFGR = 0;
    this->CIR = 0;
    this->AHB1RSTR = 0;
    this->AHB2RSTR = 0;
    this->AHB3RSTR = 0;
    this->RESERVED0 = 0;
    this->APB1RSTR = 0;
    this->APB2RSTR = 0;
    this->AHB1ENR = 0;
    this->AHB2ENR = 0;
    this->AHB3ENR = 0;
    this->RESERVED2 = 0;
    this->APB1ENR = 0;
    this->APB2ENR = 0;
    this->AHB1LPENR = 0;
    this->AHB2LPENR = 0;
    this->AHB3LPENR = 0;
    this->RESERVED4 = 0;
    this->APB1LPENR = 0;
    this->APB2LPENR = 0;
    this->BDCR = 0;
    this->CSR = 0;
    this->SSCGR = 0;
    this->PLLI2SCFGR = 0;
    this->PLLSAICFGR = 0;
    this->DCKCFGR = 0;
};