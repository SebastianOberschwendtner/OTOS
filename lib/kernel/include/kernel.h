/**
 * OTOS - Open Tec Operating System
 * Copyright (c) 2021 - 2024 Sebastian Oberschwendtner,
 * sebastian.oberschwendtner@gmail.com
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

#ifndef KERNEL_H_
#define KERNEL_H_

/* === Includes === */
#include "thread.h"
#include <algorithm>
#include <array>
#include <optional>
#include <processors.h>

/* === Defines === */
/** The defines are just default values here.
 * They can be overwritten by defining them before including kernel.h
 * => that is why they are not defined with constexpr...
 */
#ifndef OTOS_STACK_SIZE
#define OTOS_STACK_SIZE 1024 /* Stack Size in words or u_base_t */
#endif

#ifndef OTOS_NUMBER_THREADS
#define OTOS_NUMBER_THREADS 5 /* Maximum number of threads */
#endif

namespace OTOS
{
    /* === Parameters === */
    constexpr std::size_t stack_size = OTOS_STACK_SIZE;
    constexpr std::size_t number_threads = OTOS_NUMBER_THREADS;
    constexpr u_base_t ms_per_tick = 1;
    constexpr u_base_t number_priorities = std::size(Available_Priorities);

    class Kernel
    {
      public:
        /* === Constructors === */
        /**
         * @brief Constructor for kernel object.
         * @note Only one kernel object should exist in your project!
         */
        Kernel();

        /* No other method of construction */
        Kernel(const Kernel &) = delete;
        Kernel(Kernel &&) = delete;
        auto operator=(const Kernel &) -> Kernel & = delete;
        auto operator=(Kernel &&) -> Kernel & = delete;

        /* === Setters === */
        /**
         * @brief Add a thread schedule to the kernel and activate its execution.
         * No timing is specified with this. The task is executed when calling this function!
         * @note The task will be executed immediately and will ALWAYS be executed. So take
         * care about the priority of the task, so that it does not block other tasks.
         * 
         * @tparam stack_size The size of the thread stack in words.
         * @param TaskFunc Function pointer to the task of the thread.
         * @param Priority Priority of the scheduled thread.
         * @return Kernel& Returns a reference to the kernel object.
         */
        template <u_base_t stack_size>
        auto schedule_thread(const taskpointer_t TaskFunc, const Priority Priority) -> Kernel &
        {
            /* Schedule a thread with no specified timing */
            this->schedule_thread(TaskFunc, check::StackSize<stack_size>(), Priority, 0);

            /* Return the reference to the kernel */
            return *this;
        };

        /**
         * @brief Add a thread schedule to the kernel and activate its execution.
         * The task is executed when calling this function!
         * @tparam stack_size The size of the thread stack in words.
         * @param TaskFunc Function pointer to the task of the thread.
         * @param Priority Priority of the scheduled thread.
         * @param Frequency The frequency of execution in [Hz].
         * @return Kernel& Returns a reference to the kernel object.
         */
        template <u_base_t stack_size>
        auto schedule_thread(const taskpointer_t TaskFunc, const Priority Priority, const u_base_t Frequency) -> Kernel &
        {
            /* Calculate the thread timing and schedule it */
            const u_base_t time_ms = 1000 / Frequency;
            const u_base_t schedule = time_ms / ms_per_tick;
            this->schedule_thread(TaskFunc, check::StackSize<stack_size>(), Priority, schedule);

            /* Return the reference to the kernel */
            return *this;
        };

        /* === Getters === */
        /**
         * @brief Get the current size of the allocated stack.
         * Loops through all the stack sizes of the scheduled threads.
         * @return Returns the currently allocated stack size in words.
         */
        auto get_allocated_stacksize() const -> u_base_t;

        /**
         * @brief Determine the next thread to run.
         * The object stores this internally.
         * @return Returns the number of the thread which should be executed next.
         * The optional evaluates to false, when no thread is currently runnable.
         * @details This currently only implements a priority based round-robin scheme.
         */
        auto get_next_thread() const -> std::optional<u_base_t>;

        /**
         * @brief Get the current system time in milli-seconds.
         * @return Returns the current time in milli-seconds.
         */
        static auto get_time_ms() -> std::uint32_t;

        /* === Methods === */
        /**
         * @brief Increase the milli-seconds timer by one milli-second.
         */
        static void count_time_ms();

        /**
         * @brief Start the kernel execution.
         */
        void start();

        /**
         * @brief Switch to the thread which is currently active and handover control.
         * @param next_thread The number of the next thread to be executed.
         */
        void switch_to_thread(u_base_t next_thread);

        /**
         * @brief Update the thread tick counters and determine which
         * thread is runnable.
         */
        void update_schedule();

      private:
        /* === Methods === */
        /**
         * @brief Add a thread schedule to the kernel and activate its execution.
         * @param TaskFunc Function pointer to the task of the thread.
         * @param StackSize The size of the thread stack in words. => Should be checked with 'Check::StackSize<Size>()' first.
         * @param Priority Priority of the scheduled thread.
         * @param Schedule The call schedule of the thread in ticks. Leave at 0 when thread can be executed whenever.
         */
        void schedule_thread(
            taskpointer_t TaskFunc,
            u_base_t StackSize,
            Priority Priority,
            u_base_t Schedule);

        /**
         * @brief Determine the next thread to run based on priority in
         * a round-robin fashion.
         *
         * @param thread_priority The priority group to get the next thread for.
         * @return The next thread number to run. Returns False if there is nor
         * thread ready to be scheduled.
         */
        auto find_next_thread(OTOS::Priority thread_priority) const -> std::optional<u_base_t>;

        /* === Properties === */
        u_base_t thread_count{0};                               /**< Number of scheduled threads */
        std::array<Thread, number_threads> Threads{};           /**< Array with stack data and schedule of each thread */
        std::array<u_base_t, stack_size> Stack{0};              /**< The total stack for the threads */
        std::array<u_base_t, number_priorities> last_thread{0}; /**< The ID of the last thread which ran for every priority level */
        static std::uint32_t Time_ms;                           /**< Kernel timer with ms resolution */
    };

    /* === Functions === */
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
    auto get_time_ms() -> std::uint32_t;

};     // namespace OTOS
#endif // KERNEL_H_
