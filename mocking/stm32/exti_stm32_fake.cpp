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
 * @file    exti_stm32_fake.cpp
 * @author  SO
 * @version v1.3.0
 * @date    04-Oktober-2021
 * @brief   Fakes for the stm32 EXTI peripheral for unit testing.
 ==============================================================================
 */

// *** Includes ***
#include "exti_stm32_fake.h"

// *** Fakes ***
// Peripheral objects
static EXTI_TypeDef EXTI_Fake;

// Pointer to peripherals
EXTI_TypeDef* EXTI = &EXTI_Fake;

// Base addresses to peripherals
unsigned long EXTI_BASE = reinterpret_cast<unsigned long>(EXTI);

// *** Methods ***

/**
 * @brief Constructor for port object which initializes
 * the port to default values.
 */
EXTI_TypeDef::EXTI_TypeDef()
{
    // Reinit to default values
    registers_to_default();
};

/**
 * @brief Reset all the registers to the default values
 */
void EXTI_TypeDef::registers_to_default(void)
{
    this->IMR   = 0x00;
    this->EMR   = 0x00;
    this->RTSR  = 0x00;
    this->FTSR  = 0x00;
    this->SWIER = 0x00;
    this->PR    = 0x00;
};
