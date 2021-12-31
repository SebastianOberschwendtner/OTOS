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

#ifndef SDIO_STM32_H_
#define SDIO_STM32_H_

// === Includes ===
#include <optional>
#include "vendors.h"

// === Needed Interfaces ===
#include "interface.h"

#ifdef STM32F4 // Only STM32F4 devices have the SDIO peripheral

// === Declarations ===
namespace SD
{
    // === Enums ===
    // SDIO Bus modes
    enum class Width: unsigned char
    {
        Default     = 0b00,
        _4Bit       = 0b01,
        _8Bit       = 0b10
    };

    // === Classes ===
    class Controller : public Driver::Base
    {
    private:
        // *** Properties ***
        SDIO_TypeDef *const peripheral;

        // *** Methods ***

    public:
        // *** Constructor ***
        Controller() = delete;
        Controller(const unsigned long clock_rate);

        // *** Methods ***
        void set_clock(const unsigned long clock_rate, const bool enable_save_power = false);
        void set_bus_width(const Width width);
        void set_timeout(const unsigned long sdio_ticks);
        void set_data_length(const unsigned long number_bytes);
        void enable(void);
        bool command_sent(void);
        bool command_response_received(void);
        bool command_response_crc_fail(void);
        bool data_block_transfer_finished(void);
        bool data_RX_available(void);
        bool data_TX_empty(void);
        bool is_busy(void);
        bool hardware_timeout(void);
        void clear_command_flags(void);
        void clear_data_flags(void);
        bool send_command_no_response(const unsigned char command, const unsigned long argument);
        std::optional<unsigned long> send_command_R1_response(const unsigned char command, const unsigned long argument);
        std::optional<unsigned long> send_command_R2_response(const unsigned char command, const unsigned long argument);
        std::optional<unsigned long> send_command_R3_response(const unsigned char command, const unsigned long argument);
        std::optional<unsigned long> send_command_R6_response(const unsigned char command, const unsigned long argument);
        std::optional<unsigned long> send_command_R7_response(const unsigned char command, const unsigned long argument);
        bool read_single_block(const unsigned long* buffer_begin, const unsigned long* buffer_end);
        bool write_single_block(const unsigned long* buffer_begin, const unsigned long* buffer_end);


        // *** Methods templates ***
        template<unsigned char index>
        unsigned long get_long_response()
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
    };
};
#endif
#endif
