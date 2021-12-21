/**
 * OTOS - Open Tec Operating System
 * Copyright (c) 2021 Sebastian Oberschwendtner,
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

// *** Includes ***
#include <array>
#include <optional>
#include <algorithm>
// #include <ranges>

#include "processors.h"
#include "thread.h"

// *** Defines ***
/** The defines are just default values here.
 * They can be overritten by defining them before including kernel.h
 * => that is why they are not defined with constexpr...
 */
#ifndef OTOS_STACK_SIZE
#define OTOS_STACK_SIZE 1024 // Stack Size in words or u_base_t
#endif

#ifndef OTOS_NUMBER_THREADS
#define OTOS_NUMBER_THREADS 5 // Maximum number of threads
#endif

// *** Declarations ***
namespace OTOS
{
    // === Parameters ===
    constexpr std::size_t stack_size = OTOS_STACK_SIZE;
    constexpr std::size_t number_threads = OTOS_NUMBER_THREADS;
    constexpr u_base_t ms_per_tick = 1;
    constexpr u_base_t number_priorities = std::size(Available_Priorities);

    // === Classes ===
    class Kernel
    {
    private:
        // Properties
        u_base_t thread_count{0};                       // Number of scheduled threads
        std::array<Thread, number_threads> Threads{};   // Array with stack data and schedule of each thread
        std::array<u_base_t, stack_size> Stack{0};      // The total stack for the threads
        std::array<u_base_t, number_priorities> last_thread{0};          // The ID of the last thread which ran for every priority level
        static std::uint32_t Time_ms;                   // Kernel timer with ms resolution

        // Methods
        void schedule_thread(
            const taskpointer_t TaskFunc,
            const u_base_t StackSize,
            const Priority Priority,
            const u_base_t Schedule);
        std::optional<u_base_t> find_next_thread(const OTOS::Priority thread_priority) const;

    public:
        // Methods
        Kernel();
        void switch_to_thread(const u_base_t next_thread);
        std::optional<u_base_t> get_next_thread(void) const;
        void update_schedule(void);
        void start(void);
        u_base_t get_allocated_stacksize(void) const;
        static std::uint32_t get_time_ms(void);
        static void count_time_ms(void);

        // Templated methods

        /**
         * @brief Add a thread schedule to the kernel and activate its execution.
         * No timing is specified with this. The task is executed when calling this function!
         * @tparam stack_size The size of the thread stack in words.
         * @param TaskFunc Function pointer to the task of the thread.
         * @param Priority Priority of the scheduled thread.
         */
        template<u_base_t stack_size>
        void schedule_thread(const taskpointer_t TaskFunc, const Priority Priority)
        {
            this->schedule_thread(TaskFunc, Check::StackSize<stack_size>(), Priority, 0);
        };
        
        /**
         * @brief Add a thread schedule to the kernel and activate its execution.
         * The task is executed when calling this function!
         * @tparam stack_size The size of the thread stack in words.
         * @param TaskFunc Function pointer to the task of the thread.
         * @param Priority Priority of the scheduled thread.
         * @param Frequency The frequency of execution in [Hz].
         */
        template<u_base_t stack_size>
        void schedule_thread(const taskpointer_t TaskFunc, const Priority Priority, const u_base_t Frequency)
        {
            const u_base_t time_ms = 1000/Frequency;
            const u_base_t schedule = time_ms / ms_per_tick;
            this->schedule_thread(TaskFunc, Check::StackSize<stack_size>(), Priority, schedule);
        };
    };

    // === Functions ===
    std::uint32_t get_time_ms(void);

}; // namespace OTOS
#endif
