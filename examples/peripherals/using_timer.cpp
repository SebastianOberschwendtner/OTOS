/**
 * OTOS - Open Tec Operating System
 * Copyright (c) 2022 Sebastian Oberschwendtner, sebastian.oberschwendtner@gmail.com
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
 * @file    using_timer.cpp
 * @author  SO
 * @version v4.2.0
 * @date    30-May-2023
 * @brief   This example shows how to use the timer driver interface.
 ==============================================================================
 */
#include "main.h" // => include not strictly necessary, but for intellisense to work
#include <chrono>

/**
 * @brief Example task for using the timer driver.
 */
void Run_Timer_Example(void)
{
    // namespace aliases
    using namespace OTOS::literals;        // OTOS defines frequency literals
    using namespace std::chrono_literals;  // std::chrono defines time literals

    // Create timed task
    OTOS::Timed_Task task(OTOS::get_time_ms);

    // Create timer instance
    Timer::Timer timer(IO::TIM_2);

    // Configure timer
    timer.set_frequency(1000_Hz);
    timer.set_duration(1s);

    // Configure the compare channel
    auto compare_channel = timer.get_channel(1);
    compare_channel.set_mode(Timer::Mode::PWM);
    compare_channel.set_duty_cycle(0.5f);

    // Assign the GPIO to the compare channel
    GPIO::PIN pin(GPIO::Port::A, 5);
    pin.set_alternate_function(IO::TIM_2);

    // Start the timer and the compare channel
    compare_channel.enable();
    timer.start();

    while (1)
    {
        OTOS::Task::yield();
    }
};

