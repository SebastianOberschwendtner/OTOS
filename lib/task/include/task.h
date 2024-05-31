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

#ifndef TASK_H_
#define TASK_H_

/* === Includes === */
#include <chrono>
#include <processors.h>
#include <misc/types.h>

/* === defines === */

/**
 * @brief This macro can be used to wait as long as 
 * the condition is true without blocking other tasks.
 * 
 * @param condition The condition which has to be true to wait.
 */
#define YIELD_WHILE(condition) while(condition) { __otos_yield(); }

namespace OTOS {

    /* === Class Definitions === */
    /**
     * @struct Task
     * @brief Basic task class for yielding execution.
     */
    struct Task
    {
        Task() = delete;
        static void yield() { __otos_yield(); };
    };

    /**
     * @brief Task class which can be timed to
     * be executed with in a certain time interval.
     */
    class TimedTask
    {
    public:
        /* === Constructors === */
        TimedTask() = delete;

        /**
         * @brief Construct a new timed Task object.
         *
         * @param timer_handle The function handle to the timer which provides the time in ms.
         */
        explicit TimedTask(std::uint32_t(*timer_handle)());

        /* === Methods === */
        /**
         * @brief Wait for a specified amount of time. The
         * task blocks execution as long as the wait time
         * is not over.
         * 
         * This function can be used for timing when the
         * OS itself is not used. The SysTick timer has
         * to be updated, though.
         * 
         * @param time_ms The wait time in [ms].
         * @details Blocking function
         */
        void block_ms(const uint32_t time_ms)
        {
            /* Remember the time when the function was called */
            this->tic();

            /* Block the task until the wait time is over */
            while(this->time_elapsed_ms() < static_cast<std::uint32_t>(time_ms));
        };

        /**
         * @brief Start a time measurement by saving the current time.
         */
        void tic();

        /**
         * @brief Measure the elapsed time since tic() was called.
         *
         * @return std::uint32_t Elapsed time in [ms].
         */
        auto time_elapsed_ms() const -> std::uint32_t;

        /**
         * @brief Return the current time of the assigned timer in ms.
         *
         * @return std::uint32_t: Current time in [ms].
         */
        auto toc() const -> std::uint32_t;

        /**
         * @brief Wait for a specified amount of time. The
         * task yields as long as the wait time is not over.
         * 
         * @param time_ms The wait time in [ms].
         */
        void wait_ms(const uint32_t time_ms)
        {
            this->tic();
            while(this->time_elapsed_ms() < static_cast<std::uint32_t>(time_ms))
                this->yield();
        };

        /**
         * @brief Yield execution and give control to kernel.
         */
        void yield();

    private:
        /* === Properties === */
        std::uint32_t time_last{0}; /**< The last time the tic function was called. */
        std::uint32_t (*const get_time_ms)(); /**< Function pointer to get the current time in ms. */
    };
}; // namespace OTOS

#endif // TASK_H_
