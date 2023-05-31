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

// === Declarations ===
namespace Timer
{
    // === Enums ===
    enum class Mode
    {
        Normal,
        PWM
    };

    // === Classes ===
    class Timer
    {
      private:
        // properties
        volatile TIM_TypeDef *thisTimer;
        const IO thisInstance;
        OTOS::hertz f_tick{F_CPU};

        /**
         * @brief Class representing a timer channel.
         * The class keeps are reference to the timer and sets its
         * capture/compare channel related properties.
         */
        class Channel
        {
          private:
            Timer *const theTimer;
            const uint8_t thisChannel{0};
            volatile uint32_t &compareValue;

          public:
            // === Constructor ===
            Channel() = delete;
            Channel(const uint8_t channel, Timer *const timer, volatile uint32_t &CompareRegister)
                : theTimer(timer), thisChannel(channel), compareValue(CompareRegister)
            {
            }

            // === Methods ===
            void enable() { this->theTimer->enable_channel(this->thisChannel); }
            void disable() { this->theTimer->disable_channel(this->thisChannel); }
            void set_mode(Mode mode) { this->theTimer->set_channel(this->thisChannel, mode); }
            void set_compare_value(const uint32_t value) { this->compareValue = value; }
            template <typename rep, typename period>
            void set_pulse_width(const std::chrono::duration<rep, period> duration)
            {
                // calculate the compare value and match the ratios
                rep calc_value = duration.count() * this->theTimer->f_tick.count();
                calc_value *= period::num;
                calc_value /= period::den;
                this->compareValue = calc_value;
            }
            void set_duty_cycle(const float percentage)
            {
                // check if percentage is in range
                if (percentage > 1.0f || percentage < 0.0f)
                    return;
                this->compareValue = static_cast<uint32_t>(percentage * this->theTimer->thisTimer->ARR);
            }
        };

      public:
        // === Constructor ===
        Timer() = delete;
        Timer(const IO timer);

        // === Methods ===
        [[nodiscard]] auto is_running() const -> bool;
        [[nodiscard]] auto get_count() const -> uint32_t;
        [[nodiscard]] auto get_channel(uint8_t channel) -> Channel;
        void start();
        void stop();
        void set_frequency(OTOS::hertz frequency);
        void set_top_value(uint32_t top_value);
        void set_channel(uint8_t channel, Mode mode);
        void enable_channel(uint8_t channel);
        void disable_channel(uint8_t channel);

        /**
         * @brief Set the duration of the timer until overflow/underflow.
         *
         * @tparam rep The representation type of the duration.
         * @tparam period The ratio defining the period of the duration.
         * @param duration The duration value of the timer until it overflows/underflows.
         */
        template <typename rep, typename period>
        void set_duration(std::chrono::duration<rep, period> duration)
        {
            // calculate the top value
            rep top_value = duration.count() * f_tick.count();
            top_value /= period::den;
            top_value *= period::num;

            // set the top value
            this->set_top_value(static_cast<uint32_t>(top_value));
        }
    };

    // === Functions ===
    void SysTick_Configure();
};

#endif