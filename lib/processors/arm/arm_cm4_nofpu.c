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
/**
 ==============================================================================
 * @file    arm_m4_nofpu.c
 * @author  SO
 * @version v1.2.0
 * @date    09-March-2021
 * @brief   Defines the assembly code for:
 *          - ARM Cortex M4
 *          - No FPU enabled
 ==============================================================================
 */

/* === Includes === */
#include "arm_cm4_nofpu.h"

/** @attention Only include implementation when correct processor is used, 
 * otherwise the compiler complains when compiling this translation unit.
 */
#if defined(__CORTEX_M)
#if __CORTEX_M == 4

/* === Functions === */
uint32_t* __otos_switch(uint32_t* ThreadStack)
{
    /* Declare return type */
    uint32_t* NewThreadStack;

    /* Hand over control to the task belonging to the stack address */
    __asm__ volatile(
        /* Save kernel context */
        "push   {R1-R7,LR}      \n\t" /* Push first half of registers on msp stack */
        "push   {R8-R12}        \n\t" /* second half => now stack is compatible with M0+ stack */
        "mrs    R1, PSR         \n\t"
        "push   {R1}            \n\t" /* Push PSR on msp stack */
        /* Restore thread context */
        "ldmia  R0!,{R4-R11}    \n\t" /* Load R4-R11 from task stack (psp) */
        "ldmia  R0!,{R1}        \n\t"
        "mov    LR, R1          \n\t" /* Load LR from task stack (psp) */
        "msr    PSP, R0         \n\t" /* Restore the psp stack pointer */
        "bx     LR                  "
        /* Jump to user task, LR should contain a valid exception return behavior */
        : "=r" (NewThreadStack) /* Define the output item */
        : "r" (ThreadStack) /* Define the input item */
    );
    
    /* Return the current task stack pointer, when resuming kernel operation */
    return NewThreadStack;
};

void __otos_yield()
{
    __asm__ volatile(
        "nop    \n\t"
        "svc 0  \n\t" /* Call the SVC interrupt */
        "nop    \n\t"
        "bx lr      " /* Resume operation when the SVC call is finished */
    );
};

inline void __attribute__((always_inline)) __otos_call_kernel()
{
    /* Give control back to the kernel */
    __asm__ volatile(
        /* Save thread context */
        "mrs    R0, PSP         \n\t"   /* Save the current stack pointer (psp) */
        "stmdb  R0!, {R4-R11,LR}\n\t"   /* Save LR, and registers onto psp stack */
        /* R0 now contains the end of the occupied stack */
        /* Restore kernel context */
        "pop    {R1}            \n\t"
        "msr    PSR_NZCVQ, R1   \n\t" /* Write PSR with bitmask with value from msp stack */
        "pop    {R8-R12}        \n\t" /* Pop the second half of registers from the msp stack */
        "pop    {R1-R7}         \n\t" /* Pop the first half => Stack is compatible with M0+ stack */
        /* Restore LR and set program counter to resume kernel operation */
        /* bit[0] os the loaded value has to be 1 to stay in thumb mode! */
        "pop    {PC}" 
    );
};
  
/**
 * @brief SVC Interrupt handler. This interrupt stores the context of the
 * calling thread and restores the context of the kernel.
 * @details interrupt-handler, Stack: msp
 */
void SVC_Handler()
{
    /* Give control back to the kernel */
    __otos_call_kernel();
};
  
void __otos_init_kernel(uint32_t* ThreadStack)
{
    /* Initialize the CONTROL */
    __asm__ volatile(
        "push   {R1-R7,LR}      \n\t" /* Push first half of registers on msp stack */
        "push   {R8-R12}        \n\t" /* second half => now stack is compatible with M0+ stack */
        "mrs    R1, PSR         \n\t"
        "push   {R1}            \n\t" /* Save PSR to msp stack */
        "msr    psp, R0         \t\n" /* Set the psp to temporary memory */
        "mov    R0, #0b10       \n\t" /* Set Stack to PSP and PRIVILEGED mode */
        "msr    CONTROL, R0     \n\t" /* Set CONTROL for Thread mode */
        "isb                    \n\t" /* Has to follow the write to CONTROL */
        /* inline __otos_yield() afterwards */
        "nop                    \n\t"
        "svc 0                  \n\t" /* Call the SVC interrupt */
        "nop                    \n\t"
        "bx lr                      " /* Resume operation when the SVC call is finished */
        : /* No outputs */
        : "r" (ThreadStack) /* Input item */
    );

    /* ---------------------------------------------------------------------------------- 
    The __otos_yield() function is inlined in the above assembler code, because
    otherwise it does not work in debug mode. It would only work with the optimization
    -O2, which would inline the function call as an optimization result. Since the inline
    keyword is only a suggestion to the compiler and not a binding rule, the code is 
    inlined here as a hardcoded function.
    -------------------------------------------------------------------------------------*/
};
#endif // __CORTEX_M == 4
#endif // __CORTEX_M