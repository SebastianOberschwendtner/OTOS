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

#ifndef SDIO_STM32_H_
#define SDIO_STM32_H_

/* === Includes === */
#include <optional>
#include "vendors.h"

/* === Needed Interfaces === */
#include "interface.h"
#include "peripherals_stm32.h"

#ifdef STM32F4 /* Only STM32F4 devices have the SDIO peripheral */

/* === Declarations === */
namespace sdio
{
    /* === Enums === */
    /* SDIO Bus modes */
    enum class Width: uint8_t
    {
        Default     = 0b00,
        _4Bit       = 0b01,
        _8Bit       = 0b10
    };

    /* === Classes === */
    class Controller : public Driver::Base<stm32::Peripheral>
    {
    public:
        /* === Builder === */
        static auto create(uint32_t clock_rate) -> Controller;

        /* === Constructors === */
        Controller() = delete;
        Controller(const Controller &) = default;
        Controller(Controller &&) = default;
        Controller &operator=(const Controller &) = default;
        Controller &operator=(Controller &&) = default;

        /* === Setters === */
        /**
         * @brief Set the bus width for the communication.
         *
         * @param width The width to use.
        */
        auto set_bus_width(Width width) -> Controller &;

        /**
         * @brief Set the clock rate for the bus communication.
         *
         * @param clock_rate The clock rate in [Hz]. Should be [400 kHz ... 25 MHz] for SDHC-Cards.
         * @param enable_power_save If True the clock is only enabled when bus is active.
         */
        auto set_clock(uint32_t clock_rate, bool enable_save_power = false) -> Controller &;

        /**
         * @brief Set the number of bytes to transfer in one block.
         *
         * @param number_bytes The number of bytes to transfer during one block transfer.
         */
        auto set_data_length(uint32_t number_bytes) -> Controller &;

        /**
         * @brief Set the hardware timeout during data transfer in
         * SDIO clock cycles.
         *
         * @param sdio_ticks The hardware timeout value.
         */
        auto set_hardware_timeout(uint32_t sdio_ticks) -> Controller &;

        /* === Getters === */
        /**
         * @brief Check whether command was sent, when no
         * response is required.
         */
        auto command_sent() const -> bool;

        /**
         * @brief Check whether a response for the active
         * command was received.
         */
        auto command_response_received() const -> bool;

        /**
         * @brief Check whether the CRC check of response for the active
         * command failed.
         */
        auto command_response_crc_fail() const -> bool;

        /**
         * @brief Check whether current data block
         * transfer is finished.
         */
        auto data_block_transfer_finished() const -> bool;

        /**
         * @brief Check whether RX FIFO contains valid data.
         */
        auto data_RX_available() const -> bool;

        /**
         * @brief Check whether TX FIFO is empty.
         */
        auto data_TX_empty() const -> bool;

        /**
         * @brief Get a part of the response of the active command.
         * 
         * @tparam index The index of the response part.
         * @return uint32_t The response part.
         */
        template<uint8_t index>
        auto get_long_response() const -> uint32_t
        {
            if constexpr (index == 0)
                return this->peripheral->RESP1;
            if constexpr (index == 1)
                return this->peripheral->RESP2;
            if constexpr (index == 2)
                return this->peripheral->RESP3;
            if constexpr (index == 3)
                return this->peripheral->RESP4;
        }

        /**
         * @brief Check whether a hardware timeout occurred.
         */
        auto hardware_timeout() const -> bool;

        /**
         * @brief Check whether communication is ongoing
         * on the bus.
         */
        auto is_busy() const -> bool;

        /* === Methods === */
        /**
         * @brief Enable the peripheral and the bus.
         */
        void enable();

        /**
         * @brief Clear the status flags for command transfer
         * which do not clear automatically.
         */
        void clear_command_flags();

        /**
         * @brief Clear the status flags for data transfer
         * which do not clear automatically.
         */
        void clear_data_flags();

        /**
         * @brief Clear all error flags
         */
        void clear_error_flags();

        /**
         * @brief Read a data block.
         * The length of the block is defined by the length of the
         * given buffer.
         *
         * The length of the buffer should be 2^N!
         *
         * @param buffer_begin The begin iterator of the receive buffer.
         * @param buffer_end The end of the receive buffer.
         * @return Return True when the block was read successfully.
         */
        auto read_single_block(const uint32_t* buffer_begin, const uint32_t* buffer_end) -> bool;

        /**
         * @brief Send a command without an expected response.
         *
         * Sets the following errors:
         * - SDIO_Timeout
         * - SDIO_BUS_Busy_Error
         *
         * @param command  The command number to send.
         * @param argument The argument of the command
         * @return Returns True when the command was sent successfully.
         * @details blocking-function
         */
        auto send_command_no_response(const uint8_t command, const uint32_t argument) -> bool;

        /**
         * @brief Send a command with an expected R1 response.
         *
         * Sets the following errors:
         * - SDIO_Timeout
         * - SDIO_BUS_Busy_Error
         *
         * @param command  The command number to send.
         * @param argument The argument of the command
         * @return Returns True and the 32 MSB bits of the response, when the command was sent successfully.
         * @details blocking-function
         */
        auto send_command_R1_response(const uint8_t command, const uint32_t argument) -> std::optional<uint32_t>;

        /**
         * @brief Send a command with an expected R2 response.
         *
         * Sets the following errors:
         * - SDIO_Timeout
         * - SDIO_BUS_Busy_Error
         *
         * @param command  The command number to send.
         * @param argument The argument of the command
         * @return Returns True and the 32 MSB bits of the response, when the command was sent successfully.
         * @details blocking-function
         */
        auto send_command_R2_response(const uint8_t command, const uint32_t argument) -> std::optional<uint32_t>;

        /**
         * @brief Send a command with an expected R3 response.
         *
         * Sets the following errors:
         * - SDIO_Timeout
         * - SDIO_BUS_Busy_Error
         *
         * @param command  The command number to send.
         * @param argument The argument of the command
         * @return Returns True and the 32 MSB bits of the response, when the command was sent successfully.
         * @details blocking-function
         */
        auto send_command_R3_response(const uint8_t command, const uint32_t argument) -> std::optional<uint32_t>;

        /**
         * @brief Send a command with an expected R6 response.
         *
         * Sets the following errors:
         * - SDIO_Timeout
         * - SDIO_BUS_Busy_Error
         *
         * @param command  The command number to send.
         * @param argument The argument of the command
         * @return Returns True and the 32 MSB bits of the response, when the command was sent successfully.
         * @details blocking-function
         */
        auto send_command_R6_response(const uint8_t command, const uint32_t argument) -> std::optional<uint32_t>;

        /**
         * @brief Send a command with an expected R7 response.
         *
         * Sets the following errors:
         * - SDIO_Timeout
         * - SDIO_BUS_Busy_Error
         *
         * @param command  The command number to send.
         * @param argument The argument of the command
         * @return Returns True and the 32 MSB bits of the response, when the command was sent successfully.
         * @details blocking-function
         */
        auto send_command_R7_response(const uint8_t command, const uint32_t argument) -> std::optional<uint32_t>;

        /**
         * @brief Write a data block.
         * The length of the block is defined by the length of the
         * given buffer.
         *
         * The length of the buffer should be 2^N!
         *
         * @param buffer_begin The begin iterator of the transmit data.
         * @param buffer_end The end of the transmit buffer.
         * @return Return True when the block was sent successfully.
         */
        auto write_single_block(const uint32_t* buffer_begin, const uint32_t* buffer_end) -> bool;

    private:
        /* === Constructors === */
        /**
         * @brief Construct a new SDIO controller object.
         * The constructor sets everything up for default communication.
         *
         * @param clock_rate The desired bus clock in [Hz].
         */
        Controller(uint32_t clock_rate);

        /* === Properties === */
        SDIO_TypeDef *peripheral; /**< Pointer to the SDIO peripheral. */
    };
}; // namespace sdio
#endif // STM32F4
#endif // SDIO_STM32_H_
