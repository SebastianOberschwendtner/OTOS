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

#ifndef SCHEDULE_H_
#define SCHEDULE_H_

// *** Includes ***
#include "types.h"

// *** Declarations ***
namespace OTOS 
{
    // Enumeration for task priority
    enum class Priority : u_base_t
    {
        Low = 1,
        Normal = 2,
        High = 3
    };

    // Class for schedule data of one thread
    class Schedule
    {
    private:
        // Properties:
        Priority priority{Priority::Low}; // Priority of task
        u_base_t schedule_ticks{0};       // The scheduled execution time of thread
        u_base_t counter_ticks{0};        // Ticks since last execution of thread

    public:
        // Methods
        Schedule(void) = default;
        void set_schedule(u_base_t ticks, Priority priority);
        void count_tick(void);
        bool is_runable(void) const;
    };
};
#endif
