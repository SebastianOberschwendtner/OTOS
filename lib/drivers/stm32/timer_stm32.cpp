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
 * @file    timer_stm32.cpp
 * @author  SO
 * @version v2.9.1
 * @date    31-October-2021
 * @brief   Timer driver for STM32 microcontrollers.
 ==============================================================================
 */

// *** Includes ***
#include "timer_stm32.h"

/**
 * @brief Get the timer base address depending on the timer instance.
 * @param timer The timer instance which is used.
 * @return Return the peripheral base address of the timer instance.
 */
constexpr std::uintptr_t get_timer_address(const IO timer)
{
    switch (timer)
    {
#ifdef STM32F4
    case IO::TIM_1: return TIM1_BASE;
#endif
    case IO::TIM_2: return TIM2_BASE;
    default: return 0;
    }
};

// === Functions ===
/**
 * @brief Configure the SysTick timer for interrupts every 1 ms. 
 */
void Timer::SysTick_Configure(void)
{
    // Compute the ticks per ms
    constexpr unsigned int ticks_ms = (F_CPU / 1000);

    // Configure the SysTick timer
    SysTick_Config(ticks_ms);

    // Reconfigure Priorities so that SysTick has the highest priority
#if defined(STM32L0)
    NVIC_SetPriority(SVC_IRQn, 1);
#elif defined(STM32F4)
    NVIC_SetPriority(SVCall_IRQn, 1);
#endif
    NVIC_SetPriority(SysTick_IRQn, 0);
    return;
};

/**
 * @brief Constructor for timer object
 * @param timer The instance of the timer to be used.
 */
Timer::Timer::Timer(const IO timer)
: thisTimer{reinterpret_cast<volatile TIM_TypeDef*> ( get_timer_address(timer) )}
, thisInstance(timer)
{
};

/**
 * @brief Get the current count of the timer
 * @return The current count of the timer in ticks.
 */
unsigned int Timer::Timer::get_count(void) const
{
    return this->thisTimer->CNT;
};