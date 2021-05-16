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
 * @file    kernel.c
 * @author  SO
 * @version v1.0.0
 * @date    09-March-2021
 * @brief   The kernel of the OTOS. It manages the task scheduling and context
 *          switching.
 ******************************************************************************
 */

// ****** Includes ******
#include "kernel.h"

// ****** Variables ******

// ****** Methods ******

/**
 * @brief Contructor for kernel object.
 * Only one kernel object should exist in your project!
 */
OTOS::Kernel::Kernel()
{
    // initialize properties
    this->CurrentThread = 0;
    this->ThreadCount = 0;

    // Initialize thread handler
    // for (u_base_t i=0; i < OTOS_NUMBER_THREADS; i++)
    //     this->Threads[i] = OTOS::ThreadHandler(0, PrioLow);
};

/**
 * @brief Get the current size of the allocated stack.
 * Loops through all the stack sizes of the scheduled threads.
 * @return Returns the currently allocated stack size in words.
 */
u_base_t OTOS::Kernel::AllocatedStackSize(void)
{
    // Counter variable
    u_base_t _stack = 0;

    // Loop through all allocated stacks
    for(u_base_t i = 0; i < this->ThreadCount; i++)
        _stack += this->Threads[i].StackSize;
    return _stack;
};