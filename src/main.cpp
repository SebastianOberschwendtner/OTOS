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
 ==============================================================================
 * @file    main.c
 * @author  SO
 * @version v2.9.0
 * @date    09-March-2021
 * @brief   Main function for the OTOS. Mainly used to demonstrate how the OTOS
 *          functions work and should be used.
 ==============================================================================
 */

// *** Includes ***
#include "main.h"

// *** Variables ***

// *** Functions ***

/**
 * @brief Example task which blinks LED3 of the STM32F429-Discovery board.
 * @details Example Task
 */
void Blink_LED3(void)
{
    GPIO::PIN LED3(GPIO::Port::G, 13, GPIO::Mode::Output);

    while (1)
    {
        LED3.toggle();
        OTOS::Task::yield();
    }
};

/**
 * @brief Example task which blinks LED4 of the STM32F429-Discovery board.
 * @details Example Timed Task
 */
void Blink_LED4(void)
{
    // Create timed task and wait without blocking
    OTOS::Timed_Task Task(OTOS::get_time_ms);
    Task.wait_ms(1000);

    // Create the LED
    GPIO::PIN LED4(GPIO::Port::G, 14, GPIO::Mode::Output);

    while (1)
    {
        LED4.toggle();
        OTOS::Task::yield();
    }
};

// Create the kernel object
OTOS::Kernel OS;

// *** Main ***
int main(void)
{
    ST_Core::switch_system_clock<ST_Core::Clock::PLL_HSI, 120, 30, 60>();
    // Configure SysTick timer for interrupts every 1 ms
    Timer::SysTick_Configure();

    // Schedule Threads
    OS.schedule_thread<128>(&Blink_LED3, OTOS::Priority::Normal, 1);
    OS.schedule_thread<128>(&Blink_LED4, OTOS::Priority::Normal, 5);

    // Start the task execution
    OS.start();

    // Never reached
    return 0;
};

/** 
 * @brief Provide a Interrupt handler for the systick timer,
 * which gets called every 1 ms.
 */
extern "C" void SysTick_Handler(void)
{
    OS.count_time_ms();
    OS.update_schedule();
};