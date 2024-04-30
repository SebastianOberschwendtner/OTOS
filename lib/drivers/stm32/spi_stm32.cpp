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
 * @file    spi_stm32.cpp
 * @author  SO
 * @version v5.0.0
 * @date    22-Dezember-2021
 * @brief   SPI driver for STM32 microcontrollers.
 ==============================================================================
 */

/* === Includes === */
#include "spi_stm32.h"
using stm32::Peripheral;

/* Provide template instantiations with all allowed bus instances */
template auto spi::Controller::create<Peripheral::SPI_1>(uint32_t baudrate) -> Controller;
template auto spi::Controller::create<Peripheral::SPI_2>(uint32_t baudrate) -> Controller;
#ifdef STM32F4
template auto spi::Controller::create<Peripheral::SPI_3>(uint32_t baudrate) -> Controller;
#ifdef STM32F429xx
template auto spi::Controller::create<Peripheral::SPI_4>(uint32_t baudrate) -> Controller;
template auto spi::Controller::create<Peripheral::SPI_5>(uint32_t baudrate) -> Controller;
template auto spi::Controller::create<Peripheral::SPI_6>(uint32_t baudrate) -> Controller;
#endif // STM32F429xx
#endif // STM32F4

/* === Helper functions === */
namespace spi
{
    /**
     * @brief Get the peripheral address
     *
     * @tparam instance SPI IO peripheral
     * @return constexpr uint32_t Address of the SPI peripheral
     */
    template <Peripheral instance>
    constexpr auto get_peripheral_address() -> std::uintptr_t
    {

        /* Return the peripheral address */
        if constexpr (instance == Peripheral::SPI_1)
            return SPI1_BASE;
        if constexpr (instance == Peripheral::SPI_2)
            return SPI2_BASE;
#ifdef STM32F4
        if constexpr (instance == Peripheral::SPI_3)
            return SPI3_BASE;
#ifdef STM32F429xx
        if constexpr (instance == Peripheral::SPI_4)
            return SPI4_BASE;
        if constexpr (instance == Peripheral::SPI_5)
            return SPI5_BASE;
        if constexpr (instance == Peripheral::SPI_6)
            return SPI6_BASE;
#endif // STM32F429xx
#endif // STM32F4
    };

    /**
     * @brief Enable the corresponding clock in the RCC register.
     *
     * @tparam instance SPI peripheral instance.
     */
    template <Peripheral instance>
    void enable_rcc_clock()
    {
        if constexpr (instance == Peripheral::SPI_1)
            RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
        if constexpr (instance == Peripheral::SPI_2)
            RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
#ifdef STM32F4
        if constexpr (instance == Peripheral::SPI_3)
            RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;
#ifdef STM32F429xx
        if constexpr (instance == Peripheral::SPI_4)
            RCC->APB2ENR |= RCC_APB2ENR_SPI4EN;
        if constexpr (instance == Peripheral::SPI_5)
            RCC->APB2ENR |= RCC_APB2ENR_SPI5EN;
        if constexpr (instance == Peripheral::SPI_6)
            RCC->APB2ENR |= RCC_APB2ENR_SPI6EN;
#endif // STM32F429xx
#endif // STM32F4
    }

    /**
     * @brief Get the prescaler for teh desired baudrate.
     * The prescaler value depends on the APB clock which
     * is connected to the SPI peripheral.
     *
     * @tparam instance SPI peripheral instance.
     * @param baudrate The desired baudrate of the bus.
     * @return Returns the 3 significant bits for the prescaler.
     */
    template <Peripheral instance>
    constexpr auto calculate_prescaler(const uint32_t baudrate) -> uint8_t
    {
        /* Variable for calculation */
        uint32_t prescaler = 0;

        /* Get the prescaler for the according peripheral */
        if constexpr (instance == Peripheral::SPI_1)
            prescaler = (F_APB2 / baudrate);
        if constexpr (instance == Peripheral::SPI_2)
            prescaler = (F_APB1 / baudrate);
#ifdef STM32F4
        if constexpr (instance == Peripheral::SPI_3)
            prescaler = (F_APB1 / baudrate);
        if constexpr (instance == Peripheral::SPI_4)
            prescaler = (F_APB2 / baudrate);
        if constexpr (instance == Peripheral::SPI_5)
            prescaler = (F_APB2 / baudrate);
        if constexpr (instance == Peripheral::SPI_6)
            prescaler = (F_APB2 / baudrate);
#endif // STM32F4
       /* Compute the exponent: prescaler = 2^(N+1) */
       /* See how many times 2 fits in prescaler */
        if (prescaler <= (1 << 1))
            return 0;
        else if (prescaler <= (1 << 2))
            return 1;
        else if (prescaler <= (1 << 3))
            return 2;
        else if (prescaler <= (1 << 4))
            return 3;
        else if (prescaler <= (1 << 5))
            return 4;
        else if (prescaler <= (1 << 6))
            return 5;
        else if (prescaler <= (1 << 7))
            return 6;
        else
            return 7;
    }

    /* === Factories === */
    template <Peripheral spi_instance>
    auto Controller::create(uint32_t baudrate) -> Controller
    {
        /* Get the peripheral address */
        std::uintptr_t spi_address = get_peripheral_address<spi_instance>();

        /* Calculate prescaler (3 significant bits) for the desired baudrate */
        uint8_t prescaler = calculate_prescaler<spi_instance>(baudrate);

        /* Enable the peripheral clock */
        enable_rcc_clock<spi_instance>();

        /* Create the controller object */
        return Controller(spi_instance, spi_address, prescaler);
    };

    /* === Constructors === */
    Controller::Controller(const Peripheral spi_instance, const std::uintptr_t spi_address, const uint8_t prescaler)
        : Base{spi_instance}, peripheral{reinterpret_cast<SPI_TypeDef *>(spi_address)}
    {
        /* Set the configuration register */
        this->peripheral->CR1 &= ~(SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0); /* Clear BR bits */
        this->peripheral->CR1 |= (prescaler << 3) | SPI_CR1_MSTR;

        /* Set the timeout limit */
        this->set_timeout(100);
    };

    /* === Methods === */
    auto Controller::set_clock_timing(const Level idle, const Edge data_valid) -> Controller &
    {
        /* Read old register value and delete the CPOL and CPHA bits */
        uint32_t reg = this->peripheral->CR1 & ~(SPI_CR1_CPHA | SPI_CR1_CPOL);

        /* Set the new bits */
        if (idle == Level::High)
            reg |= SPI_CR1_CPOL;
        if ((idle == Level::High) xor (data_valid == Edge::Falling))
            reg |= SPI_CR1_CPHA;

        /* Assign new register content */
        this->peripheral->CR1 = reg;

        /* Return the reference to the controller object */
        return *this;
    };

    auto Controller::set_data_to_16bit() -> Controller &
    {
        /* Disable peripheral and wait until it is disabled */
        this->disable();

        /* Wait for disabling */
        this->reset_timeout();
        while (this->peripheral->CR1 & SPI_CR1_SPE)
        {
            /* Check for timeouts */
            if (this->timed_out())
            {
                /* Peripheral timed out -> set error */
                this->set_error(error::Code::SPI_Timeout);
                return *this;
            }
        }

        /* Set the bit in the CR1 register */
        this->peripheral->CR1 |= SPI_CR1_DFF;

        /* Enable peripheral again */
        this->enable();

        /* Return the reference to the controller object */
        return *this;
    };

    auto Controller::set_use_hardware_chip_select(const bool use_hardware) -> Controller &
    {
        /* Enable hardware mode */
        this->peripheral->CR1 &= ~(SPI_CR1_SSM | SPI_CR1_SSI);
        this->peripheral->CR2 |= SPI_CR2_SSOE;

        /* Enable software mode */
        if (not use_hardware)
        {
            this->peripheral->CR1 |= (SPI_CR1_SSM | SPI_CR1_SSI);
            this->peripheral->CR2 &= ~SPI_CR2_SSOE;
        }

        /* Return the reference to the controller object */
        return *this;
    };

    auto Controller::last_transmit_finished() const -> bool
    {
        return this->peripheral->SR & SPI_SR_TXE;
    };

    auto Controller::is_busy() const -> bool
    {
        return this->peripheral->SR & SPI_SR_BSY;
    };

    auto Controller::get_rx_data() const -> bus::Data_t
    {
        return this->rx_data;
    };

    auto Controller::RX_data_valid() const -> bool
    {
        return this->peripheral->SR & SPI_SR_RXNE;
    };

    void Controller::enable()
    {
        this->peripheral->CR1 |= SPI_CR1_SPE;
    };

    void Controller::disable()
    {
        this->peripheral->CR1 &= ~SPI_CR1_SPE;
    };

    auto Controller::send_data_byte(const uint8_t data) -> bool
    {

        /* Wait until TX buffer is empty */
        this->reset_timeout();
        while (not this->last_transmit_finished())
        {
            /* Check for timeouts */
            if (this->timed_out())
            {
                /* Peripheral timed out -> set error */
                this->set_error(error::Code::SPI_Timeout);
                return false;
            }
        }
        /* Send data when DR is empty */
        this->peripheral->DR = data;

        /* return success */
        return true;
    };

    auto Controller::read_data_byte() -> std::optional<uint8_t>
    {
        /* Empty the RX buffer if its not empty */
        uint8_t rx = 0;
        if (this->RX_data_valid())
            rx = this->peripheral->DR;

        /* Wait until TX buffer is empty */
        this->reset_timeout();
        while (not this->last_transmit_finished())
        {
            /* Check for timeouts */
            if (this->timed_out())
            {
                /* Peripheral timed out -> set error */
                this->set_error(error::Code::SPI_Timeout);
                return {};
            }
        }

        /* Initiate receive by sending dummy data => SPI is in Full Duplex mode */
        this->peripheral->DR = 0x00;

        /* Wait until RX buffer contains data */
        this->reset_timeout();
        while (not this->RX_data_valid())
        {
            /* Check for timeouts */
            if (this->timed_out())
            {
                /* Peripheral timed out -> set error */
                this->set_error(error::Code::SPI_Timeout);
                return {};
            }
        }

        /* read the byte */
        rx = this->peripheral->DR;
        return rx;
    };

    auto Controller::send_data(const bus::Data_t payload,
                               const uint8_t n_bytes) -> bool
    {
        /* only when bus is not busy! */
        if (this->is_busy())
        {
            this->set_error(error::Code::SPI_BUS_Busy_Error);
            return false;
        }

        /* send the bytes */
        for (uint32_t i = n_bytes; i > 0; i--)
        {
            if (!this->send_data_byte(payload.byte[i - 1]))
                return false;
        }

        /* Wait for transfer to finish */
        this->reset_timeout();
        while (this->is_busy())
        {
            if (this->timed_out())
            {
                this->set_error(error::Code::SPI_Timeout);
                return false;
            }
        }

        /* Send was successful */
        return true;
    };

    auto Controller::send_array(const uint8_t *data,
                                const uint8_t n_bytes) -> bool
    {
        /* only when bus is not busy! */
        if (this->is_busy())
        {
            this->set_error(error::Code::SPI_BUS_Busy_Error);
            return false;
        }

        /* send the bytes */
        for (uint32_t i = 0; i < n_bytes; i++)
        {
            if (!this->send_data_byte(*(data + i)))
                return false;
        }

        /* Wait for transfer to finish */
        this->reset_timeout();
        while (this->is_busy())
        {
            if (this->timed_out())
            {
                this->set_error(error::Code::SPI_Timeout);
                return false;
            }
        }

        /* Send was successful */
        return true;
    };

    auto Controller::read_data(const uint8_t reg,
                               const uint8_t n_bytes) -> bool
    {
        /* Variable to store the received data */
        std::optional<uint8_t> rx;

        /* only when bus is not busy! */
        if (this->is_busy())
        {
            this->set_error(error::Code::SPI_BUS_Busy_Error);
            return false;
        }

        /* send the register to read from */
        if (!this->send_data_byte(reg))
            return false;

        /* Start reading the data */
        for (uint32_t iByte = n_bytes; iByte > 0; iByte--)
        {
            /* Read the byte and check whether it is valid */
            rx = this->read_data_byte();
            if (!rx)
                return false;
            /* data is valid */
            this->rx_data.byte[iByte - 1] = rx.value();
        }

        /* Wait for transfer to finish */
        this->reset_timeout();
        while (this->is_busy())
        {
            if (this->timed_out())
            {
                this->set_error(error::Code::SPI_Timeout);
                return false;
            }
        }

        /* Read was successful */
        return true;
    };

    auto Controller::read_array(
        uint8_t *const data,
        const uint8_t n_bytes) -> bool
    {
        /* only when bus is not busy! */
        if (this->is_busy())
        {
            this->set_error(error::Code::SPI_BUS_Busy_Error);
            return false;
        }

        /* Receive the bytes */
        for (uint32_t i = 0; i < n_bytes; i++)
        {
            /* Check for valid data */
            auto rx = this->read_data_byte();
            if (not rx)
                return false;
            /* Store the data */
            *(data + i) = rx.value();
        }

        /* Wait for transfer to finish */
        this->reset_timeout();
        while (this->is_busy())
        {
            if (this->timed_out())
            {
                this->set_error(error::Code::SPI_Timeout);
                return false;
            }
        }

        /* Read was successful */
        return true;
    };
}; // namespace spi
