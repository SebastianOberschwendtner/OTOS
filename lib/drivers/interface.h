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

#ifndef INTERFACE_H_
#define INTERFACE_H_

// === Includes ===
#include "error_codes.h"
#include <optional>

// === Common identifier for specialized IOs ===
enum class IO: unsigned char
{
    SYSTEM_ = 0, TIM_1, TIM_2, TIM_3, TIM_4, TIM_5, TIM_6,
    TIM_7, TIM_8, TIM_9, TIM_10, TIM_11, TIM_12, TIM_13, TIM_14,
    I2C_1, I2C_2, I2C_3, SPI_1, SPI_2, SPI_3, USART_1,
    USART_2, USART_3, USART_4, USART_5, USART_6, CAN_1,
    CAN_2, OTG_FS_, OTG_HS_, ETH_, FSMC_, SDIO_, DCMI_, EVENTOUT_
};

// === Common base class for every driver ===
namespace Driver {
    class Base {
    private:
        // *** Properties ***
        Error::Code   error{Error::Code::None};
        unsigned char timeout{0};
        unsigned char called{0};

    public:
    
        // *** Constructor ***
        Base() = default;
        Base(const IO IO_instance): instance{IO_instance}{};

        // *** Methods ***
        void            set_error       (const Error::Code err) { this->error = err; };
        void            set_timeout     (const unsigned char call_count) { this -> timeout = call_count; };
        void            reset_timeout   (void) { this->called = 0; };
        bool            timed_out       (void) { return (++this->called > this->timeout); };
        Error::Code     get_error       (void) const { return this->error; };

        // *** Properties ***
        IO            instance{IO::SYSTEM_};
    };
};

// === Define the Interfaces ===

// => GPIO Interface
namespace GPIO {
    template<class IO>
    void assign(IO& Pin, Driver::Base& IO_Controller)
    {
        Pin.set_alternate_function(IO_Controller.instance);
    }
};

// => Bus Communication Interface
namespace Bus {

    // Data type to handle different byte formats
    union Data_t
    {
        unsigned long value;
        unsigned int word[2];
        unsigned char byte[4];

        // Overload the cast operator to int(), mainly for testing but maybe it can be usefull in general
        operator int() { return static_cast<unsigned int>(this->value); };
    };

    /* === Bus interface ===
    * Bus drivers have to implement the following functions:
    */
    template<class bus_controller>
    void change_address(bus_controller& bus, const unsigned char address)
    {
        bus.set_target_address(address);
        return;
    };

    /**
     * @brief Send a byte to a bus target
     * @param byte Byte to be sent
     * @return Returns True when the byte was sent successfully, False otherwise.
     * @details blocking function
     */
    template<class bus_controller>
    bool send_byte(bus_controller& bus, const unsigned char byte)
    { 
        // set the payload data
        Data_t payload{};
        payload.byte[0] = byte;

        // send the data
        return bus.send_data(payload, 1);
    };

    template<class bus_controller>
    bool send_bytes(
        bus_controller& bus,
        const unsigned char first_byte,
        const unsigned char second_byte
        )
    {
        // set the payload data
        Data_t payload{};
        payload.byte[1] = first_byte;
        payload.byte[0] = second_byte;

        // send the data
        return bus.send_data(payload, 2);
    };

    template<class bus_controller>
    bool send_bytes(
        bus_controller& bus,
        const unsigned char first_byte,
        const unsigned char second_byte,
        const unsigned char third_byte
        )
    {
        // set the payload data
        Data_t payload{};
        payload.byte[2] = first_byte;
        payload.byte[1] = second_byte;
        payload.byte[0] = third_byte;

        // send the data
        return bus.send_data(payload, 3);
    };

    /**
     * @brief Send a word to a bus target
     * @param data Word (2 bytes) to be sent
     * @return Returns True when the byte was sent successfully, False otherwise.
     * @details blocking function
     */
    template<class bus_controller>
    bool send_word(bus_controller& bus, const unsigned int word) 
    { 
        // set the payload data
        Data_t payload{};
        payload.word[0] = word;

        // send the data
        return bus.send_data(payload, 2);
    };

    template<class bus_controller>
    bool send_array(bus_controller& bus, const unsigned char* data, const unsigned char n_bytes) 
    { 
        return bus.send_array(data, n_bytes); 
    };

    template<class bus_controller>
    bool send_array_leader(bus_controller& bus, const unsigned char byte, const unsigned char* data, const unsigned char n_bytes) 
    { 
        return bus.send_array_leader(byte, data, n_bytes); 
    };

    template<class bus_controller>
    std::optional<unsigned int> read_word(bus_controller& bus, const unsigned char reg)
    {
        if (bus.read_data(reg, 2))
            return bus.get_rx_data().word[0];
        return {};
    };

    template<class bus_controller>
    bool read_array(bus_controller &bus, const unsigned char reg, unsigned char* dest, const unsigned char n_bytes)
    {
        return bus.read_array(reg, dest, n_bytes);
    };
};

// => Timer Interface
namespace Timer {

};
#endif
