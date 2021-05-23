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
 * @file    task.c
 * @author  SO
 * @version v1.0.0
 * @date    09-March-2021
 * @brief   The basic functions for task interaction are defined here.
 ******************************************************************************
 */

// ****** Includes ******
#include "task.h"

// ****** Methods ******

/**
 * @brief Create instance of thread class.
 * @details Only necessary when the thread uses Sleep()
 */
OTOS::Task::Task()
{
    this->LastTick = 0;
};

/**
 * @brief Disable the SysTick interrupt, so that this thread
 * cannot be interrupted. Make sure to enable interrupts again
 * after important section is finished!.
 */
void OTOS::Task::lock(void)
{
    ///@todo Disable SysTick interrupt
};

/**
 * @brief Enable the SysTick interrupt again, after important
 * section finished execution.
 */
void OTOS::Task::unlock(void)
{
    ///@todo Enable SysTick interrupt
};

/**
 * @brief Yield the execution until a condition becomes true.
 * @param Condition This value has to be true in order to resume execution.
 */
void OTOS::Task::waitFor(bool Condition)
{
    while (!Condition)
        __otos_yield();
};

/**
 * @brief Yield execution and give control to kernel.
 */
void OTOS::Task::yield(void)
{
    __otos_yield();
};

/**
 * @brief Yield execution for a specific amount of time.
 * @param Time Time to sleep in SysTicks.
 */
void OTOS::Task::sleep(unsigned int Time)
{
    ///@todo Add sleep function.
};