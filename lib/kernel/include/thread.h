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

#ifndef THREAD_H_
#define THREAD_H_

// *** Includes ***
#include <array>
#include "types.h"

// *** Declarations ***
namespace OTOS
{
    namespace Check
    {
        /**
    * @brief Constexpr to check the minimum required stack size for a thread.
    * @details The actual minimum size is 17, 42 is an arbitrary choice. ;)
    */
        template <u_base_t Size>
        constexpr u_base_t StackSize()
        {
            static_assert(Size > 42, "Minimum Stack Size required for a thread is 42!");
            return Size;
        };
    }; // namespace Check

    // === Enums ===
    // Enumeration for task priority
    enum class Priority : u_base_t
    {
        Low = 0,
        Normal = 1,
        High = 2
    };

    /*
     * Group all available priorites in an array.
     * The scheduler use the sorting in this array to
     * determine the priority sequence. The order
     * in the array has to be from highest priority
     * to the lowest priority!
     */
    constexpr std::array<Priority, 3> Available_Priorities{
        Priority::High,
        Priority::Normal,
        Priority::Low};

    // State of thread execution
    enum class State
    {
        Runnable,
        Running,
        Blocked,
        Inactive
    };

    // === Classes ===
    // Class for stack data of one thread
    class Thread
    {
    private:
        // Properties
        u_base_t Stacksize{0};            // Allocated stack size of the thread
        stackpointer_t Stack_top{0};      // Pointer to the top of allocated stack for thread
        State state{State::Inactive};     // State of the thread
        Priority priority{Priority::Low}; // Priority of task
        u_base_t schedule_ticks{0};       // The scheduled execution time of thread
        u_base_t counter_ticks{0};        // Ticks since last execution of thread

    public:
        // Constructor
        Thread() = default;

        // Properties
        stackpointer_t Stack_pointer{0}; // Pointer to the current top of stack of the thread

        // Methods
        void set_stack(stackpointer_t stack_position, u_base_t stacksize);
        u_base_t get_stacksize(void) const;
        bool get_stackoverflow(void) const;
        Priority get_priority(void) const;
        void set_schedule(u_base_t ticks, Priority priority);
        void set_running(void);
        void set_blocked(void);
        void count_tick(void);
        bool is_runnable(void) const;
    };
}; // namespace OTOS
#endif
