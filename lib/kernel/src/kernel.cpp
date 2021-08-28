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
 * @file    kernel.c
 * @author  SO
 * @version v1.0.3
 * @date    09-March-2021
 * @brief   The kernel of the OTOS. It manages the task scheduling and context
 *          switching.
 ==============================================================================
 */

// *** Includes ***
#include "kernel.h"

// *** Static Variables ***
std::uint32_t OTOS::Kernel::Time_ms = 0; // Initialize the kernel time with 0

// *** Methods ***

/**
 * @brief Contructor for kernel object.
 * Only one kernel object should exist in your project!
 */
OTOS::Kernel::Kernel()
    : CurrentThread(0), ThreadCount(0)
{
    // Call assembler function to return to kernel in handler mode
    // Uses the thread stack as temporary memory
    __otos_init_kernel(this->Stack.end(), 16000);
};

/**
 * @brief Update the thread tick counters and determine which
 * thread is runable.
 */
void OTOS::Kernel::updateSchedule(void)
{
    ///@todo Implement the scheduling!
};

/**
 * @brief Determine the next thread to run.
 * The object stores this internally.
 * @details This currently only implements a simple round-robin scheme.
 * @todo Add scheduling with priority.
 */
void OTOS::Kernel::getNextThread(void)
{
    // Increment to next thread
    this->CurrentThread++;

    // Check whether thread counter overflowed
    if (this->CurrentThread >= this->ThreadCount)
        this->CurrentThread = 0;
};

/**
 * @brief Switch to the thread which is currently active and handover control.
 */
void OTOS::Kernel::switchThread(void)
{
    // Invoke the assembler function to switch context
    this->Threads[this->CurrentThread].StackPointer =
        __otos_switch(this->Threads[this->CurrentThread].StackPointer);
};

/**
 * @brief Add a thread schedule to the kernel and activate its execution.
 * @param TaskFunc Function pointer to the task of the thread.
 * @param StackSize The size of the thread stack in words. => Should be checked with 'OTOS::Check::StackSize<Size>()' first.
 * @param Priority Priority of the scheduled thread.
 */
void OTOS::Kernel::scheduleThread(taskpointer_t TaskFunc, const u_base_t StackSize, Priority Priority)
{
    // Check whether maximum number of tasks is reached
    if ( this->ThreadCount < (this->Threads.size() - 1) )
    {
        // Get pointer to next thread
        OTOS::Thread *_NewThread = &this->Threads[this->CurrentThread];

        // Get the top of the next thread stack
        stackpointer_t _newStack = 0;

        // The next thread stack begins at the end of the currently
        // allocated stack
        _newStack = this->Stack.end() - this->getAllocatedStackSize();

        // Init the stack data
        _NewThread->setStack(_newStack, StackSize);
        _NewThread->setSchedule(0, Priority);
        
        // Initialize and mimic the psp stack frame
        // -> See Stack-Layout.md for details
        _newStack -= 17; // The stack frame stores 17 bytes
        _newStack[16] = 0x01000000; // Thread PSR
        _newStack[15] = reinterpret_cast<u_base_t>(TaskFunc); // Thread PC
        _newStack[8] = 0xFFFFFFFD; // Thread LR, Exception return mode

        // Init task
        _NewThread->StackPointer = __otos_switch(_newStack);
        
        // Set next thread active for scheduling
        this->CurrentThread++;

        // increase thread counter
        this->ThreadCount++;
    }
};

/**
 * @brief Start the kernel execution.
 */
void OTOS::Kernel::start(void)
{
    // Loop forever
    while(1)
    {
        // Check whether the the SysTick timer overflowed
        // if (__otos_tick_passed())
        //     this->countTime_ms();

        // Determine the next thread to run
        this->getNextThread();

        // Give the control to the next thread
        this->switchThread();
    };
};

/**
 * @brief Get the current size of the allocated stack.
 * Loops through all the stack sizes of the scheduled threads.
 * @return Returns the currently allocated stack size in words.
 */
u_base_t OTOS::Kernel::getAllocatedStackSize(void) const
{
    // Counter variable
    u_base_t _stack = 0;

    // Loop through all allocated stacks
    for(u_base_t i = 0; i < this->ThreadCount; i++)
        _stack += this->Threads[i].getStackSize();

    // Return the total allocated stack size
    return _stack;
};

/**
 * @brief Increase the milli-seconds timer by one milli-second.
 */
void OTOS::Kernel::countTime_ms(void)
{
    OTOS::Kernel::Time_ms++;
};

/**
 * @brief Get the current system time in milli-seconds.
 * @return Returns the current time in milli-seconds.
 */
std::uint32_t OTOS::Kernel::getTime_ms(void) 
{
    return OTOS::Kernel::Time_ms;
};