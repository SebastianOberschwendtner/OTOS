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

#ifndef I2C_STM32_H_
#define I2C_STM32_H_

// === Includes ===
#include <optional>
#include "vendors.h"

// === Needed Interfaces ===
#include "interface.h"

// === Declarations ===
namespace I2C {

    // === Classes ===
    class Controller: public Controller_Base, public Driver::Base
    {
    private:
        // *** Properties ***
        volatile I2C_TypeDef*   peripheral;
        unsigned char           target;
        Data_t                  rx_data;

        // *** Methods ***
        void                         write_data_register (const unsigned char data);
        unsigned char                read_data_register  (void) const;
        bool                         send_data_byte      (const unsigned char data);
        std::optional<unsigned char> read_data_byte      (void);

    public:
        // *** Constructor ***
        Controller(const Instance i2c_instance, const unsigned long frequency);

        // *** Methods ***
        void            set_target_address  (const unsigned char address)                               final;
        void            assign_pin          (GPIO::PIN_Base& output_pin) const;
        void            enable              (void);
        void            disable             (void);
        void            generate_start      (void);
        void            generate_stop       (void);
        void            write_address       (bool read = false);
        bool            send_address        (bool read = false);
        bool            send_data           (const Data_t payload, const unsigned char n_bytes)         final;
        bool            send_byte           (const unsigned char data)                                  final;
        bool            send_word           (const unsigned int data)                                   final;
        bool            send_array          (const unsigned char* data, const unsigned char n_bytes)    final;
        bool            send_array_leader   (const unsigned char byte, const unsigned char* data, const unsigned char n_bytes)    final;
        unsigned char   get_target_address  (void) const;
        bool            read_data           (const unsigned char reg, unsigned char n_bytes)            final;
        bool            read_byte           (const unsigned char reg)                                   final;
        bool            read_word           (const unsigned char reg)                                   final;
        bool            read_array          (const unsigned char reg, unsigned char* dest, const unsigned char n_bytes)    final;
        Data_t          get_rx_data         (void) const                                                final;
        bool            in_controller_mode  (void) const;
        bool            start_sent          (void) const;
        bool            address_sent        (void) const;
        bool            ack_received        (void) const;
        bool            TX_register_empty   (void) const;
        bool            RX_data_valid       (void) const;
        bool            transfer_finished   (void) const;
        bool            bus_busy            (void) const;
    };

};
#endif