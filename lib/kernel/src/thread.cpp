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
 * @file    thread.c
 * @author  SO
 * @version v2.0.0
 * @date    16-March-2021
 * @brief   The internal thread handler for the OTOS kernel.
 ==============================================================================
 */

// *** Includes ***
#include "thread.h"

// *** Methods ***

/**
 * @brief Initialize the stack information of the thread.
 * @param StackPosition Pointer to beginning (top) of thread stack.
 * @param StackSize The size of the thread stack in words.
 */
void OTOS::Thread::set_stack(stackpointer_t stack_position, u_base_t stacksize)
{
    // Set the stack information the kernel provides
    this->Stack_pointer = stack_position;
    this->Stack_top = stack_position;
    this->Stacksize = stacksize;
};

/**
 * @brief Get the allocated stack size of the thread.
 * @return Allocated stack size of the thread in words.
 * @details getter
 */
u_base_t OTOS::Thread::get_stacksize(void) const
{
    return this->Stacksize;
};

/**
 * @brief check whether the current thread shows a stack overflow.
 * @return Returns true when a stack overflow occurred.
 */
bool OTOS::Thread::get_stackoverflow(void) const
{
    // When the current stack pointer occupies more or all of the stack, return true
    return (u_base_t)(this->Stack_top - this->Stack_pointer) >= this->Stacksize;
};

/**
 * @brief Get the priority of the thread.
 * 
 * @return OTOS::Priority 
 */
OTOS::Priority OTOS::Thread::get_priority(void) const
{
    return this->priority;
};

/**
 * @brief Set the schedule data of one thread
 * @param ticks The execution periode of the thread in Ticks.
 * @param priority The execution priority of the thread.
 */
void OTOS::Thread::set_schedule(u_base_t ticks, Priority priority)
{
    // Set schedule data
    this->priority = priority;
    this->schedule_ticks = ticks;

    // Reset tick counter
    this->counter_ticks = ticks;

    // When ticks is zero, thread is runnable immediately
    if (ticks == 0)
        this->state = State::Runnable;
    else
        this->state = State::Blocked;
};

/**
 * @brief Set the thread as running.
 * 
 */
void OTOS::Thread::set_running(void)
{
    this->state = State::Running;
};

/**
 * @brief Set the thread as blocked. Resets the thread tick counter.
 * 
 */
void OTOS::Thread::set_blocked(void)
{
    // only go to blocked state when schedule is not zero
    if (this->schedule_ticks)
    {
        this->counter_ticks = this->schedule_ticks;
        this->state = State::Blocked;
    }
    else // thread is runnable immediately
        this->state = State::Runnable;
};

/**
 * @brief Count SysTicks to determine whether the thread is runnable.
 */
void OTOS::Thread::count_tick(void)
{
    // Only count, when counter is not already at 0
    if (this->counter_ticks)
    {
        this->counter_ticks--;

        // When counter reached zero, thread is runnable
        if (this->counter_ticks == 0)
            this->state = State::Runnable;
    }
};

/**
 * @brief Check whether the current thread is runnable.
 * @return Returns true, when the thread is runnable.
 */
bool OTOS::Thread::is_runnable(void) const
{
    // Return whether task is runnable
    return this->state == State::Runnable;
};