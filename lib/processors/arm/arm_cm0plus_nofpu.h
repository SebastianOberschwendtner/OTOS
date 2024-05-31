/**
 * OTOS - Open Tec Operating System
 * Copyright (c) 2021 - 2024 Sebastian Oberschwendtner, sebastian.oberschwendtner@gmail.com
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

#ifndef ARM_CM0PLUS_NOFPU_H_
#define ARM_CM0PLUS_NOFPU_H_

/* === Includes === */
#include <vendors.h>

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

    /* === Function Declarations === */
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
    uint32_t *__otos_switch(uint32_t *ThreadStack);

    /**
     * @brief Yield execution of the current thread and give the control
     * back to the kernel. Calls the SVC interrupt. When the thread gets the
     * control back from the kernel, the thread execution resumes where the
     * yield was called from.
     * @details Thread Mode, Stack: psp
     */
    void __otos_yield();

    /**
     * @brief This function stores the context of the
     * calling thread and restores the context of the kernel.
     * This function should only be called within interrupts!
     * @details Stack: msp
     */
    void __otos_call_kernel();

    /**
     * @brief Initializes the kernel. The kernel starts out in thread mode
     * with privileged operation. It uses the SVC interrupt to switch to
     * handler mode with privileged operation. For the switch the thread
     * stack is used as temporary memory.
     * @param ThreadStack Beginning of thread stack as temporary memory.
     * @details Thread Mode -> Handler Mode, Stack: msp
     */
    void __otos_init_kernel(uint32_t *ThreadStack);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // ARM_CM0PLUS_NOFPU_H_