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
 * @version v1.3.0
 * @date    04-Oktober-2021
 * @brief   Mocks of the CMSIS driver functions.
 ==============================================================================
 */

// *** Includes ***
#include "cmsis_mock.h"

// *** mocks ***
Mock::Callable EnableIRQ;

// *** Functions ***

/**
 * @brief Mock the enabling of IRQs.
 * @param IRQn The IRQ number to enable.
 */
void NVIC_EnableIRQ(IRQn_Type IRQn)
{
    EnableIRQ.add_call(static_cast<int>(IRQn));
};