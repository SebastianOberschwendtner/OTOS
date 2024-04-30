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
 * @file    sdio_stm32.cpp
 * @author  SO
 * @version v5.0.0
 * @date    29-Dezember-2021
 * @brief   SDIO driver for STM32 microcontrollers.
 ==============================================================================
 */

/* === Includes === */
#include "sdio_stm32.h"
using stm32::Peripheral;

#ifdef STM32F4
namespace sdio
{
    /* === Builder === */
    auto Controller::create(uint32_t clock_rate) -> Controller
    {
        return Controller(clock_rate);
    };

    /* === Constructors === */
    Controller::Controller(const uint32_t clock_rate)
        : Base{Peripheral::SDIO_}, peripheral{reinterpret_cast<SDIO_TypeDef *>(SDIO_BASE)}
    {
        /* Enable the clock of the peripheral */
        RCC->APB2ENR |= RCC_APB2ENR_SDIOEN;

        /* Set the initial clock rate */
        this->set_clock(clock_rate, true);

        /* Set the timeout limit -> This value is high, because
         * during the card identification the clock speed
         * has to be 400 kHz. */
        this->set_timeout(65000);
        this->set_hardware_timeout(0xFFFFFFFF);

        /* enable the peripheral */
        this->enable();
    };

    /* === Methods === */
    auto Controller::set_bus_width(const Width width) -> Controller &
    {
        /* Save the old register value without the prescaler bits */
        const uint32_t register_old = (this->peripheral->CLKCR & ~SDIO_CLKCR_WIDBUS_Msk);

        /* Set the new clock configuration */
        this->peripheral->CLKCR = register_old | (static_cast<uint8_t>(width) << SDIO_CLKCR_WIDBUS_Pos);

        /* Return the reference to the controller */
        return *this;
    };

    auto Controller::set_clock(
        const uint32_t clock_rate,
        const bool enable_save_power) -> Controller &
    {
        /* Get the prescaler value with its 8 significant bits */
        /* -> SDIO clock is ALWAYS 48 MHz */
        const uint8_t prescaler = ((48'000'000 / clock_rate) - 2);

        /* Save the old register value without the prescaler bits */
        const uint32_t register_old = (this->peripheral->CLKCR & 0b1111101100000000);

        /* Set the new clock configuration */
        this->peripheral->CLKCR = register_old | prescaler;
        if (enable_save_power)
            this->peripheral->CLKCR |= SDIO_CLKCR_PWRSAV;

        /* Return the reference to the controller */
        return *this;
    };

    auto Controller::set_data_length(const uint32_t number_bytes) -> Controller &
    {
        this->peripheral->DLEN = number_bytes;

        /* Return the reference to the controller */
        return *this;
    };

    auto Controller::set_hardware_timeout(const uint32_t sdio_ticks) -> Controller &
    {
        this->peripheral->DTIMER = sdio_ticks;

        /* Return the reference to the controller */
        return *this;
    };

    auto Controller::command_sent() const -> bool
    {
        return this->peripheral->STA & SDIO_STA_CMDSENT;
    };

    auto Controller::command_response_received() const -> bool
    {
        return this->peripheral->STA & SDIO_STA_CMDREND;
    };

    auto Controller::command_response_crc_fail() const -> bool
    {
        return this->peripheral->STA & SDIO_STA_CCRCFAIL;
    };

    auto Controller::data_block_transfer_finished() const -> bool
    {
        return this->peripheral->STA & SDIO_STA_DBCKEND;
    };

    auto Controller::data_RX_available() const -> bool
    {
        return this->peripheral->STA & SDIO_STA_RXDAVL;
    };

    auto Controller::data_TX_empty() const -> bool
    {
        return this->peripheral->STA & SDIO_STA_TXFIFOE;
    };

    auto Controller::hardware_timeout() const -> bool
    {
        return this->peripheral->STA & (SDIO_STA_DTIMEOUT | SDIO_STA_CTIMEOUT);
    };

    auto Controller::is_busy() const -> bool
    {
        return this->peripheral->STA & (SDIO_STA_CMDACT | SDIO_STA_RXACT | SDIO_STA_TXACT);
    };

    void Controller::enable()
    {
        /* Enable the bus block */
        this->peripheral->CLKCR |= SDIO_CLKCR_CLKEN;

        /* Enable the power */
        this->peripheral->POWER = 0b11;
    };

    void Controller::clear_command_flags()
    {
        this->peripheral->ICR = (SDIO_ICR_CMDSENTC | SDIO_ICR_CMDRENDC | SDIO_ICR_CCRCFAILC);
    };

    void Controller::clear_data_flags()
    {
        this->peripheral->ICR = (SDIO_ICR_DBCKENDC | SDIO_ICR_DATAENDC);
    };

    void Controller::clear_error_flags()
    {
        this->peripheral->ICR = SDIO_ICR_RXOVERRC | SDIO_ICR_TXUNDERRC | SDIO_ICR_DTIMEOUTC | SDIO_ICR_CTIMEOUTC | SDIO_ICR_DCRCFAILC | SDIO_ICR_CCRCFAILC;
    };

    auto Controller::read_single_block(
        const uint32_t *buffer_begin,
        const uint32_t *buffer_end) -> bool
    {
        /* Only when bus is not busy */
        if (this->is_busy())
        {
            this->set_error(error::Code::SDIO_BUS_Busy_Error);
            return false;
        }

        /*
         * Get the numer of bytes to transfer and since this is
         * a block transfer the exponent of the number of bytes
         * for the DCTRL register.
         * The buffer has 4 bytes per entry -> n_bytes = 4 * len(buffer)
         */
        const uint32_t n_bytes = std::distance(buffer_begin, buffer_end) * 4;

        /* Calculate the exponent of the byte length */
        uint8_t byte_exponent = 0;
        switch (n_bytes)
        {
            case 1:
                byte_exponent = 0;
                break;
            case 2:
                byte_exponent = 1;
                break;
            case 4:
                byte_exponent = 2;
                break;
            case 8:
                byte_exponent = 3;
                break;
            case 16:
                byte_exponent = 4;
                break;
            case 32:
                byte_exponent = 5;
                break;
            case 64:
                byte_exponent = 6;
                break;
            case 128:
                byte_exponent = 7;
                break;
            case 256:
                byte_exponent = 8;
                break;
            case 512:
                byte_exponent = 9;
                break;
            case 1024:
                byte_exponent = 10;
                break;
            case 2048:
                byte_exponent = 11;
                break;
            case 4096:
                byte_exponent = 12;
                break;
            case 8192:
                byte_exponent = 13;
                break;
            case 16384:
                byte_exponent = 14;
                break;
            default:
                return false;
        }

        /* Set the data length in bytes. */
        this->set_data_length(n_bytes);

        /* Start transfer */
        this->peripheral->DCTRL = (byte_exponent << 4) | SDIO_DCTRL_DTDIR | SDIO_DCTRL_DTEN;

        /* Wait for transfer to finish */
        uint32_t *iter = const_cast<uint32_t *>(buffer_begin);
        this->reset_timeout();
        while (not this->data_block_transfer_finished())
        {
            /* Check for timeouts */
            if (this->hardware_timeout() || this->timed_out())
            {
                this->set_error(error::Code::SDIO_Timeout);
                return false;
            }

            /* Save the FIFO data */
            if (this->data_RX_available())
            {
                *iter = this->peripheral->FIFO;
                iter++;
            }
        }

        /* Transfer finished without errors, clear flags and exit. */
        this->clear_data_flags();
        return true;
    };

    auto Controller::send_command_no_response(
        const uint8_t command,
        const uint32_t argument) -> bool
    {
        /* Only when bus is not busy */
        if (this->is_busy())
        {
            this->set_error(error::Code::SDIO_BUS_Busy_Error);
            return false;
        };

        /* Start Command transfer */
        this->peripheral->ARG = argument;
        this->peripheral->CMD = SDIO_CMD_CPSMEN | SDIO_CMD_ENCMDCOMPL | (command & 0b111111);

        /* Wait for transfer to finish */
        this->reset_timeout();
        while (!this->command_sent())
        {
            if (this->hardware_timeout() || this->timed_out())
            {
                this->set_error(error::Code::SDIO_Timeout);
                return false;
            }
        }

        /* transfer finished, clear flags and return */
        this->clear_command_flags();
        return true;
    };

    auto Controller::send_command_R1_response(
        const uint8_t command,
        const uint32_t argument) -> std::optional<uint32_t>
    {
        /* Only when bus is not busy */
        if (this->is_busy())
        {
            this->set_error(error::Code::SDIO_BUS_Busy_Error);
            return {};
        };

        /* Start Command transfer */
        this->peripheral->ARG = argument;
        this->peripheral->CMD = SDIO_CMD_CPSMEN | SDIO_CMD_ENCMDCOMPL | SDIO_CMD_WAITRESP_0 | (command & 0b111111);

        /* Wait for transfer to finish */
        this->reset_timeout();
        while (!this->command_response_received())
        {
            if (this->hardware_timeout() || this->timed_out())
            {
                this->set_error(error::Code::SDIO_Timeout);
                return {};
            }
        }

        /* transfer finished, clear flags and return */
        this->clear_command_flags();
        return this->peripheral->RESP1;
    };

    auto Controller::send_command_R2_response(
        const uint8_t command,
        const uint32_t argument) -> std::optional<uint32_t>
    {
        /* Only when bus is not busy */
        if (this->is_busy())
        {
            this->set_error(error::Code::SDIO_BUS_Busy_Error);
            return {};
        };

        /* Start Command transfer */
        this->peripheral->ARG = argument;
        this->peripheral->CMD = SDIO_CMD_CPSMEN | SDIO_CMD_ENCMDCOMPL | SDIO_CMD_WAITRESP_1 | SDIO_CMD_WAITRESP_0 | (command & 0b111111);

        /* Wait for transfer to finish */
        this->reset_timeout();
        while (!this->command_response_received())
        {
            if (this->hardware_timeout() || this->timed_out())
            {
                this->set_error(error::Code::SDIO_Timeout);
                return {};
            }
        }

        /* transfer finished, clear flags and return */
        this->clear_command_flags();
        return this->peripheral->RESP1;
    };

    auto Controller::send_command_R3_response(
        const uint8_t command,
        const uint32_t argument) -> std::optional<uint32_t>
    {
        /* Only when bus is not busy */
        if (this->is_busy())
        {
            this->set_error(error::Code::SDIO_BUS_Busy_Error);
            return {};
        };

        /* Start Command transfer */
        this->peripheral->ARG = argument;
        this->peripheral->CMD = SDIO_CMD_CPSMEN | SDIO_CMD_ENCMDCOMPL | SDIO_CMD_WAITRESP_0 | (command & 0b111111);

        /* Wait for transfer to finish */
        this->reset_timeout();
        while (!this->command_response_crc_fail() && !this->command_response_received())
        {
            if (this->hardware_timeout() || this->timed_out())
            {
                this->set_error(error::Code::SDIO_Timeout);
                return {};
            }
        }

        /* transfer finished, clear flags and return */
        this->clear_command_flags();
        return this->peripheral->RESP1;
    };

    auto Controller::send_command_R6_response(
        const uint8_t command,
        const uint32_t argument) -> std::optional<uint32_t>
    {
        return this->send_command_R1_response(command, argument);
    };

    auto Controller::send_command_R7_response(
        const uint8_t command,
        const uint32_t argument) -> std::optional<uint32_t>
    {
        return this->send_command_R1_response(command, argument);
    };

    auto Controller::write_single_block(
        const uint32_t *buffer_begin,
        const uint32_t *buffer_end) -> bool
    {
        /* Only when bus is not busy */
        if (this->is_busy())
        {
            this->set_error(error::Code::SDIO_BUS_Busy_Error);
            return false;
        }

        /*
         * Get the numer of bytes to transfer and since this is
         * a block transfer the exponent of the number of bytes
         * for the DCTRL register.
         * The buffer has 4 bytes per entry -> n_bytes = 4 * len(buffer)
         */
        const uint32_t n_bytes = (buffer_end - buffer_begin) * 4;

        /* Calculate the exponent of the byte length */
        uint8_t byte_exponent = 0;
        switch (n_bytes)
        {
            case 1:
                byte_exponent = 0;
                break;
            case 2:
                byte_exponent = 1;
                break;
            case 4:
                byte_exponent = 2;
                break;
            case 8:
                byte_exponent = 3;
                break;
            case 16:
                byte_exponent = 4;
                break;
            case 32:
                byte_exponent = 5;
                break;
            case 64:
                byte_exponent = 6;
                break;
            case 128:
                byte_exponent = 7;
                break;
            case 256:
                byte_exponent = 8;
                break;
            case 512:
                byte_exponent = 9;
                break;
            case 1024:
                byte_exponent = 10;
                break;
            case 2048:
                byte_exponent = 11;
                break;
            case 4096:
                byte_exponent = 12;
                break;
            case 8192:
                byte_exponent = 13;
                break;
            case 16384:
                byte_exponent = 14;
                break;
            default:
                return false;
        }

        /* Set the data length in bytes. */
        this->set_data_length(n_bytes);

        /* Start transfer */
        this->peripheral->DCTRL = (byte_exponent << 4) | SDIO_DCTRL_DTEN;

        /* Wait for transfer to finish */
        uint32_t *iter = const_cast<uint32_t *>(buffer_begin);
        this->reset_timeout();
        while ((not this->data_block_transfer_finished()) || this->is_busy())
        {
            /* Check for timeouts */
            if (this->hardware_timeout()) /* || this->timed_out() ) */
            {
                this->set_error(error::Code::SDIO_Timeout);
                return false;
            }

            /* Save the FIFO data */
            if (this->data_TX_empty())
            {
                this->peripheral->FIFO = *iter;
                iter++;
            }
        }

        /* Transfer finished without errors, clear flags and exit. */
        this->clear_data_flags();
        return true;
    };
};     // namespace sdio
#endif // STM32F4