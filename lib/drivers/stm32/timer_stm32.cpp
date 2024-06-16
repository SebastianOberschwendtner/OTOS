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
 * @version v5.1.0
 * @date    31-October-2021
 * @brief   Timer driver for STM32 microcontrollers.
 ==============================================================================
 */

/* === Includes === */
#include "timer_stm32.h"
#include <bitset>

/* === Defines === */
using stm32::Peripheral;

/* === Valid Timer peripheral instantiations === */
template auto timer::Timer::create<Peripheral::TIM_1>() -> timer::Timer;
template auto timer::Timer::create<Peripheral::TIM_2>() -> timer::Timer;
template auto timer::Timer::create<Peripheral::TIM_3>() -> timer::Timer;

namespace detail
{
    /**
     * @brief Get the timer base address depending on the timer instance.
     * @tparam timer The timer instance which is used.
     * @return Return the peripheral base address of the timer instance.
     */
    template <Peripheral timer>
    constexpr auto get_timer_address() -> std::uintptr_t
    {
        switch (timer)
        {
            case Peripheral::TIM_2:
                return TIM2_BASE;
#ifndef STM32L053xx
            case Peripheral::TIM_3:
                return TIM3_BASE;
#endif
#ifdef STM32F4
            case Peripheral::TIM_1:
                return TIM1_BASE;
            case Peripheral::TIM_4:
                return TIM4_BASE;
            case Peripheral::TIM_5:
                return TIM5_BASE;
#endif
            default:
                return 0;
        }
    }

    /**
     * @brief Enable the clock for the selected timer instance.
     * @tparam timer The timer instance which is used.
     */
    template <Peripheral timer>
    constexpr void enable_timer_clock()
    {
        /* Enable the clock */
        switch (timer)
        {
            case Peripheral::TIM_2:
                RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
                break;
#ifndef STM32L053xx
            case Peripheral::TIM_3:
                RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
                break;
#endif
#ifdef STM32F4
            case Peripheral::TIM_1:
                RCC->APB2ENR |= RCC_APB2ENR_TIM1EN;
                break;
            case Peripheral::TIM_4:
                RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
                break;
            case Peripheral::TIM_5:
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
     * @tparam timer The timer instance which is used.
     * @return The base clock frequency of the timer.
     */
    template <Peripheral timer>
    constexpr auto get_timer_clock_frequency() -> uint32_t
    {
        switch (timer)
        {
#ifdef STM32F4
            case Peripheral::TIM_1:
                return F_APB2;
            case Peripheral::TIM_4:
                return F_APB1;
            case Peripheral::TIM_5:
                return F_APB1;
#endif
            case Peripheral::TIM_2:
                return F_APB1;
#ifndef STM32L053xx
            case Peripheral::TIM_3:
                return F_APB1;
#endif
            default:
                return F_CPU;
        }
    }
}; // namespace detail

namespace timer
{
    /* === Functions === */
    void SysTick_Configure()
    {
        /* Compute the ticks per ms */
        constexpr uint32_t ticks_ms = (F_CPU / 1000);

        /* Configure the SysTick timer */
        SysTick_Config(ticks_ms);

        /* Reconfigure Priorities so that SysTick has the highest priority */
#if defined(STM32L0)
        NVIC_SetPriority(SVC_IRQn, 1);
#elif defined(STM32F4)
        NVIC_SetPriority(SVCall_IRQn, 1);
#endif
        NVIC_SetPriority(SysTick_IRQn, 0);
    }

    /* === Factory === */
    template <Peripheral timer>
    auto Timer::create() -> Timer
    {
        /* Enable the peripheral clock */
        detail::enable_timer_clock<timer>();

        /* Get the peripheral address */
        auto address = detail::get_timer_address<timer>();

        /* Get the APB frequency of the timer */
        const uint32_t f_apb = detail::get_timer_clock_frequency<timer>();

        /* Return the timer instance */
        return Timer(address, timer, f_apb);
    }

    /* === Constructors === */
    Timer::Timer(
        const std::uintptr_t timer_address,
        const Peripheral timer,
        const uint32_t f_apb)
        : timer{reinterpret_cast<volatile TIM_TypeDef *>(timer_address)}, instance{timer}, f_base{f_apb}
    {
        // Set the top value to the maximum
        this->timer->ARR = 0xFFFF;
    }

    /* === Methods === */
    auto Timer::set_channel(const uint8_t channel, const Mode mode) -> Timer &
    {
        /* Get the bits for the mode */
        uint16_t mode_bits{0};
        switch (mode)
        {
            case Mode::Capture:
                mode_bits = TIM_CCMR1_CC1S_0;
                break;
            case Mode::PWM:
                mode_bits = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;
                break;
            default:
                break;
        }

        /* Get the pointer to the correct register */
        auto *CCMRx = channel > 2 ? &this->timer->CCMR2 : &this->timer->CCMR1;

        /* Update the mode bits */
        uint8_t bit_pos = (((channel - 1) % 2) * 8);
        *CCMRx = bits::set(*CCMRx, {0xff, bit_pos, mode_bits});

        /* Return the timer reference */
        return *this;
    }

    auto Timer::set_tick_frequency(const OTOS::hertz frequency) -> Timer &
    {
        /* Compute the prescaler value */
        const uint32_t prescaler = (this->f_base / frequency.count()) - 1;

        /* Set the prescaler value */
        this->timer->PSC = prescaler;

        /* Save the actual tick frequency */
        this->f_tick = this->f_base / (prescaler + 1);

        /* Return the timer reference */
        return *this;
    }

    auto Timer::set_top_value(const uint32_t top_value) -> Timer &
    {
        /* Set the top value */
        this->timer->ARR = top_value;

        /* Return the timer reference */
        return *this;
    }

    auto Timer::get_channel(const uint8_t channel) -> Channel
    {
        /* Switch according to the channel */
        switch (channel)
        {
            case 1:
                return {channel, this, this->timer->CCR1};
            case 2:
                return {channel, this, this->timer->CCR2};
            case 3:
                return {channel, this, this->timer->CCR3};
            default:
                return {channel, this, this->timer->CCR4};
        }
    }

    auto Timer::get_count() const volatile -> uint32_t
    {
        return this->timer->CNT;
    }

    auto Timer::get_status() const volatile -> status::Flags
    {
        return static_cast<status::Flags>(this->timer->SR);
    }

    auto Timer::is_running() const -> bool
    {
        return (timer->CR1 & TIM_CR1_CEN) > 0;
    }

    auto Timer::clear_status(const status::Flags status) -> Timer &
    {
        /* Clear the status flags */
        this->timer->SR &= ~static_cast<uint32_t>(status);

        /* Return the timer reference */
        return *this;
    }

    void Timer::enable_channel(const uint8_t channel)
    {
        this->timer->CCER |= (1 << ((channel - 1) * 4));
    }

    auto Timer::enable_interrupt(const interrupt::Flags interrupt) -> Timer &
    {
        /* Enable the interrupt in the timer */
        this->timer->DIER |= static_cast<uint32_t>(interrupt);

        /* Enable the interrupt in the NVIC */
        switch (this->instance)
        {
            case Peripheral::TIM_2:
                NVIC_EnableIRQ(TIM2_IRQn);
                break;
#ifndef STM32L053xx
            case Peripheral::TIM_3:
                NVIC_EnableIRQ(TIM3_IRQn);
                break;
#endif // STM32L053xx
#ifdef STM32F4
            case Peripheral::TIM_1:
                if (0 != (interrupt & interrupt::Update))
                    NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
                if (0 != (interrupt &
                          (interrupt::Channel1 | interrupt::Channel2 | interrupt::Channel3 | interrupt::Channel4)))
                    NVIC_EnableIRQ(TIM1_CC_IRQn);
                break;
#endif // STM32F4
            default:
                break;
        }

        /* Return the timer reference */
        return *this;
    }

    void Timer::disable_channel(const uint8_t channel)
    {
        this->timer->CCER &= ~(1 << ((channel - 1) * 4));
    }

    auto Timer::disable_interrupt(const interrupt::Flags interrupt) -> Timer &
    {
        /* Disable the interrupt in the timer */
        this->timer->DIER &= ~static_cast<uint32_t>(interrupt);

        /* Get the still enabled interrupts */
        interrupt::Flags enabled_interrupts = static_cast<interrupt::Flags>(this->timer->DIER & 0x0F);

        /* Disable the interrupt in the NVIC */
#ifdef STM32F4
        /* TIM1 has a different IRQn assignment */
        if (this->instance == Peripheral::TIM_1)
        {
            /* Check which interrupts can be disabled */
            if (0 != (interrupt & interrupt::Update) && 0 == (enabled_interrupts & interrupt::Update))
                NVIC_DisableIRQ(TIM1_UP_TIM10_IRQn);
            if (0 != (interrupt &
                      (interrupt::Channel1 | interrupt::Channel2 | interrupt::Channel3 | interrupt::Channel4)) &&
                0 == (enabled_interrupts & (interrupt::Channel1 | interrupt::Channel2 | interrupt::Channel3 | interrupt::Channel4)))
                NVIC_DisableIRQ(TIM1_CC_IRQn);

            /* Return the timer reference */
            return *this;
        }
#endif // STM32F4

        /* Check wether all interrupts are disabled */
        if (0 == enabled_interrupts)
        {
            switch (this->instance)
            {
                case Peripheral::TIM_2:
                    NVIC_DisableIRQ(TIM2_IRQn);
                    break;
#ifndef STM32L053xx
                case Peripheral::TIM_3:
                    NVIC_DisableIRQ(TIM3_IRQn);
                    break;
#endif // STM32L053xx
                default:
                    break;
            }
        }

        /* Return the timer reference */
        return *this;
    }

    void Timer::start()
    {
        /* Set the CEN bit */
        this->timer->CR1 |= TIM_CR1_CEN;
    }

    void Timer::stop()
    {
        /* Unset the CEN bit */
        this->timer->CR1 &= ~TIM_CR1_CEN;
    }

    /* === Channel === */
    auto Timer::Channel::set_compare_value(const uint32_t value) -> Channel &
    {
        this->compare_value = value;

        // Return the channel reference
        return *this;
    }

    auto Timer::Channel::set_duty_cycle(const float percentage) -> Channel &
    {
        // check if percentage is in range
        if (percentage > 1.0f || percentage < 0.0f)
            return *this;
        this->compare_value = static_cast<uint32_t>(percentage * this->theTimer->timer->ARR);

        // Return the channel reference
        return *this;
    }

    auto Timer::Channel::set_mode(const Mode mode) -> Channel &
    {
        this->theTimer->set_channel(this->channel, mode);

        // Return the channel reference
        return *this;
    }

    auto Timer::Channel::input_capture() volatile -> std::optional<uint32_t>
    {
        /* Check whether an input value is available */
        if (this->theTimer->timer->SR & (TIM_SR_CC1IF << (this->channel - 1)))
        {
            // Clear the interrupt flag
            this->theTimer->timer->SR &= ~(TIM_SR_CC1IF << (this->channel - 1));

            // Return the captured value
            return this->theTimer->timer->CCR1;
        }

        /* No recent capture */
        return std::nullopt;
    }
}; // namespace timer