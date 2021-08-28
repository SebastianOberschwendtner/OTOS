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
 * @version v1.0.3
 * @date    27-August-2021
 * @brief   Fakes the clock interface for STM32 microcontrollers.
 ==============================================================================
 */

// *** Includes ***
#include "clock_stm32_fake.h"

// *** Fakes ***
static RCC_TypeDef RCC_Fake;    // Fake Peripheral
RCC_TypeDef* RCC = &RCC_Fake;   // Public Pointer to fake which mimics peripheral behaviour.

// *** Methods ***

/**
 * @brief Constructor for the fake RCC peripheral. Initializes the
 * register values to the default values as defined in the datasheet
 * of the STM32.
 * @details There should only be one instance of this object during testing.
 * This static instance is made available by the pointer to the object, which
 * fakes the peripheral during testing.
 */
RCC_TypeDef::RCC_TypeDef():
    CR(0x83),
    PLLCFGR(0x24003010),
    AHB1ENR(0)
{
};