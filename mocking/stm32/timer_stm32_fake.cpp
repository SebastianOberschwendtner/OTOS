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
 * @file    timer_stm32_fake.cpp
 * @author  SO
 * @version v2.7.3
 * @date    31-October-2021
 * @brief   Fakes the timer interface for STM32 microcontrollers.
 ==============================================================================
 */

// *** Includes ***
#include "timer_stm32_fake.h"

// *** Fakes ***
// Fake Peripheral
static TIM_TypeDef TIM1_Fake;
static TIM_TypeDef TIM2_Fake;

// Public Pointer to fake which mimics peripheral behaviour.
TIM_TypeDef* TIM1 = &TIM1_Fake;
TIM_TypeDef* TIM2 = &TIM2_Fake;

// Fake register address
std::uintptr_t TIM1_BASE = reinterpret_cast<std::uintptr_t>(TIM1);
std::uintptr_t TIM2_BASE = reinterpret_cast<std::uintptr_t>(TIM2);

// *** Methods ***

/**
 * @brief Reset all the registers to the default values
 */
void TIM_TypeDef::registers_to_default(void)
{
};