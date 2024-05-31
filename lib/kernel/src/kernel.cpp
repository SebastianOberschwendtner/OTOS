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
 * @file    kernel.cpp
 * @author  SO
 * @version v5.0.0
 * @date    09-March-2021
 * @brief   The kernel of the OTOS. It manages the task scheduling and context
 *          switching.
 ==============================================================================
 */

/* === Includes === */
#include "kernel.h"

namespace OTOS
{
    /* === Static Variables === */
    std::uint32_t Kernel::Time_ms = 0; /* Initialize the kernel time with 0 */

    /* === Constructors === */
    Kernel::Kernel()
    {
        /* Call assembler function to return to kernel in handler mode */
        /* Uses the thread stack as temporary memory */
        __otos_init_kernel(this->Stack.end());
    };

    /* === Methods === */
    auto Kernel::get_allocated_stacksize() const -> u_base_t
    {
        /* Counter variable */
        u_base_t _stack = 0;

        /* Loop through all allocated stacks */
        for (u_base_t i = 0; i < this->thread_count; i++)
            _stack += this->Threads[i].get_stacksize();

        /* Return the total allocated stack size */
        return _stack;
    };

    auto Kernel::get_next_thread() const -> std::optional<u_base_t>
    {
        /* Find next thread for every priority level */
        for (auto priority : Available_Priorities)
        {
            /* Find thread with specific priority level */
            auto next = this->find_next_thread(priority);

            /* When thread was found return, otherwise try next priority level */
            if (next)
                return next.value();
        }

        /* no thread is runnable */
        return {};
    };

    auto Kernel::get_time_ms() -> std::uint32_t
    {
        return Kernel::Time_ms;
    };

    void Kernel::count_time_ms()
    {
        Kernel::Time_ms++;
    };

    void Kernel::start()
    {
        /* Loop forever */
        while (1)
        {
            /* Determine the next thread to run */
            auto next_thread = this->get_next_thread();

            /* When a thread is runnable switch to it */
            if (next_thread)
                this->switch_to_thread(next_thread.value());
        };
    };

    void Kernel::switch_to_thread(const u_base_t next_thread)
    {
        /* Remember active thread */
        const u_base_t index = static_cast<u_base_t>(this->Threads[next_thread].get_priority());
        this->last_thread[index] = next_thread;

        /* Invoke the assembler function to switch context */
        this->Threads[next_thread].set_running();
        this->Threads[next_thread].Stack_pointer = __otos_switch(this->Threads[next_thread].Stack_pointer);
        this->Threads[next_thread].set_blocked();
    };

    void Kernel::update_schedule()
    {
        /* Update the ticks of every active thread */
        std::for_each(
            this->Threads.begin(),
            this->Threads.begin() + this->thread_count,
            [](Thread &thread)
            { thread.count_tick(); });
    };

    void Kernel::schedule_thread(
        taskpointer_t TaskFunc,
        const u_base_t StackSize,
        const Priority Priority,
        const u_base_t Schedule)
    {
        /* Check whether maximum number of tasks is reached */
        if (this->thread_count < (this->Threads.size()))
        {
            /* Get pointer to next thread */
            Thread *_NewThread = &this->Threads[this->thread_count];

            /* Get the top of the next thread stack */
            stackpointer_t _newStack = 0;

            /* The next thread stack begins at the end of the currently */
            /* allocated stack */
            _newStack = this->Stack.end() - this->get_allocated_stacksize();

            /* Init the stack data */
            _NewThread->set_stack(_newStack, StackSize);
            _NewThread->set_schedule(Schedule, Priority);

            /* Initialize and mimic the psp stack frame */
            /* -> See Stack-Layout.md for details */
            _newStack -= 17;                                      /* The stack frame stores 17 bytes */
            _newStack[16] = 0x01000000;                           /* Thread PSR */
            _newStack[15] = reinterpret_cast<u_base_t>(TaskFunc); /* Thread PC */
            _newStack[8] = 0xFFFFFFFD;                            /* Thread LR, Exception return mode */

            /* Init task */
            _NewThread->Stack_pointer = __otos_switch(_newStack);

            /* update last run thread */
            const u_base_t index = static_cast<u_base_t>(Priority);
            this->last_thread[index] = this->thread_count;

            /* increase thread counter */
            this->thread_count++;
        }
    };

    auto Kernel::find_next_thread(const Priority thread_priority) const -> std::optional<u_base_t>
    {
        /* Lambda to check whether thread with specified priority is runnable */
        auto check_runnable = [thread_priority](Thread thread) -> bool
        {
            return (thread.is_runnable() & (thread.get_priority() == thread_priority));
        };

        /*
         * Find the next runnable thread.
         * This is done in two passes. One
         * pass after the currently active thread, where
         * the nearest candidate for round-robin most
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

        /* === First pass === */
        auto next = std::find_if(
            threads.cbegin() + last_thread + 1,
            threads.cend(),
            check_runnable);
        /* Check whether thread was found */
        if (next != threads.cend())
            return std::distance(threads.cbegin(), next);

        /* === Second pass === */
        next = std::find_if(
            threads.cbegin(),
            threads.cbegin() + last_thread + 1,
            check_runnable);
        /* Check whether thread was found */
        if (next != (threads.cbegin() + last_thread + 1))
            return std::distance(threads.cbegin(), next);

        /* No runnable thread was found */
        return {};
    };

    /* === Functions === */
    auto get_time_ms() -> std::uint32_t
    {
        return Kernel::get_time_ms();
    };
}; // namespace OTOS