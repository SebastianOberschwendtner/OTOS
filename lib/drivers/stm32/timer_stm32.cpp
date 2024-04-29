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
 * @file    timer_stm32.cpp
 * @author  SO
 * @version v4.2.0
 * @date    31-October-2021
 * @brief   Timer driver for STM32 microcontrollers.
 ==============================================================================
 */

// *** Includes ***
#include "timer_stm32.h"

namespace detail
{
    /**
     * @brief Get the timer base address depending on the timer instance.
     * @param timer The timer instance which is used.
     * @return Return the peripheral base address of the timer instance.
     */
    constexpr auto get_timer_address(const IO timer) -> std::uintptr_t
    {
        switch (timer)
        {
            case IO::TIM_2:
                return TIM2_BASE;
#ifndef STM32L053xx
            case IO::TIM_3:
                return TIM3_BASE;
#endif
#ifdef STM32F4
            case IO::TIM_1:
                return TIM1_BASE;
            case IO::TIM_4:
                return TIM4_BASE;
            case IO::TIM_5:
                return TIM5_BASE;
#endif
            default:
                return 0;
        }
    }

    /**
     * @brief Enable the clock for the selected timer instance.
     * @param timer The timer instance which is used.
     */
    constexpr void enable_timer_clock(const IO timer)
    {
        // Enable the clock
        switch (timer)
        {
            case IO::TIM_2:
                RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
                break;
#ifndef STM32L053xx
            case IO::TIM_3:
                RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
                break;
#endif
#ifdef STM32F4
            case IO::TIM_1:
                RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
                break;
            case IO::TIM_4:
                RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
                break;
            case IO::TIM_5:
                RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
                break;
#endif
            default:
                break;
        }
    }

    /**
     * @brief Get the base clock frequency of the timer.
     *
     * @param timer The timer instance which is used.
     * @return The base clock frequency of the timer.
     */
    constexpr auto get_timer_clock_frequency(const IO timer) -> uint32_t
    {
        switch (timer)
        {
#ifdef STM32F4
            case IO::TIM_1:
                return F_APB2;
            case IO::TIM_4:
                return F_APB1;
            case IO::TIM_5:
                return F_APB1;
#endif
            case IO::TIM_2:
                return F_APB1;
#ifndef STM32L053xx
            case IO::TIM_3:
                return F_APB1;
#endif
            default:
                return F_CPU;
        }
    }

}; // namespace detail

namespace Timer
{
    /* === Functions === */
    void SysTick_Configure()
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
    }

    /* === Constructors === */
    Timer::Timer(const IO timer)
        : thisTimer{reinterpret_cast<volatile TIM_TypeDef *>(detail::get_timer_address(timer))}, thisInstance(timer)
    {
        // Enable the peripheral clock
        detail::enable_timer_clock(timer);

        // Set the top value to the maximum
        this->thisTimer->ARR = 0xFFFF;
    }

    /* === Methods === */
    void Timer::set_channel(const uint8_t channel, const Mode mode)
    {
        // Get the bits for the mode
        uint32_t mode_bits = 0;
        switch (mode)
        {
        case Mode::Normal:
            break;
        case Mode::PWM:
            mode_bits = 0b110;
            break;
        default:
            break;
        }

        // Get the pointer to the correct register
        auto* CCMRx = channel > 2 ? &this->thisTimer->CCMR2 : &this->thisTimer->CCMR1;

        // Update the mode bits
        uint8_t bit_pos = (((channel - 1) % 2) * 8);
        bit_pos += 4;
        *CCMRx = bits::set(*CCMRx, {0xff, bit_pos, mode_bits});
    }

    void Timer::set_tick_frequency(const OTOS::hertz frequency)
    {
        // Compute the prescaler value
        const uint32_t f_cpu = detail::get_timer_clock_frequency(this->thisInstance);
        const uint32_t prescaler = (f_cpu / frequency.count()) - 1;

        // Set the prescaler value
        this->thisTimer->PSC = prescaler;

        // Save the actual tick frequency
        this->f_tick = f_cpu / (prescaler + 1);
    }

    void Timer::set_top_value(const uint32_t top_value)
    {
        // Set the top value
        this->thisTimer->ARR = top_value;
    }

    auto Timer::get_channel(const uint8_t channel) -> Channel
    {
        // Switch according to the channel
        switch (channel)
        {
        case 1:
            return {channel, this, this->thisTimer->CCR1};
        case 2:
            return {channel, this, this->thisTimer->CCR2};
        case 3:
            return {channel, this, this->thisTimer->CCR3};
        default:
            return {channel, this, this->thisTimer->CCR4};
        }
    }

    auto Timer::get_count() const -> uint32_t
    {
        return this->thisTimer->CNT;
    }

    auto Timer::is_running() const -> bool
    {
        return (thisTimer->CR1 & TIM_CR1_CEN) > 0;
    }

    void Timer::enable_channel(const uint8_t channel)
    {
        this->thisTimer->CCER |= (1 << ((channel - 1)*4));
    }

    void Timer::disable_channel(const uint8_t channel)
    {
        this->thisTimer->CCER &= ~(1 << ((channel - 1)*4));
    }

    void Timer::start()
    {
        // Set the CEN bit
        this->thisTimer->CR1 |= TIM_CR1_CEN;
    }

    void Timer::stop()
    {
        // Unset the CEN bit
        this->thisTimer->CR1 &= ~TIM_CR1_CEN;
    }
}; // namespace Timer