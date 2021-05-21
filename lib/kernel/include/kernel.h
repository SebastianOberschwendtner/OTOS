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

#ifndef KERNEL_H_
#define KERNEL_H_

// ****** Includes ******
#include "thread.h"
#include "processors.h"
#include <array>

// ****** Defines *******
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

namespace OTOS
{
    class Kernel
    {
    private:
        // Properties
        u_base_t CurrentThread;              // The current running thread
        u_base_t ThreadCount;                // Number of scheduled threads
        std::array<Thread, OTOS_NUMBER_THREADS> Threads; // Array with stack data of each thread
        std::array<u_base_t, OTOS_STACK_SIZE> Stack;     // The total stack for the threads

        // Methods
        void UpdateSchedule(void);
        void GetNextThread(void);
        void SwitchThread(void);
        // unsigned int Time_ms(void);

    public:
        // Methods
        Kernel();
        void ScheduleThread(taskpointer_t TaskFunc, u_base_t StackSize, Priority Priority);
        // void ScheduleThread_Hz(void);
        void Start(void);
        u_base_t AllocatedStackSize(void) const;
    };

};
#endif
