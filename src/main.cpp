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
/**
 ==============================================================================
 * @file    main.c
 * @author  SO
 * @version v5.0.0
 * @date    09-March-2021
 * @brief   Main function for the OTOS. Mainly used to demonstrate how the OTOS
 *          functions work and should be used.
 ==============================================================================
 */

/* === Includes === */
#include "main.h"

/* === Variables === */

/* === Tasks === */
/**
 * @brief Example task which blinks LED3 of the STM32F429-Discovery board.
 * @details Example Task
 */
void Blink_LED3()
{
#ifdef STM32F4
    auto LED3 = gpio::Pin::create<gpio::Port::G>(13, gpio::Mode::Output);

    while (1)
    {
        LED3.toggle();
        OTOS::Task::yield();
    }
#else
    OTOS::Task::yield();
#endif // STM32F4
};

/**
 * @brief Example task which blinks LED4 of the STM32F429-Discovery board.
 * @details Example Timed Task
 */
void Blink_LED4()
{
    // Create timed task and wait without blocking
    OTOS::TimedTask Task(OTOS::get_time_ms);
    Task.wait_ms(1000);

#ifdef STM32F4
    // Create the LED
    auto LED4 = gpio::Pin::create<gpio::Port::G>(14, gpio::Mode::Output);

    while (1)
    {
        LED4.toggle();
        OTOS::Task::yield();
    }
#else
    OTOS::Task::yield();
#endif // STM32F4
};

/* Create the kernel object */
OTOS::Kernel OS;

/* === Main === */
auto main() -> int
{
    stm_core::switch_system_clock<stm_core::Clock::PLL_HSI, F_CPU/1000000, F_APB1/1000000, F_APB2/1000000>();
    // Configure SysTick timer for interrupts every 1 ms
    timer::SysTick_Configure();

    // Schedule Threads
    OS.schedule_thread<128U>(&Blink_LED3, OTOS::Priority::Normal, 1U);
    OS.schedule_thread<128U>(&Blink_LED4, OTOS::Priority::Normal, 5U);

    // Start the task execution
    OS.start();

    // Never reached
    return 0;
};

/** 
 * @brief Provide a Interrupt handler for the systick timer,
 * which gets called every 1 ms.
 */
extern "C" void SysTick_Handler()
{
    OS.count_time_ms();
    OS.update_schedule();
};