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
    for (u_base_t i=0; i < OTOS_NUMBER_THREADS; i++)
        this->Threads[i] = OTOS::Thread();

    // Set first thread to top of thread stack, the other threads are still nullptr!
    stackpointer_t _StackTop = this->Stack + OTOS_STACK_SIZE - 1;
    this->Threads[0].StackTop = _StackTop;
    this->Threads[0].StackPointer = _StackTop;

    // Call assembler function to return to kernel in handler mode
    __otos_init_kernel(_StackTop);
};

/**
 * @brief Update the thread tick counters and determine which
 * thread is runable.
 */
void OTOS::Kernel::UpdateSchedule(void)
{
    ///@todo Implement the scheduling!
};

/**
 * @brief Determine the next thread to run.
 * The object stores this internally.
 * @details This currently only implements a simple round-robin scheme.
 * @todo Add scheduling with priority.
 */
void OTOS::Kernel::GetNextThread(void)
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
void OTOS::Kernel::SwitchThread(void)
{
    // Invoke the assembler function to switch context
    this->Threads[this->CurrentThread].StackPointer =
        __otos_switch(this->Threads[this->CurrentThread].StackPointer);
};

/**
 * @brief Add a thread schedule to the kernel and activate its execution.
 * @param TaskFunc Function pointer to the task of the thread.
 * @param Priority Priority of the scheduled thread.
 */
void OTOS::Kernel::ScheduleThread(taskpointer_t TaskFunc, Priority Priority)
{
    // Check whether maximum number of tasks is reached
    if (this->ThreadCount < OTOS_NUMBER_THREADS - 1)
    {
        // increase thread counter
        this->ThreadCount++;

        // set thread handler
        this->Threads[this->ThreadCount].ThreadPriority = Priority;
        this->Threads[this->ThreadCount].StackPointer = nullptr;
        this->Threads[this->ThreadCount].StackSize = 1;
        this->Threads[this->ThreadCount].TickCounter = 0;
        this->Threads[this->ThreadCount].TickSchedule = 0;

        // Init the stack data
        stackpointer_t _newStack = this->Stack;
        _newStack[16] = 0x100000;
        _newStack[15] = 0xFFFFFFFD;
        _newStack[8] = reinterpret_cast<u_base_t>(TaskFunc);

        // Init task
        this->Threads[this->CurrentThread].StackPointer = 
            __otos_switch(_newStack);
    }
};

/**
 * @brief Start the kernel execution.
 */
void OTOS::Kernel::Start(void)
{
    // Loop forever
    while(1)
    {
        // Determine the next thread to run
        this->GetNextThread();

        // Give the control to the next thread
        this->SwitchThread();
    };
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