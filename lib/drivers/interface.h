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

// === Common base class for every driver ===
namespace Driver {
    class Base {
    private:
        Error::Code   error;
        unsigned char timeout;
        unsigned char called;

    public:
        // *** Constructor ***
        Base(): error(Error::Code::None), timeout(0), called(0) {};

        // *** Methods ***
        void            set_error       (const Error::Code err) { this->error = err; };
        void            set_timeout     (const unsigned char call_count) { this -> timeout = call_count; };
        void            reset_timeout   (void) { this->called = 0; };
        bool            timed_out       (void) { return (++this->called > this->timeout); };
        Error::Code     get_error       (void) const { return this->error; };
    };
};

// === Define the Interfaces ===

// => GPIO Interface
namespace GPIO {

    // === Enums ===
    enum PinPort: unsigned char
    {
        PORTA = 0, PORTB, PORTC, PORTD, PORTE, PORTF,
        PORTG, PORTH, PORTI, PORTJ, PORTK 
    };
    enum PinNumber: unsigned char
    {
        PIN0 = 0, PIN1, PIN2, PIN3, PIN4, PIN5, PIN6, PIN7,
        PIN8, PIN9, PIN10, PIN11, PIN12, PIN13, PIN14, PIN15
    };
    enum Mode: unsigned char
    {
        INPUT = 0, OUTPUT, AF_Mode, ANALOG
    };
    enum Type: bool
    {
        PUSH_PULL = false, OPEN_DRAIN = true
    };
    enum Speed: unsigned char
    {
        LOW = 0, MEDIUM, HIGH, VERY_HIGH
    };
    enum Pull: unsigned char
    {
        NO_PP = 0, PULL_UP, PULL_DOWN
    };
    enum Alternate: unsigned char
    {
        SYSTEM_ = 0, TIM_1, TIM_2, TIM_3, TIM_4, TIM_5, TIM_6,
        TIM_7, TIM_8, TIM_9, TIM_10, TIM_11, TIM_12, TIM_13, TIM_14,
        I2C_1, I2C_2, I2C_3, SPI_1, SPI_2, SPI_3, USART_1,
        USART_2, USART_3, USART_4, USART_5, USART_6, CAN_1,
        CAN_2, OTG_FS_, OTG_HS_, ETH_, FSMC_, SDIO_, DCMI_, EVENTOUT_
    };
    enum class Edge: unsigned char
    {
        Rising = 1, Falling = 2, Both = 3
    };

    // === GPIO Interface ===
    /**
     * @brief Abstract interface class for implementing
     * GPIO drivers. Use the `final` keyword in the implementation
     * of the final functions for the compiler to do its
     * optimization thing.
     */
    class PIN_Base
    {
    public:
        // Methods
        virtual void setMode                (const Mode NewMode)        = 0;
        virtual void setType                (const Type NewType)        = 0;
        virtual void setSpeed               (const Speed NewSpeed)      = 0;
        virtual void setPull                (const Pull NewPull)        = 0;
        virtual void set_alternate_function (const Alternate function)  = 0;
        virtual void set                    (const bool NewState)       = 0;
        virtual void setHigh                (void)                      = 0;
        virtual void setLow                 (void)                      = 0;
        virtual void toggle                 (void)                      = 0;
        virtual bool get                    (void) const                = 0;
        virtual void read_edge              (void)                      = 0;
        virtual bool rising_edge            (void) const                = 0;
        virtual bool falling_edge           (void) const                = 0;
        virtual bool enable_interrupt       (const Edge NewEdge) const  = 0;
    };
};

// => I2C Interface
namespace I2C {
    // === Enums ===
    enum Instance: unsigned char
    {
        I2C_1 = 0, I2C_2, I2C_3
    };

    enum State: unsigned char
    {
        Init = 1, Idle, Busy, Error
    };

    union Data_t
    {
        unsigned long value;
        unsigned int word[2];
        unsigned char byte[4];
    };
    

    // === Interface ===
    class Controller_Base
    {
    public:
        // *** Methods ***
        virtual void    set_target_address  (const unsigned char address)                               = 0;
        virtual bool    send_data           (const Data_t payload, const unsigned char n_bytes)         = 0;
        virtual bool    send_byte           (const unsigned char data)                                  = 0;
        virtual bool    send_word           (const unsigned int data)                                   = 0;
        virtual bool    send_array          (const unsigned char* data, const unsigned char n_bytes)    = 0;
        virtual bool    send_array_leader   (const unsigned char byte, const unsigned char* data, const unsigned char n_bytes) = 0;
        virtual bool    read_data           (const unsigned char reg, unsigned char n_bytes)            = 0;
        virtual bool    read_byte           (const unsigned char reg)                                   = 0;
        virtual bool    read_word           (const unsigned char reg)                                   = 0;
        virtual bool    read_array          (const unsigned char reg, unsigned char* dest, const unsigned char n_bytes)    = 0;
        virtual Data_t  get_rx_data         (void) const                                                = 0;
    };

};

// => Timer Interface
namespace Timer {
    // === Enums ===
    enum Instance: unsigned char
    {
        TIM_1 = 0, TIM_2, TIM_3,
        TIM_4, TIM_5, TIM_6, TIM_7, TIM_8,
        TIM_9, TIM_10, TIM_11, TIM_12 
    };

    enum class Mode
    {
        Normal, PWM
    };

    // === Interface ===
    class Timer_Base
    {
    public:
        // === Methods ===
        virtual unsigned int    get_count       (void) const            = 0;
    };
};
#endif
