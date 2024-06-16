/**
 * OTOS - Open Tec Operating System
 * Copyright (c) 2022 - 2024 Sebastian Oberschwendtner, sebastian.oberschwendtner@gmail.com
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
 * @version v5.1.0
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
    /* namespace aliases */
    using namespace OTOS::literals;       /* OTOS defines frequency literals */
    using namespace std::chrono_literals; /* std::chrono defines time literals */

    /* Create timed task */
    OTOS::TimedTask task(OTOS::get_time_ms);

    /* Create timer instance */
    constexpr stm32::Peripheral timer_used = stm32::Peripheral::TIM_2;
    auto timer = timer::Timer::create<timer_used>();

    /* Configure timer */
    timer.set_tick_frequency(1000_Hz).set_period(1s);

    /* Assign the GPIO to the compare channel */
    auto pin = gpio::Pin::create<gpio::Port::A>(5);
    pin.set_alternate_function(timer_used);

    /* Configure the compare channel */
    timer.get_channel(1)
        .set_mode(timer::Mode::PWM)
        .set_duty_cycle(0.5f)
        .enable();

    /* Enable the update interrupt and start the timer */
    timer.enable_interrupt(timer::interrupt::Update).start();

    while (1)
    {
        OTOS::Task::yield();
    }
};
