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
 * @file    i2c_stm32.cpp
 * @author  SO
 * @version v5.0.0
 * @date    02-September-2021
 * @brief   I2C driver for STM32 microcontrollers.
 ==============================================================================
 */

// === Includes ===
#include "i2c_stm32.h"
using stm32::Peripheral;

/* === Valid I2C peripheral instantiations === */
template auto i2c::Controller::create<Peripheral::I2C_1>(const uint32_t) -> i2c::Controller;
template auto i2c::Controller::create<Peripheral::I2C_2>(const uint32_t) -> i2c::Controller;
#ifndef STM32L053xx
template auto i2c::Controller::create<Peripheral::I2C_3>(const uint32_t) -> i2c::Controller;
#endif // STM32L053xx

namespace detail
{
    /* === Needed Clock Information === */
    /**
     * @brief Get the APB clock speed in MHz and check whether it is valid for the
     * i2c peripheral
     * @return Returns the defined APB clock speed in MHz.
     * @details constexpr
     */
    constexpr auto get_FREQ() -> uint32_t
    {
        /* Check whether peripheral clock frequency is valid is valid */
        static_assert(F_APB1 >= 2'000'000, "Minimum APB clock speed for I2C peripheral is 2 MHz!");
        static_assert(F_APB1 < 50'000'000, "Maximum APB clock speed for I2C peripheral is 50 MHz!");

        /* Return F_I2C in MHz */
        return F_APB1 / 1'000'000;
    };

    /**
     * @brief Calculate the optimal prescaler value for STM32L0 devices
     * @param frequency The output frequency for the bus
     * @return Returns the 4-bit prescaler value.
     */
    constexpr auto get_prescaler(const uint32_t frequency) -> uint8_t
    {
        return (1 + ((F_APB1 / frequency) / 255)) & 0b1111;
    };

    /**
     * @brief Get the control bits to define the rise time.
     * @param rise_time_ns The desired maximum rise time in ns.
     * @return Returns the bits for the TRISE control register.
     */
    constexpr auto get_TRISE(const uint32_t rise_time_ns) -> uint32_t
    {
        return (rise_time_ns * get_FREQ() / 1000) + 1;
    };

    /**
     * @brief Get the i2c peripheral address of the desired instance
     * of the peripheral.
     * @tparam i2c The I2C hardware instance to be used as the controller hardware
     * @return The address of the hardware instance.
     */
    template <Peripheral i2c>
    constexpr auto get_peripheral_address() -> std::uintptr_t
    {
        switch (i2c)
        {
            case Peripheral::I2C_1:
                return I2C1_BASE;
            case Peripheral::I2C_2:
                return I2C2_BASE;
#ifndef STM32L053xx
            case Peripheral::I2C_3:
                return I2C3_BASE;
#endif // STM32L053xx
            default:
                return 0;
        }
    };

    /**
     * @brief Calculate the control bits for the clock control
     * register of the i2c peripheral.
     * @param frequency The desired clock frequency in Hz.
     * @return The control bits for the timing ot clock control register.
     * @details For i2c frequencies below 100 kHz the SM mode is used.
     * For frequencies higher than 100 kHz, the FM mode with DUTY=1 is used (STM32F4xx).
     */
    constexpr auto get_clock_control(const uint32_t frequency) -> uint32_t
    {
        uint32_t reg_val = 0;
#if defined(STM32F4)
        /* Set F/S and DUTY bits according to output frequency */
        if (frequency <= 100'000)
        {
            /* get the CCR value */
            reg_val = (get_FREQ() * 1'000'000) / (2 * frequency);
            /* limit CCR to 0x01 since SM is enabled with this frequency */
            if (reg_val < 0x04)
                reg_val = 0x04;
            /* limit CCR value to 12 bits */
            reg_val &= 0xFFF;
        }
        else
        {
            /* get the CCR value */
            reg_val = (get_FREQ() * 1'000'000) / (25 * frequency) + 1;
            /* limit CCR value to 12 bits */
            reg_val &= 0xFFF;
            reg_val |= I2C_CCR_FS | I2C_CCR_DUTY;
        }
#elif defined(STM32L0)
        uint8_t _pre = get_prescaler(frequency);
        uint8_t _pre_l = ((F_APB1 / (_pre * frequency)) / 2) & 0b11111111;
        uint8_t _pre_h = ((F_APB1 / (_pre * frequency)) / 2) & 0b11111111;
        reg_val = (--_pre << 28) | (--_pre_h << 8) | (--_pre_l << 0);
#endif // STM32F4
        return reg_val;
    };
}; // namespace detail

namespace i2c
{
    /* === Builder === */
    template <Peripheral i2c_instance>
    auto Controller::create(const uint32_t frequency) -> Controller
    {
        /* Enable the peripheral clock */
        uint32_t _reg = 0;
        switch (i2c_instance)
        {
            case Peripheral::I2C_1:
                _reg = RCC_APB1ENR_I2C1EN;
                break;
            case Peripheral::I2C_2:
                _reg = RCC_APB1ENR_I2C2EN;
                break;
#ifndef STM32L053xx
            case Peripheral::I2C_3:
                _reg = RCC_APB1ENR_I2C3EN;
                break;
#endif // STM32L053xx
            default:
                break;
        };
        RCC->APB1ENR |= _reg;

        /* Get the peripheral address */
        std::uintptr_t address = detail::get_peripheral_address<i2c_instance>();

        /* Return the controller instance */
        return Controller{i2c_instance, address, frequency};
    };

    /* === Constructors === */
    Controller::Controller(
        const Peripheral i2c_instance,
        const std::uintptr_t i2c_address,
        const uint32_t frequency)
        : Base{i2c_instance}, peripheral{reinterpret_cast<volatile I2C_TypeDef *>(i2c_address)}
    {

        /* Set the configuration registers */
        peripheral->CR1 = 0;
#if defined(STM32F4)
        peripheral->CR2 = detail::get_FREQ();
        peripheral->CCR = detail::get_clock_control(frequency);
        peripheral->TRISE = detail::get_TRISE(500); /** @todo Rise time is fixed to 500ns for now. Adjust that later. */
#ifdef STM32F429xx
        peripheral->FLTR = 0;
#endif // STM32F429xx
#elif defined(STM32L0)
        peripheral->CR2 = 0;
        peripheral->TIMINGR = detail::get_clock_control(frequency);
        peripheral->TIMEOUTR = 0;
#endif // STM32F4

        /* Enable a standard timeout of 100 calls */
        set_timeout(100);

        /* initialize received data buffer */
        rx_data.value = 0;
    };

    /* === Methods === */
    auto Controller::set_target_address(const uint8_t address) -> Controller &
    {
        /* only save the first 7-bits of the address.
         * the rest of the driver assumes that the last bit of
         * the address is always zero.
         */
        this->target = (address & 0xFE);

        /* Return a reference to the controller */
        return *this;
    };

    auto Controller::ack_received() const -> bool
    {
#if defined(STM32F4)
        /* Check whether nack was received and clear error */
        bool nack = (this->peripheral->SR1 & I2C_SR1_AF);
        this->peripheral->SR1 &= ~I2C_SR1_AF;

        /* Return whether there was NO acknowledge failure */
        return !nack;
#elif defined(STM32L0)
        /* Check whether nack was received and clear error */
        bool nack = (this->peripheral->ISR & I2C_ISR_NACKF);
        this->peripheral->ICR |= I2C_ICR_NACKCF;

        /* Return whether there was NO acknowledge failure */
        return !nack;
#endif // STM32F4
    };

    auto Controller::address_sent() const -> bool
    {
#if defined(STM32F4)
        return this->peripheral->SR1 & I2C_SR1_ADDR;
#elif defined(STM32L0)
        /* This L0 devices combine the start generation an the address transmission
         * therefore the address is sent, when the start condition is generated */
        return this->start_sent();
#endif // STM32F4
    };

    auto Controller::bus_busy() const -> bool
    {
#if defined(STM32F4)
        return this->peripheral->SR2 & I2C_SR2_BUSY;
#elif defined(STM32L0)
        return this->peripheral->ISR & I2C_ISR_BUSY;
#endif // STM32F4
    };

    auto Controller::get_rx_data() const -> Data_t
    {
        return this->rx_data;
    };

    auto Controller::get_target_address() const -> uint8_t
    {
        return this->target;
    };

    auto Controller::in_controller_mode() const -> bool
    {
#if defined(STM32F4)
        return this->peripheral->SR2 & I2C_SR2_MSL;
#elif defined(STM32L0)
        return true;
#endif // STM32F4
    };

    auto Controller::RX_data_valid() const -> bool
    {
#if defined(STM32F4)
        return this->peripheral->SR1 & I2C_SR1_RXNE;
#elif defined(STM32L0)
        return this->peripheral->ISR & I2C_ISR_RXNE;
#endif // STM32F4
    };

    auto Controller::start_sent() const -> bool
    {
#if defined(STM32F4)
        return this->peripheral->SR1 & I2C_SR1_SB;
#elif defined(STM32L0)
        return !(this->peripheral->CR2 & I2C_CR2_START);
#endif // STM32F4
    };

    auto Controller::transfer_finished() const -> bool
    {
#if defined(STM32F4)
        return this->peripheral->SR1 & I2C_SR1_BTF;
#elif defined(STM32L0)
        return this->peripheral->ISR & (I2C_ISR_TXIS | I2C_ISR_RXNE | I2C_ISR_TC);
#endif // STM32F4
    };

    auto Controller::TX_register_empty() const -> bool
    {
#if defined(STM32F4)
        return this->peripheral->SR1 & I2C_SR1_TXE;
#elif defined(STM32L0)
        return this->peripheral->ISR & I2C_ISR_TXE;
#endif // STM32F4
    };

    /*
     * @brief Set the clock pin as an output
     * @param output_pin The PIN object for the clock pin.
     */
    // void Bus::assign_pin(GPIO::PIN_Base& output_pin)
    // {
    //     output_pin.setMode(GPIO::AF_Mode);
    //     output_pin.setType(GPIO::OPEN_DRAIN);

    //     // Get the peripheral which is used, since the AF modes depend on that
    //     if (this->peripheral == I2C1)
    //         output_pin.set_alternate_function(GPIO::I2C_1);
    //     else if (this->peripheral == I2C2)
    //         output_pin.set_alternate_function(GPIO::I2C_2);
    //     else
    //         output_pin.set_alternate_function(GPIO::I2C_3);
    // };

    void Controller::enable()
    {
        this->peripheral->CR1 |= I2C_CR1_PE;
    };

    void Controller::disable()
    {
        this->peripheral->CR1 &= ~I2C_CR1_PE;
    };

    void Controller::generate_start() const
    {
#if defined(STM32F4)
        this->peripheral->CR1 |= I2C_CR1_START;
#elif defined(STM32L0)
        this->peripheral->CR2 |= I2C_CR2_START;
#endif // STM32F4
    };

    void Controller::generate_stop() const
    {
#if defined(STM32F4)
        this->peripheral->CR1 &= ~I2C_CR1_ACK;
        this->peripheral->CR1 |= I2C_CR1_STOP;
#elif defined(STM32L0)
        this->peripheral->CR2 |= I2C_CR2_STOP;
#endif // STM32F4
    };

    auto Controller::read_data(const uint8_t reg,
                               const uint8_t n_bytes) -> bool
    {
        /* Only start transfer when bus is idle */
        if (!this->bus_busy())
        {
            std::optional<uint8_t> rx;
            this->rx_data.value = 0;

#ifdef STM32L0
            /* Set to transfer 1 byte */
            this->peripheral->CR2 = (1 << 16) | this->target;
#endif // STM32L0
            /* Send address indicating a write */
            if (!this->send_address(false))
                return false;

            /* Send the register to read from */
            if (!this->send_data_byte(reg))
                return false;

#ifdef STM32L0
            /* Set the number of bytes to be received and target address */
            this->peripheral->CR2 = (n_bytes << 16) | I2C_CR2_RD_WRN | this->target;
#endif // STM32L0
            /* After sending the register address, send the address indicating a read */
            if (!this->send_address(true))
                return false;

            /* when more than one byte is received */
            for (uint8_t iByte = n_bytes; iByte > 1; iByte--)
            {
#if defined(STM32F4)
                /* Enable ACK bit */
                this->peripheral->CR1 |= I2C_CR1_ACK;
#endif // STM32F4
                /* Read the byte and check whether it is valid */
                rx = this->read_data_byte();
                if (!rx)
                    return false;
                /* data is valid */
                this->rx_data.byte[iByte - 1] = rx.value();
            }

            /* Generate stop after last byte is received */
            this->generate_stop();

            /* Read the byte and check whether it is valid */
            rx = this->read_data_byte();
            if (!rx)
                return false;
            /* data is valid */
            this->rx_data.byte[0] = rx.value();

            return true;
        }
        else
        {
            this->set_error(Error::Code::I2C_BUS_Busy_Error);
            return false;
        }
        return true;
    };

    auto Controller::read_array(const uint8_t reg,
                                uint8_t *dest,
                                const uint8_t n_bytes) -> bool
    {
        /* Only start transfer when bus is idle */
        if (!this->bus_busy())
        {
            std::optional<uint8_t> rx;
            this->rx_data.value = 0;

#ifdef STM32L0
            /* Set to transfer 1 byte */
            this->peripheral->CR2 = (1 << 16) | this->target;
#endif // STM32L0
            /* Send address indicating a write */
            if (!this->send_address(false))
                return false;

            /* Send the register to read from */
            if (!this->send_data_byte(reg))
                return false;

#ifdef STM32L0
            /* Set the number of bytes to be received and target address */
            this->peripheral->CR2 = (n_bytes << 16) | I2C_CR2_RD_WRN | this->target;
#endif // STM32L0
            /* After sending the register address, send the address indicating a read */
            if (!this->send_address(true))
                return false;

            /* when more than one byte is received */
            for (uint8_t iByte = n_bytes; iByte > 1; iByte--)
            {
#if defined(STM32F4)
                /* Enable ACK bit */
                this->peripheral->CR1 |= I2C_CR1_ACK;
#endif // STM32F4
                /* Read the byte and check whether it is valid */
                rx = this->read_data_byte();
                if (!rx)
                    return false;
                /* data is valid */
                dest[iByte - 1] = rx.value();
            }

            /* Generate stop after last byte is received */
            this->generate_stop();

            /* Read the byte and check whether it is valid */
            rx = this->read_data_byte();
            if (!rx)
                return false;
            /* data is valid */
            dest[0] = rx.value();

            return true;
        }
        else
        {
            this->set_error(Error::Code::I2C_BUS_Busy_Error);
            return false;
        }
        return true;
    };

    auto Controller::send_address(bool read) -> bool
    {
        /* Reset timeout */
        this->reset_timeout();

        /* Generate Start condition */
        this->generate_start();
        while (!this->start_sent())
        {
            if (this->timed_out())
            {
                this->set_error(Error::Code::I2C_Timeout);
                return false;
            }
        }

        /* send the address */
        this->write_address();

        /* Wait for transmission and check whether target responded */
#if defined(STM32F4)
        while (!this->address_sent())
#elif defined(STM32L0)
        /* the L0 devices set the TXIS flag after an successful
         * address transmission */
        while (!this->transfer_finished())
#endif // STM32F4
        {
            if (!this->ack_received())
            {
                /* Set the error */
                this->set_error(Error::Code::I2C_Address_Error);
                /* reset the error flag and stop waiting */
                return false;
            }
        }

        /* Address was sent successfully. Return controller mode,
         * which should be true after sending the address.
         */
        return this->in_controller_mode();
    };

    auto Controller::send_array(uint8_t *const data,
                                const uint8_t n_bytes) -> bool
    {
        /* Only start transfer when bus is idle */
        if (!this->bus_busy())
        {
#ifdef STM32L0
            /* Set the number of bytes and target address */
            this->peripheral->CR2 = (n_bytes << 16) | this->target;
#endif // STM32L0
            if (this->send_address())
            {
                for (uint8_t n_byte = 0; n_byte < n_bytes; n_byte++)
                    if (!this->send_data_byte(data[n_byte]))
                        return false;

                /* After sending all bytes generate the stop condition */
                this->generate_stop();
                return true;
            }
            return false;
        }
        else
        {
            this->set_error(Error::Code::I2C_BUS_Busy_Error);
            return false;
        }
    };

    auto Controller::send_array_leader(const uint8_t byte,
                                       uint8_t *const data,
                                       const uint8_t n_bytes) -> bool
    {
        /* Only start transfer when bus is idle */
        if (!this->bus_busy())
        {
#ifdef STM32L0
            /* Set the number of bytes (array + leading byte) and target address */
            this->peripheral->CR2 = ((n_bytes + 1) << 16) | this->target;
#endif // STM32L0
            if (this->send_address())
            {
                /* Send the leading byte */
                if (!this->send_data_byte(byte))
                    return false;

                /* proceed with the array */
                for (uint8_t n_byte = 0; n_byte < n_bytes; n_byte++)
                    if (!this->send_data_byte(data[n_byte]))
                        return false;

                /* After sending all bytes generate the stop condition */
                this->generate_stop();
                return true;
            }
            return false;
        }
        else
        {
            this->set_error(Error::Code::I2C_BUS_Busy_Error);
            return false;
        }
    };

    auto Controller::send_data(const Data_t payload,
                               const uint8_t n_bytes) -> bool
    {
        /* Only start transfer when bus is idle */
        if (!this->bus_busy())
        {
#ifdef STM32L0
            /* Set the number of bytes and target address */
            this->peripheral->CR2 = (n_bytes << 16) | this->target;
#endif // STM32L0
            if (this->send_address())
            {
                for (uint8_t n_byte = n_bytes; n_byte > 0; n_byte--)
                    if (!this->send_data_byte(payload.byte[n_byte - 1]))
                        return false;

                /* After sending all bytes generate the stop condition */
                this->generate_stop();
                return true;
            }
            return false;
        }
        else
        {
            this->set_error(Error::Code::I2C_BUS_Busy_Error);
            return false;
        }
    };

    void Controller::write_address(bool read)
    {
#ifdef STM32F4
        this->write_data_register(this->target | read);
#endif // STM32F4
    };

    auto Controller::read_data_byte() -> std::optional<uint8_t>
    {
        this->reset_timeout();

        /* Wait until the peripheral can except new data */
        while (!this->RX_data_valid())
        {
            /* Check for timeouts */
            if (this->timed_out())
            {
                this->set_error(Error::Code::I2C_Timeout);
                return {};
            }
        }

        /* read the byte */
        return this->read_data_register();
    };

    auto Controller::read_data_register() const -> uint8_t
    {
#if defined(STM32F4)
        return this->peripheral->DR;
#elif defined(STM32L0)
        return this->peripheral->RXDR;
#endif // STM32F4
    };

    auto Controller::send_data_byte(const uint8_t data) -> bool
    {
        this->reset_timeout();

        /* Wait until the peripheral can except new data */
        while (!this->TX_register_empty())
        {
            /* Check for timeouts */
            if (this->timed_out())
            {
                this->set_error(Error::Code::I2C_Timeout);
                return false;
            }
        }

        /* write the byte */
        this->write_data_register(data);

        /* Wait for the byte transfer */
        while (!this->transfer_finished())
        {
            /* Check for ack or nack */
            if (!this->ack_received())
            {
                this->set_error(Error::Code::I2C_Data_ACK_Error);
                return false;
            }

            /* Check for timeouts */
            if (this->timed_out())
            {
                this->set_error(Error::Code::I2C_Timeout);
                return false;
            }
        }
        /* Byte was sent successfully */
        return true;
    };

    void Controller::write_data_register(const uint8_t data)
    {
#if defined(STM32F4)
        this->peripheral->DR = data;
#elif defined(STM32L0)
        this->peripheral->TXDR = data;
#endif // STM32F4
    };
}; // namespace i2c
