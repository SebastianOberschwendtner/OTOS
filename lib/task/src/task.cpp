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
 * @file    task.c
 * @author  SO
 * @version v2.12.0
 * @date    09-March-2021
 * @brief   The basic functions for task interaction are defined here.
 ==============================================================================
 */

// *** Includes ***
#include "task.h"

// *** Methods ***
/**
 * @brief Construct a new timed Task object.
 * 
 * @param timer_handle The function handle to the timer which provides the time in ms.
 */
OTOS::Timed_Task::Timed_Task(std::uint32_t(*timer_handle)())
: get_time_ms{timer_handle}
{

};

/**
 * @brief Yield execution and give control to kernel.
 */
void OTOS::Timed_Task::yield(void)
{
    __otos_yield();
};

/**
 * @brief Start a time measurement by saving the current time.
 * 
 */
void OTOS::Timed_Task::tic(void)
{
    this->time_last = this->get_time_ms();
};

/**
 * @brief Return the current time of the assigned timer in ms.
 * 
 * @return std::uint32_t: Current time in [ms].
 */
std::uint32_t OTOS::Timed_Task::toc(void) const
{
    return this->get_time_ms();
};

/**
 * @brief Measure the elapsed time since tic() was called.
 * 
 * @return std::uint32_t Elapsed time in [ms].
 */
std::uint32_t OTOS::Timed_Task::time_elapsed_ms(void) const
{
    return this->get_time_ms() - this->time_last;
};