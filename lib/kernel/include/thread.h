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

#ifndef THREAD_H_
#define THREAD_H_

// ****** Includes ******
#include "schedule.h"

namespace OTOS
{
    // Class for stack data of one thread
    class Thread : public Schedule
    {
    private:
        // Properties
        u_base_t StackSize;          // Allocated stack size of the thread
        stackpointer_t StackTop; // Pointer to the top of allocated stack for thread

    public:
        //Properties
        stackpointer_t StackPointer; // Pointer to the current top of stack of the thread

        // Methods
        Thread(void);
        void SetStack(stackpointer_t StackPosition, u_base_t StackSize);
        u_base_t GetStackSize(void) const;
        bool StackOverflow(void) const;
    };
};
#endif
