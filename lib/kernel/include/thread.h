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

#ifndef THREAD_H_
#define THREAD_H_

/* === Includes === */
#include <array>
#include <misc/types.h>

namespace OTOS
{
    namespace check
    {
        /**
         * @brief Constexpr to check the minimum required stack size for a thread.
         * @details The actual minimum size is 17, 42 is an arbitrary choice. ;)
         * @return The minimum stack size for a thread.
         */
        template <u_base_t Size>
        constexpr auto StackSize() -> u_base_t
        {
            static_assert(Size > 42, "Minimum Stack Size required for a thread is 42!");
            return Size;
        };
    }; // namespace check

    /* === Enums === */
    /* Enumeration for task priority */
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

    /* State of thread execution */
    enum class State
    {
        Runnable,
        Running,
        Blocked,
        Inactive
    };

    /** 
     * @class Thread
     * @brief Class for stack data of one thread
     */
    class Thread
    {
      public:
        /* === Constructors === */
        Thread() = default;

        /* === Setters === */
        /**
         * @brief Set the thread to the blocked state.
         * Resets the thread tick counter.
         */
        void set_blocked();

        /**
         * @brief Set the thread to the running state.
         */
        void set_running();

        /**
         * @brief Set the schedule data of one thread
         * @note A thread with a schedule of *0* is runnable immediately and
         * will ALWAYS be executed! So think about the task priority when
         * setting a schedule of *0*.
         * 
         * @param ticks The execution period of the thread in Ticks.
         * @param priority The execution priority of the thread.
         */
        void set_schedule(u_base_t ticks, Priority priority);

        /**
         * @brief Initialize the stack information of the thread.
         * @param StackPosition Pointer to beginning (top) of thread stack.
         * @param StackSize The size of the thread stack in words.
         */
        void set_stack(stackpointer_t stack_position, u_base_t stacksize);

        /* === Getters === */
        /**
         * @brief Get the priority of the thread.
         *
         * @return Priority
         */
        auto get_priority() const -> Priority;

        /**
         * @brief Get the allocated stack size of the thread.
         * @return Allocated stack size of the thread in words.
         */
        auto get_stacksize() const -> u_base_t;

        /**
         * @brief check whether the current thread shows a stack overflow.
         * @return Returns true when a stack overflow occurred.
         */
        auto get_stackoverflow() const -> bool;

        /**
         * @brief Check whether the current thread is runnable.
         * @return Returns true, when the thread is runnable.
         */
        auto is_runnable() const -> bool;

        /* === Methods === */
        /**
         * @brief Count SysTicks to determine whether the thread is runnable.
         */
        void count_tick();

        /* === Properties === */
        stackpointer_t Stack_pointer{0}; /* Pointer to the current top of stack of the thread */

      private:
        /* === Properties === */
        u_base_t Stacksize{0};            /**< Allocated stack size of the thread */
        stackpointer_t Stack_top{0};      /**< Pointer to the top of allocated stack for thread */
        State state{State::Inactive};     /**< State of the thread */
        Priority priority{Priority::Low}; /**< Priority of task */
        u_base_t schedule_ticks{0};       /**< The scheduled execution time of thread */
        u_base_t counter_ticks{0};        /**< Ticks since last execution of thread */
    };
}; // namespace OTOS
#endif
