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

#include "processors.h"
#include "thread.h"

// *** Defines ****
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
    constexpr size_t stack_size     = OTOS_STACK_SIZE;
    constexpr size_t number_threads = OTOS_NUMBER_THREADS;

    // === Classes ===
    class Kernel
    {
    private:
        // Properties
        u_base_t active_thread{0};                       // The current active thread
        u_base_t thread_count{0};                        // Number of scheduled threads
        std::array<Thread, number_threads> Threads; // Array with stack data of each thread
        std::array<u_base_t, stack_size> Stack{0};  // The total stack for the threads
        static std::uint32_t Time_ms;                    // Kernel timer with ms resolution

        // Methods
        void update_schedule(void);
        void get_next_thread(void);
        void switch_thread(void);

    public:
        // Methods
        Kernel();
        void schedule_thread(taskpointer_t TaskFunc, u_base_t StackSize, Priority Priority);
        void start(void);
        u_base_t get_allocated_stacksize(void) const;
        static std::uint32_t get_time_ms(void);
        static void count_time_ms(void);
    };

    // === Functions ===
    std::uint32_t   get_time_ms         (void);

}; // namespace OTOS
#endif
