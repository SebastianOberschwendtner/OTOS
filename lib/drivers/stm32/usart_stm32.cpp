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
 * @file    usart_stm32.cpp
 * @author  SO
 * @version v5.0.0
 * @date    23-Dezember-2021
 * @brief   USART driver for STM32 microcontrollers.
 ==============================================================================
 */

/* === Includes === */
#include "usart_stm32.h"
using stm32::Peripheral;

/* Provide template instantiations with all allowed bus instances */
template auto usart::Controller::create<Peripheral::USART_1>(const uint32_t, uint8_t, StopBits) -> usart::Controller;
template auto usart::Controller::create<Peripheral::USART_2>(const uint32_t, uint8_t, StopBits) -> usart::Controller;
#ifndef STM32L053xx
template auto usart::Controller::create<Peripheral::USART_4>(const uint32_t, uint8_t, StopBits) -> usart::Controller;
template auto usart::Controller::create<Peripheral::USART_5>(const uint32_t, uint8_t, StopBits) -> usart::Controller;
#endif // STM32L053xx
#ifdef STM32F4
template auto usart::Controller::create<Peripheral::USART_3>(const uint32_t, uint8_t, StopBits) -> usart::Controller;
template auto usart::Controller::create<Peripheral::USART_6>(const uint32_t, uint8_t, StopBits) -> usart::Controller;
#ifdef STM32F429xx
template auto usart::Controller::create<Peripheral::USART_7>(const uint32_t, uint8_t, StopBits) -> usart::Controller;
template auto usart::Controller::create<Peripheral::USART_8>(const uint32_t, uint8_t, StopBits) -> usart::Controller;
#endif // STM32F429xx
#endif // STM32F4

/* === Helper functions === */
namespace detail
{
    /**
     * @brief Get the peripheral address
     *
     * @tparam instance USART IO peripheral
     * @return constexpr uint32_t Address of the SPI peripheral
     */
    template <Peripheral usart>
    constexpr auto get_peripheral_address() -> std::uintptr_t
    {
        /* Return the peripheral address */
        if constexpr (usart == Peripheral::USART_1)
            return USART1_BASE;
        if constexpr (usart == Peripheral::USART_2)
            return USART2_BASE;
#if defined(STM32L0) && !defined(STM32L053xx)
        if constexpr (usart == Peripheral::USART_4)
            return USART4_BASE;
        if constexpr (usart == Peripheral::USART_5)
            return USART5_BASE;
#endif // STM32L0
#ifdef STM32F4
        if constexpr (usart == Peripheral::USART_3)
            return USART3_BASE;
        if constexpr (usart == Peripheral::USART_4)
            return UART4_BASE;
        if constexpr (usart == Peripheral::USART_5)
            return UART5_BASE;
        if constexpr (usart == Peripheral::USART_6)
            return USART6_BASE;
#ifdef STM32F429xx
        if constexpr (usart == Peripheral::USART_7)
            return UART7_BASE;
        if constexpr (usart == Peripheral::USART_8)
            return UART8_BASE;
#endif // STM32F429xx
#endif // STM32F4
    };

    /**
     * @brief Enable the corresponding clock in the RCC register.
     *
     * @tparam instance USART IO peripheral instance.
     */
    template <Peripheral usart>
    void enable_rcc_clock()
    {
        if constexpr (usart == Peripheral::USART_1)
            RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
        if constexpr (usart == Peripheral::USART_2)
            RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
#if defined(STM32L0) && !defined(STM32L053xx)
        if constexpr (usart == Peripheral::USART_4)
            RCC->APB1ENR |= RCC_APB1ENR_USART4EN;
        if constexpr (usart == Peripheral::USART_5)
            RCC->APB1ENR |= RCC_APB1ENR_USART5EN;
#endif // STM32L0
#ifdef STM32F4
        if constexpr (usart == Peripheral::USART_3)
            RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
        if constexpr (usart == Peripheral::USART_4)
            RCC->APB1ENR |= RCC_APB1ENR_UART4EN;
        if constexpr (usart == Peripheral::USART_5)
            RCC->APB1ENR |= RCC_APB1ENR_UART5EN;
        if constexpr (usart == Peripheral::USART_6)
            RCC->APB2ENR |= RCC_APB2ENR_USART6EN;
#ifdef STM32F429xx
        if constexpr (usart == Peripheral::USART_7)
            RCC->APB1ENR |= RCC_APB1ENR_UART7EN;
        if constexpr (usart == Peripheral::USART_8)
            RCC->APB1ENR |= RCC_APB1ENR_UART8EN;
#endif // STM32F429xx
#endif // STM32F4
    };

    template <Peripheral instance>
    constexpr auto calculate_prescaler(const uint32_t baudrate) -> uint16_t
    {
        /* Calculation variables */
        uint32_t prescaler = 0;
        uint16_t mantissa = 0;
        uint8_t fraction = 0;
        constexpr uint32_t scaling = 100;

        /* Scale baudrate to get significant digits for
         * the integer prescaler */
        const uint32_t baudrate_scaled = baudrate / scaling;

        /* Get the prescaler according to the used clock */
        if constexpr (instance == Peripheral::USART_1)
            prescaler = F_APB2 / baudrate_scaled;
        if constexpr (instance == Peripheral::USART_2)
            prescaler = F_APB1 / baudrate_scaled;
        if constexpr (instance == Peripheral::USART_3)
            prescaler = F_APB1 / baudrate_scaled;
        if constexpr (instance == Peripheral::USART_4)
            prescaler = F_APB1 / baudrate_scaled;
        if constexpr (instance == Peripheral::USART_5)
            prescaler = F_APB1 / baudrate_scaled;
        if constexpr (instance == Peripheral::USART_6)
            prescaler = F_APB2 / baudrate_scaled;
        if constexpr (instance == Peripheral::USART_7)
            prescaler = F_APB1 / baudrate_scaled;
        if constexpr (instance == Peripheral::USART_8)
            prescaler = F_APB1 / baudrate_scaled;

        /* Convert the integer value to mantissa */
        prescaler /= 8 * (2 - 0);
        mantissa = static_cast<uint16_t>(prescaler / scaling);

        /* Get the fraction of the prescaler to calculate the fraction part */
        prescaler -= static_cast<uint16_t>(mantissa) * scaling;
        prescaler *= 16;
        fraction = static_cast<uint8_t>(prescaler / scaling);

        return static_cast<uint16_t>((mantissa << 4) | (fraction & 0b1111));
    };
}; // namespace detail

namespace usart
{
    /* === Factory === */
    template <Peripheral usart>
    Controller Controller::create(
        const uint32_t baudrate,
        const uint8_t data_bits,
        const StopBits stop_bits)
    {
        /* Enable the peripheral clock */
        detail::enable_rcc_clock<usart>();

        /* Calculate the prescaler */
        auto prescaler = detail::calculate_prescaler<usart>(baudrate);

        /* Get the peripheral address */
        auto address = detail::get_peripheral_address<usart>();

        /* Create the controller */
        return Controller(usart, address, prescaler, data_bits, stop_bits);
    };

    /* === Constructors === */
    Controller::Controller(
        const Peripheral usart_id,
        const std::uintptr_t usart_address,
        const uint16_t prescaler,
        const uint8_t data_bits,
        const StopBits stop_bits)
        : Base{usart_id}, peripheral{reinterpret_cast<USART_TypeDef *>(usart_address)}
    {

        /* Set the configuration register */
        this->peripheral->CR1 |= USART_CR1_TE | USART_CR1_RE;
        this->peripheral->CR2 |= static_cast<uint32_t>(stop_bits) << USART_CR2_STOP_Pos;

        /* Set the data format */
        if (data_bits > 8)
            this->peripheral->CR1 |= USART_CR1_M;

        /* Set prescaler */
        this->peripheral->BRR = prescaler;

        /* Set the timeout limit */
        this->set_timeout(250);
    };

    /* === Methods === */
    auto Controller::last_transmit_finished() const -> bool
    {
#if defined(STM32F4)
        return this->peripheral->SR & USART_SR_TXE;
#elif defined(STM32L0)
        return not(this->peripheral->ISR & USART_ISR_TXE);
#endif
    };

    auto Controller::is_busy() const -> bool
    {
#if defined(STM32F4)
        return not(this->peripheral->SR & USART_SR_TC);
#elif defined(STM32L0)
        return not(this->peripheral->ISR & USART_ISR_TC);
#endif
    };

    void Controller::enable()
    {
        this->peripheral->CR1 |= USART_CR1_UE;
    };

    void Controller::disable()
    {
        this->peripheral->CR1 &= ~USART_CR1_UE;
    };

    auto Controller::send_data_byte(const uint8_t data) -> bool
    {
        /* Wait for the TX register to become empty */
        this->reset_timeout();
        while (not this->last_transmit_finished())
        {
            /* Catch timeouts */
            if (this->timed_out())
            {
                this->set_error(Error::Code::USART_Timeout);
                return false;
            }
        }
#if defined(STM32F4)
        this->peripheral->DR = data;
#elif defined(STM32L0)
        this->peripheral->TDR = data;
#endif
        return true;
    }

    auto Controller::send_data(
        const Bus::Data_t payload,
        const uint8_t n_bytes) -> bool
    {
        /* Only when bus is not busy */
        if (this->is_busy())
        {
            this->set_error(Error::Code::USART_BUS_Busy_Error);
            return false;
        }

        /* Transfer bytes */
        for (uint8_t i = n_bytes; i > 0; i--)
        {
            if (not this->send_data_byte(payload.byte[i - 1]))
                return false;
        }

        /* Transfer successful */
        return true;
    };

    auto Controller::send_array(const uint8_t *data,
                                const uint8_t n_bytes) -> bool
    {
        /* Only when bus is not busy */
        if (this->is_busy())
        {
            this->set_error(Error::Code::USART_BUS_Busy_Error);
            return false;
        }

        /* Send the bytes */
        for (uint32_t i = 0; i < n_bytes; i++)
        {
            if (!this->send_data_byte(*(data + i)))
                return false;
        }
        return true;
    };

}; // namespace usart