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

/* === Includes === */
#include "task.h"

namespace OTOS
{
    /* === Constructors === */
    TimedTask::TimedTask(std::uint32_t (*timer_handle)())
        : get_time_ms{timer_handle} {};

    /* === Methods === */
    void TimedTask::tic()
    {
        this->time_last = this->get_time_ms();
    };

    auto TimedTask::time_elapsed_ms() const -> std::uint32_t
    {
        return this->get_time_ms() - this->time_last;
    };

    auto TimedTask::toc() const -> std::uint32_t
    {
        return this->get_time_ms();
    };

    void TimedTask::yield()
    {
        __otos_yield();
    };
}; // namespace OTOS
