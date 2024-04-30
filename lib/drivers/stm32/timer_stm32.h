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

#ifndef TIMER_STM32_H_
#define TIMER_STM32_H_

// === Includes ===
#include "interface.h"
#include "misc/bits.h"
#include "misc/units.h"
#include "vendors.h"
#include <chrono>
#include <cstdint>
#include <memory>
#include <ratio>

/* === Get the available peripherals === */
#include "peripherals_stm32.h"

// === Declarations ===
namespace timer
{
    // === Enums ===
    enum class Mode
    {
        Normal,
        PWM
    };

    // === Classes ===
    /**
     * @brief Timer abstraction class for STM32 microcontrollers.
     * It provides a simple interface to configure the timer peripherals
     * and set the operating mode of the timer channels.
     */
    class Timer
    {
      private:
        class Channel; // forward declaration so that get_channel can return a Channel object.

      public:
        /* === Builder === */
        /**
         * @brief Create a new timer instance.
         * This enables the peripheral clock, initializes the timer
         * tick frequency to the default CPU frequency and sets the
         * top value of the timer to the maximum value.
         *
         * @tparam timer The timer instance to be used.
         * @return Timer The timer object.
         */
        template <stm32::Peripheral timer>
        static auto create() -> Timer;

        /* === Constructors === */
        /* No default constructor */
        Timer() = delete;
        Timer(const Timer &) = default;
        Timer(Timer &&) = default;
        auto operator=(const Timer &) -> Timer & = default;
        auto operator=(Timer &&) -> Timer & = default;

        /* === Setters === */
        /**
         * @brief Set the operating of one timer capture/compare channel.
         * @note Is is more flexible to use the get_channel method to get a channel instance
         * and configure the channel using its methods.
         *
         * @param channel The channel to be configured.
         * @param mode The mode of the channel.
         * @return Timer& Returns a reference to the timer object.
         */
        auto set_channel(uint8_t channel, Mode mode) -> Timer &;

        /**
         * @brief Set the period of the timer until overflow/underflow.
         * @note This template structure enables the use of std::chrono::duration
         * types to be used as input for the period duration.
         *
         * @tparam rep The representation type of the duration.
         * @tparam period The ratio defining the period of the duration.
         * @param duration The period value of the timer until it overflows/underflows.
         * @return Timer& Returns a reference to the timer object.
         */
        template <typename rep, typename period>
        auto set_period(std::chrono::duration<rep, period> duration) -> Timer &
        {
            // calculate the top value
            rep top_value = duration.count() * f_tick.count();
            top_value /= period::den;
            top_value *= period::num;

            // set the top value
            this->set_top_value(static_cast<uint32_t>(top_value));

            // Return the timer reference
            return *this;
        }

        /**
         * @brief Set the tick frequency of the timer.
         * @param frequency The frequency in Hz.
         * @return Timer& Returns a reference to the timer object.
         */
        auto set_tick_frequency(OTOS::hertz frequency) -> Timer &;

        /**
         * @brief Set the top tick count of the timer.
         * @param top The top value in ticks.
         * @return Timer& Returns a reference to the timer object.
         */
        auto set_top_value(uint32_t top_value) -> Timer &;

        /* === Getters === */
        /**
         * @brief Get the channel class instance of one timer capture/compare channel.
         * @attention The returned channel keeps a reference to the timer and can
         * modify the timer properties. This getter is therefore not const!
         *
         * @param channel The channel to be configured.
         */
        [[nodiscard]] auto get_channel(uint8_t channel) -> Channel;

        /**
         * @brief Get the current count of the timer
         * @return The current count of the timer in ticks.
         */
        [[nodiscard]] auto get_count() const -> uint32_t;

        /**
         * @brief Check if the timer is enabled.
         * @return True if the timer is enabled, false otherwise.
         */
        [[nodiscard]] auto is_running() const -> bool;

        /* === Methods === */
        /**
         * @brief Enable one capture/compare channel of the timer.
         * @details This should in theory take less CPU cycles than
         * getting the channel instance and enabling it.
         *
         * @param channel The channel to be enabled.
         */
        void enable_channel(uint8_t channel);

        /**
         * @brief Disable one capture/compare channel of the timer.
         * @details This should in theory take less CPU cycles than
         * getting the channel instance and disable it.
         *
         * @param channel The channel to be disabled.
         */
        void disable_channel(uint8_t channel);

        /**
         * @brief Start the timer.
         */
        void start();

        /**
         * @brief Stop the timer.
         */
        void stop();

        /* Let the GPIO assign function access the private members */
        template <class IO>
        friend void gpio::assign(IO pin, Timer &timer);

      private:
        /**
         * @brief Class representing a timer channel.
         * The class keeps are reference to the timer and sets its
         * capture/compare channel related properties.
         *
         * @details This is an abstraction of the capture and compare
         * channels of an timer. Since the channel is inherently bound to
         * the timer, it keeps a reference to the timer instance.
         * This class is not intended to be used directly, hence the private
         * declaration.
         */
        class Channel
        {
          public:
            // === Constructor ===
            /* No default constructor */
            Channel() = delete;

            /**
             * @brief Construct a new Timer Channel object
             * @details This is an abstraction of the capture and compare
             * channels of an timer. Since the channel is inherently bound to
             * the timer, it keeps a reference to the timer instance.
             * This class is not intended to be used directly, hence the private
             * declaration.
             *
             * @param channel The channel number of the channel instance.
             * @param timer Pointer to the timer instance this channel belongs to.
             * @param CompareRegister The reference to the compare register of the channel.
             */
            Channel(const uint8_t channel, Timer *const timer, volatile uint32_t &CompareRegister)
                : theTimer(timer), channel(channel), compare_value(CompareRegister)
            {
            }

            /* === Setters === */
            /**
             * @brief Set the compare value of the channel.
             * @param value The compare value of the channel.
             * @return Channel& Returns a reference to the channel object.
             */
            auto set_compare_value(const uint32_t value) -> Channel &
            {
                this->compare_value = value;

                // Return the channel reference
                return *this;
            }

            /**
             * @brief Set the duty cycle of the channel when in PWM mode.
             * @note When the duty cycle is set to a value greater than 1.0f or less than 0.0f,
             * the function will return without setting the duty cycle!
             *
             * @param percentage The duty cycle percentage of the channel.
             * @return Channel& Returns a reference to the channel object.
             */
            auto set_duty_cycle(const float percentage) -> Channel &
            {
                // check if percentage is in range
                if (percentage > 1.0f || percentage < 0.0f)
                    return *this;
                this->compare_value = static_cast<uint32_t>(percentage * this->theTimer->timer->ARR);

                // Return the channel reference
                return *this;
            }

            /**
             * @brief Set the operating mode of the channel.
             * @param mode The mode of the channel.
             * @return Channel& Returns a reference to the channel object.
             */
            auto set_mode(const Mode mode) -> Channel &
            {
                this->theTimer->set_channel(this->channel, mode);

                // Return the channel reference
                return *this;
            }

            /**
             * @brief Set the pulse width of the channel when in PWM mode.
             * @note This template structure enables the use of std::chrono::duration
             * types to be used as input for the pulse width.
             *
             * @tparam rep The representation type of the duration.
             * @tparam period The ratio defining the period of the duration.
             * @param duration The duration value of the pulse width.
             * @return Channel& Returns a reference to the channel object.
             */
            template <typename rep, typename period>
            auto set_pulse_width(const std::chrono::duration<rep, period> duration) -> Channel &
            {
                // calculate the compare value and match the ratios
                rep calc_value = duration.count() * this->theTimer->f_tick.count();
                calc_value *= period::num;
                calc_value /= period::den;
                this->compare_value = calc_value;

                // Return the channel reference
                return *this;
            }

            /* === Methods === */
            /**
             * @brief Enable the operation of the channel.
             */
            void enable() { this->theTimer->enable_channel(this->channel); }

            /**
             * @brief Disable the operation of the channel.
             */
            void disable() { this->theTimer->disable_channel(this->channel); }

          private:
            /* === Properties === */
            Timer *theTimer;                  /**< Pointer to the timer instance the channel belongs to.*/
            uint8_t channel{0};               /**< The channel number of the channel instance.*/
            volatile uint32_t &compare_value; /**< Reference to the compare register of the channel.*/
        };

        /* === Constructor === */
        /**
         * @brief Constructor for timer object.
         * @details
         * - Enables the clock for the selected timer instance.
         * - Sets the timer tick frequency to the default CPU frequency.
         * - Sets the top value of the timer to the maximum value.
         * @note This does not enable the peripheral clock. This constructor
         * is intended to be used by the builder method.
         *
         * @param timer_address The address of the timer peripheral.
         * @param timer The instance of the timer to be used.
         * @param f_apb [Hz] The frequency of the APB bus.
         */
        Timer(std::uintptr_t timer_address, stm32::Peripheral timer, uint32_t f_apb);

        /* === Properties === */
        volatile TIM_TypeDef *timer; /**< The underlying timer hardware address of the peripheral. */
        stm32::Peripheral instance;  /**< The instance of the timer. */
        uint32_t f_base{0};          /**< [Hz] The frequency of the timer base clock. */
        OTOS::hertz f_tick{F_CPU};   /**< [Hz] The tick frequency of the timer. */
    };

    // === Functions ===
    /**
     * @brief Configure the SysTick timer for interrupts every 1 ms.
     */
    void SysTick_Configure();
}; // namespace timer

namespace gpio
{
    /**
     * @brief Assign the I/O pin to the timer.
     *
     * @attention This does not check whether the alternate function is
     * actually available for the pin. It is the responsibility of the
     * user to ensure that the pin can be assigned to the timer!
     *
     * @tparam IO The class implementing the GPIO interface.
     * @param pin The pin to be assigned to the timer.
     * @param timer The timer the pin is assigned to.
     */
    template <class IO>
    void assign(IO pin, timer::Timer &timer)
    {
        pin.set_alternate_function(timer.instance);
    }
}; // namespace gpio

#endif