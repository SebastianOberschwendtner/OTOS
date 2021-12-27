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
 * @version v2.0.0
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
{
    // Call assembler function to return to kernel in handler mode
    // Uses the thread stack as temporary memory
    __otos_init_kernel(this->Stack.end());
};

/**
 * @brief Switch to the thread which is currently active and handover control.
 * @param next_thread The number of the next thread to be executed.
 */
void OTOS::Kernel::switch_to_thread(const u_base_t next_thread)
{
    // Remember active thread
    const u_base_t index = static_cast<u_base_t>(this->Threads[next_thread].get_priority());
    this->last_thread[index] = next_thread;

    // Invoke the assembler function to switch context
    this->Threads[next_thread].set_running();
    this->Threads[next_thread].Stack_pointer = __otos_switch(this->Threads[next_thread].Stack_pointer);
    this->Threads[next_thread].set_blocked();
};

/**
 * @brief Determine the next thread to run based on priority in
 * a round-robin fashion.
 * 
 * @param thread_priority The priority group to get the next thread for.
 * @return The next thread number to run. Returns False if there is nor
 * thread ready to be scheduled.
 */
std::optional<u_base_t> OTOS::Kernel::find_next_thread(const OTOS::Priority thread_priority) const
{
    // Lambda to check whether thread with specified priority is runnable
    auto check_runnable = [thread_priority](OTOS::Thread thread) -> bool
    {
        return (thread.is_runnable() & (thread.get_priority() == thread_priority));
    };

    /*
     * Find the next runnable thread.
     * This is done in two passes. One
     * pass after the currently active thread, where
     * the nearest canditate for round-robin most
     * likely is.
     * The second pass is before the currently
     * active thread.
     * When only using one loop, you would constantly
     * need to check whether you reached the end. These
     * comparisons are unnecessary.
     */
    const auto &threads = this->Threads;
    const u_base_t index = static_cast<u_base_t>(thread_priority);
    const u_base_t last_thread = this->last_thread[index];

    // *** First pass ***
    auto next = std::find_if(
        threads.cbegin() + last_thread + 1,
        threads.cend(),
        check_runnable);
    // Check whether thread was found
    if (next != threads.cend())
        return std::distance(threads.cbegin(), next);

    // *** Second pass ***
    next = std::find_if(
        threads.cbegin(),
        threads.cbegin() + last_thread + 1,
        check_runnable);
    // Check whether thread was found
    if (next != (threads.cbegin() + last_thread + 1))
        return std::distance(threads.cbegin(), next);

    // No runnable thread was found
    return {};
};

/**
 * @brief Determine the next thread to run.
 * The object stores this internally.
 * @return Returns the number of the thread which should be executed next.
 * The optional evaluates to false, when no thread is currently runnable.
 * @details This currently only implements a priority based round-robin scheme.
 */
std::optional<u_base_t> OTOS::Kernel::get_next_thread(void) const
{
    // Find next thread for every priority level
    for (auto priority : Available_Priorities)
    {
        // Find thread with specific priority level
        auto next = this->find_next_thread(priority);

        // When thread was found return, otherwise try next priority level
        if (next)
            return next.value();
    }

    // no thread is runnable
    return {};
};

/**
 * @brief Update the thread tick counters and determine which
 * thread is runnable.
 */
void OTOS::Kernel::update_schedule(void)
{
    // Update the ticks of every active thread
    std::for_each(
        this->Threads.begin(),
        this->Threads.begin() + this->thread_count,
        [](OTOS::Thread &thread)
        { thread.count_tick(); });
};

/**
 * @brief Add a thread schedule to the kernel and activate its execution.
 * @param TaskFunc Function pointer to the task of the thread.
 * @param StackSize The size of the thread stack in words. => Should be checked with 'OTOS::Check::StackSize<Size>()' first.
 * @param Priority Priority of the scheduled thread.
 * @param Schedule The call schedule of the thread in ticks. Leave at 0 when thread can be executed whenever.
 */
void OTOS::Kernel::schedule_thread(
    const taskpointer_t TaskFunc,
    const u_base_t StackSize,
    const Priority Priority,
    const u_base_t Schedule)
{
    // Check whether maximum number of tasks is reached
    if (this->thread_count < (this->Threads.size()))
    {
        // Get pointer to next thread
        OTOS::Thread *_NewThread = &this->Threads[this->thread_count];

        // Get the top of the next thread stack
        stackpointer_t _newStack = 0;

        // The next thread stack begins at the end of the currently
        // allocated stack
        _newStack = this->Stack.end() - this->get_allocated_stacksize();

        // Init the stack data
        _NewThread->set_stack(_newStack, StackSize);
        _NewThread->set_schedule(Schedule, Priority);

        // Initialize and mimic the psp stack frame
        // -> See Stack-Layout.md for details
        _newStack -= 17;                                      // The stack frame stores 17 bytes
        _newStack[16] = 0x01000000;                           // Thread PSR
        _newStack[15] = reinterpret_cast<u_base_t>(TaskFunc); // Thread PC
        _newStack[8] = 0xFFFFFFFD;                            // Thread LR, Exception return mode

        // Init task
        _NewThread->Stack_pointer = __otos_switch(_newStack);

        // update last run thread
        const u_base_t index = static_cast<u_base_t>(Priority);
        this->last_thread[index] = this->thread_count;

        // increase thread counter
        this->thread_count++;
    }
};

/**
 * @brief Start the kernel execution.
 */
void OTOS::Kernel::start(void)
{
    // Loop forever
    while (1)
    {
        // Determine the next thread to run
        auto next_thread = this->get_next_thread();

        // When a thread is runnable switch to it
        if (next_thread)
            this->switch_to_thread(next_thread.value());
    };
};

/**
 * @brief Get the current size of the allocated stack.
 * Loops through all the stack sizes of the scheduled threads.
 * @return Returns the currently allocated stack size in words.
 */
u_base_t OTOS::Kernel::get_allocated_stacksize(void) const
{
    // Counter variable
    u_base_t _stack = 0;

    // Loop through all allocated stacks
    for (u_base_t i = 0; i < this->thread_count; i++)
        _stack += this->Threads[i].get_stacksize();

    // Return the total allocated stack size
    return _stack;
};

/**
 * @brief Increase the milli-seconds timer by one milli-second.
 */
void OTOS::Kernel::count_time_ms(void)
{
    OTOS::Kernel::Time_ms++;
};

/**
 * @brief Get the current system time in milli-seconds.
 * @return Returns the current time in milli-seconds.
 */
std::uint32_t OTOS::Kernel::get_time_ms(void)
{
    return OTOS::Kernel::Time_ms;
};

/**
 * @brief Get the current system time in milli-seconds as
 * a function call.
 * 
 * The return type is hardcoded to be 32-bit, because the
 * system should also have 32bit on 8-bit systems, where
 * u_base_t would evaluate to char.
 * 
 * @return Returns the current time in milli-seconds.
 */
std::uint32_t OTOS::get_time_ms(void)
{
    return OTOS::Kernel::get_time_ms();
};