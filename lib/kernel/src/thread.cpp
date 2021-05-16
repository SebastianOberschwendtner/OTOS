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
 ******************************************************************************
 * @file    thread.c
 * @author  SO
 * @version v1.0.0
 * @date    16-March-2021
 * @brief   The internal thread handler for the OTOS kernel.
 ******************************************************************************
 */

// ****** Includes ******
#include "thread.h"

// ****** Methods ******

/**
 * @brief Constructor for a ThreadHandler object.
 * @param StackSize The size of the allocated stack in words.
 * @param Priority  The Priority of the thread.
 */
OTOS::Thread::Thread(void)
{
    // Initialize properties
    this->StackSize = 0;
    this->StackPointer = nullptr;
    this->StackTop = nullptr;
};

/**
 * @brief check whether the current thread shows a stack overflow.
 * @return Returns true when a stack overflow occurred.
 */
bool OTOS::Thread::StackOverflow(void)
{
    // When the current stack pointer occupies more or all of the stack, return true
    return (this->StackTop - this->StackPointer) >= StackSize;
};
