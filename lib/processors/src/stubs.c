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
 * @file    stubs.c
 * @author  SO
 * @version v1.0.0
 * @date    16-March-2021
 * @brief   Stub assembler functions for unit testing.
 ******************************************************************************
 */

// ****** Includes ******

// Only include functions when  unit testing!
#ifdef UNIT_TEST

// ****** Functions ******

/**
 * @brief This function is called by the kernel to give the control
 * to a specific thread, which is defined by the given stack address.
 * It saves the kernel context an restores the context of the thread which
 * is executed next.
 * @param ThreadStack Pointer with the current stack address of next task.
 * @return Returns the new stack pointer of the executed thread when it yields.
 * The address of this pointer is used to catch stack overflows.
 * @details: Handler Mode, Stack: msp
 */
unsigned int* __otos_switch(unsigned int* ThreadStack)
{
    // Return the current task stack pointer, when resuming kernel operation
    return ThreadStack;
};

/**
 * @brief Yield execution of the current thread and give the control
 * back to the kernel. Calls the SVC interrupt. When the thread gets the 
 * control back from the kernel, the thread execution resumes where the
 * yield was called from.
 * @details Thread Mode, Stack: psp
 */
void __otos_yield(void)
{
};

/**
 * @brief SVC Interrupt handler. This interrupt stores the context of the
 * calling thread and restores the context of the kernel.
 * @details interrupt-handler, Stack: msp
 */
void SVC_Handler(void)
{
};
  
/**
 * @brief Initializes the kernel. The kernel starts out in thread mode
 * with privileged operation. It uses the SVC interrupt to switch to
 * handler mode with privileged operation. For the switch the thread
 * stack is used as temporary memory.
 * @param ThreadStack Beginning of thread stack as temporary memory.
 * @details Thread Mode -> Handler Mode, Stack: msp
 */
void __otos_init_kernel(unsigned int* ThreadStack)
{
};

#endif