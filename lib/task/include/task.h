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

#ifndef TASK_H_
#define TASK_H_

// === Includes ===
#include "processors.h"
#include "types.h"
#include <chrono>

// === defines ===

/**
 * @brief This macro can be used to wait as long as 
 * the condition is true without blocking other tasks.
 */
#define YIELD_WHILE(condition) while(condition) { __otos_yield(); }

namespace OTOS {

    // === Class Definitions ===
    struct Task
    {
        Task() = delete;
        static void yield(void) { __otos_yield(); };
    };

    class Timed_Task
    {
    private:
        // *** Properties ***
        std::uint32_t time_last{0};
        std::uint32_t (*const get_time_ms)();

    public:
        // *** Constructor ***
        Timed_Task() = delete;
        Timed_Task(std::uint32_t(*timer_handle)());

        // *** Methods ***
        void yield(void);
        void tic(void);
        std::uint32_t toc(void) const;
        std::uint32_t time_elapsed_ms(void) const;

        // *** Method templates ***
        /**
         * @brief Wait for a specified amount of time. The
         * task yields as long as the wait time is not over.
         * 
         * @param time_ms The wait time in [ms].
         */
        void wait_ms(const unsigned long time_ms)
        {
            this->tic();
            while(this->time_elapsed_ms() < static_cast<std::uint32_t>(time_ms))
                this->yield();
        };
    };
};

#endif