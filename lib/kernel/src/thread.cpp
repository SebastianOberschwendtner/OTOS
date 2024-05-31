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
 * @file    thread.cpp
 * @author  SO
 * @version v5.0.0
 * @date    16-March-2021
 * @brief   The internal thread handler for the OTOS kernel.
 ==============================================================================
 */

/* === Includes === */
#include "thread.h"

namespace OTOS
{
    /* === Methods === */
    void Thread::set_blocked()
    {
        /* only go to blocked state when schedule is not zero */
        if (this->schedule_ticks)
        {
            this->counter_ticks = this->schedule_ticks;
            this->state = State::Blocked;
        }
        else /* thread is runnable immediately */
            this->state = State::Runnable;
    };

    void Thread::set_running()
    {
        this->state = State::Running;
    };

    void Thread::set_schedule(const u_base_t ticks, const Priority priority)
    {
        /* Set schedule data */
        this->priority = priority;
        this->schedule_ticks = ticks;

        /* Reset tick counter */
        this->counter_ticks = ticks;

        /* When ticks is zero, thread is runnable immediately */
        if (ticks == 0)
            this->state = State::Runnable;
        else
            this->state = State::Blocked;
    };

    void Thread::set_stack(stackpointer_t stack_position, const u_base_t stacksize)
    {
        /* Set the stack information the kernel provides */
        this->Stack_pointer = stack_position;
        this->Stack_top = stack_position;
        this->Stacksize = stacksize;
    };

    auto Thread::get_priority() const -> Priority
    {
        return this->priority;
    };

    auto Thread::get_stacksize() const -> u_base_t
    {
        return this->Stacksize;
    };

    auto Thread::get_stackoverflow() const -> bool
    {
        /* When the current stack pointer occupies more or all of the stack, return true */
        return (u_base_t)(this->Stack_top - this->Stack_pointer) >= this->Stacksize;
    };

    auto Thread::is_runnable() const -> bool
    {
        /* Return whether task is runnable */
        return this->state == State::Runnable;
    };

    void Thread::count_tick()
    {
        /* Only count, when counter is not already at 0 */
        if (this->counter_ticks)
        {
            this->counter_ticks--;

            /* When counter reached zero, thread is runnable */
            if (this->counter_ticks == 0)
                this->state = State::Runnable;
        }
    };
}; // namespace OTOS