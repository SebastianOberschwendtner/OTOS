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
 * @file    cmsis_mock.cpp
 * @author  SO
 * @version v2.9.2
 * @date    04-Oktober-2021
 * @brief   Mocks of the CMSIS driver functions.
 ==============================================================================
 */

// === Includes ===
#include "cmsis_mock.h"


// === Mocks ===
Mock::Callable<bool> CMSIS_NVIC_EnableIRQ;
Mock::Callable<bool> CMSIS_NVIC_SetPriority;
Mock::Callable<uint32_t> CMSIS_SysTick_Config;

// === Functions ===

/**
 * @brief Mock the enabling of IRQs.
 * @param IRQn The IRQ number to enable.
 */
void NVIC_EnableIRQ(IRQn_Type IRQn)
{
    CMSIS_NVIC_EnableIRQ.add_call(static_cast<int>(IRQn));
};

/**
 * @brief Mock setting the interrupt priority.
 * @param IRQn The IRQ number to set the priority for.
 * @param priority The priority to set.
 */
void NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority)
{
    CMSIS_NVIC_SetPriority.add_call(static_cast<int>(IRQn));
};

/**
 * @brief Mock the configuration of the SysTick timer using CMSIS.
 * 
 * @param ticks The amount of ticks between interrupts
 * @return Returns 0 when SysTick timer was initialized successfully, 1 when there was an error.
 */
uint32_t SysTick_Config(uint32_t ticks)
{
    CMSIS_SysTick_Config.add_call(static_cast<int>(ticks));
    return 0;
};