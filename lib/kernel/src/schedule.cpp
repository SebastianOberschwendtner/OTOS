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
/**
 ******************************************************************************
 * @file    schedule.c
 * @author  SO
 * @version v1.0.0
 * @date    16-March-2021
 * @brief   Handles and determines the scheduling of the threads of the OTOS kernel.
 ******************************************************************************
 */

// ****** Includes ******
#include "schedule.h"

// ****** Methods ******

/**
 * @brief Constructor for a Schedule object.
 * When the TickSchedule is 0, the thread is executed as often as possible.
 * @param Priority The priority of the thread.
 * @param TickSchedule (Optional, Default: 0) Defines the fixed periode for the thread.
 */
OTOS::Schedule::Schedule(void)
    : ThreadPriority(PrioLow), TickSchedule(0), TickCounter(0)
{

};

/**
 * @brief Set the schedule data of one thread
 * @param ThreadTicks The execution periode of the thread in Ticks.
 * @param ThreadPriority The execution priority of the thread.
 */
void OTOS::Schedule::setSchedule(u_base_t ThreadTicks, Priority Threadpriority)
{
    // Set schedule data
    this->ThreadPriority = ThreadPriority;
    this->TickSchedule = ThreadTicks;

    // Reset tick counter
    this->TickCounter = ThreadTicks;    
};

/**
 * @brief Count SysTicks to determine whether the thread is runable.
 */
void OTOS::Schedule::countTick(void)
{
    // Only count, when counter is not already at 0
    if (this->TickCounter)
        this->TickCounter--;
};

/**
 * @brief Check whether the current thread is runable.
 * @return Returns true, when the thread is runable.
 */
bool OTOS::Schedule::isRunable(void) const
{
    // When the tick counter reached 0, the task is runable
    return (this->TickCounter == 0);
};
