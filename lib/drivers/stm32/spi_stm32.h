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

#ifndef SPI_STM32_H_
#define SPI_STM32_H_

// === Includes ===
#include <optional>
#include "vendors.h"

// === Needed Interfaces ===
#include "interface.h"

// === Declarations ===
namespace SPI
{
    // === Classes ===
    template <IO spi_instance>
    class Controller : public Driver::Base
    {
    private:
        // *** Properties ***
        SPI_TypeDef *const peripheral;
        Bus::Data_t rx_data{0};

        // *** Methods ***
        bool send_data_byte(const unsigned char data);
        // std::optional<unsigned char> read_data_byte(void);

    public:
        // *** Constructor ***
        Controller() = delete;
        Controller(const unsigned long baudrate);

        // *** Methods ***
        void set_clock_timing(const Level idle, const Edge data_valid);
        void set_use_hardware_chip_select(const bool use_hardware);
        void set_target_address(const unsigned char address) { return; }; // To be compatible with the bus interface
        void enable(void);
        void disable(void);
        bool last_transmit_finished(void) const;
        bool is_busy(void) const;
        bool send_data(const Bus::Data_t payload, const unsigned char n_bytes);
        bool send_array(const unsigned char *data, const unsigned char n_bytes);
        // bool send_array_leader(const unsigned char byte, const unsigned char *data, const unsigned char n_bytes);
        // bool read_data(const unsigned char reg, unsigned char n_bytes);
        // bool read_array(const unsigned char reg, unsigned char *dest, const unsigned char n_bytes);
        // Bus::Data_t get_rx_data(void) const;
        // bool            in_controller_mode  (void) const;
        // bool            start_sent          (void) const;
        // bool            address_sent        (void) const;
        // bool            ack_received        (void) const;
        // bool            RX_data_valid       (void) const;
        // bool            transfer_finished   (void) const;
    };
};

#endif
